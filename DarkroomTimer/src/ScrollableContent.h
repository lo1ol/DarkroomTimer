#pragma once

#include "Config.h"

#include <stdint.h>

class ScrollableContent {
public:
    static constexpr uint8_t kMaxLineCnt = 16;

    void reset();
    void paint();
    void forcePaint();

    void scroll(int8_t dir);

    void startNewLine();
    void print(const char* src, bool current = false, const char* mark = nullptr);

    [[nodiscard]] bool currentIsPrinted() const { return m_currentDisplayLine != -1; }
    [[nodiscard]] bool hasCurrent() const { return m_currentLine != -1; }
    [[nodiscard]] uint8_t lineCnt() const { return m_line + static_cast<bool>(m_lastLineLength); }

    void goToFirstLine() { m_firstPrintedLine = 0; }

private:
    void paintUnchanged();
    void updateFirstPrintingLine();

#ifdef PIO_UNIT_TESTING
public:
#endif
    uint8_t m_lastLineLength = 0;
    uint8_t m_line = 0;
    const char* m_currentMark = nullptr;
    int8_t m_currentDisplayLine = -1;
    int8_t m_currentLine = -1;
    int8_t m_currentShift = 0;
    int8_t m_currentAlign = 0;
    bool m_changed = false;

    uint8_t m_firstPrintedLine = 0;
    bool m_needGoToCurrent = false;

    char(m_lines[kMaxLineCnt])[DISPLAY_COLS + 1] = {};

#ifdef PIO_UNIT_TESTING
public:
    struct Desc {
        const char* lines[kMaxLineCnt];
        int8_t currentLine;
        int8_t currentShift;
        int8_t currentAlign;
        const char* currentMark;
    };

    [[nodiscard]] Desc getDesc() const;
#endif
};
