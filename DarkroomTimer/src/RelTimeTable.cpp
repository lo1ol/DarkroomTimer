#include "RelTimeTable.h"

#include <assert.h>

#include "Hardware.h"
#include "Utils.h"

void RelTimeTable::setBuffer(void* buf, uint8_t size) {
    m_relTimes = reinterpret_cast<RelTime*>(buf);
    m_capacity = size / sizeof(RelTime);
    reset();
}

void RelTimeTable::reset() {
    m_size = 0;
    m_currentId = -2;
    m_secView = false;
    m_prefix = nullptr;
}

void RelTimeTable::resize(uint8_t size) {
    m_size = size;

    if (m_currentId >= m_size)
        m_currentId = -2;
}

void RelTimeTable::setPrefix(const char* prefix) {
    m_prefix = prefix;
}

void RelTimeTable::setBaseTime(Time time) {
    m_base = time;
}

void RelTimeTable::setRelTime(uint8_t id, RelTime time) {
    assert(id < m_capacity);
    if (id >= m_size)
        m_size = id + 1;
    m_relTimes[id] = time;
}

Time RelTimeTable::getTime(int8_t id) const {
    assert(id < m_size && id > -2);
    if (id == -1)
        return m_base;

    return m_relTimes[id] ^ m_base;
}

void RelTimeTable::toggleSecView() {
    m_secView = !m_secView;
}

void RelTimeTable::setSecView(bool val) {
    m_secView = val;
}

void RelTimeTable::setCurrent(uint8_t id) {
    m_currentId = id;
}

void RelTimeTable::flush() const {
    gScrollableContent.startNewLine();
    if (m_prefix && m_prefix[0])
        gScrollableContent.print(m_prefix);

    int8_t id = -1;
    while (id != m_size) {
        char str[DISPLAY_COLS + 1];
        bool current = id == m_currentId;
        const char* mark = nullptr;
        Time t = getTime(id);
        bool isBadTime = t == kBadTime;

        if (isBadTime) {
            t = 0_s;
            if (current)
                mark = "ovr";

            strcpy(str, "ovr");
        }

        if (m_secView || id == -1) {
            if (!isBadTime || current)
                t.getFormatedTime(str, current, current);
        } else {
            uint8_t alignSize = 0;
            uint8_t strLen = 0;

            if (isBadTime) {
                alignSize = 3;
                strLen = 3;
            } else if (current) {
                t.getFormatedTime(str, current, current);
                alignSize = strlen(str);
            }

            if (!isBadTime || current)
                strLen = m_relTimes[id].toStr(str);

            if (current && strLen < alignSize)
                alignStr(str, alignSize);
        }

        gScrollableContent.print(str, current, mark);
        ++id;
    }
}
