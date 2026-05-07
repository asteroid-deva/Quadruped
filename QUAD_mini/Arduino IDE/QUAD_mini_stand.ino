#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// Create the PWM driver object
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

#define SERVOMIN  150 // Minimum pulse length count (0 degrees)
#define SERVOMAX  600 // Maximum pulse length count (180 degrees)

// I2C Pins for the Driver
#define PCA_SDA 22
#define PCA_SCL 23

// --- CHANNEL DEFINITIONS ---
// Based on your mapping [cite: 1]
const int RR_Shoulder = 0;
const int RR_Hip = 1;
const int FR_Shoulder = 4;
const int FR_Hip = 5;
const int RL_Shoulder = 8;
const int RL_Hip = 9;
const int FL_Shoulder = 12;
const int FL_Hip = 13;

void setup() {
  Serial.begin(115200);
  
  // Start I2C on your chosen pins
  Wire.begin(PCA_SDA, PCA_SCL);

  // Initialize the PCA9685
  pwm.begin();
  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(50);  

  delay(1000); 

  Serial.println("=======================================");
  Serial.println("    QUADRUPED mini - Pose Control");
  Serial.println("=======================================");
  Serial.println("Type 'STAND' or 'SIT' and press Enter.");
  
  // Start in the standing position by default
  commandStand();
}

void loop() {
  // Check if you have typed a command into the Serial Monitor
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim(); // Remove any extra spaces or hidden characters
    command.toUpperCase(); // Make it all uppercase so "sit", "Sit", and "SIT" all work

    if (command == "STAND") {
      Serial.println("Executing: STAND");
      commandStand();
    } 
    else if (command == "SIT") {
      Serial.println("Executing: SIT");
      commandSit();
    } 
    else if (command.length() > 0) {
      Serial.println("Unknown command. Please type 'STAND' or 'SIT'.");
    }
  }
}

// --- HELPER FUNCTION TO MOVE SERVO ---
void moveServo(int channel, int angle) {
  // Constrain angle between 0 and 180 to protect the motors
  angle = constrain(angle, 0, 180);
  int pulseLength = map(angle, 0, 180, SERVOMIN, SERVOMAX);
  pwm.setPWM(channel, 0, pulseLength);
}

// --- POSE: STAND ---
void commandStand() {
  // Shoulders
  moveServo(RR_Shoulder, 60);
  moveServo(FR_Shoulder, 45);
  moveServo(RL_Shoulder, 40);
  moveServo(FL_Shoulder, 55);

  // Hips (Your baseline standing angles)
  moveServo(RR_Hip, 10);
  moveServo(FR_Hip, 5);
  moveServo(RL_Hip, 110);
  moveServo(FL_Hip, 85);
}

// --- POSE: SIT ---
void commandSit() {
  // Shoulders remain the same as standing so the legs don't swing inward/outward
  moveServo(RR_Shoulder, 60);
  moveServo(FR_Shoulder, 50);
  moveServo(RL_Shoulder, 40);
  moveServo(FL_Shoulder, 60);

  // Hips: Right side +50°, Left side -50° based on your logic
  moveServo(RR_Hip, 10 + 50);   // 60°
  moveServo(FR_Hip, 10 + 50);   // 60°
  moveServo(RL_Hip, 110 - 50);  // 60°
  moveServo(FL_Hip, 90 - 50);   // 40°
}