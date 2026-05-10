# Vision-Guided Quadruped Robot using Raspberry Pi 4B and ESP32




<img width="4080" height="3072" alt="IMG_20260426_224708560" src="https://github.com/user-attachments/assets/d09076d8-81c2-47dc-9fb6-f0fb3c8dbb92" />


A distributed quadruped robotics system combining real-time computer vision, gesture recognition, embedded motion control and IMU-assisted stabilization.

This project uses a Raspberry Pi 4B for high-level AI perception and an ESP32 for low-level actuator control. Hand gestures detected using MediaPipe are translated into predefined quadruped poses such as standing, sitting and waving.

---

# Project Overview

This quadruped robot was developed as a robotics and embedded systems project focusing on:

- Real-time gesture recognition
- Distributed control architecture
- Embedded servo actuation
- IMU-assisted balancing
- Web-based telemetry dashboard
- Dual I2C bus motor control
- Smooth servo interpolation

The system architecture separates:
- High-level perception and decision making → Raspberry Pi
- Low-level deterministic motor control → ESP32

---

# Features

- Gesture-controlled quadruped motion
- Intel RealSense based hand tracking
- MediaPipe AI hand detection
- Smooth pose interpolation
- IMU-assisted stabilization using BNO085
- Flask-based live dashboard
- Dual PCA9685 servo driver architecture
- Serial communication between Raspberry Pi and ESP32
- Modular embedded control architecture

---

# Demonstrated Gestures

| Gesture | Action |
|---|---|
| 👍 Thumbs Up | Stand |
| ✊ Fist / 0 Fingers | Sit |
| 🖐 5 Fingers | Wave / Custom Pose |

---

# System Architecture

```text
Intel RealSense Camera
        ↓
Raspberry Pi 4B
(MediaPipe + Flask Dashboard)
        ↓
Serial USB Communication
        ↓
ESP32
(Motion + IMU Control)
        ↓
Dual PCA9685 Drivers
        ↓
12 Servo Motors
        ↓
Quadruped Motion
```

---

# Hardware Used

| Component | Model |
|---|---|
| Main Controller | Raspberry Pi 4B (8GB) |
| Embedded Controller | ESP32 |
| Camera | Intel RealSense |
| Servo Driver | PCA9685 x2 |
| IMU | BNO085 |
| GPS | Neo-6M |
| Servos | DS3225MG 35KG |
| Power Supply | 450W SMPS |

---

# 💰 Bill of Materials (Approximate)

| Component                       | Quantity | Approx Cost       |
| ------------------------------- | -------- | ----------------- |
| Raspberry Pi 4B 8GB             | 1        | ₹9500             |
| ESP32                           | 1        | ₹700              |
| Intel RealSense Camera          | 1        | ₹50000+           |
| PCA9685 Servo Drivers           | 2        | ₹500              |
| DS3225MG 25KG Servos            | 12       | ₹21000+           |
| BNO085 IMU                      | 1        | ₹3500             |
| Neo-6M GPS Module               | 1        | ₹700              |
| 450W SMPS Power Supply          | 1        | ₹500              |
| 3D Printed Quadruped Body       | 1        | ₹4000+ (Variable) |
| M3 Square Nuts                  | 20       | ₹100              |
| M4 Square Nuts                  | 20       | ₹120              |
| M3x10mm Bolts                   | 20       | ₹150              |
| M4x20mm Bolts                   | 20       | ₹180              |
| Jumper Wires / Connectors       | Multiple | ₹300+             |
| USB 3.0 Cables                  | 2        | ₹400+             |
| Miscellaneous Mounting Hardware | Multiple | ₹500+             |

---

# 📌 Notes

* Prices are approximate and may vary depending on:

  * supplier,
  * region,
  * import availability,
  * printing material,
  * servo quality.

* The Intel RealSense camera contributed the largest portion of the total project cost.

* Mechanical hardware costs may vary depending on:

  * frame modifications,
  * mounting methods,
  * reinforcement structures.

* Additional prototyping expenses such as:

  * soldering,
  * wiring,
  * adapters,
  * and failed print iterations
    are not fully included in this estimate.

---

# Mechanical Design

Open-source quadruped body used:

https://www.thingiverse.com/thing:3445283

