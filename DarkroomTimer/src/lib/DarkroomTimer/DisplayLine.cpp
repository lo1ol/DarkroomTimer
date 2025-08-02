#include "DisplayLine.h"

#include <Arduino.h>

#include "LiquidCrystalWrap.h"
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

void DisplayLine::print(const char* src, bool current, const char* mark) {
    uint8_t srclen = strlen(src);
    uint8_t dstlen = strlen(m_fwInfo);

    m_needRepaint = true;

    if (current) {
        m_blinkLength = srclen;
        m_blinkPos = dstlen;
        m_mark = mark;
    }

    concat(m_fwInfo, src);
}

void DisplayLine::reset() {
    m_needRepaint = true;
    m_fwInfo[0] = 0;
    m_bwInfo[0] = 0;
    m_blinkLength = 0;
    m_mark = 0;
}

void DisplayLine::resetBlink(bool state) {
    m_blinkTimer = gMillis();
    m_blinkState = state;
}

void DisplayLine::tick() {
    char printBuf[DISPLAY_COLS + 1];

    if (m_needRepaint) {
        m_needRepaint = false;
        m_hasFastRepaint = false;

        auto fwLen = strlen(m_fwInfo);
        memcpy(printBuf, m_fwInfo, fwLen);
        memset(printBuf + fwLen, ' ', DISPLAY_COLS - fwLen);
        printBuf[DISPLAY_COLS] = 0;

        auto bwLen = strlen(m_bwInfo);
        memcpy(printBuf + DISPLAY_COLS - bwLen, m_bwInfo, bwLen);

        m_lcd->setCursor(0, m_line);
        m_lcd->print(printBuf);
    }

    if (m_needFastRepaint) {
        m_needFastRepaint = false;
        m_hasFastRepaint = true;
        m_lcd->setCursor(m_fastChangePos, m_line);
        m_lcd->print(m_fastChange);
    }

    if (m_blinkLength && !m_hasFastRepaint) {
        if (gMillis() - m_blinkTimer > 500) {
            m_blinkState = !m_blinkState;
            m_blinkTimer = gMillis();
        }

        if (m_blinkState) {
            memset(printBuf, ' ', m_blinkLength);

            if (m_mark) {
                uint8_t marklen = strlen(m_mark);
                memcpy(printBuf + m_blinkLength - marklen, m_mark, marklen);
            }
        } else {
            memcpy(printBuf, m_fwInfo + m_blinkPos, m_blinkLength);
        }

        printBuf[m_blinkLength] = 0;
        m_lcd->setCursor(m_blinkPos, m_line);
        m_lcd->print(printBuf);
    }
}

void DisplayLine::restore() {
    m_needRepaint = true;
    m_needFastRepaint = false;
    m_hasFastRepaint = false;
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

void DisplayLine::fastRepaint(const char* src, uint8_t pos) {
    m_needFastRepaint = true;
    m_hasFastRepaint = true;
    m_fastChangePos = pos;
    strcpy(m_fastChange, src);
}
