#pragma once
#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

class LCD_I2C {
private:
    LiquidCrystal_I2C _lcd;
    uint8_t _cols;
    uint8_t _rows;

public:
    // Constructeur : adresse I2C (par défaut 0x27), colonnes et lignes
    LCD_I2C(uint8_t address = 0x27, uint8_t cols = 16, uint8_t rows = 2);

    // Initialisation du LCD
    void begin();

    // Effacer l'écran
    void clear();

    // Afficher du texte sur une ligne (0-indexed)
    void printLine(uint8_t line, const String &text);

    // Déplacer le curseur à une position (col, row)
    void setCursor(uint8_t col, uint8_t row);

    // Afficher du texte à la position courante
    void print(const String &text);

    // Allumer/éteindre le rétroéclairage
    void backlight(bool on);

    // Retour à la ligne 1, colonne 0
    void home();
};
