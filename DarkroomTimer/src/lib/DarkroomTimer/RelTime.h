#pragma once

#include "Time.h"

class RelTime {
public:
    constexpr RelTime() : RelTime(0) {}
    constexpr explicit RelTime(uint8_t id) : m_id(id) {}

    Time operator^(Time) const;
    uint8_t toStr(char* buf) const;

    constexpr uint8_t getId() const { return m_id; }

private:
    uint8_t m_id;
};

constexpr RelTime kMaxRelTime = RelTime(5 * 12);
