#include "Timer.h"

#include <Arduino.h>

void Timer::tick() {
    m_currentTime = millis();
    if (m_status != RUNNING)
        return;

    if (m_currentTime > m_stopTime) {
        m_status = STOPPED;
        m_total += m_leftTime;
        m_leftTime = 0;
    }
}

void Timer::start(unsigned long ms) {
    m_leftTime = ms;
    m_stopTime = m_currentTime + ms;
    m_status = RUNNING;
}

void Timer::pause() {
    if (m_status == RUNNING) {
        m_total += m_leftTime + m_currentTime;
        m_leftTime = m_stopTime - m_currentTime;
        m_status = PAUSED;
    }
}

void Timer::resume() {
    if (m_status == PAUSED) {
        m_stopTime = m_currentTime + m_leftTime;
        m_status = RUNNING;
    }
}

void Timer::stop() {
    if (m_status != STOPPED) {
        m_status = STOPPED;
        m_total += m_leftTime + m_currentTime - m_stopTime;
        m_leftTime = 0;
    }
}

unsigned long Timer::left() {
    if (m_status == RUNNING) {
        return m_stopTime - m_currentTime;
    }
    return m_leftTime;
}

unsigned long Timer::total() {
    return m_total + (m_leftTime - left());
}

void Timer::resetTotal() {
    m_total = 0;
}

Timer::status_t Timer::state() {
    return m_status;
}
