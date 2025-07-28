#include "RelTimeTable.h"

#include <assert.h>

#include "Tools.h"

void RelTimeTable::reset() {
    m_size = 0;
    m_currentId = -2;
    m_secView = false;
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
    m_base = time;
    m_changed = true;
}

void RelTimeTable::setRelTime(uint8_t id, RelTime time) {
    assert(id < kTableSize);
    if (id >= m_size)
        m_size = id + 1;
    m_relTimes[id] = time;
    m_changed = true;
}

Time RelTimeTable::getTime(int8_t id) const {
    assert(id < m_size && id > -2);
    if (id == -1)
        return m_base;

    return m_relTimes[id] ^ m_base;
}

void RelTimeTable::toggleSecView() {
    m_secView = !m_secView;
    m_changed = true;
}

void RelTimeTable::setSecView(bool val) {
    m_secView = val;
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

    int8_t id = -1;
    while (id != m_size) {
        char str[DISPLAY_COLS + 1];
        bool current = id == m_currentId;

        if (m_secView) {
            getTime(id).getFormatedTime(str, current, current);
        } else {
            if (id == -1) {
                m_base.getFormatedTime(str, current, current);
            } else {
                int8_t alignSize;
                if (current) {
                    getTime(id).getFormatedTime(str, current, current);
                    alignSize = strlen(str);
                }

                auto strLen = m_relTimes[id].toStr(str);
                if (current && strLen < alignSize)
                    alignStr(str, alignSize);
            }
        }

        gScrollableContent.print(str, current);
        ++id;
    }
}
