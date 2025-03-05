#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

// Initialize LCD with I2C address 0x27
LiquidCrystal_I2C lcd(0x27, 16, 2);

const byte ROWS = 4;
const byte COLS = 3;
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};
byte rowPins[ROWS] = {8, 3, 4, 6};
byte colPins[COLS] = {7, 9, 5};

// Create keypad object
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Predefined list of valid numbers
String validNumbers[] = {"1234", "5678", "9999"};
const int validCount = sizeof(validNumbers) / sizeof(validNumbers[0]);

String input = ""; // Stores user input

void setup() {
  Serial.begin(9600);
  lcd.init();        // Initialize the LCD
  lcd.backlight();   // Turn on backlight
  lcd.setCursor(0, 0);
  lcd.print("Enter number:");
}

void loop() {
  char key = keypad.getKey();

  if (key) {
    Serial.print("Key Pressed: ");
    Serial.println(key);

    if (key >= '0' && key <= '9') {
      // Append number if less than 10 characters
      if (input.length() < 10) {
        input += key;
        lcd.setCursor(0, 1);
        lcd.print(input); // Display user input
      }
    } 
    else if (key == '*') {  
      // Backspace function
      if (input.length() > 0) {
        input.remove(input.length() - 1);
        lcd.setCursor(0, 1);
        lcd.print("                "); // Clear line
        lcd.setCursor(0, 1);
        lcd.print(input);
      }
    } 
    else if (key == '#') {  
      // Check if number is valid
      bool found = false;
      for (int i = 0; i < validCount; i++) {
        if (input == validNumbers[i]) {
          found = true;
          break;
        }
      }

      lcd.setCursor(0, 1);
      lcd.print("                "); // Clear line
      lcd.setCursor(0, 1);
      if (found) {
        lcd.print("Accepted");
      } else {
        lcd.print("Denied");
      }
      
      delay(2000); // Display message for 2 seconds
      lcd.setCursor(0, 1);
      lcd.print("                "); // Clear line
      input = "";  // Reset input
    }
  }
}
