/*
keyless_entry.ino
The Arduino "language" is just C++.

This demo uses an ESP32 DevKit microcontroller,
a MFRC522 RFID scanner, and a SG90 Micro Servo motor.

When you scan the blue RFID key fob next to the scanner, it activates the key fob.
The scanner constantly sends out a radio frequency and then the key fob transmits its unique
identification code back to the scanner. The microcontroller reads this value from
the scanner and if it successfully authenticates it, it turns the servo, simulating a
lock being opened to perhaps a door or a gate. The user is given confirmation by the
red and green LED bulbs whether or not their key was successful for entry.
*/

#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>

#include <ESP32Servo.h>

//PINS
//avoid allocating memory for pin numbers
#define RST_PIN 22
#define SS_PIN  5
#define SERVO_PIN 21
#define RED_BULB 4
#define GREEN_BULB 2

//INSTANCES
MFRC522 mfrc522(SS_PIN, RST_PIN);
Servo lock;

String KEY_FOB_UID = "e32ca8d";
//String RFID_CARD_UID = "b3202f10";
String KEY_IN_USE = KEY_FOB_UID;

void setup() {
  Serial.begin(9600);
  while (!Serial);
  SPI.begin();

  mfrc522.PCD_Init();

  lock.attach(SERVO_PIN);
  lock.write(0); //set to 0 degrees "locked"

  pinMode(RED_BULB, OUTPUT);
  pinMode(GREEN_BULB, OUTPUT);
}

void loop() {
  byte uid_byte;
  String scanned_uid = "";
  if (mfrc522.PICC_IsNewCardPresent()) {
    if (mfrc522.PICC_ReadCardSerial()) {
      for (int idx = 0; idx < 10; idx++) {
        uid_byte = mfrc522.uid.uidByte[idx];
        if (uid_byte) {
          scanned_uid += (String(uid_byte, HEX));
        }
      }
    }
  }
  if (scanned_uid == KEY_IN_USE) {
    Serial.println("Valid Key: Unlock");
    digitalWrite(GREEN_BULB, HIGH);
    lock.write(90);
    for (int i = 5; i != 0; i--) {
      Serial.println(i);
      delay(1000);
    }
    Serial.println("Lock");
    digitalWrite(GREEN_BULB, LOW);
    lock.write(0);
  }
  else if (scanned_uid != "" && scanned_uid != KEY_IN_USE) {
    Serial.println("Invalid Key: Denied");
    digitalWrite(RED_BULB, HIGH);
    for (int i = 5; i != 0; i--) {
      Serial.println(i);
      delay(1000);
    }
    Serial.println("Try again");
    digitalWrite(RED_BULB, LOW);
  }
}
