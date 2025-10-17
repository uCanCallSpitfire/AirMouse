#include <Wire.h>
#include <MPU6050_light.h>

MPU6050 mpu(Wire);

// Smooth variables for drift filtering
float smoothX = 0;
float smoothY = 0;

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22); // ESP32 SDA, SCL
  delay(1000);

  Serial.println("Initializing MPU...");
  if (mpu.begin() != 0) {
    Serial.println("MPU initialization failed!");
    while (1); // Stop here if MPU fails
  } else {
    Serial.println("MPU ready!");
  }

  Serial.println("Starting calibration, keep ESP32 still...");
  mpu.calcOffsets(); // Calibration
  Serial.println("Calibration complete.");
}

void loop() {
  mpu.update();

  // Read gyro data
  int moveX = mpu.getGyroY() * 1;   // left/right
  int moveY = -mpu.getGyroX() * 1;  // up/down

  // Moving average filter
  smoothX = (smoothX * 3 + moveX) / 4;
  smoothY = (smoothY * 3 + moveY) / 4;

  // Deadzone
  int deadzone = 5;
  if (abs(smoothX) < deadzone) smoothX = 0;
  if (abs(smoothY) < deadzone) smoothY = 0;

  // Send movement data if there's movement
  if (smoothX != 0 || smoothY != 0) {
    Serial.print("MOVE ");
    Serial.print(int(smoothX));
    Serial.print(" ");
    Serial.println(int(smoothY));
  }

  delay(20); // ~50 FPS
}