The design was modified and adapted for:
- Dual PCA architecture
- ESP32 integration
- IMU placement
- Additional sensor routing
- Power distribution

---

# Software Stack

## Raspberry Pi
- Python
- OpenCV
- MediaPipe
- Flask
- PySerial

## ESP32
- Arduino Framework
- Adafruit PWM Servo Driver
- Adafruit BNO08x Library

---

# Raspberry Pi Responsibilities

The Raspberry Pi performs:
- Real-time camera processing
- Gesture detection
- AI inference using MediaPipe
- Flask dashboard hosting
- Serial communication with ESP32

---

# ESP32 Responsibilities

The ESP32 performs:
- Dual PCA9685 motor control
- Servo interpolation
- Pose execution
- IMU-based balance compensation
- Low-level actuator synchronization

---

# IMU Balancing System

The BNO085 IMU is used for:
- Pitch detection
- Roll detection
- Dynamic balancing offsets

A proportional control method is used to generate corrective offsets for:
- Hip joints
- Knee joints

This improves:
- Standing stability
- Weight distribution
- Dynamic posture correction

---

# Motion Architecture

The robot currently uses:
- Pose-based motion control
- Smooth angle interpolation
- Open-loop gait transitions

Current implemented poses:
- Stand
- Sit
- Wave / Custom pose

Future work includes:
- Dynamic gait generation
- Closed-loop locomotion
- Continuous walking stabilization

---

# Communication Architecture

```text
Raspberry Pi
    ↓ Serial USB
ESP32
    ↓ I2C Bus 1
Front PCA9685
    ↓ I2C Bus 2
Rear PCA9685 + BNO085
```

---

# Wiring Configuration

## ESP32 → Front PCA9685
| Signal | GPIO |
|---|---|
| SDA | 21 |
| SCL | 19 |

## ESP32 → Rear PCA9685 + IMU
| Signal | GPIO |
|---|---|
| SDA | 23 |
| SCL | 22 |

## GPS Neo-6M
| Signal | GPIO |
|---|---|
| RX | 16 |
| TX | 17 |

---

# Installation

## Raspberry Pi

Install dependencies:

```bash
pip install opencv-python mediapipe flask pyserial
```

Run the gesture detection server:

```bash
python3 gesture_control.py
```

---

## ESP32

Install required Arduino libraries:
- Adafruit PWM Servo Driver
- Adafruit BNO08x

Upload:
- Basic pose controller
OR
- IMU balancing controller

---

# Web Dashboard

The Flask dashboard provides:
- Live camera stream
- Gesture telemetry
- FPS monitoring
- System feedback

Access:
```text
http://<RASPBERRY_PI_IP>:5000
```

---

# Current Limitations

- Open-loop locomotion
- No inverse kinematics
- No autonomous navigation
- Basic proportional balancing only
- No terrain adaptation

---

# Future Improvements

Planned future additions:
- Dynamic gait generation
- Full IMU stabilization
- Closed-loop walking
- ROS2 integration
- SLAM / mapping
- Autonomous navigation
- Real-time path planning
- Voice interaction
- Reinforcement learning based gait tuning

---

# Repository Structure

```text
quadruped-robot/
│
├── raspberry_pi/
│   ├── gesture_control.py
│   ├── dashboard/
│   └── requirements.txt
│
├── esp32/
│   ├── basic_pose_controller/
│   ├── imu_balanced_controller/
│   └── libraries_required.md
│
├── docs/
│   ├── architecture.png
│   ├── wiring_diagram.png
│   └── gait_notes.md
│
├── media/
│   ├── images/
│   └── demo_videos/
│
└── README.md
```

---

# Open Source References

Special thanks to all these Open-Source Projects!

## Quadruped Design
https://www.thingiverse.com/thing:3445283

## Referenced Projects
https://github.com/mike4192/spotMicro

https://github.com/mangdangroboticsclub/mini_pupper_ros

These projects greatly helped in understanding:
- quadruped mechanics
- servo configurations
- gait structures
- embedded robotics architecture

---

# Disclaimer

This project is experimental and intended for:
- robotics learning
- embedded systems experimentation
- computer vision integration
- educational research

---

# Author

Developed by:
Sanjith Saji

Robotics and Automation Engineering
Saintgits College of Engineering

---
