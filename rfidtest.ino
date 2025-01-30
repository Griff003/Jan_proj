#include <MFRC522v2.h>
#include <MFRC522DriverSPI.h>
#include <MFRC522DriverPinSimple.h>

MFRC522DriverPinSimple ss_pin(21);
MFRC522DriverSPI driver{ss_pin}; // Create SPI driver
MFRC522 mfrc522{driver};         // Create MFRC522 instance

// List of authorized UIDs
const byte authorizedCards[][4] = {
    {0xDE, 0xAD, 0xBE, 0xEF},
    {0x12, 0x34, 0x56, 0x78},
    {0x90, 0xAB, 0xCD, 0xEF},
    {0x53, 0x69, 0xB2, 0xD9} // Newly added card
};
const int numAuthorizedCards = sizeof(authorizedCards) / sizeof(authorizedCards[0]);

bool checkAuthorized(byte *uid, byte size) {
    for (int i = 0; i < numAuthorizedCards; i++) {
        bool match = true;
        for (int j = 0; j < size; j++) {
            if (authorizedCards[i][j] != uid[j]) {
                match = false;
                break;
            }
        }
        if (match) return true;
    }
    return false;
}

void setup() {
    Serial.begin(115200);
    while (!Serial);
    
    mfrc522.PCD_Init();
    Serial.println(F("Scan a PICC to retrieve its UID..."));
}

void loop() {
    if (!mfrc522.PICC_IsNewCardPresent()) {
        return;
    }

    if (!mfrc522.PICC_ReadCardSerial()) {
        return;
    }

    Serial.print("Card UID: ");
    for (byte i = 0; i < mfrc522.uid.size; i++) {
        Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
        Serial.print(mfrc522.uid.uidByte[i], HEX);
    }
    Serial.println();
    
    if (checkAuthorized(mfrc522.uid.uidByte, mfrc522.uid.size)) {
        Serial.println("Card FOUND in the authorized list!");
    } else {
        Serial.println("Card NOT FOUND in the authorized list!");
    }

    delay(2000);
}
