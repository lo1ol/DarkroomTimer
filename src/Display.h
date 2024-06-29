#pragma once

#include <LiquidCrystal.h>

#include "Config.h"
#include "DisplayLine.h"

class Display {
public:
    Display(LiquidCrystal&& lcd);
    DisplayLine& operator[](uint8_t n) { return m_lines[n]; }

    void tick();

    void printTimeLog(const char* prefix, uint32_t (*timeGetter)(uint8_t), uint8_t maxIndex);

private:
    LiquidCrystal m_lcd;
    DisplayLine m_lines[DISPLAY_ROWS];
};
