#include "Display.h"

namespace {
uint8_t printTimeLogOnLine(const char* prefix, uint32_t (*timeGetter)(uint8_t), uint8_t maxIndex, uint8_t index,
                           DisplayLine& dl) {
    dl << prefix;

    while (true) {
        if (index == maxIndex)
            break;

        if (dl.tryPrintTime(timeGetter(index)))
            return index;
        dl << " ";
        ++index;
    }

    return index;
}

} // namespace

Display::Display(LiquidCrystal&& lcd) : m_lcd(lcd), m_lines({ DisplayLine(0, m_lcd), DisplayLine(1, m_lcd) }) {
    m_lcd.begin(DISPLAY_COLS, DISPLAY_ROWS);
}

void Display::printTimeLog(const char* prefix, uint32_t (*timeGetter)(uint8_t), uint8_t maxIndex) {
    uint8_t lastNonPrintedIndex = 0;
    for (auto&& line : m_lines) {
        lastNonPrintedIndex = printTimeLogOnLine(prefix, timeGetter, maxIndex, lastNonPrintedIndex, line);
        prefix = "";
    }
}

void Display::tick() {
    for (auto&& line : m_lines)
        line.tick();
}
