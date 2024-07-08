#pragma once

#include <LiquidCrystal.h>

#include "Config.h"
#include "DisplayLine.h"

class Display {
public:
    Display(LiquidCrystal&& lcd);
    DisplayLine& operator[](uint8_t n) { return m_lines[n]; }
    DisplayLine* getLines() { return m_lines; }

    void tick();
    void reset();
    void resetBlink(bool state = false);

private:
    LiquidCrystal m_lcd;
    DisplayLine m_lines[DISPLAY_ROWS];
};
