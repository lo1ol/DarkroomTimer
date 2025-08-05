#pragma once

#include <math.h>
#include <stdint.h>

class DisplayLine;

class Time {
public:
    constexpr Time() : Time(0) {}
    constexpr explicit Time(int32_t ts) : m_ts((ts > INT16_MAX || ts < INT16_MIN) ? INT16_MAX : ts) {}

    [[nodiscard]] constexpr explicit operator bool() const { return m_ts; }
    [[nodiscard]] constexpr explicit operator int16_t() const { return m_ts; }

    [[nodiscard]] Time operator*(double x) const {
        int32_t res = lround(m_ts * x);
        if (res > INT16_MAX)
            return Time{ INT16_MAX };

        return Time{ static_cast<int16_t>(res) };
    }

    [[nodiscard]] constexpr Time operator*(int16_t x) const { return Time{ m_ts * x }; }
    [[nodiscard]] constexpr Time operator/(int n) const { return Time{ m_ts / n }; }
    [[nodiscard]] constexpr Time operator+(const Time& o) const { return Time{ m_ts + o.m_ts }; }
    [[nodiscard]] constexpr Time operator-(const Time& o) const { return Time{ m_ts - o.m_ts }; }
    [[nodiscard]] constexpr Time operator-() const { return Time{ -m_ts }; }

    Time& operator*=(const int16_t x) {
        m_ts *= x;
        return *this;
    }

    Time& operator+=(const Time& o) {
        m_ts += o.m_ts;
        return *this;
    }

    [[nodiscard]] constexpr bool operator<(const Time& o) const { return m_ts < o.m_ts; }
    [[nodiscard]] constexpr bool operator>(const Time& o) const { return m_ts > o.m_ts; }
    [[nodiscard]] constexpr bool operator==(const Time& o) const { return m_ts == o.m_ts; }
    [[nodiscard]] constexpr bool operator!=(const Time& o) const { return m_ts != o.m_ts; }

    [[nodiscard]] constexpr int32_t toMillis() const { return m_ts * 100L; }
    [[nodiscard]] static Time fromMillis(int32_t ms) {
        int16_t ts = static_cast<int16_t>(ms / 100);
        return Time{ ts + (ms % 100 > 50) };
    }

    [[nodiscard]] constexpr int16_t secs() const { return m_ts / 10; }
    [[nodiscard]] constexpr uint8_t tenth() const { return m_ts % 10; }

    void getFormatedTime(char* buf, bool accurate = true, bool addZero = false) const;

private:
    int16_t m_ts; // tenth of seconds
};

[[nodiscard]] inline Time operator*(int16_t x, const Time& time) {
    return time * x;
}

DisplayLine& operator<<(DisplayLine&, const Time&);
DisplayLine& operator>>(DisplayLine&, const Time&);

[[nodiscard]] constexpr inline Time operator""_ts(unsigned long long ts) {
    return Time(ts);
}

[[nodiscard]] constexpr inline Time operator""_s(unsigned long long s) {
    return Time(s * 10L);
}

constexpr Time kMaxTime{ INT16_MAX - 1 };
constexpr Time kBadTime{ INT16_MAX };
