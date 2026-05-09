# 🔧 Hardware Notes and Development Challenges

This document summarizes important hardware observations, issues encountered during development, and practical lessons learned while building the quadruped robot.

---

# ⚡ Power Distribution Challenges

The quadruped uses multiple high-torque servos which require substantial current during motion.

A:

```text
450W SMPS
```

was used to provide stable 5V power to:

* PCA9685 servo drivers,
* servo motors.

The ESP32 is powered separately through:

```text
USB serial connection from Raspberry Pi
```

---

# ⚠️ Servo Current Spikes

One of the largest issues encountered was sudden current draw during:

* standing transitions,
* rapid movement,
* multiple servo startup.

This occasionally caused:

* servo jitter,
* temporary instability,
* power fluctuations.

---

# ✅ Mitigation Strategies

The following approaches improved reliability:

* smoother servo interpolation,
* slower pose transitions,
* separating logic and servo power systems,
* reducing sudden multi-servo acceleration.

---

# 🔥 Raspberry Pi Heating

The Raspberry Pi 4B experienced noticeable heating during:

* MediaPipe inference,
* OpenCV processing,
* Flask streaming,
* RealSense camera usage.

---

# ⚠️ Observed Effects

Under heavy load:

* FPS reduction occurred,
* processing latency increased,
* system responsiveness decreased.

---

# ✅ Cooling Recommendation

A dedicated cooling system is strongly recommended:

* active cooling fan,
* heatsink,
* ventilated enclosure.

---

# 📷 USB Bandwidth Considerations

The Intel RealSense camera uses:

```text
USB 3.0
```

High-bandwidth camera streaming combined with:

* AI processing,
* Flask video streaming,
* serial communication

placed significant load on the Raspberry Pi.

---

# ⚠️ Camera Performance Issues

Occasionally observed:

* frame drops,
* latency spikes,
* delayed gesture response.

These effects became more noticeable during:

* extended runtime,
* thermal throttling,
* high CPU usage.

---

# ⚙️ Servo Calibration Issues

Servo calibration was highly dependent on:

* horn alignment,
* assembly precision,
* frame geometry,
* mechanical tightening.

Even small assembly differences caused:

* body tilt,
* uneven stance,
* gait asymmetry.

---

# ⚠️ Open-Loop Motion Limitations

The current motion architecture is primarily:

```text
Open-Loop Pose Control
```

Without full real-time correction:

* balance drift accumulates,
* gait consistency decreases,
* repeated walking cycles become unstable.

---

# 🧠 IMU Stabilization Benefits

Adding the BNO085 IMU improved:

* standing balance,
* posture correction,
* gesture stability.

The stabilization layer reduced:

* lateral tilt,
* front-back imbalance,
* posture inconsistency.

---

# 🔌 Grounding Importance

A shared common ground between:

* ESP32,
* PCA9685 boards,
* sensors,
* SMPS

was critical for stable operation.

Improper grounding caused:

* unstable servo behavior,
* communication noise,
* inconsistent sensor readings.

---

# 🚀 Future Hardware Improvements

Planned future upgrades include:

## ✅ Better Mechanical Rigidity

Reducing frame flex and backlash.

## ✅ Dedicated Servo Power Monitoring

Real-time current and voltage monitoring.

## ✅ Improved Cooling

Dedicated active cooling for Raspberry Pi.

## ✅ Higher Precision Gait Control

Adding:

* inverse kinematics,
* closed-loop locomotion,
* dynamic stabilization.

---

# 📌 Final Observation

Most real-world instability issues originated from:

* mechanical tolerances,
* power fluctuations,
* servo inconsistency,
* open-loop control limitations,

rather than purely software-related problems.

This project demonstrated the importance of balancing:

* software architecture,
* embedded control,
* power distribution,
* and mechanical design together.
