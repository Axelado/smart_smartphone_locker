#pragma once
#include <Arduino.h>

class Relay {
private:
    uint8_t _pin;
    bool _activeHigh;

public:
    // Constructeur
    Relay(uint8_t pin, bool activeHigh = true);

    // Initialisation
    void begin();

    // Allumer le relais
    void on();

    // Éteindre le relais
    void off();

    // Inverser l’état
    void toggle();

    // Lire l’état (true = activé)
    bool isOn();
};
