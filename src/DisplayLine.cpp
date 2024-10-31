#include "DisplayLine.h"

#include <Arduino.h>

#include <LiquidCrystal.h>

#include "Tools.h"

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

bool DisplayLine::tryPrint(const char* src, bool blink, uint8_t alignSize, const char* mark) {
    uint8_t srclen = strlen(src);
    uint8_t dstlen = strlen(m_fwInfo);

    if (!alignSize)
        alignSize = srclen;

    if (alignSize + dstlen > DISPLAY_COLS)
        return false;

    m_needRepaint = true;

    memset(m_fwInfo + dstlen, ' ', alignSize - srclen);
    m_fwInfo[dstlen + alignSize - srclen] = 0;

    if (blink) {
        m_blinkLength = alignSize;
        m_blinkPos = dstlen;

        if (mark)
            m_mark = mark;
    }

    concat(m_fwInfo, src);
    return true;
}

void DisplayLine::reset() {
    m_needRepaint = true;
    m_fwInfo[0] = 0;
    m_bwInfo[0] = 0;
    m_blinkLength = 0;
    m_mark = 0;
}

void DisplayLine::resetBlink(bool state) {
    m_blinkTimer = millis();
    m_blinkState = state;
}

void DisplayLine::tick() {
    if (!m_needRepaint && !m_blinkLength)
        return;
    m_needRepaint = false;

    char printBuf[DISPLAY_COLS + 1];

    auto fwLen = strlen(m_fwInfo);
    memcpy(printBuf, m_fwInfo, fwLen);
    memset(printBuf + fwLen, ' ', DISPLAY_COLS - fwLen);

    auto bwLen = strlen(m_bwInfo);
    memcpy(printBuf + DISPLAY_COLS - bwLen, m_bwInfo, bwLen);

    if (m_blinkLength) {
        if (millis() - m_blinkTimer > 500) {
            m_blinkState = !m_blinkState;
            m_blinkTimer = millis();
        }

        if (m_blinkState) {
            memset(printBuf + m_blinkPos, ' ', m_blinkLength);

            if (m_mark) {
                uint8_t marklen = strlen(m_mark);
                memcpy(printBuf + m_blinkPos + m_blinkLength - marklen, m_mark, marklen);
            }
        }
    }

    m_lcd.setCursor(0, m_line);
    m_lcd.print(printBuf);
}

DisplayLine& DisplayLine::operator<<(const char* src) {
    m_needRepaint = true;
    concat(m_fwInfo, src);
    return *this;
}

DisplayLine& DisplayLine::operator<<(int value) {
    m_needRepaint = true;
    concatInt(m_fwInfo, value);
    return *this;
}

DisplayLine& DisplayLine::operator>>(const char* src) {
    m_needRepaint = true;
    concat(m_bwInfo, src);
    return *this;
}

DisplayLine& DisplayLine::operator>>(int value) {
    m_needRepaint = true;
    concatInt(m_bwInfo, value);
    return *this;
}
