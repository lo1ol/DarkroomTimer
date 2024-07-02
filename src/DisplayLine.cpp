#include "DisplayLine.h"

#include <Arduino.h>

#include <LiquidCrystal.h>

void DisplayLine::concat(char* dst, const char* src) {
    int srcLen = strlen(src);
    int shift = strlen(dst);
    int rest = DISPLAY_COLS - shift;

    srcLen = min(srcLen, rest);

    memcpy(dst + shift, src, srcLen);
    dst[shift + srcLen] = 0;
}

void DisplayLine::concatInt(char* dst, int value) {
    char str[DISPLAY_COLS + 1];
    itoa(value, str, 10);
    concat(dst, str);
}

bool DisplayLine::tryPrint(const char* src, bool blink) {
    uint8_t srclen = strlen(src);
    uint8_t dstlen = strlen(m_fwInfo);
    if (srclen + dstlen > DISPLAY_COLS)
        return false;

    if (blink) {
        m_blinkLength = srclen;
        m_blinkPos = dstlen;
    }

    concat(m_fwInfo, src);
    return true;
}

void DisplayLine::tick() {
    auto fwLen = strlen(m_fwInfo);
    memset(m_fwInfo + fwLen, ' ', DISPLAY_COLS - fwLen);

    auto bwLen = strlen(m_bwInfo);
    memcpy(m_fwInfo + DISPLAY_COLS - bwLen, m_bwInfo, bwLen);

    if (m_blinkLength) {
        if (millis() - m_blinkTimer > 500) {
            m_blinkState = !m_blinkState;
            m_blinkTimer = millis();
        }

        if (m_blinkState)
            memset(m_fwInfo + m_blinkPos, ' ', m_blinkLength);
    }

    m_lcd.setCursor(0, m_line);

    m_lcd.print(m_fwInfo);

    m_fwInfo[0] = 0;
    m_bwInfo[0] = 0;

    m_blinkLength = 0;
}

DisplayLine& DisplayLine::operator<<(const char* src) {
    concat(m_fwInfo, src);
    return *this;
}

DisplayLine& DisplayLine::operator<<(int value) {
    concatInt(m_fwInfo, value);
    return *this;
}

DisplayLine& DisplayLine::operator>>(const char* src) {
    concat(m_bwInfo, src);
    return *this;
}

DisplayLine& DisplayLine::operator>>(int value) {
    concatInt(m_bwInfo, value);
    return *this;
}
