#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// Create the PWM driver object
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

// Servo pulse width parameters 
#define SERVOMIN  150 // Minimum pulse length count (0 degrees)
#define SERVOMAX  600 // Maximum pulse length count (180 degrees)

// I2C Pins for the Driver
#define PCA_SDA 22
#define PCA_SCL 23

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
  Serial.println("  QUADRUPED mini - Live Tuning Mode");
  Serial.println("=======================================");
  Serial.println("Type an angle (0 to 180) and hit Enter.");
  Serial.println("Testing Channel 0 (Rear Right)...");
}

void loop() {
  // Check if you have typed something into the Serial Monitor
  if (Serial.available() > 0) {
    
    // Read the number you typed
    int targetAngle = Serial.parseInt();
    
    // Validate the angle is within safe servo limits (0 to 180 degrees)
    if (targetAngle >= 0 && targetAngle <= 180) {
      
      // Convert the 0-180 angle into the 150-600 PWM pulse length
      int pulseLength = map(targetAngle, 0, 180, SERVOMIN, SERVOMAX);
      
      // Move Channel 0 to the calculated pulse length
      pwm.setPWM(0, 0, pulseLength);
      
      // Print the results back to you
      Serial.print("Success! Moved to Angle: "); 
      Serial.print(targetAngle);
      Serial.print("°  |  Raw PWM Value: ");
      Serial.println(pulseLength);
      
    } else if (targetAngle != 0) { // Ignores random zero readings from line endings
      Serial.println("Please enter a valid angle between 0 and 180.");
    }
    
    // Clear out any remaining invisible characters (like pressing Enter) from the buffer
    while(Serial.available() > 0) {
      Serial.read();
    }
  }
}