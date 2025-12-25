#include <Arduino.h>
#include "Button.h"

#define BUTTON_OK_PIN D5
#define BUTTON_MINUS_PIN D6
#define BUTTON_PLUS_PIN D7

#define LED_PIN LED_BUILTIN

Button buttonOk(BUTTON_OK_PIN);
Button buttonMinus(BUTTON_MINUS_PIN);
Button buttonPlus(BUTTON_PLUS_PIN);

void setup() {
    Serial.begin(115200);
    pinMode(LED_PIN, OUTPUT);
    buttonOk.begin();
    buttonMinus.begin();
    buttonPlus.begin();
    Serial.println("Setup complete");

}

void loop() {
    buttonOk.update();
    buttonMinus.update();
    buttonPlus.update();

   if (buttonOk.wasPressed()) {
    Serial.println("OK pressé");
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));
}

    if (buttonMinus.wasPressed()) {
        Serial.println("MINUS pressé");
        digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    }

    if (buttonPlus.wasPressed()) {
        Serial.println("PLUS pressé");
        digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    }

    
}
