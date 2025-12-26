#include <Arduino.h>
#include "LCD_I2C.h"
#include "Button.h"
#include "Relay.h"
#include "Locker.h"

#define BUTTON_OK_PIN D5
#define BUTTON_MINUS_PIN D6
#define BUTTON_PLUS_PIN D7
#define RELAY_LOCK_PIN D8

LCD_I2C lcd(0x27, 16, 2);
Button buttonOk(BUTTON_OK_PIN);
Button buttonMinus(BUTTON_MINUS_PIN);
Button buttonPlus(BUTTON_PLUS_PIN);
Relay lockRelay(RELAY_LOCK_PIN, true);

Locker locker(&lcd, &lockRelay, &buttonPlus, &buttonMinus, &buttonOk);

void setup() {
	Serial.begin(115200);
	lcd.begin();
	buttonOk.begin();
	buttonMinus.begin();
	buttonPlus.begin();
	lockRelay.begin();

	// Locker begin will check persistence and start timer if needed
	locker.begin();
}

void loop() {
	locker.update();
	delay(10);
}
