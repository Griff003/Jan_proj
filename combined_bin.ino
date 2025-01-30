#include <SoftwareSerial.h>
#include "HX711.h"

#define TRIG_PIN D8
#define ECHO_PIN D7
#define GSM_TX D5  // GSM TX to NodeMCU D7 (GPIO13)
#define GSM_RX D6  // GSM RX to NodeMCU D8 (GPIO15)
#define LOADCELL_DOUT_PIN D2
#define LOADCELL_SCK_PIN D3

#define CALIBRATION_FACTOR 705.0  // Adjust this based on calibration

SoftwareSerial gsmSerial(GSM_TX, GSM_RX); // SoftwareSerial for GSM
HX711 scale;

void setup() {
  Serial.begin(9600);
  gsmSerial.begin(9600); // GSM module communication

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Initialize Load Cell
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(CALIBRATION_FACTOR);
  scale.tare(); // Reset scale to 0

  Serial.println("Initializing GSM module...");
  delay(2000);

  gsmSerial.println("AT"); // Test GSM module
  delay(1000);
}

void loop() {
  float distance = measureDistance();
  float weight = measureWeight();

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  Serial.print("Weight: ");
  Serial.print(weight);
  Serial.println(" g");

  if (distance <= 10) { // If distance is less than or equal to 10cm
    Serial.println("Distance below 10cm! Sending SMS...");
    sendSMS("Warning! Object detected within 10cm.");
    delay(60000); // Wait 1 minute before rechecking
  }

  if (weight > 500) { // If weight exceeds 500g
    Serial.println("Weight above 500g! Sending SMS...");
    sendSMS("Warning! Load detected above 500g.");
    delay(60000); // Wait 1 minute before rechecking
  }

  delay(500);
}

float measureDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH);
  float distance = (duration * 0.0343) / 2; // Convert time to distance

  return distance;
}

float measureWeight() {
  return scale.get_units(); // Get weight in grams
}

void sendSMS(String message) {
  gsmSerial.println("AT+CMGF=1"); // Set SMS mode
  delay(1000);
  gsmSerial.print("AT+CMGS=\"0740390420\"\r"); // Replace with actual number
  delay(1000);
  gsmSerial.print(message);
  delay(100);
  gsmSerial.write(26); // Send SMS (Ctrl+Z)
  delay(5000);
}
