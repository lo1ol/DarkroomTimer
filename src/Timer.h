#pragma once

#include <stdint.h>

#include "Time.h"

class DisplayLine;

class Timer {
public:
    enum State { STOPPED, RUNNING, PAUSED };

    Timer(uint8_t beepPin, uint8_t controlPin) : m_beepPin(beepPin), m_controlPin(controlPin) {}

    void setup();

    void tick();
    void start(Time ms);
    // info was it really paused
    bool pause();
    void resume();
    void stop();
    uint32_t left() const;

    bool stopped() const { return m_justStopped; }

    void printFormatedState() const;

    void resetAfterLastResume();
    Time afterLastResume() const;

    Time total() const;
    void resetTotal();

    void reset();

    State state() const;

private:
    void updateAfterLastResume();
    uint32_t afterResume() const;
    uint32_t realStopTime() const;
    bool reallyStarted() const;

    bool m_justStopped;

    State m_status = STOPPED;

    uint8_t m_beepPin;
    uint8_t m_controlPin;

    Time m_afterLastResume;

    uint32_t m_currentTime;
    uint32_t m_resumeTime;
    uint32_t m_leftTime;
    uint32_t m_total = 0;
};
