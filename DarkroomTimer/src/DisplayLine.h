#pragma once

#include <stdint.h>

#include "Config.h"

class Lcd;

class DisplayLine {
public:
    DisplayLine() = default;
    DisplayLine(int line, Lcd* lcd) : m_line(line), m_lcd(lcd) {}

    void tick();
    void reset();
    void resetBlink(bool showBlinked);

    DisplayLine& operator<<(const char* src);
    DisplayLine& operator<<(int value);

    DisplayLine& operator>>(const char* src);
    DisplayLine& operator>>(int value);

#ifndef DT_NATIVE
    DisplayLine& operator<<(const __FlashStringHelper*);
    DisplayLine& operator>>(const __FlashStringHelper*);
#endif

    void print(const char* src, bool current = false, const char* mark = nullptr);

    void printWithAnimation(const __FlashStringHelper*, uint16_t time);
    bool animationIsRunning() const { return m_showingAnimation; }

    // it's responsobility of caller to keep fast repaint length less then current length
    void fastCurrentRepaint(const char* src);
    void restore();

private:
    void tickAnimation();

    static void concat(char* dst, const char* src);
    static void concatInt(char* dst, int value);

    bool m_needRepaint = false;
    int m_line = -1;
    Lcd* m_lcd = nullptr;

    static constexpr uint8_t kAnimationUpdateRate = 40;

    bool m_showingAnimation = false;
    uint16_t m_animationStartTime = 0;
    uint16_t m_animationTime = 0;
    uint16_t m_lastAnimationUpdateTime = 0;

    uint32_t m_baseBlinkTime = 0;
    bool m_startBlinkState = false;
    bool m_currentBlinkState = false;

    uint8_t m_currentPos = 0;
    uint8_t m_currentLength = 0;
    const char* m_currentMark = nullptr;
    bool m_needCurrentFastRepaint = false;
    bool m_hasCurrentFastRepaint = false;
    char m_fastCurrentStr[DISPLAY_COLS + 1] = "";

    char m_fwInfo[DISPLAY_COLS + 1] = "";
    char m_bwInfo[DISPLAY_COLS + 1] = "";
};
