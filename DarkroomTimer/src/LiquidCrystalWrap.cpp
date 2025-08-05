#include "LiquidCrystalWrap.h"

#include <LiquidCrystal.h>

#include "Config.h"

LiquidCrystalWrap::LiquidCrystalWrap(LiquidCrystal* lcd) : m_lcd(lcd) {
    if (m_lcd)
        m_lcd->begin(DISPLAY_COLS, DISPLAY_ROWS);
}

void LiquidCrystalWrap::setCursor(uint8_t c, uint8_t r) {
    assert(m_lcd);
    m_lcd->setCursor(c, r);
}
void LiquidCrystalWrap::print(const char* str) {
    assert(m_lcd);
    m_lcd->print(str);
}
