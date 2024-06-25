#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define SS_PIN 10    // Define the Slave Select (SS) pin for the RFID module
#define RST_PIN 9    // Define the Reset pin for the RFID module
#define GREEN_LED 7  // Define the pin for the green LED
#define RED_LED 6    // Define the pin for the red LED
#define BUZZER 5     // Define the pin for the buzzer
#define SERVO_PIN 3  // Define the pin for the servo motor

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create an MFRC522 instance
Servo myServo;  // Create a servo object to control a servo motor
LiquidCrystal_I2C lcd(0x27, 16, 2); // Set the LCD I2C address and dimensions (16x2)

void setup() {
  Serial.begin(9600);  // Initialize serial communication
  SPI.begin();         // Init SPI bus
  mfrc522.PCD_Init();  // Init MFRC522 card
  myServo.attach(SERVO_PIN); // Attach the servo to the corresponding pin

  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  digitalWrite(GREEN_LED, LOW); // Ensure green LED is off initially
  digitalWrite(RED_LED, LOW);   // Ensure red LED is off initially
  digitalWrite(BUZZER, LOW);    // Ensure buzzer is off initially
  myServo.write(180); // Set the initial position of the servo to 180 degrees

  lcd.init(); // Initialize the LCD
  lcd.backlight(); // Turn on the backlight of the LCD
  lcd.setCursor(0, 0);
  lcd.print("Show card to");
  lcd.setCursor(0, 1);
  lcd.print("open");
}

void loop() {
  // Look for new cards
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    String tagUID = "";  // Initialize a string to store the detected tag's UID

    for (byte i = 0; i < mfrc522.uid.size; i++) {
      tagUID.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : ""));
      tagUID.concat(String(mfrc522.uid.uidByte[i], HEX));
    }
    tagUID.toUpperCase();  // Convert UID to uppercase letters

    Serial.println("Scanned RFID tag UID: " + tagUID); // Print the detected UID

    // Define the UID of the authorized RFID tag
    String authorizedTag = "533C42F4"; // Replace with your tag's UID

    lcd.clear(); // Clear the LCD screen

    if (tagUID == authorizedTag) {
      // Correct tag detected, turn on green LED, move servo to 0 degrees for 5 seconds
      digitalWrite(GREEN_LED, HIGH);
      lcd.setCursor(0, 0);
      lcd.print("Access Granted");
      myServo.write(0); // Move the servo to 0 degrees
      delay(5000); // Wait for 5 seconds
      myServo.write(180); // Move the servo back to 180 degrees
      digitalWrite(GREEN_LED, LOW); // Turn off green LED
    } else {
      // Incorrect tag detected, turn on red LED and activate buzzer for 5 seconds
      digitalWrite(RED_LED, HIGH);
      digitalWrite(BUZZER, HIGH);
      lcd.setCursor(0, 0);
      lcd.print("Access Denied");
      delay(1000); // Wait for 5 seconds
      digitalWrite(RED_LED, LOW); // Turn off red LED
      digitalWrite(BUZZER, LOW); // Turn off the buzzer
    }

    mfrc522.PICC_HaltA(); // Halt PICC
    mfrc522.PCD_StopCrypto1(); // Stop encryption on PCD
  } else {
    // No card detected, display "Show card to open"
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Show card to");
    lcd.setCursor(0, 1);
    lcd.print("open");
  }
}
