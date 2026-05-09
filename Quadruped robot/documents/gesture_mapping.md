# ✋ Gesture Mapping

The quadruped robot uses computer vision-based hand gesture detection to trigger predefined robot actions.

Hand detection and gesture recognition are performed on the Raspberry Pi using:

* OpenCV
* MediaPipe

Detected gestures are converted into serial commands and transmitted to the ESP32 for execution.

---

# 🧠 Gesture Recognition Pipeline

```text
Camera Feed
    ↓
MediaPipe Hand Detection
    ↓
Finger Counting / Gesture Classification
    ↓
Serial Command Generation
    ↓
ESP32 Motion Execution
```

---

# 🎮 Current Gesture Mapping

| Gesture            | Robot Action |
| ------------------ | ------------ |
| 👍 Thumbs Up       | Stand        |
| ✊ Fist / 0 Fingers | Sit          |
| 🖐 5 Fingers       | Wave / Pose  |

---

# 📡 Communication Method

The Raspberry Pi communicates with the ESP32 using:

```text
USB Serial Communication
```

Example commands:

```text
STAND
SIT
WAVE
```

---

# ⚙️ Motion Trigger Process

Once a gesture is detected:

1. Raspberry Pi identifies gesture
2. Corresponding command is generated
3. Command sent through serial USB
4. ESP32 receives command
5. ESP32 executes corresponding pose

---

# ⚠️ Challenges Encountered

## 🔹 False Positives

Rapid hand movement occasionally caused incorrect gesture detection.

## 🔹 Lighting Dependency

Poor lighting reduced MediaPipe detection accuracy.

## 🔹 Frame Latency

Higher processing load reduced FPS on Raspberry Pi.

## 🔹 Serial Flooding

Repeated command transmission initially caused motion instability.

This was improved by:

* tracking previous command state,
* only sending new commands when gestures changed.

---

# 🚀 Planned Improvements

Future upgrades may include:

* dynamic gesture sets,
* multi-hand interaction,
* custom gesture training,
* voice + gesture hybrid control,
* autonomous gesture response behaviors.
