#include "Relay.h"

// Constructeur
Relay::Relay(uint8_t pin, bool activeHigh)
    : _pin(pin), _activeHigh(activeHigh) {}

// Initialisation
void Relay::begin() {
    pinMode(_pin, OUTPUT);
    off();  // état par défaut
}

// Allumer le relais
void Relay::on() {
    digitalWrite(_pin, _activeHigh ? HIGH : LOW);
}

// Éteindre le relais
void Relay::off() {
    digitalWrite(_pin, _activeHigh ? LOW : HIGH);
}

// Inverser l’état
void Relay::toggle() {
    if (digitalRead(_pin) == (_activeHigh ? HIGH : LOW)) {
        off();
    } else {
        on();
    }
}

// Lire l’état
bool Relay::isOn() {
    return digitalRead(_pin) == (_activeHigh ? HIGH : LOW);
}
