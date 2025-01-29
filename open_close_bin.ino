#include <Servo.h>

Servo myServo; // Create a Servo object

const int servoPin = D1; // Pin connected to the servo control
int currentAngle = 0;    // Current servo position
int step = 1;            // Angle step for smooth movement

void setup() {
  myServo.attach(servoPin);  // Attach the servo to the pin
  myServo.write(currentAngle); // Start at 0 degrees
  delay(1000);               // Delay to ensure servo initializes
}

void loop() {
  // Move clockwise for 3 seconds
  unsigned long startTime = millis(); 
  while (millis() - startTime < 3000) {
    currentAngle += step;    // Increase the angle
    if (currentAngle > 180)  // Limit the angle to 180 degrees
      currentAngle = 180;

    myServo.write(currentAngle); // Update servo position
    delay(20);                   // Small delay for smooth movement
  }

  // Move anticlockwise for 3 seconds
  startTime = millis(); 
  while (millis() - startTime < 3000) {
    currentAngle -= step;    // Decrease the angle
    if (currentAngle < 0)    // Limit the angle to 0 degrees
      currentAngle = 0;

    myServo.write(currentAngle); // Update servo position
    delay(20);                   // Small delay for smooth movement
  }
}
