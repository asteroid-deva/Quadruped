import cv2
import mediapipe as mp
import time
import json
import serial
from flask import Flask, Response, render_template_string, jsonify

# --- Setup Serial to ESP32 ---
SERIAL_PORT = '/dev/ttyUSB0'  # Adjust to ttyACM0 if necessary
BAUD_RATE = 115200
try:
    esp32 = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
    time.sleep(2) # Give ESP32 time to reset after connection
    print(f"Successfully connected to ESP32 on {SERIAL_PORT}")
except Exception as e:
    print(f"Failed to connect to ESP32 Serial: {e}")
    esp32 = None

app = Flask(__name__)

# --- Setup MediaPipe ---
mp_hands = mp.solutions.hands
hands = mp_hands.Hands(
    static_image_mode=False,
    max_num_hands=1,
    model_complexity=0, # 0 is much faster for Raspberry Pi
    min_detection_confidence=0.5,
    min_tracking_confidence=0.5
)
mp_drawing = mp.solutions.drawing_utils

tipIds = [4, 8, 12, 16, 20]

# Global variables for the dashboard
system_status = {
    "gesture": "None",
    "fingers": 0,
    "fps": 0
}

def generate_frames():
    global system_status
   
    # Auto-detect the correct camera index for Intel RealSense
    cap = None
    for i in range(10):
        print(f"Trying camera index {i}...")
        temp_cap = cv2.VideoCapture(i)
        if temp_cap.isOpened():
            success, frame = temp_cap.read()
            # Ensure it actually reads a frame AND it is a color frame (3 channels)
            if success and frame is not None and len(frame.shape) == 3 and frame.shape[2] == 3:
                cap = temp_cap
                print(f"Success! Using camera index {i}")
                break
            else:
                temp_cap.release()
        else:
            temp_cap.release()
           
    if cap is None:
        print("CRITICAL ERROR: Could not find a working RGB camera.")
        return

    cap.set(cv2.CAP_PROP_FRAME_WIDTH, 320)
    cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 240)
    cap.set(cv2.CAP_PROP_FPS, 30)
   
    prev_time = time.time()
    frame_counter = 0
    last_results = None
   
    # Pre-allocate variables to cache the last known state
    cached_gesture = "None"
    cached_fingers = 0
    cached_is_thumbs_up = False

    while cap.isOpened():
        success, frame = cap.read()
        if not success:
            break

        # Calculate FPS
        curr_time = time.time()
        if curr_time - prev_time > 0:
            fps = int(1 / (curr_time - prev_time))
            system_status["fps"] = fps
        prev_time = curr_time

        frame = cv2.flip(frame, 1)
        rgb_frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
       
        frame_counter += 1
       
        # Only run the heavy AI model every 4 frames
        if frame_counter % 4 == 0:
            last_results = hands.process(rgb_frame)
            cached_gesture = "None"
            cached_fingers = 0
            cached_is_thumbs_up = False
       
            if last_results.multi_hand_landmarks:
                for hand_idx, hand_landmarks in enumerate(last_results.multi_hand_landmarks):
                    lmList = []
                    h, w, c = frame.shape
                    for id, lm in enumerate(hand_landmarks.landmark):
                        cx, cy = int(lm.x * w), int(lm.y * h)
                        lmList.append([id, cx, cy])
                       
                    hand_label = last_results.multi_handedness[hand_idx].classification[0].label
                   
                    if len(lmList) == 21:
                        fingers = []

                        # Thumb
                        if hand_label == "Right":
                            fingers.append(1 if lmList[tipIds[0]][1] < lmList[tipIds[0] - 1][1] else 0)
                        else:
                            fingers.append(1 if lmList[tipIds[0]][1] > lmList[tipIds[0] - 1][1] else 0)

                        # 4 Fingers
                        for id in range(1, 5):
                            fingers.append(1 if lmList[tipIds[id]][2] < lmList[tipIds[id] - 2][2] else 0)

                        cached_fingers = fingers.count(1)
                       
                        if cached_fingers == 1 and fingers[0] == 1:
                            if lmList[4][2] < lmList[2][2]: # Thumb pointing up
                                cached_is_thumbs_up = True

                        if cached_is_thumbs_up:
                            cached_gesture = "Thumbs Up!"
                        else:
                            cached_gesture = "Fingers Detected"

            system_status["fingers"] = cached_fingers
            system_status["gesture"] = cached_gesture
           
            # --- Send Command to ESP32 ---
            if esp32 is not None and last_results and last_results.multi_hand_landmarks:
                current_cmd = None
                if cached_is_thumbs_up:
                    current_cmd = "STAND\n"
                elif cached_fingers == 0:
                    current_cmd = "SIT\n"
                elif cached_fingers == 5:
                    current_cmd = "POSE\n"
                   
                # Only send if the command changed so we don't spam the Serial port
                if current_cmd and current_cmd != system_status.get("last_cmd"):
                    try:
                        esp32.write(current_cmd.encode('utf-8'))
                        system_status["last_cmd"] = current_cmd
                        print(f"Command Sent to ESP32: {current_cmd.strip()}")
                    except Exception as e:
                        print(f"Serial write error: {e}")

        # Always draw the cached results on EVERY frame so it looks smooth
        if last_results and last_results.multi_hand_landmarks:
            for hand_landmarks in last_results.multi_hand_landmarks:
                mp_drawing.draw_landmarks(frame, hand_landmarks, mp_hands.HAND_CONNECTIONS)
               
        if cached_is_thumbs_up:
            cv2.putText(frame, "THUMBS UP!", (50, 100), cv2.FONT_HERSHEY_SIMPLEX, 1.5, (0, 255, 0), 3)
        elif cached_fingers > 0:
            cv2.putText(frame, f'Fingers: {cached_fingers}', (50, 100), cv2.FONT_HERSHEY_SIMPLEX, 1.5, (255, 0, 0), 3)

        # Encode frame to JPEG
        ret, buffer = cv2.imencode('.jpg', frame)
        frame_bytes = buffer.tobytes()
       
        # Yield as a multipart HTTP response (MJPEG stream)
        yield (b'--frame\r\n'
               b'Content-Type: image/jpeg\r\n\r\n' + frame_bytes + b'\r\n')
               
    cap.release()

