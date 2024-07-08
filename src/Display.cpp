#include "Display.h"

Display::Display(LiquidCrystal&& lcd) : m_lcd(lcd), m_lines({ DisplayLine(0, m_lcd), DisplayLine(1, m_lcd) }) {
    m_lcd.begin(DISPLAY_COLS, DISPLAY_ROWS);
}

void Display::tick() {
    for (auto&& line : m_lines)
        line.tick();
}

void Display::reset() {
    for (auto&& line : m_lines)
        line.reset();
}

void Display::resetBlink(bool state) {
    for (auto&& line : m_lines)
        line.resetBlink(state);
}
