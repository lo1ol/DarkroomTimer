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
