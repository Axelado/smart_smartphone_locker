#include <Arduino.h>
#include "LCD_I2C.h"

LCD_I2C lcd(0x27, 16, 2);  // adresse I2C 0x27, 16 colonnes, 2 lignes

void setup() {
    Serial.begin(115200);
    lcd.begin();
    lcd.printLine(0, "Hello World!");
    lcd.printLine(1, "LCD I2C Test");
}

void loop() {
    static unsigned long lastTime = 0;
    unsigned long now = millis();
    if (now - lastTime > 1000) {
        lastTime = now;
        lcd.setCursor(0,1);
        lcd.print("Millis: " + String(now/1000));
    }
}
