#include "DisplayLine.h"

#include <Arduino.h>

#include <LiquidCrystal.h>

namespace {
void getFormatedTime(uint32_t ms, char* buf, bool addZero = false) {
    uint16_t decs = ms / 100;
    uint16_t sec = decs / 10;
    uint8_t dec = decs % 10;

    if (sec > 9) {
        itoa(sec + (dec > 4), buf, 10);
        return;
    }

    if (dec == 0 && !addZero) {
        itoa(sec, buf, 10);
        return;
    }

    itoa(sec, buf, 10);
    strcpy(buf + strlen(buf), ".");
    itoa(dec, buf + strlen(buf), 10);
}
} // namespace

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

void DisplayLine::concatTime(char* dst, uint32_t ms, bool addZero) {
    char str[DISPLAY_COLS + 1];
    getFormatedTime(ms, str, addZero);
    concat(dst, str);
}

bool DisplayLine::tryPrintTime(uint32_t ms) {
    char str[DISPLAY_COLS + 1];
    getFormatedTime(ms, str);
    if (strlen(str) + strlen(m_fwInfo) > DISPLAY_COLS)
        return false;

    concat(m_fwInfo, str);
    return true;
}

void DisplayLine::tick() {
    auto fwLen = strlen(m_fwInfo);
    memset(m_fwInfo + fwLen, ' ', DISPLAY_COLS - fwLen);

    auto bwLen = strlen(m_bwInfo);
    memcpy(m_fwInfo + DISPLAY_COLS - bwLen, m_bwInfo, bwLen);

    m_lcd.setCursor(0, m_line);
    m_lcd.print(m_fwInfo);

    m_fwInfo[0] = 0;
    m_bwInfo[0] = 0;
}

DisplayLine& DisplayLine::operator<<(const char* src) {
    concat(m_fwInfo, src);
    return *this;
}

DisplayLine& DisplayLine::operator<<(int value) {
    concatInt(m_fwInfo, value);
    return *this;
}
DisplayLine& DisplayLine::operator<<(uint32_t ms) {
    concatTime(m_fwInfo, ms);
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
DisplayLine& DisplayLine::operator>>(uint32_t ms) {
    concatTime(m_bwInfo, ms, true);
    return *this;
}
