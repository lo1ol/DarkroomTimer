#pragma once

#include <stdint.h>

#include "Config.h"

class LiquidCrystal;

class DisplayLine {
public:
    DisplayLine(int line, LiquidCrystal& lcd) : m_line(line), m_lcd(lcd) {}

    void tick();
    void reset();
    void resetBlink(bool state = false);

    DisplayLine& operator<<(const char* src);
    DisplayLine& operator<<(int value);

    DisplayLine& operator>>(const char* src);
    DisplayLine& operator>>(int value);

    bool tryPrint(const char* src, bool blink = false, uint8_t alignSize = 0, const char* mark = nullptr);

private:
    static void concat(char* dst, const char* src);
    static void concatInt(char* dst, int value);

    int m_line;
    LiquidCrystal& m_lcd;

    uint8_t m_blinkPos = 0;
    uint8_t m_blinkLength = 0;
    uint32_t m_blinkTimer = 0;
    bool m_blinkState = 0;
    const char* m_mark;

    char m_fwInfo[DISPLAY_COLS + 1] = "";
    char m_bwInfo[DISPLAY_COLS + 1] = "";
};
