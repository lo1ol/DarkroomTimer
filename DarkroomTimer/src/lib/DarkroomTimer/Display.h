#pragma once

#include "Config.h"
#include "DisplayLine.h"
#include "TimeTable.h"

class LiquidCrystalWrap;

class Display {
public:
    Display(LiquidCrystalWrap* lcd);
    DisplayLine& operator[](uint8_t n) { return m_lines[n]; }
    DisplayLine* getLines() { return m_lines; }

    void tick();
    void reset();
    void resetBlink(bool state = false);

private:
    LiquidCrystalWrap* m_lcd;
    DisplayLine m_lines[DISPLAY_ROWS];
};
