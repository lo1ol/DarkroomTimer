#pragma once

#include <stdint.h>

#include "RelTime.h"
#include "Time.h"

class DTEncoder {
private:
    DTEncoder() = default;

public:
    static DTEncoder& getInstance();

    [[nodiscard]] int8_t getDir() const;
    [[nodiscard]] bool getBool(bool& choosen) const;
    [[nodiscard]] bool getInt(uint8_t& choosen, uint8_t min, uint8_t max) const;
    [[nodiscard]] bool getTime(Time& time, bool smooth = false) const;
    [[nodiscard]] bool getRelTime(RelTime& time) const;

    void tick();
    void clear();

private:
    [[nodiscard]] int8_t getShift() const;
    [[nodiscard]] int8_t getAceleratedShift(uint8_t factor1, uint8_t factor2) const;

#ifndef PIO_UNIT_TESTING
    static void isr_();
    void isr();
#endif

    int8_t m_turnCounters[3] = {};
    int8_t m_lastEncoderState = 0;
    int8_t m_subPos = 0;
    uint16_t m_lastTurnChangeTime = 0;

    int8_t m_retTurnCounters[3] = {};

#ifdef PIO_UNIT_TESTING
public:
    void emulTurns(int8_t turns) { m_turnCounters[0] += turns; }
    void emulFastTurns(int8_t turns) { m_turnCounters[1] += turns; }
    void emulFastFastTurns(int8_t turns) { m_turnCounters[2] += turns; }

    void emulRetInt(int v) {
        m_reqRetInt = true;
        m_int = v;
    }

    void emulRetTime(const Time& v) {
        m_reqRetTime = true;
        m_time = v;
    }

    void emulRetRelTime(const RelTime& v) {
        m_reqRetRelTime = true;
        m_relTime = v;
    }

private:
    bool m_retInt = false;
    bool m_retTime = false;
    bool m_retRelTime = false;

    int m_int = 0;
    Time m_time{};
    RelTime m_relTime{};

    bool m_reqRetInt = false;
    bool m_reqRetTime = false;
    bool m_reqRetRelTime = false;
#endif
};
