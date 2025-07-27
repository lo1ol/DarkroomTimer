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
    uint8_t lineCnt() const { return m_line + static_cast<bool>(m_lastLineLength); }

private:
    void paintUnchanged();

    uint8_t m_lastLineLength = 0;
    uint8_t m_line = 0;
    const char* m_currentMark = nullptr;
    int8_t m_currentDisplayLine = -1;
    int8_t m_currentLine = -1;
    int8_t m_currentShift = 0;
    int8_t m_currentAlign = 0;
    bool m_changed = true;

    uint8_t m_firstPrintedLine = 0;
    bool m_needGoToCurrent = false;

    char m_lines[DISPLAY_COLS + 1][16] = {};
};
