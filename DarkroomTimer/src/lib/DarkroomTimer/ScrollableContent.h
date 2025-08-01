#pragma once

#include "Config.h"

#include <stdint.h>

int8_t getEncoderDir();

class ScrollableContent {
public:
    static constexpr uint8_t kMaxLineCnt = 16;

    void reset();
    void paint();
    void forcePaint();

    void scroll(int8_t dir = getEncoderDir());

    void startNewLine();
    void print(const char* src, bool current = false, const char* mark = nullptr);

    bool currentIsPrinted() const { return m_currentDisplayLine != -1; }
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

    char(m_lines[kMaxLineCnt])[DISPLAY_COLS + 1] = {};

#ifdef PIO_UNIT_TESTING
public:
    struct Desc {
        bool operator==(const Desc& o) const;
        const char* lines[kMaxLineCnt];
        int8_t currentLine;
        int8_t currentShift;
        int8_t currentAlign;
        const char* currentMark;
    };

    Desc getDesc() const;
#endif
};
