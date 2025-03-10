// Include necessary libraries
#include <Servo.h>
#include <Stepper.h>

// Pin definitions
const int irSensorPin = 2;
const int servoPin = 5;
const int ENA_PIN = 7;
const int IN1_PIN = 6;
const int IN2_PIN = 8;
const int trigPin = 9;
const int echoPin = 10;
const int relayPin = 3;
const int servo1Pin = 22;
const int servo2Pin = 24;
const int servo3Pin = 26;

// Stepper motor pins and settings
const int stepsPerRevolution = 200;
Stepper myStepper(stepsPerRevolution, 4, 11, 12, 13);

// Ultrasonic sensor variables
long duration;
int distance;

// Servo objects
Servo myservo;
Servo servo1;
Servo servo2;
Servo servo3;

// Flags to control stepper motor
bool stepperActive = false;
bool actuatorRunning = false;
bool stepperCompleted = false; // New flag to ensure stepper runs only once

void setup() {
  pinMode(irSensorPin, INPUT);
  myservo.attach(servoPin);
  myservo.write(90);
  servo1.attach(servo1Pin);
  servo2.attach(servo2Pin);
  servo3.attach(servo3Pin);
  servo1.write(90);
  servo2.write(90);
  servo3.write(80);
  pinMode(ENA_PIN, OUTPUT);
  pinMode(IN1_PIN, OUTPUT);
  pinMode(IN2_PIN, OUTPUT);
  digitalWrite(ENA_PIN, HIGH);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW);
  Serial.begin(9600);
  myStepper.setSpeed(30);
}

void loop() {
  int sensorValue = digitalRead(irSensorPin);

  if (sensorValue == HIGH) {
    Serial.println("Obstacle detected!");
    myservo.write(80);
    delay(3000);
    myservo.write(100);
    delay(1000);
    myservo.write(90);
    delay(1000);
    stepperActive = true;
    stepperCompleted = false; // Reset flag for next cycle
  }

  // Ultrasonic functionality
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;
  Serial.print("Distance: ");
  Serial.println(distance);

  if (distance < 16) {

    servo2.write(180);
    servo1.write(180);
    servo3.write(10);
    delay(1000);
    stepperActive = false;
    actuatorRunning = true;

    

    analogWrite(ENA_PIN, 255); // Controls the speed
    digitalWrite(IN1_PIN, HIGH);
    digitalWrite(IN2_PIN, LOW);
    delay(4000);

    digitalWrite(IN1_PIN, LOW);
    digitalWrite(IN2_PIN, LOW);


    digitalWrite(relayPin, HIGH);
    delay(2000);
    digitalWrite(relayPin, LOW);
    delay(500);

    //locking the animal functionality

    servo1.write(40);
    servo2.write(40);
    delay(1000);
    servo3.write(170);
    delay(100);

    digitalWrite(IN1_PIN, LOW);
    digitalWrite(IN2_PIN, HIGH);
    delay(8000);

    digitalWrite(IN1_PIN, LOW);
    digitalWrite(IN2_PIN, LOW);
    delay(500);
    servo3.write(180);
    delay(200);

    actuatorRunning = false;
  }

  if (stepperActive && !actuatorRunning && !stepperCompleted) {
    Serial.println("Stepper clockwise");
    myStepper.step(stepsPerRevolution / 3);
    delay(500);
    Serial.println("Stepper counterclockwise");
    myStepper.step(-(stepsPerRevolution / 3));
    delay(500);
    stepperCompleted = true; // Ensure stepper runs only once
  }

  delay(1000);
}
