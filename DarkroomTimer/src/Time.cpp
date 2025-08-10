#include "Time.h"

#include "Config.h"
#include "DisplayLine.h"
#include "Hardware.h"

void Time::getFormatedTime(char* buf, bool accurate, bool addZero) const {
    uint16_t sec = secs();
    uint8_t dec = tenth();

    if (!accurate && sec > 19) {
        itoa(sec + (dec > 4), buf, 10);
        return;
    }

    if (dec == 0 && !addZero) {
        itoa(sec, buf, 10);
        return;
    }

    itoa(sec, buf, 10);
    strcpy(buf + strlen(buf), ".");
    itoa(dec, buf + strlen(buf), 10);
}

DisplayLine& operator<<(DisplayLine& dl, const Time& time) {
    char str[DISPLAY_COLS + 1];
    time.getFormatedTime(str);
    return dl << str;
}

DisplayLine& operator>>(DisplayLine& dl, const Time& time) {
    char str[DISPLAY_COLS + 1];
    time.getFormatedTime(str, true, true);
    return dl >> str;
}

namespace {
// Values are changing with accuracy 1/12 stop
// You can check accuracy range by this formule:
// t*(2**(1/12)-1)
constexpr uint16_t k10sId = 100;
constexpr uint16_t k20sId = k10sId + (200 - 100) / 5;
constexpr uint16_t k100sId = k20sId + (1000 - 200) / 10;
constexpr uint16_t k200sId = k100sId + (2000 - 1000) / 50;
constexpr uint16_t k1000sId = k200sId + (10000 - 2000) / 100;
constexpr uint16_t k2000sId = k1000sId + (20000 - 10000) / 500;
constexpr uint16_t k3600sId = k2000sId + (36000 - 20000) / 1000;
} // namespace

uint16_t Time::getId() const {
    if (m_ts == kBadTime.m_ts)
        return 0;

    if (m_ts < 0)
        return 0;
    if (m_ts <= 100)
        return m_ts;
    if (m_ts <= 200)
        return k10sId + (m_ts - 100) / 5;
    if (m_ts <= 1000)
        return k20sId + (m_ts - 200) / 10;
    if (m_ts <= 2000)
        return k100sId + (m_ts - 1000) / 50;
    if (m_ts <= 10000)
        return k200sId + (m_ts - 2000) / 100;
    if (m_ts <= 20000)
        return k1000sId + (m_ts - 10000) / 500;
    if (m_ts <= 36000)
        return k2000sId + (m_ts - 20000) / 1000;

    return k3600sId;
}

Time Time::fromId(uint16_t id) {
    Time t;

    if (id <= 0)
        t = 0_s;
    else if (id <= k10sId)
        t.m_ts = id;
    else if (id <= k20sId)
        t.m_ts = 100 + (id - k10sId) * 5;
    else if (id <= k100sId)
        t.m_ts = 200 + (id - k20sId) * 10;
    else if (id <= k200sId)
        t.m_ts = 1000 + (id - k100sId) * 50;
    else if (id <= k1000sId)
        t.m_ts = 2000 + (id - k200sId) * 100;
    else if (id <= k2000sId)
        t.m_ts = 10000 + (id - k1000sId) * 500;
    else if (id <= k3600sId)
        t.m_ts = 20000 + (id - k2000sId) * 1000;
    else
        t = kMaxTime;

    return t;
}

