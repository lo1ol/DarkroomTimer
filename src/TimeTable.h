#pragma once

#include "Time.h"

// class for effective keep and printing time table
class TimeTable {
public:
    static constexpr uint8_t kTimeTableSize = 32;

    void setPrefix(const char* prefix);

    void paint();
    void forcePaint();
    void empty();
    void resize(uint8_t);

    void printBadAsZero(bool);

    void setTime(uint8_t id, Time);
    Time getTime(uint8_t id) const { return m_times[id]; }

    void setCurrent(uint8_t id, const char* mark = nullptr);
    bool currentIsPrinted() const { return m_currentLine != -1; }

    void scroll();

private:
    void paintUnchanged();

    const char* m_prefix;
    uint8_t m_size = 0;
    int8_t m_currentId = -1;
    const char* m_currentMark = nullptr;
    int8_t m_currentDisplayLine = -1;
    int8_t m_currentLine = -1;
    int8_t m_currentShift;
    int8_t m_currentAlign;
    bool m_changed = true;
    bool m_printBadAsZero = false;

    uint8_t m_fistPrintedLine = 0;
    uint8_t m_maxLineNum = 0;
    bool m_needGoToCurrent = false;

    Time m_times[kTimeTableSize];
};
