// Include the Servo library
#include <Servo.h>

// Define the pin connections for IR sensor and servo
const int irSensorPin = 2;  // IR sensor output pin connected to digital pin 2
const int servoPin = 5;     // Servo signal pin connected to digital pin 4

// Define the pin connections for actuator and ultrasonic sensor
const int ENA_PIN = 6;  // the Arduino pin connected to the EN1 pin L298N
const int IN1_PIN = 7;  // the Arduino pin connected to the IN1 pin L298N
const int IN2_PIN = 8;  // the Arduino pin connected to the IN2 pin L298N
const int trigPin = 9;  // Pin connected to the trigger pin of the ultrasonic sensor
const int echoPin = 10; // Pin connected to the echo pin of the ultrasonic sensor

// Define the pin connection for the relay
const int relayPin = 3; // Relay connected to D3 (GPIO 3)

// Variables for ultrasonic sensor
long duration;
int distance;

// Create the servo object
Servo myservo;

void setup() {
  // Set IR sensor pin as input
  pinMode(irSensorPin, INPUT);

  // Attach the servo to the servo object
  myservo.attach(servoPin);

  // Initialize the servo to the closed position (90 degrees)
  myservo.write(90);

  // Initialize actuator pins as outputs
  pinMode(ENA_PIN, OUTPUT);
  pinMode(IN1_PIN, OUTPUT);
  pinMode(IN2_PIN, OUTPUT);
  digitalWrite(ENA_PIN, HIGH); // Enable the actuator

  // Initialize ultrasonic sensor pins
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Initialize relay pin as output
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW); // Ensure relay is initially off

  // Begin serial communication for debugging
  Serial.begin(9600);
}

void loop() {
  // Read the value from the IR sensor
  int sensorValue = digitalRead(irSensorPin);

  if (sensorValue == HIGH) {
    // Obstacle detected
    Serial.println("Obstacle detected!");

    // Open the servo (e.g., move to 80 degrees)
    myservo.write(80);
    delay(2000);  // Keep the servo open for 2 seconds

    myservo.write(100);
    delay(1000);

    // Close the servo (e.g., move back to 90 degrees)
    myservo.write(90);
    delay(1000);  // Small delay for stability
  } else {
    // No obstacle
    Serial.println("No obstacle.");
  }

  // Measure distance using the ultrasonic sensor
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2; // Calculate distance in cm

  // Print distance to Serial Monitor
  Serial.print("Distance: ");
  Serial.println(distance);
  delay(500);

  // Check if distance is less than 13 cm
  if (distance < 13) {
    // Extend the actuator for 4 seconds
    digitalWrite(IN1_PIN, HIGH);
    digitalWrite(IN2_PIN, LOW);
    Serial.println("Actuator Extending...");
    delay(4000); // Extend for 4 seconds

    // Stop the actuator for 4 seconds
    digitalWrite(IN1_PIN, LOW);
    digitalWrite(IN2_PIN, LOW);
    Serial.println("Actuator Stopped");

    // Turn on the relay for 2 seconds during the stop period
    digitalWrite(relayPin, HIGH); // Turn relay ON
    Serial.println("Relay ON");
    delay(2000); // Keep relay ON for 2 seconds

    // Turn off the relay
    digitalWrite(relayPin, LOW); // Turn relay OFF
    Serial.println("Relay OFF");
    delay(500);

    // Retract the actuator for 4 seconds
    digitalWrite(IN1_PIN, LOW);
    digitalWrite(IN2_PIN, HIGH);
    Serial.println("Actuator Retracting...");
    delay(8000); // Retract for 8 seconds

    // Stop the actuator
    digitalWrite(IN1_PIN, LOW);
    digitalWrite(IN2_PIN, LOW);
    Serial.println("Actuator Stopped");
    delay(500); 
  }

  delay(1000);  // Small delay for stability
}
