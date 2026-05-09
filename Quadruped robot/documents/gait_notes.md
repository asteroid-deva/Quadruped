# 🐾 Gait Notes

This document explains the motion development process used while building the quadruped robot.

The robot currently operates using:

* predefined pose transitions,
* smooth servo interpolation,
* open-loop motion control.

The primary goal during development was to achieve:

* stable standing,
* repeatable sitting,
* smooth gesture-based responses,
* experimental crawling motion.

---

# ⚙️ Motion Control Philosophy

The robot currently uses a pose-based architecture instead of full inverse kinematics.

Each motion is represented as:

```text
Predefined Servo Angle Arrays
```

These angle arrays are manually tuned and then smoothly interpolated during execution.

This approach was chosen because:

* it simplifies debugging,
* reduces computational overhead,
* and allows fast experimentation during hardware assembly.

---

# 🧍 Stand Pose Development

The standing pose was the first stable configuration achieved during development.

## Development Goals

* Equal weight distribution
* Minimal body tilt
* Reduced servo strain
* Stable idle posture

---

## Challenges Faced

### 🔹 Uneven Servo Alignment

Even identical servos produced slightly different neutral positions.

### 🔹 Mirrored Servo Rotation

Some servos rotated in opposite directions because of mirrored mounting.

### 🔹 Mechanical Asymmetry

Small assembly differences created:

* lateral tilt,
* uneven leg height,
* body imbalance.

---

## Calibration Method

The stand pose was manually calibrated by:

1. Adjusting one joint at a time
2. Observing center-of-gravity changes
3. Monitoring body tilt
4. Iteratively tuning servo angles

---

# 🪑 Sit Pose Development

The sit pose required lowering the center of gravity while maintaining stability.

## Development Goals

* Controlled crouching
* Reduced servo stress
* Stable resting posture

---

## Observations

The final sitting pose intentionally uses:

* asymmetric servo angles,
* different knee compression values,
* compensation offsets.

This was necessary because:

* left and right legs behaved differently mechanically,
* servo horn alignment was not perfectly identical.

---

# 👋 Wave Gesture Development

The wave gesture was implemented as the first dynamic motion.

## Motion Strategy

The robot:

1. Stabilizes into standing posture
2. Lifts one front leg
3. Applies compensation using remaining legs
4. Performs repeated waving motion

---

## Challenges

### ⚠️ Weight Redistribution

Lifting one leg significantly shifts the center of gravity.

### ⚠️ Servo Torque Variation

Different servos responded with slightly different speeds under load.

### ⚠️ Balance Instability

Without compensation, the robot tilted toward the lifted side.

---

## Current Solution

The current implementation uses:

* predefined compensation angles,
* smooth interpolation,
* slower movement speeds.

This improved stability during waving.

---

# 🚶 Crawl / Walking Experiments

Basic crawl-style gait experiments were attempted using manually tuned leg movements.

The robot was tested using:

* sequential leg lifting,
* small forward step offsets,
* compensation adjustments.

---

# ⚠️ Problems Encountered

## 🔹 Continuous Drift

The robot gradually leaned during repeated stepping cycles.

## 🔹 Leg Synchronization Issues

Small timing mismatches caused instability.

## 🔹 Open-Loop Limitations

Without real-time correction:

* balance errors accumulated,
* body tilt increased over time.

## 🔹 Mechanical Compliance

Servo backlash and frame flex affected gait consistency.

---

# 🧠 Current Motion Architecture

The robot currently uses:

```text
Open-Loop Pose Control
```

Meaning:

* motions are predefined,
* there is limited environmental feedback,
* no inverse kinematics is currently implemented.

---

# 🔬 Future Improvements

Planned future upgrades include:

## ✅ Dynamic Gait Generation

Continuous walking instead of discrete pose transitions.

## ✅ Inverse Kinematics

Real-time leg positioning calculations.

## ✅ Closed-Loop Stabilization

Using IMU feedback for:

* roll correction,
* pitch correction,
* adaptive posture balancing.

## ✅ Autonomous Locomotion

Combining:

* perception,
* planning,
* stabilization,
* gait generation.

---

# 📌 Important Note

All gait values and poses are highly dependent on:

* assembly precision,
* servo alignment,
* weight distribution,
* mechanical tolerances.

Different builds may require recalibration.
