#include "DTEncoder.h"

#include <Arduino.h>

#include "Config.h"
#include "Hardware.h"

#ifndef PIO_UNIT_TESTING
void DTEncoder::isr() {
    uint8_t state = (PIND >> 2) & 0x03;
    uint8_t transition = (m_lastEncoderState << 2) | state;

    // clang-format off
    static const int8_t gGrayCodeTable[16] = {
        0, -ENCODER_DIRECTION, ENCODER_DIRECTION, 0,
        ENCODER_DIRECTION, 0, 0, -ENCODER_DIRECTION,
        -ENCODER_DIRECTION, 0, 0, ENCODER_DIRECTION,
        0, ENCODER_DIRECTION, -ENCODER_DIRECTION, 0 };
    //clang-format on

    m_subPos += gGrayCodeTable[transition];
    m_lastEncoderState = state;

    // check we made one click
    if (m_lastEncoderState != 0x03 || m_subPos == 0)
        return;

    int8_t dir = (m_subPos > 0) ? 1 : -1;
    m_subPos = 0;

    uint16_t curTime = gMillis();
    if (static_cast<uint16_t>(curTime - m_lastTurnChangeTime) < ENCODER_FAST_FAST_TIMEOUT)
        m_turnCounters[2] += dir;
    else if (static_cast<uint16_t>(curTime - m_lastTurnChangeTime) < ENCODER_FAST_TIMEOUT)
        m_turnCounters[1] += dir;
    else
        m_turnCounters[0] += dir;

    m_lastTurnChangeTime = curTime;
}

void DTEncoder::isr_() {
    getInstance().isr();
}
#endif

DTEncoder& DTEncoder::getInstance() {
    static bool gInited = false;
    static DTEncoder gInstance;

    if (!gInited) {
        gInited = true;

#ifndef PIO_UNIT_TESTING 
        pinMode(ENCODER_DT, INPUT_PULLUP);
        pinMode(ENCODER_CLK, INPUT_PULLUP);
        attachInterrupt(digitalPinToInterrupt(ENCODER_DT), isr_, CHANGE);
        attachInterrupt(digitalPinToInterrupt(ENCODER_CLK), isr_, CHANGE);
        gInstance.m_lastEncoderState = (PIND >> 2) & 0x03;
#endif
    }

    return gInstance;
}

int8_t DTEncoder::getDir() const {
    int8_t turns = getShift();

    if (!turns)
        return 0;

    if (turns > 0)
        return 1;

    return -1;
}

int8_t DTEncoder::getShift() const {
    return m_retTurnCounters[0] + m_retTurnCounters[1] + m_retTurnCounters[2];
}

int8_t DTEncoder::getAceleratedShift(uint8_t factor1, uint8_t factor2) const {
    return m_retTurnCounters[0] + m_retTurnCounters[1] * factor1 + m_retTurnCounters[2] * factor2;
}

void DTEncoder::clear() {
    memset(m_retTurnCounters, 0, sizeof(m_retTurnCounters));

#ifdef PIO_UNIT_TESTING 
    m_retInt = m_reqRetInt;
    m_retTime = m_reqRetTime;
    m_retRelTime = m_reqRetRelTime;
#endif
}

void DTEncoder::tick() {
    noInterrupts();
    static_assert(sizeof(m_retTurnCounters) == sizeof(m_turnCounters));
    memcpy(m_retTurnCounters, m_turnCounters, sizeof(m_retTurnCounters));
    memset(m_turnCounters, 0, sizeof(m_turnCounters));
    interrupts();

#ifdef PIO_UNIT_TESTING 
    m_retInt = m_reqRetInt;
    m_retTime = m_reqRetTime;
    m_retRelTime = m_reqRetRelTime;

    m_reqRetInt = false;
    m_reqRetTime = false;
    m_reqRetRelTime = false;
#endif
}


bool DTEncoder::getInt(uint8_t& choosen, uint8_t min, uint8_t max) const {
#ifdef PIO_UNIT_TESTING
    if (m_retInt) {
        choosen = m_int;
        return true;
    }
#endif
    int8_t shift;
    if (max - min > 30)
        shift = getAceleratedShift(2, 4);
    else
        shift = getShift();

    uint8_t res;

    if (choosen > max) {
        choosen = max;
        return true;
    }

    if (choosen < min) {
        choosen = min;
        return true;
    }

    if (!shift)
        return false;

    if (shift < 0 && (choosen - min) < -shift)
        res = min;
    else if (shift > 0 && (max - choosen) < shift)
        res = max;
    else
        res = choosen + shift;

    if (res == choosen)
        return false;

    choosen = res;
    return true;
}

bool DTEncoder::getTime(Time& time, bool smooth) const {
#ifdef PIO_UNIT_TESTING
    if (m_retTime) {
        time = m_time;
        return true;
    }
#endif
    if (time == kBadTime) {
        time = 0_s;
        return true;
    }
    
    if (time > 1800_s) {
        time = 1800_s;
        return true;
    }

    int16_t shift;
    if (!smooth)
        shift = getAceleratedShift(3, 6);
    else
        shift = getShift();

    if (!shift)
        return false;

    int16_t id = time.getId() + shift;
    Time newTime = Time::fromId(id);

    if (newTime == time)
        return false;

    time = newTime;
    return true;
}

bool DTEncoder::getRelTime(RelTime& time) const {
#ifdef PIO_UNIT_TESTING
    if (m_retRelTime) {
        time = m_relTime;
        return true;
    }
#endif
    uint8_t relTimeId = time.getId();
    bool res = getInt(relTimeId, 0, kMaxRelTime.getId());

    time = RelTime(relTimeId);
    return res;
}

