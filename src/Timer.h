#pragma once

#include <stdint.h>

class Timer {
public:
    enum State { STOPPED, RUNNING, PAUSED };

    Timer(uint8_t beepPin, uint8_t controlPin) : m_beepPin(beepPin), m_controlPin(controlPin) {}

    void setup();

    void tick();
    void start(uint32_t ms);
    void pause();
    void resume();
    void stop();
    uint32_t left() const;

    void printFormatedState(const char* additionalInfo = "") const;

    void resetAfterLastResume();
    uint32_t afterLastResume() const;

    uint32_t total() const;
    void resetTotal();

    State state() const;

private:
    void updateAfterLastResume();
    uint32_t afterResume() const;
    uint32_t realStopTime() const;

    State m_status = STOPPED;

    uint8_t m_beepPin;
    uint8_t m_controlPin;

    uint32_t m_currentTime;
    uint32_t m_leftTime;
    uint32_t m_stopTime;
    uint32_t m_total = 0;
    uint32_t m_afterLastResume;
};
