#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// ---------------------------------------------------------
// PCA9685 Setup
// ---------------------------------------------------------
// Two I2C buses on ESP32
TwoWire I2C_1 = TwoWire(0); // For PCA1 (Front)
TwoWire I2C_2 = TwoWire(1); // For PCA2 (Rear)

// Initialize the two PCA9685 boards. 
// Assuming both are at default address 0x40 but on different I2C buses.
Adafruit_PWMServoDriver pca1 = Adafruit_PWMServoDriver(0x40, I2C_1);
Adafruit_PWMServoDriver pca2 = Adafruit_PWMServoDriver(0x40, I2C_2);

// Servo pulse ranges (tune for your specific servos, usually 150-600)
#define SERVO_MIN 150 
#define SERVO_MAX 600 

// ---------------------------------------------------------
// Motion Control Variables
// ---------------------------------------------------------
#define NUM_MOTORS 12

// Current position of the servos (starts in STAND pose)
float currentAngles[NUM_MOTORS] = {110, 130, 70,  110, 100, 40,  90, 100, 150,  110, 80, 70};

// Target position we want to move to smoothly
float targetAngles[NUM_MOTORS]  = {110, 130, 70,  110, 100, 40,  90, 100, 150,  110, 80, 70};
// Speed of smooth transition (Higher = faster). 
// 1.0 means it moves 1 degree per update.
float moveSpeed = 1.0; 

// Timing for non-blocking movement
unsigned long lastUpdate = 0;
int updateInterval = 15; // update every 15ms

// ---------------------------------------------------------
// Predefined Poses (EDIT THESE ANGLES!)
// Array format:
// [FL_S, FL_H, FL_K,  FR_S, FR_H, FR_K,  RL_S, RL_H, RL_K,  RR_S, RR_H, RR_K]
// S=Shoulder, H=Hip, K=Knee
// ---------------------------------------------------------
float POSE_STAND[NUM_MOTORS] = {110, 130, 70,  110, 100, 40,  90, 100, 160,  110, 80, 60};
float POSE_SIT[NUM_MOTORS]   = {110, 150, 10,  110, 80, 100,  90, 150, 90,  110, 25, 130};
float POSE_CUSTOM[NUM_MOTORS]= {60, 90, 60,   110, 100, 55,  90, 100, 150,   110, 80, 80}; 

// ---------------------------------------------------------
// Helper Functions
// ---------------------------------------------------------
uint16_t angleToPulse(int angle) {
  angle = constrain(angle, 0, 180);
  return map(angle, 0, 180, SERVO_MIN, SERVO_MAX);
}

// Function to map our 0-11 index to the correct PCA board and channel
void updateServo(int index, float angle) {
  uint16_t pulse = angleToPulse((int)angle);
  
  // PCA 1 (Front)
  if (index == 0) pca1.setPWM(13, 0, pulse);      // Front Left Shoulder (Left = 13,14,15)
  else if (index == 1) pca1.setPWM(14, 0, pulse); // Front Left Hip
  else if (index == 2) pca1.setPWM(15, 0, pulse); // Front Left Knee
  else if (index == 3) pca1.setPWM(2, 0, pulse);  // Front Right Shoulder (Right = 0,1,2)
  else if (index == 4) pca1.setPWM(1, 0, pulse);  // Front Right Hip
  else if (index == 5) pca1.setPWM(0, 0, pulse);  // Front Right Knee
  
  // PCA 2 (Rear)
  else if (index == 6) pca2.setPWM(13, 0, pulse);      // Rear Left Shoulder
  else if (index == 7) pca2.setPWM(14, 0, pulse); // Rear Left Hip
  else if (index == 8) pca2.setPWM(15, 0, pulse); // Rear Left Knee
  else if (index == 9) pca2.setPWM(2, 0, pulse);  // Rear Right Shoulder
  else if (index == 10) pca2.setPWM(1, 0, pulse);  // Rear Right Hip
  else if (index == 11) pca2.setPWM(0, 0, pulse);  // Rear Right Knee
}

void setTargetPose(float poseArray[]) {
  for (int i = 0; i < NUM_MOTORS; i++) {
    targetAngles[i] = poseArray[i];
  }
}

// ---------------------------------------------------------
// Setup
// ---------------------------------------------------------
void setup() {
  Serial.begin(115200);
  
  // Initialize I2C Bus 1 (PCA1 - Front) -> SDA: 21, SCL: 19
  I2C_1.begin(21, 19, 100000);
  
  // Initialize I2C Bus 2 (PCA2 - Rear) -> SDA: 23, SCL: 22
  I2C_2.begin(23, 22, 100000);

  pca1.begin();
  pca1.setPWMFreq(50); 
  
  pca2.begin();
  pca2.setPWMFreq(50);

  // Jump immediately to initial positions
  for (int i = 0; i < NUM_MOTORS; i++) {
    updateServo(i, currentAngles[i]);
  }
  
  Serial.println("Quadruped Dual-PCA Controller Ready!");
  Serial.println("Send commands: 'STAND', 'SIT', or 'POSE'");
}

// ---------------------------------------------------------
// Main Loop
// ---------------------------------------------------------
void loop() {
  
  // 1. Handle incoming Serial Commands
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();
    input.toUpperCase(); // Make case-insensitive
    
    if (input == "STAND") {
      Serial.println("Moving smoothly to STAND pose...");
      setTargetPose(POSE_STAND);
    } 
    else if (input == "SIT") {
      Serial.println("Moving smoothly to SIT pose...");
      setTargetPose(POSE_SIT);
    } 
    else if (input == "POSE") {
      Serial.println("Moving smoothly to CUSTOM pose...");
      setTargetPose(POSE_CUSTOM);
    }
  }

  // 2. Smoothly move current angles towards target angles
  unsigned long currentMillis = millis();
  if (currentMillis - lastUpdate >= updateInterval) {
    lastUpdate = currentMillis;
    
    for (int i = 0; i < NUM_MOTORS; i++) {
      if (abs(targetAngles[i] - currentAngles[i]) > moveSpeed) {
        // Move slightly towards target
        if (targetAngles[i] > currentAngles[i]) {
          currentAngles[i] += moveSpeed;
        } else {
          currentAngles[i] -= moveSpeed;
        }
        updateServo(i, currentAngles[i]);
      } else if (currentAngles[i] != targetAngles[i]) {
        // Snap to exact target if very close
        currentAngles[i] = targetAngles[i];
        updateServo(i, currentAngles[i]);
      }
    }
  }
}