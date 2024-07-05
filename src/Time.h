#pragma once

#include <math.h>
#include <stdint.h>

class DisplayLine;

class Time {
public:
    constexpr Time() : Time(0) {}
    constexpr explicit Time(int16_t ts) : m_ts(ts) {}

    explicit operator bool() { return m_ts; }
    explicit operator int16_t() { return m_ts; }
    explicit operator uint32_t() { return m_ts; }

    Time operator*(double x) const { return Time{ static_cast<int16_t>(lround(m_ts * x)) }; }
    Time operator*(int16_t x) const { return Time{ m_ts * x }; }
    Time operator/(int n) const { return Time{ m_ts / n }; }
    Time operator+(const Time& o) const { return Time{ m_ts + o.m_ts }; }
    Time operator-() const { return Time{ -m_ts }; }

    Time& operator*=(const int16_t x) {
        m_ts *= x;
        return *this;
    }

    Time& operator+=(const Time& o) {
        m_ts += o.m_ts;
        return *this;
    }

    bool operator<(const Time& o) const { return m_ts < o.m_ts; }
    bool operator>(const Time& o) const { return m_ts > o.m_ts; }
    bool operator==(const Time& o) const { return m_ts == o.m_ts; }
    bool operator!=(const Time& o) const { return m_ts != o.m_ts; }

    int32_t toMillis() const { return m_ts * 100L; }
    static Time fromMillis(uint32_t ms) {
        int16_t ts = static_cast<int16_t>(ms / 100);
        return Time{ ts + (ms % 100 > 50) };
    }

    int16_t secs() const { return m_ts / 10; }
    uint8_t tenth() const { return m_ts % 10; }

    void getFormatedTime(char* buf, bool accurate = true, bool addZero = false) const;

private:
    int16_t m_ts; // tenth of seconds
};

inline Time operator*(int16_t x, const Time& time) {
    return time * x;
}

DisplayLine& operator<<(DisplayLine&, const Time&);
DisplayLine& operator>>(DisplayLine&, const Time&);

constexpr inline Time operator""_ts(unsigned long long ts) {
    return Time(ts);
}
