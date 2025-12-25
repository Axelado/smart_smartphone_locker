#include <Arduino.h>
#include "Relay.h"

#define RELAY_LOCK_PIN D8

Relay lock(RELAY_LOCK_PIN, true); // active HIGH

void setup() {
    Serial.begin(115200);
    lock.begin();
    Serial.println("Test relais LOCK");
}

void loop() {
    Serial.println("Allume le relais LOCK");
    lock.on();
    delay(2000);

    Serial.println("Éteint le relais LOCK");
    lock.off();
    delay(2000);

    Serial.println("Toggle le relais LOCK");
    lock.toggle();
    delay(2000);
}
