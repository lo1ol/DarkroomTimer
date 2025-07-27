#include "RelTimeTable.h"

#include <assert.h>

#include "Tools.h"

void RelTimeTable::empty() {
    m_size = 0;
    m_changed = true;
}

void RelTimeTable::resize(uint8_t size) {
    m_size = size;
    m_changed = true;
}

void RelTimeTable::setPrefix(const char* prefix) {
    m_prefix = prefix;
    m_changed = true;
}

void RelTimeTable::setBaseTime(Time time) {
    if (m_size == 0)
        m_size = 1;
    m_base = time;
    m_changed = true;
}

void RelTimeTable::setRelTime(uint8_t id, RelTime time) {
    if (id >= m_size)
        m_size = id + 1;
    m_relTimes[id - 1] = time;
    m_changed = true;
}

void RelTimeTable::setCurrent(uint8_t id) {
    m_currentId = id;
    m_changed = true;
}

void RelTimeTable::flush(bool force) {
    if (!m_changed && !force)
        return;

    m_changed = false;

    gScrollableContent.startNewLine();
    gScrollableContent.print(m_prefix);

    uint8_t id = 0;
    while (id != m_size) {
        char str[DISPLAY_COLS + 1];
        bool current = id == m_currentId;

        if (id == 0) {
            m_base.getFormatedTime(str, current, current);
        } else {
            int8_t alignSize;
            if (current) {
                (m_relTimes[id - 1] ^ m_base).getFormatedTime(str, current, current);
                alignSize = strlen(str);
            }

            auto strLen = m_relTimes[id - 1].toStr(str);
            if (current && strLen < alignSize)
                alignStr(str, alignSize);
        }

        gScrollableContent.print(str, current);
        ++id;
    }
}
