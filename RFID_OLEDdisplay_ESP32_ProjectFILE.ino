#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

#define SS_PIN 5      // ESP32 GPIO pin for SDA
#define RST_PIN 22    // ESP32 GPIO pin for RST

MFRC522 rfid(SS_PIN, RST_PIN);
Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);  // Initialize SH1106 with width, height, and reset pin

// Predefined UID of authorized card
byte authorizedUID[] = {0x72, 0x08, 0xFA, 0x51};   //place ur UID card number here

void setup() {
  Serial.begin(9600);            // Initialize serial communication
  SPI.begin();                   // Initialize SPI bus
  rfid.PCD_Init();               // Initialize MFRC522

  // Initialize OLED display with custom I2C pins
  Wire.begin(15, 4);             // SDA = GPIO 15, SCL = GPIO 4
  if (!display.begin(0x3C, true)) {
    Serial.println(F("SH1106 OLED allocation failed"));
    for (;;);
  }
  display.clearDisplay();
  display.setTextSize(1);        // Normal 1:1 pixel scale
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0, 0);
  display.print("Place your RFID card");
  display.display();
  delay(1000);

  Serial.println("Place your RFID card near the reader...");
}

void loop() {
  // Check if a new card is present
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial())
    return;

  // Clear the display
  display.clearDisplay();

  // Compare the detected UID with the authorized UID
  if (isAuthorizedCard(rfid.uid.uidByte, rfid.uid.size)) {
    Serial.println("Access Granted: Welcome!");
    display.setCursor(0, 0);
    display.print("Access Granted");
  } else {
    Serial.println("Access Denied");
    display.setCursor(0, 0);
    display.print("Access Denied");
  }

  // Show the message on the OLED
  display.display();

  // Halt PICC
  rfid.PICC_HaltA();
  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();

  // Wait for 2 seconds before clearing the message
  delay(2000);
  display.clearDisplay();
  display.display();
}

// Function to compare detected UID with the authorized UID
bool isAuthorizedCard(byte *uid, byte size) {
  if (size != sizeof(authorizedUID)) return false;
  for (byte i = 0; i < size; i++) {
    if (uid[i] != authorizedUID[i]) return false;
  }
  return true;
}