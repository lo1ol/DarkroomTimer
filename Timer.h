#pragma once

#include <stdint.h>

class Timer {
public:
    enum status_t { STOPPED, RUNNING, PAUSED };

    Timer(uint8_t beepPin) : m_beepPin(beepPin) {}

    void setup();

    void tick();
    void start(unsigned long ms);
    void pause();
    void resume();
    void stop();
    unsigned long left();

    unsigned long total();
    void resetTotal();

    status_t state();

private:
    unsigned long m_currentTime;
    unsigned long m_leftTime;
    unsigned long m_stopTime;
    unsigned long m_total;
    status_t m_status;

    uint8_t m_beepPin;
};
