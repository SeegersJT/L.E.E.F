#include "display_wrapper.h"
#include "globals.h"

// Constructed lazily in display::init() (called from setup(), after
// config.initialConfig() has populated LCD_ADDR/COLUMNS/ROWS) rather than
// as a global object here - a global LiquidCrystal_I2C would run its
// constructor during static initialization, before config_map has any
// values in it, silently binding to I2C address 0 with 0 columns/rows.
LiquidCrystal_I2C* lcd_display = nullptr;

unsigned long display::lastDisplayTime = 0;

display::display(String message)
    : message(message), shouldClear(false), targetColumn(0), targetRow(0) {}

display& display::clear() {
    this->shouldClear = true;
    return *this;
}

display& display::row(int row) {
    this->targetRow = row;
    return *this;
}

display& display::top() {
    this->targetRow = 0;
    return *this;
}

display& display::bottom() {
    this->targetRow = 1;
    return *this;
}

display& display::column(int column) {
    this->targetColumn = column;
    return *this;
}

void display::print() {
    Serial.println("[" + this->message + "]");

    if (!lcd_display) return; // init() hasn't run yet - nothing to write to

    if (this->shouldClear) lcd_display->clear();
    lcd_display->setCursor(this->targetColumn, this->targetRow);
    lcd_display->print(this->message);

    display::lastDisplayTime = millis();
    display::backlight();
}

void display::init() {
    lcd_display = new LiquidCrystal_I2C(config["LCD_ADDR"], config["LCD_COLUMNS"], config["LCD_ROWS"]);
    lcd_display->init();
    display::backlight();
}

void display::clearDisplay() {
    if (lcd_display) lcd_display->clear();
}

void display::backlight(bool enableBacklight) {
    if (!lcd_display) return;
    if (enableBacklight) {
        lcd_display->backlight();
    } else {
        lcd_display->noBacklight();
    }
}

void display::checkBacklight() {
    unsigned long currentTime = millis();
    if (currentTime - lastDisplayTime > 15000) {
        display::backlight(false);
    }
}