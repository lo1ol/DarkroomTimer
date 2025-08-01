#include "TimeTable.h"

#include <assert.h>

#include "Tools.h"

void TimeTable::setBuffer(void* buf, uint8_t size) {
    m_times = reinterpret_cast<Time*>(buf);
    m_capacity = size / sizeof(Time);
    reset();
}

void TimeTable::reset() {
    m_size = 0;
    m_currentId = -1;
    m_prefix = nullptr;
}

void TimeTable::resize(uint8_t size) {
    m_size = size;
    if (m_currentId >= m_size)
        m_currentId = -1;
}

void TimeTable::setPrefix(const char* prefix) {
    m_prefix = prefix;
}

void TimeTable::setTime(uint8_t id, Time time) {
    assert(id < m_capacity);
    if (id >= m_size)
        m_size = id + 1;
    m_times[id] = time;
}

void TimeTable::printBadAsZero(bool val) {
    m_printBadAsZero = val;
}

void TimeTable::setCurrent(uint8_t id, const char* mark) {
    m_currentId = id;
    m_currentMark = mark;
}

void TimeTable::flush() const {
    gScrollableContent.startNewLine();
    if (m_prefix && m_prefix[0])
        gScrollableContent.print(m_prefix);

    uint8_t id = 0;

    while (id != m_size) {
        char str[DISPLAY_COLS + 1];
        bool current = id == m_currentId;
        const char* mark = current ? m_currentMark : nullptr;

        auto time = m_times[id];

        if (time == kBadTime) {
            if (m_printBadAsZero)
                time = 0_s;
            else
                return;
        }

        time.getFormatedTime(str, current, current);
        gScrollableContent.print(str, current, mark);
        ++id;
    }
}

