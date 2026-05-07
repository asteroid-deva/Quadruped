# 💻 Code Directory & Kinematics Guide

This folder contains the complete software evolution of the QUADRUPED mini. Instead of writing one massive, confusing block of code, I broke the development down into 6 distinct stages. If you are building this robot, I highly recommend uploading these sketches in the exact order listed below to safely calibrate and test your hardware.

## 📂 The 6 Stages of Development

### 1. `pwm_all_to_90_degree`
**The Hardware Calibration Code.** Before attaching any legs, I used this code to snap all 8 servos to their absolute mechanical center (90°). This is a critical first step to ensure the servo horns are attached symmetrically and the metal gears aren't stripped when the robot powers on.

### 2. `channel_0_manualcode`
**The Kinematics Explorer.** This is a custom Serial Monitor tool I wrote to manually type in angles (0-180°) and send them to a single servo channel. I used this heavily to physically observe how the legs reacted to different PWM signals and manually map out the limits of my robot's joints. 

### 3. `imu_bno085`
**The "Inner Ear" Test.** A standalone script to test the BNO085 IMU. It reads the raw sensor data and converts it into clean Euler angles (Roll, Pitch and Yaw) so I could verify the sensor was working before strapping it to a walking robot.

### 4. `QUAD_mini_stand`
**The First Pose.** This code integrates the exact, hard-coded angles I found during my manual testing. It maps the 8 specific shoulder and hip joints and features a Serial Monitor interface to command the robot to `STAND` or `SIT` symmetrically. 

### 5. `QUAD_mini_walk`
**The 2-DOF Gait.** Because this robot lacks knees, it can't just push its hips forward without dragging its feet. This code introduces a 4-step Creep/Shuffle gait. It tilts the shoulders outward to lift the foot, swings the hip forward, plants it and pulls back to drag the body forward.

### 6. `QUAD_mini_stabilization`
**The Brain Upgrade.** The final script combining everything. It utilizes the ESP32's dual hardware I2C buses to run the PCA9685 motor driver on Bus 1 and the BNO085 IMU on Bus 0. When the `LEVEL` command is sent, the robot actively adjusts its shoulder sprawl to counteract the Pitch and Roll of the chassis in real-time.

---

## 📐 My Kinematics Journey: Finding the Angles Manually

When I started programming the poses, I realized I couldn't just download a generic walking code and expect it to work. Because of how the servos were physically mounted to my custom 3D-printed/acrylic chassis, I had to figure out the kinematics completely from scratch. 

Instead of relying on guesswork, I uploaded my `channel_0_manualcode` and spent hours painstakingly typing different angles into the Serial Monitor, watching exactly how the physical legs reacted. 

Here is what I learned from my first-person testing:

### 🪞 The Mirror Effect
My first major realization was that the physical motors are mirrored on the left and right sides of the robot. An "increase" in the angle on the right side resulted in the exact *opposite* physical movement on the left side. For example:
* **Right Shoulders:** Increasing the angle moved the leg *outward*.
* **Left Shoulders:** Increasing the angle moved the leg *inward*.

### 🪑 Calculating the "Sit"
Once I had mapped my baseline standing angles, I wanted the robot to sit down. Because of the mirror effect I discovered earlier, I couldn't just add degrees to all four legs. By applying my logic, I took my baseline standing hip angles, **added 50° to the right hips**, and **subtracted 50° from the left hips**. It resulted in a perfectly symmetrical, smooth squatting motion!

### 🚧 Overcoming Hardware Limits
During my manual testing, I noticed my Right Hips required standing angles of `10°` and `5°`. Because decreasing the angle moved the leg backward, I only had 5 to 10 degrees of clearance before I physically crashed into the servo's hard `0°` limit!

To fix this, I had to completely redesign my walking algorithm. In a standard quadruped gait, a leg swings forward, plants and then pushes *backward* past its center point to physically drag the body forward. Because my right legs couldn't push backward, I made my standing angles the absolute furthest back the legs would ever go. I coded the `QUAD_mini_walk` gait to swing the legs forward into the air, plant them, and simply pull them *back to the standing position* to pull the chassis forward. It was a massive logic hurdle, but it worked perfectly!

### Youtube link 
https://www.youtube.com/playlist?list=PLHxvUT2bWsHtMa1m-egAvFF8-mJCzW7M6






<img width="2808" height="3652" alt="IMG_20260507_134154309" src="https://github.com/user-attachments/assets/3e577002-b2d6-4349-8381-6daeae585698" />
