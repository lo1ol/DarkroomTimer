#pragma once

#include "RelTime.h"

// class for effective keep and printing time table
class RelTimeTable {
public:
    static constexpr uint8_t kTableSize = 10;

    void setPrefix(const char* prefix);

    void flush(bool force = false);
    void empty();
    void resize(uint8_t);
    uint8_t size() const { return m_size; }

    void setBaseTime(Time);
    void setRelTime(uint8_t id, RelTime);
    Time getBaseTime() const { return m_base; }
    RelTime getRelTime(uint8_t id) const { return m_relTimes[id - 1]; }

    void setCurrent(uint8_t id);

private:
    const char* m_prefix = nullptr;
    uint8_t m_size = 0;
    int8_t m_currentId = -1;
    int8_t m_currentAlign = 0;
    bool m_changed = true;

    Time m_base;
    RelTime m_relTimes[kTableSize];
};
