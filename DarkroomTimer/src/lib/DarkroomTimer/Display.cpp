#include "Display.h"

#include "LiquidCrystalWrap.h"

Display::Display(LiquidCrystalWrap* lcd) : m_lcd(lcd), m_lines({ DisplayLine(0, m_lcd), DisplayLine(1, m_lcd) }) {}

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
