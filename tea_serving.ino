// Define the pins for the relay, IR sensor, and flow sensor
const int relayPin = D7; // Relay connected to D1 (GPIO 5)
const int irSensorPin = D6; // IR sensor connected to GPIO 6
const int flowSensorPin = D3; // Flow sensor connected to D3 (GPIO 0)

// Variables for flow sensor
volatile long pulse = 0;
unsigned long lastTime;
float volume;

void setup() {
  // Initialize the relay pin as an output
  pinMode(relayPin, OUTPUT);
  
  // Start with the relay off
  digitalWrite(relayPin, HIGH); // HIGH means relay is off (for most relays)
  
  // Initialize the IR sensor pin as an input
  pinMode(irSensorPin, INPUT);
  
  // Initialize the flow sensor pin as an input
  pinMode(flowSensorPin, INPUT);
  
  // Attach interrupt for the flow sensor
  attachInterrupt(digitalPinToInterrupt(flowSensorPin), increase, RISING);
  
  // Initialize Serial communication for debugging
  Serial.begin(115200);
  Serial.println("IR Sensor, Relay, and Flow Sensor control started.");
}

void loop() {
  // Read the state of the IR sensor
  int obstacleDetected = digitalRead(irSensorPin);
  
  if (obstacleDetected == LOW) { // LOW means obstacle detected (for most IR sensors)
    // Turn the relay on
    digitalWrite(relayPin, LOW); // LOW means relay is on (for most relays)
    Serial.println("Obstacle detected! Relay ON");
  } else {
    // Turn the relay off
    digitalWrite(relayPin, HIGH); // HIGH means relay is off (for most relays)
    Serial.println("No obstacle. Relay OFF");
  }
  
  // Calculate water flow
  volume = 2.663 * pulse / 1000 * 30; // Adjust the formula as per your sensor's calibration
  if (millis() - lastTime > 2000) { // Update flow rate every 2 seconds
    Serial.print("Flow Rate: ");
    Serial.print(volume);
    Serial.println(" L/m");
    pulse = 0; // Reset pulse count
    lastTime = millis();
  }
  
  // Small delay to avoid rapid toggling
  delay(100);
}

// Interrupt service routine for the flow sensor
ICACHE_RAM_ATTR void increase() {
  pulse++;
}
