#pragma once

#include <stdint.h>

#include "Config.h"
class LiquidCrystalWrap;

class DisplayLine {
public:
    DisplayLine(int line, LiquidCrystalWrap* lcd) : m_line(line), m_lcd(lcd) {}

    void tick();
    void reset();
    void resetBlink(bool state = false);

    DisplayLine& operator<<(const char* src);
    DisplayLine& operator<<(int value);

    DisplayLine& operator>>(const char* src);
    DisplayLine& operator>>(int value);

    void print(const char* src, bool current = false, const char* mark = nullptr);

    // it's responsobility of caller to keep fast repaint length less then current length
    void fastCurrentRepaint(const char* src);
    void restore();

private:
    static void concat(char* dst, const char* src);
    static void concatInt(char* dst, int value);

    bool m_needRepaint = false;
    int m_line;
    LiquidCrystalWrap* m_lcd;

    uint32_t m_blinkTimer = 0;
    bool m_blinkState = 0;

    uint8_t m_currentPos = 0;
    uint8_t m_currentLength = 0;
    const char* m_currentMark = nullptr;
    bool m_needCurrentFastRepaint = false;
    bool m_hasCurrentFastRepaint = false;
    char m_fastCurrentStr[DISPLAY_COLS + 1] = "";

    char m_fwInfo[DISPLAY_COLS + 1] = "";
    char m_bwInfo[DISPLAY_COLS + 1] = "";
};
