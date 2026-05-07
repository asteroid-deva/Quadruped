#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <Adafruit_BNO08x.h>

#define SERVOMIN  150 
#define SERVOMAX  600 

// --- DUAL I2C BUS SETUP ---
TwoWire I2C_IMU = TwoWire(0);
TwoWire I2C_PCA = TwoWire(1);

// Initialize Driver and IMU on their respective buses
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40, I2C_PCA);
Adafruit_BNO08x bno08x(-1);
sh2_SensorValue_t sensorValue;

// --- CHANNEL DEFINITIONS ---
const int RR_Shoulder = 0;
const int RR_Hip = 1;
const int FR_Shoulder = 4;
const int FR_Hip = 5;
const int RL_Shoulder = 8;
const int RL_Hip = 9;
const int FL_Shoulder = 12;
const int FL_Hip = 13;

// State tracking
bool isLevelingActive = false;

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);
  
  // Start Bus 0 (IMU) on 19/21 and Bus 1 (PCA) on 22/23
  I2C_IMU.begin(19, 21);
  I2C_PCA.begin(22, 23);

  // Initialize PCA9685
  pwm.begin();
  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(50);  

  // Initialize BNO085
  if (!bno08x.begin_I2C(BNO08x_I2CADDR_DEFAULT, &I2C_IMU)) {
    Serial.println("Failed to find BNO085 chip. Check wiring!");
    while (1) { delay(10); }
  }
  bno08x.enableReport(SH2_ROTATION_VECTOR, 50000); // 20Hz updates

  Serial.println("=======================================");
  Serial.println("  QUADRUPED mini - Active IMU Leveling");
  Serial.println("=======================================");
  Serial.println("Type 'STAND', 'SIT', or 'LEVEL' and press Enter.");
  
  commandStand();
}

// --- QUATERNION TO EULER HELPER ---
void quaternionToEuler(float qr, float qi, float qj, float qk, float& roll, float& pitch, float& yaw) {
  float sinr_cosp = 2 * (qr * qi + qj * qk);
  float cosr_cosp = 1 - 2 * (qi * qi + qj * qj);
  roll = atan2(sinr_cosp, cosr_cosp) * 180.0 / M_PI;

  float sinp = 2 * (qr * qj - qk * qi);
  if (abs(sinp) >= 1) pitch = copysign(90.0, sinp); 
  else pitch = asin(sinp) * 180.0 / M_PI;

  float siny_cosp = 2 * (qr * qk + qi * qj);
  float cosy_cosp = 1 - 2 * (qj * qj + qk * qk);
  yaw = atan2(siny_cosp, cosy_cosp) * 180.0 / M_PI;
}

void loop() {
  // 1. Handle Serial Commands
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim(); 
    command.toUpperCase(); 

    if (command == "STAND") {
      isLevelingActive = false;
      commandStand();
    } 
    else if (command == "SIT") {
      isLevelingActive = false;
      commandSit(); // Assuming your previous commandSit() logic here
    } 
    else if (command == "LEVEL") {
      Serial.println("Active Leveling: ON. Tilt the robot to see it react.");
      isLevelingActive = true;
    }
  }

  // 2. Handle IMU Updates (if active)
  if (isLevelingActive && bno08x.getSensorEvent(&sensorValue)) {
    if (sensorValue.sensorId == SH2_ROTATION_VECTOR) {
      float r = sensorValue.un.rotationVector.real;
      float i = sensorValue.un.rotationVector.i;
      float j = sensorValue.un.rotationVector.j;
      float k = sensorValue.un.rotationVector.k;

      float roll, pitch, yaw;
      quaternionToEuler(r, i, j, k, roll, pitch, yaw);
      
      dynamicLeveling(pitch, roll);
    }
  }
}

// --- HELPER TO MOVE SERVO ---
void moveServo(int channel, float angle) {
  angle = constrain(angle, 0, 180);
  int pulseLength = map(angle, 0, 180, SERVOMIN, SERVOMAX);
  pwm.setPWM(channel, 0, pulseLength);
}

// --- STATIC STAND ---
void commandStand() {
  moveServo(RR_Shoulder, 60); moveServo(FR_Shoulder, 45);
  moveServo(RL_Shoulder, 40); moveServo(FL_Shoulder, 55);
  moveServo(RR_Hip, 10);      moveServo(FR_Hip, 5);
  moveServo(RL_Hip, 110);     moveServo(FL_Hip, 85);
}

// --- DYNAMIC LEVELING ---
void dynamicLeveling(float pitch, float roll) {
  // Sensitivity Multiplier (Tune this! 0.5 means 10 degrees of tilt = 5 degrees of servo movement)
  float sensitivity = 0.5; 
  
  float pitchAdjust = pitch * sensitivity;
  float rollAdjust = roll * sensitivity;

  // Base standing shoulder angles
  float rr_sh = 60; float fr_sh = 45;
  float rl_sh = 40; float fl_sh = 55;

  // Combine Pitch and Roll adjustments for each leg.
  // Note: Depending on how your IMU is glued down, you may need to change 
  // these '+' to '-' if a leg moves the wrong way when tilted.
  
  rr_sh += (pitchAdjust - rollAdjust); 
  fr_sh += (-pitchAdjust - rollAdjust); 
  rl_sh += (-pitchAdjust + rollAdjust); 
  fl_sh += (pitchAdjust + rollAdjust); 

  // Apply the corrected angles to the shoulders (hips remain at standing base)
  moveServo(RR_Shoulder, rr_sh);
  moveServo(FR_Shoulder, fr_sh);
  moveServo(RL_Shoulder, rl_sh);
  moveServo(FL_Shoulder, fl_sh);
  
  // Keep hips locked
  moveServo(RR_Hip, 10); moveServo(FR_Hip, 5);
  moveServo(RL_Hip, 110); moveServo(FL_Hip, 85);
}

// Include your previous commandSit() and commandWalk() here as well...
void commandSit() {
  // Included from your earlier code...
}