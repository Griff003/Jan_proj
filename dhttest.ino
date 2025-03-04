#include "DHT.h"

#define DHTPIN D8 // what pin we're connected to


#define DHTTYPE DHT11 // DHT 11

// Initialize DHT sensor for normal 16mhz Arduino
DHT dht(DHTPIN, DHTTYPE);   

void setup() {
Serial.begin(9600);
Serial.println("DHT test!");

dht.begin();
}

void loop() {
// Wait a few seconds between measurements.
delay(2000);

float h = dht.readHumidity();
// Read temperature as Celsius
float t = dht.readTemperature();


// Check if any reads failed and exit early (to try again).
if (isnan(h) || isnan(t) ) {
Serial.println("Failed to read from DHT sensor!");
return;
}

Serial.print("Humidity: ");
Serial.print(h);
Serial.println(" \t");
Serial.print("Temperature: ");
Serial.print(t);
Serial.print(" *C ");

}
