#include <Wire.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 16
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setup() {
  pinMode(6, OUTPUT); // Pump 1
  pinMode(5, OUTPUT); // Pump 2
  pinMode(7, OUTPUT); // Pump 3
  Serial.begin(9600);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.clearDisplay();
}

void loop() {
  // Reading moisture sensor 1
  int moistureLevel1 = analogRead(A2);
  int reversedMoistureLevel1 = 1023 - moistureLevel1;
  int moisturePercentage1 = map(reversedMoistureLevel1, 0, 1000, 0, 100);
  if (moisturePercentage1 > 100) moisturePercentage1 = 100;
  if (moisturePercentage1 < 0) moisturePercentage1 = 0;

  // Reading moisture sensor 2
  int moistureLevel2 = analogRead(A0);
  int reversedMoistureLevel2 = 1023 - moistureLevel2;
  int moisturePercentage2 = map(reversedMoistureLevel2, 0, 1000, 0, 100);
  if (moisturePercentage2 > 100) moisturePercentage2 = 100;
  if (moisturePercentage2 < 0) moisturePercentage2 = 0;
  
  // Reading moisture sensor 3
  int moistureLevel3 = analogRead(A1);
  int reversedMoistureLevel3 = 1023 - moistureLevel3;
  int moisturePercentage3 = map(reversedMoistureLevel3, 0, 1000, 0, 100);
  if (moisturePercentage3 > 100) moisturePercentage3 = 100;
  if (moisturePercentage3 < 0) moisturePercentage3 = 0;

  // Display data
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.print("M1: ");
  display.print(moisturePercentage1);
  display.print("%  M2: ");
  display.print(moisturePercentage2);
  display.print("%  M3: ");
  display.print(moisturePercentage3);
  display.println("%");
  display.display();

  // Serial output
  Serial.print("Moisture 1: ");
  Serial.print(reversedMoistureLevel1);
  Serial.print(" | Moisture %1: ");
  Serial.print(moisturePercentage1);
  Serial.print(" | Moisture 2: ");
  Serial.print(reversedMoistureLevel2);
  Serial.print(" | Moisture %2: ");
  Serial.print(moisturePercentage2);
  Serial.print(" | Moisture 3: ");
  Serial.print(reversedMoistureLevel3);
  Serial.print(" | Moisture %3: ");
  Serial.println(moisturePercentage3);

  // Control Pump 1
  if (reversedMoistureLevel1 < 400) {
    digitalWrite(6, HIGH);
    Serial.println("Pump 1 ON - Soil 1 is dry");
  } else {
    digitalWrite(6, LOW);
    Serial.println("Pump 1 OFF - Soil 1 is wet");
  }

  // Control Pump 2
  if (reversedMoistureLevel2 < 400) {
    digitalWrite(5, HIGH);
    Serial.println("Pump 2 ON - Soil 2 is dry");
  } else {
    digitalWrite(5, LOW);
    Serial.println("Pump 2 OFF - Soil 2 is wet");
  }
  
  // Control Pump 3
  if (reversedMoistureLevel3 < 400) {
    digitalWrite(7, HIGH);
    Serial.println("Pump 3 ON - Soil 3 is dry");
  } else {
    digitalWrite(7, LOW);
    Serial.println("Pump 3 OFF - Soil 3 is wet");
  }

  delay(1000);
}
