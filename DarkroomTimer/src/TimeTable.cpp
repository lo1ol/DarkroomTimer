#include "TimeTable.h"

#include <assert.h>

#include "Tools.h"

void TimeTable::empty() {
    m_size = 0;
    m_currentId = -1;
    m_changed = true;
}

void TimeTable::resize(uint8_t size) {
    m_size = size;
    m_changed = true;
}

void TimeTable::setPrefix(const char* prefix) {
    m_prefix = prefix;
    m_changed = true;
}

void TimeTable::setTime(uint8_t id, Time time) {
    assert(id < kTimeTableSize);
    if (id >= m_size)
        m_size = id + 1;
    m_times[id] = time;
    m_changed = true;
}

void TimeTable::printBadAsZero(bool val) {
    m_printBadAsZero = val;
    m_changed = true;
}

void TimeTable::setCurrent(uint8_t id, const char* mark) {
    m_currentId = id;
    m_currentMark = mark;
    m_changed = true;
}

void TimeTable::flush(bool force) {
    if (!m_changed && !force)
        return;

    m_changed = false;

    gScrollableContent.startNewLine();
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

