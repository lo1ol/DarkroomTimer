#pragma once

#include "Config.h"

#include <stdint.h>

class ScrollableContent {
public:
    void reset();
    void paint();
    void forcePaint();

    void scroll();

    void startNewLine();
    void print(const char* src, bool current = false, const char* mark = nullptr);

    bool currentIsPrinted() const { return m_currentLine != -1; }
    uint8_t lineCnt() const { return m_line + !m_lastLineIsEmpty; }

private:
    void paintUnchanged();

    bool m_lastLineIsEmpty = true;
    uint8_t m_line = 0;
    const char* m_currentMark = nullptr;
    int8_t m_currentDisplayLine = -1;
    int8_t m_currentLine = -1;
    int8_t m_currentShift;
    int8_t m_currentAlign;
    bool m_changed = true;
    bool m_printBadAsZero = false;

    uint8_t m_fistPrintedLine = 0;
    bool m_needGoToCurrent = false;

    char m_lines[DISPLAY_COLS + 1][16];
};
