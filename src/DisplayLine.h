#pragma once

#include <stdint.h>

#include "Config.h"

class LiquidCrystal;

class DisplayLine {
public:
    DisplayLine(int line, LiquidCrystal& lcd) : m_line(line), m_lcd(lcd) {}

    void tick();
    void flush() const;

    DisplayLine& operator<<(const char* src);
    DisplayLine& operator<<(int value);

    DisplayLine& operator>>(const char* src);
    DisplayLine& operator>>(int value);

    bool tryPrint(const char* src);

private:
    static void concat(char* dst, const char* src);
    static void concatInt(char* dst, int value);

    int m_line;
    LiquidCrystal& m_lcd;

    char m_fwInfo[DISPLAY_COLS + 1] = "";
    char m_bwInfo[DISPLAY_COLS + 1] = "";
};
