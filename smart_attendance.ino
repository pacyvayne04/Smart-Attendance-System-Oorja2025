#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <MFRC522.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

#define SS_PIN 10
#define RST_PIN 9
MFRC522 rfid(SS_PIN, RST_PIN);

// Student database
struct Student {
  byte uid[4];
  const char* name;
  const char* roll;
  const char* className;
  bool marked;
};

Student students[] = {
  {{0xA7, 0xEC, 0x28, 0x03}, "Aman Krishna Dhal", "01", "9-A", false},
  {{0xC3, 0xE1, 0xA8, 0x29}, "Saswat Panda", "02", "9-A", false},
  {{0x73, 0x32, 0xB2, 0x29}, "Samarjit Paul", "03", "9-A", false},
  {{0x53, 0xBC, 0x95, 0x14}, "Demo For Delegates", "04", "9-A", false},
};
const int studentCount = sizeof(students) / sizeof(students[0]);

void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();

  lcd.init();
  lcd.backlight();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Welcome To");
  lcd.setCursor(0, 1);
  lcd.print("Oorja 2025");
  delay(3000);

  showScanPrompt();
}

void loop() {
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
    return;
  }

  bool found = false;
  for (int i = 0; i < studentCount; i++) {
    if (compareUID(rfid.uid.uidByte, students[i].uid)) {
      found = true;

      if (!students[i].marked) {
        students[i].marked = true;

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(students[i].name);
        lcd.setCursor(0, 1);
        lcd.print("Roll:");
        lcd.print(students[i].roll);

        Serial.print(students[i].name);
        Serial.print(",");
        Serial.print(students[i].roll);
        Serial.print(",");
        Serial.println(students[i].className);

        delay(2000);
      } else {
        // Already registered
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Already");
        lcd.setCursor(0, 1);
        lcd.print("Registered");
        delay(1500);
      }

      showScanPrompt();
      break;
    }
  }

  if (!found) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Unknown Card");
    delay(1500);
    showScanPrompt();
  }

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}

bool compareUID(byte* uid1, const byte* uid2) {
  for (int i = 0; i < 4; i++) {
    if (uid1[i] != uid2[i]) return false;
  }
  return true;
}

void showScanPrompt() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Scan Your Card");
}
