#pragma once

#include <stdint.h>

class Timer {
public:
    enum status_t { STOPPED, RUNNING, PAUSED };

    Timer(uint8_t beepPin, uint8_t controlPin) : m_beepPin(beepPin), m_controlPin(controlPin) {}

    void setup();

    void tick();
    void start(uint32_t ms);
    void pause();
    void resume();
    void stop();
    uint32_t left();

    uint32_t total();
    void resetTotal();

    void setLagTime(uint32_t) {}

    status_t state();

private:
    uint32_t m_currentTime;
    uint32_t m_leftTime;
    uint32_t m_stopTime;
    uint32_t m_total;
    status_t m_status;

    uint8_t m_beepPin;
    uint8_t m_controlPin;
};
