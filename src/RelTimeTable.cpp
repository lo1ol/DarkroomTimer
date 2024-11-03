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

    uint8_t id = 0;
    bool firstLine = true;

    while (true) {
        gScrollableContent.startNewLine();

        uint8_t lineLen = 0;
        if (firstLine) {
            firstLine = false;
            gScrollableContent.print(m_prefix);

            lineLen = strlen(m_prefix);
        }

        while (true) {
            if (id == m_size) {
                return;
            }

            char str[DISPLAY_COLS + 1];
            uint8_t strLen;
            bool current = id == m_currentId;

            if (id == 0) {
                m_base.getFormatedTime(str, current, current);
                strLen = strlen(str);
                if (current)
                    strLen = strLen;
            } else {
                if (current) {
                    (m_relTimes[id - 1] ^ m_base).getFormatedTime(str, current, current);
                    int alignSize = strlen(str);
                    strLen = m_relTimes[id - 1].toStr(str);
                    if (strLen < alignSize) {
                        strLen = alignSize;
                        alignStr(str, strLen);
                    }
                } else {
                    strLen = m_relTimes[id - 1].toStr(str);
                }
            }

            if (lineLen + strLen > DISPLAY_COLS)
                break;

            gScrollableContent.print(str, current);
            lineLen += strLen;
            ++id;

            if (lineLen + 1 > DISPLAY_COLS)
                break;

            gScrollableContent.print(" ");
            ++lineLen;
        }
    }
}
