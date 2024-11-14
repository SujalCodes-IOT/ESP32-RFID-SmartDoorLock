#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 5    // ESP32 GPIO pin for SDA
#define RST_PIN 22  // ESP32 GPIO pin for RST

MFRC522 rfid(SS_PIN, RST_PIN);

void setup() {
  Serial.begin(9600);    // Initialize serial communication at 9600 baud rate
  SPI.begin();           // Initialize SPI bus
  rfid.PCD_Init();       // Initialize MFRC522
  Serial.println("Place your RFID card near the reader...");
}

void loop() {
  // Check if a new card is present
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial())
    return;

  // Show UID on serial monitor
  Serial.print("Card UID: ");
  for (byte i = 0; i < rfid.uid.size; i++) {
    Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(rfid.uid.uidByte[i], HEX);
  }
  Serial.println();

  // Halt PICC
  rfid.PICC_HaltA();
  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();
}