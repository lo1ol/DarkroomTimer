#pragma once

#include "Time.h"

// class for effective keep and printing time table
class TimeTable {
public:
    static constexpr uint8_t kTimeTableSize = 16;

    void setPrefix(const char* prefix);

    void flush(bool force = false);
    void reset();
    void resize(uint8_t);

    void printBadAsZero(bool);

    void setTime(uint8_t id, Time);
    Time getTime(uint8_t id) const { return m_times[id]; }

    void setCurrent(uint8_t id, const char* mark = nullptr);

    void scroll();

private:
    const char* m_prefix = nullptr;
    uint8_t m_size = 0;
    int8_t m_currentId = -1;
    const char* m_currentMark = nullptr;
    bool m_changed = true;
    bool m_printBadAsZero = false;

    Time m_times[kTimeTableSize];
};
