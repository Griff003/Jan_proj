#include <Servo.h>
#include <ESP8266WiFi.h>      // Use WiFi.h for ESP32
#include <PubSubClient.h>

Servo myservo;  // Create servo object

// --- Define hardware parameters ---
#define CLOSE 10
#define OPEN 181
#define SERVO_PIN D5
#define LIGHT A0
#define MIN_LIGHT 100   // Note: no semicolon here!
#include <DHT.h>
#define DHTPin D6
#define DHTTYPE DHT11 
DHT dht(DHTPin, DHTTYPE);      


#define RAIN_PIN D8

int currentPos = CLOSE;  // Start at closed position
int lightLevel;
int mode = 0;            // 0 = Automatic, 1 = Manual

// --- WiFi and MQTT Settings ---
const char* ssid = "Amos";
const char* password = "0715228424";
const char* mqtt_server = "broker.emqx.io";

WiFiClient espClient;
PubSubClient client(espClient);

// --- MQTT Callback ---
// This function is called whenever a subscribed message is received.
void callback(char* topic, byte* payload, unsigned int length) {
  char message[length + 1];
  memcpy(message, payload, length);
  message[length] = '\0';

  Serial.print("Message arrived on topic: ");
  Serial.println(topic);
  Serial.print("Message: ");
  Serial.println(message);

  // Check for mode update
  if (strcmp(topic, "/tinka/kenyatta/smartwindow/mode") == 0) {
    int newMode;
    // Expecting JSON formatted as: {"mode":0} or {"mode":1}
    if (sscanf(message, "{\"mode\":%d}", &newMode) == 1) {
      mode = newMode;
      Serial.print("Mode updated to: ");
      Serial.println(mode);
    }
  }
  // Check for manual status update
  else if (strcmp(topic, "/tinka/kenyatta/smartwindow/status") == 0) {
    int status;
    // Expecting JSON formatted as: {"status":0} or {"status":1}
    if (sscanf(message, "{\"status\":%d}", &status) == 1) {
      Serial.print("Manual status received: ");
      Serial.println(status);
      // Only process manual commands if manual mode is active
      if (mode == 1) {
        if (status == 0) {
          closeWindow();
        } else if (status == 1) {
          openWindow();
        }
      }
    }
  }
}

// --- Connect to WiFi ---
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}
float getTemperature() {
  float Temperature = dht.readTemperature(); // Gets the temperature value

  if (isnan(Temperature)) { // Check if reading failed
    Serial.println("Temperature reading failed!");
    return 0; // Return 0 if no valid temperature
  }

  Serial.print("Temperature = ");
  Serial.println(Temperature);
  
  return Temperature;
}


// --- Reconnect to MQTT if necessary ---
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println(" connected");
      // Subscribe to the topics for mode and manual status updates
      client.subscribe("/tinka/kenyatta/smartwindow/mode");
      client.subscribe("/tinka/kenyatta/smartwindow/status");
    } else {
      Serial.print(" failed, rc=");
      Serial.print(client.state());
      Serial.println(" trying again in 5 seconds");
      delay(5000);
    }
  }
}

// --- Publish light and window status ---
// Publishes the current light level and window position as JSON to "/tinka/kenyatta/smartwindow"
void publishStatus(int lightLevel, int windowPos, float temperature) {
  char payload[150];
  int rain=digitalRead(RAIN_PIN);
  snprintf(payload, sizeof(payload), "{\"light\": %d, \"position\": %d, \"mode\": %d, \"temp\":%f, \"rain\":%d}", lightLevel, windowPos-CLOSE, mode, temperature, rain);
  client.publish("/tinka/kenyatta/smartwindow", payload);

}

// --- Servo Functions ---
void closeWindow() {
  for (int pos = currentPos; pos > CLOSE; pos -= 3) {
    myservo.write(pos);
    currentPos = pos;
float temperature=getTemperature();
    publishStatus(lightLevel, currentPos, temperature);
    delay(100);
  }
}

void openWindow() {
  for (int pos = currentPos; pos < OPEN; pos += 3) {
    myservo.write(pos);
    currentPos = pos;
    float temperature=getTemperature();
    publishStatus(lightLevel, currentPos, temperature);
    delay(100);
  }
}

void setup() {
  // Setup servo and serial monitor
  myservo.attach(SERVO_PIN);
  myservo.write(CLOSE);
  delay(1000);
  Serial.begin(9600);
  dht.begin(); 

  // Setup WiFi and MQTT connection
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  // Ensure the MQTT connection is active
  if (!client.connected()) {
    reconnect();
  }
  client.loop();  // Process incoming MQTT messages

  // Read light sensor value and map it (here, mapping 0-1023 to 1023-0)
  int analogValue = analogRead(LIGHT);
  lightLevel = map(analogValue, 0, 1023, 1023, 0);
  int rain=digitalRead(RAIN_PIN);
  float temperature=getTemperature();

  // Automatic mode: control window based on light level only if mode == 0
  if (mode == 0) {
    if (lightLevel > MIN_LIGHT&&rain==0&& temperature>25) {
      openWindow();
    }
    
     else {
      closeWindow();
    }
  }

  // Debug output
  Serial.print("Light: ");
  Serial.println(lightLevel);
  Serial.print("Window position: ");
  Serial.println(currentPos);

  // Publish current status
  publishStatus(lightLevel, currentPos, temperature);

  delay(100);
}
