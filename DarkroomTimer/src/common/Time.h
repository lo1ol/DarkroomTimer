#pragma once

#include <math.h>
#include <stdint.h>

class DisplayLine;

class Time {
public:
    constexpr Time() : Time(0) {}
    constexpr explicit Time(uint32_t ts) : m_ts(getTimeWithBoundCheck(ts)) {}

    [[nodiscard]] constexpr explicit operator bool() const { return m_ts; }
    [[nodiscard]] constexpr explicit operator uint16_t() const { return m_ts; }

    [[nodiscard]] constexpr Time operator*(double x) const { return Time{ static_cast<uint32_t>(lround(m_ts * x)) }; }
    [[nodiscard]] constexpr Time operator*(uint16_t x) const { return Time{ m_ts * static_cast<uint32_t>(x) }; }

    [[nodiscard]] constexpr Time operator/(uint16_t n) const { return Time{ static_cast<uint16_t>(m_ts / n) }; }
    [[nodiscard]] constexpr Time operator+(const Time& o) const { return Time{ static_cast<uint32_t>(m_ts) + o.m_ts }; }
    [[nodiscard]] constexpr Time operator-(const Time& o) const { return Time{ static_cast<uint32_t>(m_ts) - o.m_ts }; }

    Time& operator*=(const uint16_t x) {
        auto ts = static_cast<uint32_t>(x) * m_ts;
        m_ts = getTimeWithBoundCheck(ts);

        return *this;
    }

    Time& operator+=(const Time& o) {
        auto ts = static_cast<uint32_t>(o.m_ts) + m_ts;
        m_ts = getTimeWithBoundCheck(ts);
        return *this;
    }

    [[nodiscard]] constexpr bool operator<(const Time& o) const { return m_ts < o.m_ts; }
    [[nodiscard]] constexpr bool operator>(const Time& o) const { return m_ts > o.m_ts; }
    [[nodiscard]] constexpr bool operator==(const Time& o) const { return m_ts == o.m_ts; }
    [[nodiscard]] constexpr bool operator!=(const Time& o) const { return m_ts != o.m_ts; }

    [[nodiscard]] constexpr uint32_t toMillis() const { return m_ts * 100L; }
    [[nodiscard]] constexpr static Time fromMillis(uint32_t ms) {
        return Time{ static_cast<uint32_t>(ms / 100) + static_cast<bool>(ms % 100 > 50) };
    }

    [[nodiscard]] constexpr uint16_t secs() const { return m_ts / 10; }
    [[nodiscard]] constexpr uint8_t tenth() const { return m_ts % 10; }

    void getFormatedTime(char* buf, bool accurate = true, bool addZero = false) const;

    uint16_t getId() const;
    static Time fromId(uint16_t);

private:
    constexpr static uint16_t getTimeWithBoundCheck(uint32_t x) { return (x > 36000) ? UINT16_MAX : x; }

    uint16_t m_ts; // tenth of seconds
};

[[nodiscard]] constexpr inline Time operator*(uint16_t x, const Time& time) {
    return time * x;
}

[[nodiscard]] constexpr inline Time operator*(double x, const Time& time) {
    return time * x;
}

[[nodiscard]] constexpr inline Time operator""_ts(unsigned long long ts) {
    return Time(ts);
}

[[nodiscard]] constexpr inline Time operator""_s(unsigned long long s) {
    return Time(s * 10L);
}

constexpr Time kMaxTime{ 36000 };
constexpr Time kBadTime{ UINT16_MAX };
