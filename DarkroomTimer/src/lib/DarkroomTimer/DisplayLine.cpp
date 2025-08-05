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
    m_needRepaint = true;

    if (current) {
        m_currentLength = strlen(src);
        m_currentPos = strlen(m_fwInfo);
        m_currentMark = mark;
    }

    concat(m_fwInfo, src);
}

void DisplayLine::reset() {
    m_needRepaint = true;
    m_fwInfo[0] = 0;
    m_bwInfo[0] = 0;
    m_currentLength = 0;
    m_currentMark = nullptr;
}

void DisplayLine::resetBlink(bool startBlinked) {
    m_baseBlinkTime = gMillis();
    m_startBlinkState = startBlinked;
    m_currentBlinkState = !startBlinked;
}

void DisplayLine::tick() {
    char printBuf[DISPLAY_COLS + 1];

    if (m_needRepaint) {
        m_needRepaint = false;
        m_hasCurrentFastRepaint = false;

        auto fwLen = strlen(m_fwInfo);
        memcpy(printBuf, m_fwInfo, fwLen);
        memset(printBuf + fwLen, ' ', DISPLAY_COLS - fwLen);
        printBuf[DISPLAY_COLS] = 0;

        auto bwLen = strlen(m_bwInfo);
        memcpy(printBuf + DISPLAY_COLS - bwLen, m_bwInfo, bwLen);

        m_lcd->setCursor(0, m_line);
        m_lcd->print(printBuf);
    }

    if (m_needCurrentFastRepaint) {
        m_needCurrentFastRepaint = false;
        m_hasCurrentFastRepaint = true;
        m_lcd->setCursor(m_currentPos, m_line);
        m_lcd->print(m_fastCurrentStr);
    }

    if (m_currentLength && !m_hasCurrentFastRepaint) {
        bool newBlinkState = ((gMillis() - m_baseBlinkTime) / 500) % 2;
        newBlinkState ^= m_startBlinkState;

        if (newBlinkState != m_currentBlinkState) {
            m_currentBlinkState = newBlinkState;

            if (newBlinkState) {
                memset(printBuf, ' ', m_currentLength);

                if (m_currentMark) {
                    uint8_t marklen = strlen(m_currentMark);
                    memcpy(printBuf + m_currentLength - marklen, m_currentMark, marklen);
                }
            } else {
                memcpy(printBuf, m_fwInfo + m_currentPos, m_currentLength);
            }

            printBuf[m_currentLength] = 0;
            m_lcd->setCursor(m_currentPos, m_line);
            m_lcd->print(printBuf);
        }
    }
}

void DisplayLine::restore() {
    m_needRepaint = true;
    m_needCurrentFastRepaint = false;
    m_hasCurrentFastRepaint = false;
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

void DisplayLine::fastCurrentRepaint(const char* src) {
    if (!m_currentLength)
        return;

    m_needCurrentFastRepaint = true;
    m_hasCurrentFastRepaint = true;
    strcpy(m_fastCurrentStr, src);
    alignStr(m_fastCurrentStr, m_currentLength);
}
