#ifndef DISPLAY_WRAPPER_H
#define DISPLAY_WRAPPER_H

#include <LiquidCrystal_I2C.h>
#include "core/config_manager.h"

class display
{
public:
    bool shouldClear;
    int targetColumn;
    int targetRow;
    String message;

    display(String message);

    display &clear();
    display &row(int row);
    display &top();
    display &bottom();
    display &column(int column);
    void print();

    static void init();
    static void clearDisplay();
    static void backlight(bool enableBacklight = true);
    static void checkBacklight();
    static unsigned long lastDisplayTime;
};

#endif // DISPLAY_WRAPPER_H