#pragma once
#include <Arduino.h>
#include "LCD_I2C.h"
#include "Relay.h"
#include "Button.h"

class Locker {
public:
    enum class State {
        Open,
        TimeConfig,
        TimeConfirm,
        Closed
    };

    Locker(LCD_I2C *lcd, Relay *relay, Button *btnPlus, Button *btnMinus, Button *btnOk);

    // Initialise la FSM (doit être appelée dans setup)
    void begin();

    // Mettre à jour la FSM (appeler dans loop)
    void update();

    // Obtenir la configuration actuelle en secondes
    uint32_t getConfigSeconds() const;

private:
    LCD_I2C *_lcd;
    Relay *_relay;
    Button *_btnPlus;
    Button *_btnMinus;
    Button *_btnOk;

    State _state;

    // configuration du délai (en secondes)
    uint32_t _configSeconds;

    // Timer de maintien lorsque verrouillé
    bool _timerRunning;
    uint32_t _timerEndMs; // value of millis() when timer expires
    // Last time we persisted remaining ms
    uint32_t _lastSaveMs;
    // Save interval in milliseconds (default 15 minutes)
    uint32_t _saveIntervalMs;
    // Manual unlock pulse (when OK pressed in Open)
    uint32_t _manualUnlockEndMs;
    uint32_t _manualUnlockDurationMs; // default pulse duration in ms

    // Actions / helpers
    void goToState(State s);
    void displayConfig(uint8_t line = 1);
    void displayTimer();
    void increaseConfig();
    void decreaseConfig();
    void lockDoor();
    void unlockDoor();
    void pulseUnlock(uint32_t durationMs);
    void startTimer(uint32_t seconds);
    void stopTimer();

    // Persistence hooks: implement persistence (EEPROM/FS) as you wish.
    // Return true and set remainingMs to resume a previously saved timer.
    bool readSavedTimer(uint32_t &remainingMs);
    // Save remainingMs so it can be resumed after power cycle.
    void saveTimer(uint32_t remainingMs);
    // Clear any saved timer state
    void clearSavedTimer();
};
