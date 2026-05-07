# 🤖 QUADRUPED mini

An open-source, 2-DOF custom quadruped robot built from the ground up! This project serves as my foundational testbed for programming kinematics, closed-loop IMU balancing and gait mechanics before scaling up to a larger build. 

The physical design, code and logic are entirely open-sourced. Feel free to use, modify and build upon this for your own robotic projects!

---

## 👨‍💻 About Me & This Project
Hi, I'm behind QUADRUPED mini! I studied this robot to dive deep into the world of robotics, specifically focusing on how to make a machine walk and balance. 

Instead of just downloading pre-made walking code, I wanted to understand the raw math and mechanics. I spent hours manually sending PWM signals to each individual motor just to map out exactly how the physical legs reacted. I documented everything from my first successful standing pose to programming a 4-step creep/shuffle gait and finally integrating an "inner ear" for real-time balancing. 

---

## 🛠️ Hardware & Specs
The brain of this operation uses a dual-I2C bus setup to separate the motor commands from the sensor feedback.

* **Microcontroller:** ESP32 (Handles the logic, kinematics and dual I2C buses)
* **Servo Driver:** PCA9685 16-Channel PWM Driver (Connected to ESP32 pins 22 & 23)
* **Motors:** 8x EMAX ES08MA II Analog Servos (180-degree absolute positioning)
* **Balance Sensor:** BNO085 IMU (Connected to ESP32 pins 19 & 21 for Pitch/Roll feedback)
* **Power Supply:** A massive 450W PC Power Supply! *(Disclaimer: This is hilariously overqualified. You absolutely do not need 450 watts for 8 mini servos, but it is what I had on hand and it definitely gets the job done!)*

---

## 📐 The Journey: Finding the Angles
One of the biggest challenges was realizing that the physical motors are mirrored on the left and right sides of the robot! 

I couldn't just tell all the motors to go to "90 degrees". I had to test every single joint manually through the Serial Monitor. Here is what I learned from my testing:
* **The Mirror Effect:** An "increase" in angle on the right side does the exact opposite physical movement on the left side. 
* **The Sit Down:** By taking my perfectly mapped standing angles and mathematically adding 50° to the right hips while subtracting 50° from the left hips, I achieved a perfectly symmetrical squat!
* **The 2-DOF Walk:** Because the robot lacks knees, it can't just push its hips forward. I had to program a sequence that tilts the shoulders outward to lift the foot, swings the hip forward, plants the foot back down and pulls backward to drag the chassis forward.

---

## 📁 Repository Structure
* `/QUADRUPED_mini` - Contains all the ESP32 Arduino code (.ino files) for calibration, walking and IMU leveling.
* `/Videos` - Contains video demonstrations of the robot standing, walking and self-leveling.


  <img width="4080" height="3072" alt="IMG_20260507_175641412" src="https://github.com/user-attachments/assets/aa5751b1-da79-47d6-90f7-57e8fbfee7f5" />

---
*Built with coffee, code and a 450W power supply.*
