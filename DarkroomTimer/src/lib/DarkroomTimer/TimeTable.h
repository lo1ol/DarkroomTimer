#pragma once

#include "Time.h"

// class for effective keep and printing time table
class TimeTable {
public:
    void setBuffer(void* buf, uint8_t size);

    void setPrefix(const char* prefix);

    void flush(bool force = false) const;
    void reset();
    void resize(uint8_t);

    void printBadAsZero(bool);

    void setTime(uint8_t id, Time);
    Time getTime(uint8_t id) const { return m_times[id]; }

    void setCurrent(uint8_t id, const char* mark = nullptr);

    uint8_t capacity() const { return m_capacity; }

private:
    Time* m_times = nullptr;
    uint8_t m_capacity = 0;

    const char* m_prefix = nullptr;
    uint8_t m_size = 0;
    int8_t m_currentId = -1;
    const char* m_currentMark = nullptr;
    mutable bool m_changed = true;
    bool m_printBadAsZero = false;
};
