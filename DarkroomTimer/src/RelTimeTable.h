#pragma once

#include "RelTime.h"

// class for effective keep and printing time table
class RelTimeTable {
public:
    void setBuffer(void* buf, uint8_t size);

    void setPrefix(const char* prefix);

    void flush(bool force = false) const;
    void reset();
    void resize(uint8_t);
    uint8_t size() const { return m_size; }

    void setBaseTime(Time);
    void setRelTime(uint8_t id, RelTime);
    Time getBaseTime() const { return m_base; }
    RelTime getRelTime(uint8_t id) const { return m_relTimes[id]; }
    Time getTime(int8_t id) const;

    void setCurrent(uint8_t id);

    void toggleSecView();
    void setSecView(bool);

    uint8_t capacity() const { return m_capacity; }

private:
    RelTime* m_relTimes;
    uint8_t m_capacity;

    const char* m_prefix = nullptr;
    uint8_t m_size = 0;
    int8_t m_currentId = -2; // -1 is used to identify base
    mutable bool m_changed = true;
    bool m_secView = false;

    Time m_base;
};
