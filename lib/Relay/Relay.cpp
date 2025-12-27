#include "Relay.h"

// Constructeur
Relay::Relay(uint8_t pin, bool activeHigh)
    : _pin(pin), _activeHigh(activeHigh) {}

// Initialisation
void Relay::begin() {
    pinMode(_pin, OUTPUT);
    off();  // état par défaut
    Serial.print("[Relay] Initialized on pin: ");
    Serial.print(_pin);
    Serial.print(", Active High: ");
    Serial.println(_activeHigh ? "Yes" : "No");
}

// Allumer le relais
void Relay::on() {
    digitalWrite(_pin, _activeHigh ? HIGH : LOW);
    Serial.println("[Relay] ON");
}

// Éteindre le relais
void Relay::off() {
    digitalWrite(_pin, _activeHigh ? LOW : HIGH);
    Serial.println("[Relay] OFF");
}

// Inverser l’état
void Relay::toggle() {
    if (digitalRead(_pin) == (_activeHigh ? HIGH : LOW)) {
        off();
    } else {
        on();
    }    Serial.println("[Relay] TOGGLED");}

// Lire l’état
bool Relay::isOn() {
    return digitalRead(_pin) == (_activeHigh ? HIGH : LOW);
}
