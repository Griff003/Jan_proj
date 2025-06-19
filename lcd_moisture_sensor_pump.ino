#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define PUMP_PIN D5          // Pin connected to pump/transistor
#define MOISTURE_PIN A0      // Analog pin for moisture sensor

// LCD setup: I2C address 0x27, 16x2 display
LiquidCrystal_I2C lcd(0x27, 16, 2);

bool pumpStatus = false;     // Track pump state

void setup() {
  Serial.begin(9600);

  // Setup pins
  pinMode(PUMP_PIN, OUTPUT);
  pinMode(MOISTURE_PIN, INPUT);

  // LCD initialization
  Wire.begin(D2, D1);  // Adjust if you're using different I2C pins
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Soil Monitor");
  delay(2000);
  lcd.clear();
}

void loop() {
  // Read and map the moisture level
  int moistureLevel = map(analogRead(MOISTURE_PIN), 0, 1023, 1000, 0);
  Serial.print("Moisture Level: ");
  Serial.println(moistureLevel);

  // Pump control logic
  if (moistureLevel > 80) {
    digitalWrite(PUMP_PIN, LOW);  // OFF
    pumpStatus = false;
    Serial.println("Pump OFF");
  } else if (moistureLevel < 70) {
    digitalWrite(PUMP_PIN, HIGH); // ON
    pumpStatus = true;
    Serial.println("Pump ON");
  }

  // Update LCD
  lcd.setCursor(0, 0);
  lcd.print("Moisture: ");
  lcd.print(moistureLevel);
  lcd.print("   ");  // Clear trailing chars if number shrinks

  lcd.setCursor(0, 1);
  lcd.print("Pump: ");
  lcd.print(pumpStatus ? "ON " : "OFF");
  lcd.print("         ");  // Clear trailing

  delay(1000); // 1 second delay between reads
}
