#include "DisplayLine.h"

#include "Hardware.h"
#include "Lcd.h"
#include "Utils.h"

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
    m_showingAnimation = false;

    if (current) {
        m_currentLength = strlen(src);
        m_currentPos = strlen(m_fwInfo);
        m_currentMark = mark;
    }

    concat(m_fwInfo, src);
}

void DisplayLine::reset() {
    m_needRepaint = true;
    m_showingAnimation = false;
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

    if (m_showingAnimation) {
        tickAnimation();
        return;
    }

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

#ifdef PIO_UNIT_TESTING
void DisplayLine::tickAnimation() {
    m_showingAnimation = false;
    m_lcd->setCursor(0, m_line);
    m_lcd->print(m_fwInfo);
}

#else
void DisplayLine::tickAnimation() {
    uint16_t currentTime = gMillis();
    if (currentTime - m_lastAnimationUpdateTime < kAnimationUpdateRate)
        return;

    static uint16_t gSeed = analogRead(A6);

    char buf[DISPLAY_COLS + 1];
    buf[DISPLAY_COLS] = 0;

    auto t = currentTime - m_animationStartTime;
    if (t >= m_animationTime) {
        m_showingAnimation = false;
        m_lcd->setCursor(0, m_line);
        m_lcd->print(m_fwInfo);
        return;
    }

    for (uint8_t c = 0; c != DISPLAY_COLS; ++c)
        buf[c] = 32 + (gSeed + micros()) % 95; // Print random ASCII char

    uint8_t revealStage = t * DISPLAY_COLS / m_animationTime;
    constexpr uint8_t kRevealOrder[DISPLAY_COLS] = { 0, 15, 2, 14, 10, 1, 7, 4, 12, 5, 13, 6, 11, 8, 3, 9 };

    for (uint8_t c = 0; c != revealStage; ++c) {
        uint8_t id = kRevealOrder[c];
        buf[id] = m_fwInfo[id];
    }

    m_lcd->setCursor(0, m_line);
    m_lcd->print(buf);
    m_lastAnimationUpdateTime = currentTime;
}
#endif

void DisplayLine::printWithAnimation(const __FlashStringHelper* src, uint16_t time) {
    m_showingAnimation = true;
    strncpy_P(m_fwInfo, (PGM_P)src, sizeof(m_fwInfo));

    auto len = strlen(m_fwInfo);
    memset(m_fwInfo + len, ' ', DISPLAY_COLS - len);
    m_fwInfo[DISPLAY_COLS] = 0;

    m_animationTime = time;
    m_animationStartTime = gMillis();
    m_lastAnimationUpdateTime = gMillis() - kAnimationUpdateRate;
}

void DisplayLine::restore() {
    m_needRepaint = true;
    m_needCurrentFastRepaint = false;
    m_hasCurrentFastRepaint = false;
}

DisplayLine& DisplayLine::operator<<(const char* src) {
    m_needRepaint = true;
    m_showingAnimation = false;
    concat(m_fwInfo, src);
    return *this;
}

DisplayLine& DisplayLine::operator<<(int value) {
    m_needRepaint = true;
    m_showingAnimation = false;
    concatInt(m_fwInfo, value);
    return *this;
}

DisplayLine& DisplayLine::operator>>(const char* src) {
    m_needRepaint = true;
    m_showingAnimation = false;
    concat(m_bwInfo, src);
    return *this;
}

DisplayLine& DisplayLine::operator>>(int value) {
    m_needRepaint = true;
    m_showingAnimation = false;
    concatInt(m_bwInfo, value);
    return *this;
}

#ifndef DT_NATIVE
DisplayLine& DisplayLine::operator<<(const __FlashStringHelper* src) {
    char buf[DISPLAY_COLS + 1];
    strncpy_P(buf, (PGM_P)src, sizeof(buf));
    return *this << buf;
}

DisplayLine& DisplayLine::operator>>(const __FlashStringHelper* src) {
    char buf[DISPLAY_COLS + 1];
    strncpy_P(buf, (PGM_P)src, sizeof(buf));
    return *this >> buf;
}
#endif

void DisplayLine::fastCurrentRepaint(const char* src) {
    if (!m_currentLength)
        return;

    m_needCurrentFastRepaint = true;
    m_hasCurrentFastRepaint = true;
    strcpy(m_fastCurrentStr, src);
    alignStr(m_fastCurrentStr, m_currentLength);
}
