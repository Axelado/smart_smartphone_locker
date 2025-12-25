#include "LCD_I2C.h"

// Constructeur
LCD_I2C::LCD_I2C(uint8_t address, uint8_t cols, uint8_t rows)
    : _lcd(address, cols, rows), _cols(cols), _rows(rows) {}

// Initialisation
void LCD_I2C::begin() {
    _lcd.init();
    _lcd.backlight();
    _lcd.clear();
}

// Effacer l'écran
void LCD_I2C::clear() {
    _lcd.clear();
}

// Afficher du texte sur une ligne spécifique
void LCD_I2C::printLine(uint8_t line, const String &text) {
    if (line >= _rows) return;
    _lcd.setCursor(0, line);
    String toPrint = text;
    if (toPrint.length() > _cols) toPrint = toPrint.substring(0, _cols);
    _lcd.print(toPrint);
    // remplir le reste de la ligne avec des espaces pour effacer l'ancien texte
    for (size_t i = toPrint.length(); i < _cols; i++) {
        _lcd.print(" ");
    }
}

// Déplacer le curseur
void LCD_I2C::setCursor(uint8_t col, uint8_t row) {
    if (col >= _cols) col = _cols - 1;
    if (row >= _rows) row = _rows - 1;
    _lcd.setCursor(col, row);
}

// Afficher du texte à la position courante
void LCD_I2C::print(const String &text) {
    _lcd.print(text);
}

// Rétroéclairage
void LCD_I2C::backlight(bool on) {
    if (on) _lcd.backlight();
    else _lcd.noBacklight();
}

// Retour à l'origine
void LCD_I2C::home() {
    _lcd.setCursor(0,0);
}
