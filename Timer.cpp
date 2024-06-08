#include "Timer.h"

#include <Arduino.h>

namespace {
uint8_t kBeepVolume = 0.7 * 255;
} // namespace

void Timer::setup() {
    pinMode(m_beepPin, OUTPUT);
    analogWrite(m_beepPin, 0);
}

void Timer::tick() {
    m_currentTime = millis();
    if (m_status != RUNNING)
        return;

    if ((m_leftTime - left() < 500) || ((m_leftTime - left()) / 100) % 10) {
        analogWrite(m_beepPin, 0);
    } else {
        analogWrite(m_beepPin, kBeepVolume);
    }

    if (m_currentTime > m_stopTime) {
        stop();
    }
}

void Timer::start(unsigned long ms) {
    m_leftTime = ms;
    m_stopTime = m_currentTime + ms;
    m_status = RUNNING;
}

void Timer::pause() {
    if (m_status == RUNNING) {
        analogWrite(m_beepPin, 0);
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
        analogWrite(m_beepPin, 0);
        m_status = STOPPED;
        if (m_currentTime > m_stopTime)
            m_total += m_leftTime;
        else
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
