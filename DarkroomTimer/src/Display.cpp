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

void Display::setupCharset(Charset charset) {
#define ADD_CUSTOM_CHAR(symStr, symMatrix) m_lcd->addCustomChar(static_cast<uint8_t>(symStr[0]) - 0x80, symMatrix)
    switch (charset) {
    case Charset::Main:
        ADD_CUSTOM_CHAR(kLampSymTop, kLampSymTopMatrix);
        ADD_CUSTOM_CHAR(kLampSymBottom, kLampSymBottomMatrix);
        ADD_CUSTOM_CHAR(kWrenchSymTop, kWrenchSymTopMatrix);
        ADD_CUSTOM_CHAR(kWrenchSymBottom, kWrenchSymBottomMatrix);
        break;
    }
#undef ADD_CUSTOM_CHAR
}

#ifndef PIO_UNIT_TESTING

void Display::playLogo() {
    reset();
    tick();

    m_lines[0].printWithAnimation(F("    RED RAY"), 2000);

    while (m_lines[0].animationIsRunning())
        m_lines[0].tick();

    delay(800);

    m_lines[1] << F("  Let's print   ");
    m_lines[1].tick();
    delay(1400);

    reset();
}
#endif
