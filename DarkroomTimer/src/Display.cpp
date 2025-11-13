#include "Display.h"

#include "Lcd.h"

Display::Display(Lcd* lcd) : m_lcd(lcd) {
    int i = 0;
    for (auto& line : m_lines)
        line = DisplayLine(i++, m_lcd);
}

void Display::tick() {
    for (auto&& line : m_lines)
        line.tick();
}

void Display::reset() {
    for (auto&& line : m_lines)
        line.reset();
}

void Display::resetBlink(bool showBlinked) {
    for (auto&& line : m_lines)
        line.resetBlink(showBlinked);
}

#ifndef PIO_UNIT_TESTING

namespace {
constexpr uint16_t kAnimPhase1Time = 400;
constexpr uint8_t kAnimPhase1ElemCnt = 5;
constexpr uint16_t kAnimPhase2Time = 1600;
constexpr uint8_t kAnimPhase2ElemCnt = 9;
constexpr uint16_t kAnimTime = kAnimPhase1Time + kAnimPhase2Time;
constexpr uint8_t kAnimElemCnt = kAnimPhase1ElemCnt + kAnimPhase2ElemCnt;

constexpr uint16_t kAnimPhase1StageTime = kAnimPhase1Time / kAnimPhase1ElemCnt;
constexpr uint16_t kAnimPhase2StageTime = kAnimPhase2Time / kAnimPhase2ElemCnt;

constexpr const char kRevealStr[] = "    RED RAY     ";
constexpr uint8_t kRevealOrder[] PROGMEM = { 2, 14, 10, 1, 7, 4, 12, 5, 13, 6, 11, 8, 3, 9 };
static_assert(sizeof(kRevealOrder) == kAnimElemCnt, "Bad elem reveal cnt");
static_assert(sizeof(kRevealStr) == DISPLAY_COLS + 1, "Bad reveal str");
} // namespace

void Display::playLogo() {
    reset();
    tick();

    uint16_t startTime = millis();
    uint16_t seed = analogRead(A6);

    char buf[DISPLAY_COLS + 1];
    buf[0] = ' ';
    buf[DISPLAY_COLS - 1] = ' ';
    buf[DISPLAY_COLS] = 0;

    while (true) {
        auto t = millis() - startTime;
        if (t > kAnimTime)
            break;

        m_lines[0].reset();

        for (uint8_t c = 1; c != DISPLAY_COLS - 1; ++c)
            buf[c] = 32 + (seed + micros()) % 95; // Print random ASCII char

        uint8_t revealStage;
        if (t < kAnimPhase1Time)
            revealStage = t / kAnimPhase1StageTime;
        else if (t < kAnimPhase1Time + kAnimPhase2Time)
            revealStage = kAnimPhase1ElemCnt + (t - kAnimPhase1Time) / kAnimPhase2StageTime;

        for (uint8_t c = 0; c != revealStage; ++c) {
            uint8_t id = pgm_read_byte(kRevealOrder + c);
            buf[id] = kRevealStr[id];
        }

        m_lines[0] << buf;
        m_lines[0].tick();

        delay(50);
    }

    m_lines[0].reset();
    m_lines[0] << kRevealStr;
    m_lines[0].tick();

    delay(800);

    m_lines[1] << F("  Let's print   ");
    m_lines[1].tick();
    delay(1400);

    reset();
}
#endif
