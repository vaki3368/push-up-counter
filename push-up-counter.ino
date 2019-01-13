#include <EEPROM.h>
#include <Ultrasonic.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x3F, 16, 2);
Ultrasonic ultrasonic(22, 23, 5800); // (Trig PIN,Echo PIN) 58ms * 100cm

int adresse = 0;
int pushups = 0;
bool up = true;
int total = 0;
int sessionDetectorCounter = 0;

void setup()
{
  lcd.begin();
  lcd.backlight();
  lcd.print("Total: ");
  total = eepromReadInt(adresse);
  lcd.print(total);
  lcd.setCursor(0, 1);
  lcd.print("Session: ");
  lcd.print(pushups); // CM or INC
}

void loop()
{
  int dist = ultrasonic.Ranging(CM);
  if(dist < 15 && up) {
      pushups++;
      lcd.setCursor(9, 1);
      lcd.print(pushups);
      up = false;
      total++;
      eepromWriteInt(adresse, total);
      lcd.setCursor(7, 0);
      lcd.print(total);
      sessionDetectorCounter = 0;
  }
  else if(dist > 45) {
    up = true;
    sessionDetectorCounter++;
  }

  // session end detector
  if(sessionDetectorCounter > 100) {
    pushups = 0;
    sessionDetectorCounter = 0;
    lcd.setCursor(9, 1);
    lcd.print("  "); // clear old value
    lcd.setCursor(9, 1);
    lcd.print(pushups);
  } 
  delay(100);
}

void eepromWriteInt(int adr, int wert) {
  byte low, high;
  low = wert&0xFF;
  high = (wert>>8)&0xFF;
  EEPROM.write(adr, low);
  EEPROM.write(adr + 1, high);
}

int eepromReadInt(int adr) {
  byte low, high;
  low = EEPROM.read(adr);
  high = EEPROM.read(adr + 1);
  return low + ((high << 8)&0xFF00);
}
