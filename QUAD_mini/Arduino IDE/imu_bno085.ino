#include <Wire.h>
#include <Adafruit_BNO08x.h>

#define BNO08X_RESET -1

// Create the BNO08x object
Adafruit_BNO08x bno08x(BNO08X_RESET);
sh2_SensorValue_t sensorValue;

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10); // Wait for Serial Monitor to open

  Serial.println("QUADRUPED mini - BNO085 IMU Test");

  // --- I2C PIN CONFIGURATION ---
  // Standard ESP32 pins are usually 21 (SDA) and 22 (SCL).
  // If you wired SDA to 19 and SCL to 21, uncomment the line below:
  Wire.begin(19, 21); 
  
  // Otherwise, start standard I2C:
  Wire.begin(); 

  // Initialize the sensor
  if (!bno08x.begin_I2C()) {
    Serial.println("Failed to find BNO085 chip. Check wiring!");
    while (1) { delay(10); }
  }
  Serial.println("BNO085 Found!");

  // Enable the Rotation Vector report (which gives us our orientation)
  // 50,000 microseconds = 50ms report interval (20Hz update rate)
  if (!bno08x.enableReport(SH2_ROTATION_VECTOR, 50000)) {
    Serial.println("Could not enable rotation vector");
  }
}

// Helper function to convert Quaternions to Euler Angles (Roll, Pitch, Yaw)
void quaternionToEuler(float qr, float qi, float qj, float qk, float& roll, float& pitch, float& yaw) {
  // Roll (x-axis rotation)
  float sinr_cosp = 2 * (qr * qi + qj * qk);
  float cosr_cosp = 1 - 2 * (qi * qi + qj * qj);
  roll = atan2(sinr_cosp, cosr_cosp);

  // Pitch (y-axis rotation)
  float sinp = 2 * (qr * qj - qk * qi);
  if (abs(sinp) >= 1)
    pitch = copysign(M_PI / 2, sinp); // use 90 degrees if out of range
  else
    pitch = asin(sinp);

  // Yaw (z-axis rotation)
  float siny_cosp = 2 * (qr * qk + qi * qj);
  float cosy_cosp = 1 - 2 * (qj * qj + qk * qk);
  yaw = atan2(siny_cosp, cosy_cosp);

  // Convert radians to degrees
  roll = roll * 180.0 / M_PI;
  pitch = pitch * 180.0 / M_PI;
  yaw = yaw * 180.0 / M_PI;
}

void loop() {
  delay(10);
  
  if (bno08x.wasReset()) {
    Serial.print("Sensor was reset, re-enabling reports...");
    bno08x.enableReport(SH2_ROTATION_VECTOR, 50000);
  }

  // Check if there is new data from the sensor
  if (!bno08x.getSensorEvent(&sensorValue)) {
    return;
  }

  if (sensorValue.sensorId == SH2_ROTATION_VECTOR) {
    float r = sensorValue.un.rotationVector.real;
    float i = sensorValue.un.rotationVector.i;
    float j = sensorValue.un.rotationVector.j;
    float k = sensorValue.un.rotationVector.k;

    float roll, pitch, yaw;
    quaternionToEuler(r, i, j, k, roll, pitch, yaw);

    Serial.print("Roll (Tilt L/R): "); Serial.print(roll);
    Serial.print("\tPitch (Tilt F/B): "); Serial.print(pitch);
    Serial.print("\tYaw (Rotation): "); Serial.println(yaw);
  }
}