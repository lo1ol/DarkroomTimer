#pragma once

#include "Config.h"
#include "DisplayLine.h"
#include "TimeTable.h"

class Lcd;

class Display {
public:
    explicit Display(Lcd* lcd);
    [[nodiscard]] DisplayLine& operator[](uint8_t n) { return m_lines[n]; }
    [[nodiscard]] DisplayLine* getLines() { return m_lines; }

    void tick();
    void reset();
    void resetBlink(bool showBlinked);

private:
    Lcd* m_lcd;
    DisplayLine m_lines[DISPLAY_ROWS];
};
