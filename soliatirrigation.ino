#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <SoftwareSerial.h>
#include "DHT.h" 

#define DHTPIN D7       // DHT11 Data Pin
#define DHTTYPE DHT11   // DHT Sensor Type
#define RELAY_PIN D2    // Relay for controlling device
#define MOISTURE_PIN A0 // Analog pin for Soil Moisture Sensor
#define LReceiver D3    // Obstacle sensor (Bird detection)
#define Laser D6        // Laser Module
#define Buzzer D8       // Buzzer
#define PIR D1          // PIR Motion Sensor

#define GSM_TX D4  // GSM module TX to NodeMCU D4
#define GSM_RX D5  // GSM module RX to NodeMCU D5

DHT dht(DHTPIN, DHTTYPE);
ESP8266WebServer server(80);
SoftwareSerial gsmSerial(GSM_TX, GSM_RX);

/* WiFi Credentials */
const char* ssid = "Soliat";        
const char* password = "123456789"; 

/* Sensor Readings */
float temperature;
float humidity;
int moisture_percentage;
String relay_status;
String obstacle_status;
String intruder_status;

void setup() {
  Serial.begin(9600);
  gsmSerial.begin(9600);
  dht.begin();
  
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(LReceiver, INPUT);
  pinMode(Laser, OUTPUT);
  pinMode(Buzzer, OUTPUT);
  pinMode(PIR, INPUT);
  
  digitalWrite(Laser, HIGH);  // Turn on laser module
  digitalWrite(Buzzer, LOW);   // Ensure buzzer is off initially

  WiFi.softAP(ssid, password);
  Serial.print("Access Point IP: ");
  Serial.println(WiFi.softAPIP());

  server.on("/", handle_OnConnect);
  server.onNotFound(handle_NotFound);
  server.begin();
  Serial.println("HTTP Server Started");

  Serial.println("Initializing GSM module...");
  delay(2000);
  gsmSerial.println("AT");
}

void loop() {
  server.handleClient();
  
  // Read DHT11 Sensor Data
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();
  
  // Read Soil Moisture Data (Convert Analog Value to Percentage)
  int moisture_value = analogRead(MOISTURE_PIN);
  moisture_percentage = map(moisture_value, 0, 1023, 0, 100); 

  // Control Relay Based on Moisture Level
  if (moisture_percentage < 20) {
    if (relay_status != "ON") { // Prevent sending multiple messages for same state
      sendSMS("The pump is turned ON");
    }
    digitalWrite(RELAY_PIN, HIGH);
    relay_status = "ON";
  } else {
    digitalWrite(RELAY_PIN, LOW);
    relay_status = "OFF";
  }

  // Read Obstacle Detection Sensor (Bird Detection)
  bool obstacle = digitalRead(LReceiver);
  if (obstacle == 0) {
    obstacle_status = "No Birds Detected";
    digitalWrite(Buzzer, LOW);
  } else {
    if (obstacle_status != "Birds Detected in the Farm") {
      sendSMS("Birds Detected in the Farm");
    }
    obstacle_status = "Birds Detected in the Farm";
    digitalWrite(Buzzer, HIGH);
  }

  // Read PIR Motion Sensor (Intruder Detection)
  bool intruder = digitalRead(PIR);
  if (intruder == HIGH) {
    if (intruder_status != "⚠️ Intruder Detected!") {
      sendSMS(" Intruder Detected in the Farm!");
    }
    intruder_status = "⚠️ Intruder Detected!";
    digitalWrite(Buzzer, HIGH);
  } else {
    intruder_status = "✅ No Intruder Detected";
    digitalWrite(Buzzer, LOW);
  }

  // Print Sensor Data to Serial Monitor
  Serial.print("Moisture: "); Serial.print(moisture_percentage); Serial.println("%");
  Serial.print("Relay: "); Serial.println(relay_status);
  Serial.print("Obstacle: "); Serial.println(obstacle_status);
  Serial.print("Temperature: "); Serial.print(temperature); Serial.println("°C");
  Serial.print("Humidity: "); Serial.print(humidity); Serial.println("%");
  Serial.print("Intruder: "); Serial.println(intruder_status);
  Serial.println("----------------------------");

  delay(2000); // Wait before next read
}

/* Web Server Handlers */
void handle_OnConnect() {
  server.send(200, "text/html", generateWebpage()); 
}

void handle_NotFound() {
  server.send(404, "text/plain", "Not found");
}

/* Generate Styled Webpage */
String generateWebpage() {
  String ptr = "<!DOCTYPE html><html>";
  ptr += "<head><meta charset='UTF-8'>";
  ptr += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  ptr += "<title>Soliat Highschool Smart Farm</title>";
  ptr += "<style>";
  ptr += "body { font-family: Arial, sans-serif; text-align: center; background-color: #282c36; color: white; margin: 0; padding: 0; }";
  ptr += "h2 { color: #FFD700; margin-top: 20px; }";
  ptr += ".container { max-width: 600px; margin: auto; padding: 20px; border-radius: 10px; background: #3b3f4a; box-shadow: 0px 0px 10px #000; }";
  ptr += ".box { padding: 15px; margin: 10px; border-radius: 5px; background: #555; font-size: 20px; font-weight: bold; }";
  ptr += ".on { color: #0f0; } .off { color: #f00; }";
  ptr += "</style></head>";
  ptr += "<body>";
  ptr += "<div class='container'>";
  ptr += "<h2>Soliat Highschool Smart Farm</h2>";
  
  ptr += "<div class='box'>🌱 Soil Moisture: " + String(moisture_percentage) + " %</div>";
 ptr += "<div class='box'>⚡ Pump Status: <span class='" + String(relay_status == "ON" ? "on" : "off") + "'>" + relay_status + "</span></div>";
  ptr += "<div class='box'>🚧 Birds Detection: " + obstacle_status + "</div>";
  ptr += "<div class='box'>🌡️ Temperature: " + String(temperature) + " °C</div>";
  ptr += "<div class='box'>💧 Humidity: " + String(humidity) + " %</div>";
  ptr += "<div class='box'>🕵️ Intruder Status: " + intruder_status + "</div>";

  ptr += "</div></body></html>";
  
  return ptr;
}

/* Function to Send SMS */
void sendSMS(String message) {
  Serial.println("Sending SMS: " + message);
  
  gsmSerial.println("AT+CMGF=1"); // Set SMS mode
  delay(1000);
  
  gsmSerial.print("AT+CMGS=\"0729504083\"\r"); // Replace with recipient number
  delay(1000);
  
  gsmSerial.print(message); // Message content
  delay(100);
  
  gsmSerial.write(26); // ASCII code for Ctrl+Z to send
  delay(2000);
}