@app.route('/')
def index():
    html = """
    <!DOCTYPE html>
    <html lang="en">
    <head>
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>Quadruped Vision Dashboard</title>
        <link href="https://fonts.googleapis.com/css2?family=Outfit:wght@300;400;600;800&display=swap" rel="stylesheet">
        <style>
            :root {
                --bg-color: #0b0f19;
                --panel-bg: rgba(255, 255, 255, 0.05);
                --accent-color: #00f2fe;
                --text-main: #ffffff;
                --text-muted: #8b9bb4;
            }
           
            body {
                margin: 0;
                padding: 0;
                font-family: 'Outfit', sans-serif;
                background-color: var(--bg-color);
                background-image:
                    radial-gradient(circle at 15% 50%, rgba(79, 172, 254, 0.15), transparent 25%),
                    radial-gradient(circle at 85% 30%, rgba(0, 242, 254, 0.1), transparent 25%);
                color: var(--text-main);
                min-height: 100vh;
                display: flex;
                flex-direction: column;
                align-items: center;
                overflow-x: hidden;
            }

            .header {
                width: 100%;
                padding: 30px 0;
                text-align: center;
                border-bottom: 1px solid rgba(255, 255, 255, 0.05);
                margin-bottom: 40px;
                background: linear-gradient(180deg, rgba(0,0,0,0.4) 0%, transparent 100%);
            }

            h1 {
                margin: 0;
                font-size: 2.5rem;
                font-weight: 800;
                letter-spacing: 2px;
                background: linear-gradient(90deg, #4facfe 0%, #00f2fe 100%);
                -webkit-background-clip: text;
                -webkit-text-fill-color: transparent;
                text-transform: uppercase;
            }

            .subtitle {
                color: var(--text-muted);
                font-size: 1rem;
                margin-top: 10px;
                font-weight: 300;
            }

            .dashboard-container {
                display: flex;
                gap: 40px;
                max-width: 1200px;
                width: 90%;
                align-items: flex-start;
                justify-content: center;
                flex-wrap: wrap;
            }

            /* Glassmorphism Panel */
            .glass-panel {
                background: var(--panel-bg);
                backdrop-filter: blur(12px);
                -webkit-backdrop-filter: blur(12px);
                border: 1px solid rgba(255, 255, 255, 0.1);
                border-radius: 20px;
                padding: 20px;
                box-shadow: 0 8px 32px 0 rgba(0, 0, 0, 0.3);
                transition: transform 0.3s ease;
            }
           
            .glass-panel:hover {
                transform: translateY(-5px);
                border-color: rgba(0, 242, 254, 0.3);
            }

            .video-container {
                flex: 2;
                min-width: 600px;
                display: flex;
                flex-direction: column;
                align-items: center;
            }

            .video-feed {
                width: 100%;
                border-radius: 12px;
                border: 2px solid rgba(0, 242, 254, 0.2);
                box-shadow: 0 0 20px rgba(0, 242, 254, 0.1);
            }

            .stats-container {
                flex: 1;
                min-width: 300px;
                display: flex;
                flex-direction: column;
                gap: 20px;
            }

            .stat-box {
                padding: 25px;
                text-align: center;
            }

            .stat-title {
                color: var(--text-muted);
                font-size: 0.9rem;
                text-transform: uppercase;
                letter-spacing: 1.5px;
                margin-bottom: 15px;
                font-weight: 600;
            }

            .stat-value {
                font-size: 3rem;
                font-weight: 800;
                color: var(--text-main);
                text-shadow: 0 0 10px rgba(255,255,255,0.2);
            }

            .accent-text {
                color: var(--accent-color);
            }

            .status-indicator {
                display: inline-block;
                width: 12px;
                height: 12px;
                background-color: #00f2fe;
                border-radius: 50%;
                margin-right: 10px;
                box-shadow: 0 0 10px #00f2fe, 0 0 20px #00f2fe;
                animation: pulse 2s infinite;
            }

            @keyframes pulse {
                0% { opacity: 0.6; transform: scale(0.9); }
                50% { opacity: 1; transform: scale(1.1); }
                100% { opacity: 0.6; transform: scale(0.9); }
            }

            /* Responsive */
            @media (max-width: 900px) {
                .video-container { min-width: 100%; }
                .stats-container { min-width: 100%; flex-direction: row; flex-wrap: wrap; }
                .stat-box { flex: 1; min-width: 140px; }
            }
        </style>
    </head>
    <body>

        <div class="header">
            <h1>Vision Core Interface</h1>
            <div class="subtitle">
                <span class="status-indicator"></span> System Online &middot; Awaiting Commands
            </div>
        </div>

        <div class="dashboard-container">
            <!-- Video Feed -->
            <div class="glass-panel video-container">
                <div class="stat-title" style="align-self: flex-start; margin-bottom: 20px;">Live Sensor Array</div>
                <img class="video-feed" src="/video_feed" alt="Live Camera Stream" />
            </div>

            <!-- Telemetry & Stats -->
            <div class="stats-container">
                <div class="glass-panel stat-box">
                    <div class="stat-title">Current Gesture</div>
                    <div class="stat-value accent-text" id="gesture-text">Standby</div>
                </div>

                <div class="glass-panel stat-box">
                    <div class="stat-title">Fingers Active</div>
                    <div class="stat-value" id="fingers-text">0</div>
                </div>

                <div class="glass-panel stat-box">
                    <div class="stat-title">Stream FPS</div>
                    <div class="stat-value" id="fps-text" style="font-size: 2rem;">--</div>
                </div>
            </div>
        </div>

        <script>
            // Poll the server for the latest gesture state every 200ms
            setInterval(() => {
                fetch('/status')
                    .then(response => response.json())
                    .then(data => {
                        document.getElementById('gesture-text').innerText = data.gesture;
                        document.getElementById('fingers-text').innerText = data.fingers;
                        document.getElementById('fps-text').innerText = data.fps;
                       
                        // Add glow effect if Thumbs up
                        if(data.gesture === "Thumbs Up!") {
                            document.getElementById('gesture-text').style.color = '#00ff88';
                            document.getElementById('gesture-text').style.textShadow = '0 0 15px rgba(0, 255, 136, 0.5)';
                        } else {
                            document.getElementById('gesture-text').style.color = '#00f2fe';
                            document.getElementById('gesture-text').style.textShadow = 'none';
                        }
                    })
                    .catch(err => console.error('Error fetching status:', err));
            }, 200);
        </script>
    </body>
    </html>
    """
    return render_template_string(html)

@app.route('/video_feed')
def video_feed():
    return Response(generate_frames(), mimetype='multipart/x-mixed-replace; boundary=frame')

@app.route('/status')
def status():
    return jsonify(system_status)

if __name__ == '__main__':
    print("Starting Web Server! Open your browser and go to http://<YOUR_PI_IP_ADDRESS>:5000")
    # Run on all interfaces on port 5000
    app.run(host='0.0.0.0', port=5000, debug=False)

