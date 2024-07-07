#include "Beeper.h"

#include "Tools.h"

void Beeper::tick() {
    uint32_t currentTime = millis();
    switch (m_state) {
    case State::off:
        break;
    case State::on:
        if (m_timer <= currentTime) {
            m_pinState = !m_pinState;
            if (m_pinState)
                m_timer = currentTime + 100;
            else
                m_timer = currentTime + 900;
        }
        break;
    case State::single:
        if (m_timer <= currentTime) {
            m_state = State::off;
            m_pinState = false;
        }
        break;
    }

    if (m_pinState)
        analogWrite(m_pin, gSettings.beepVolume);
    else
        analogWrite(m_pin, 0);
}

void Beeper::setup() {
    pinMode(BEEPER, OUTPUT);
    analogWrite(BEEPER, 0);
}

void Beeper::beep() {
    m_state = State::single;

    m_pinState = true;
    m_timer = millis() + 100;
    analogWrite(m_pinState, gSettings.beepVolume);
}

void Beeper::start(bool silentStart) {
    m_state = State::on;

    if (silentStart) {
        m_pinState = false;
        m_timer = millis() + 1000;
    } else {
        m_pinState = true;
        m_timer = millis() + 100;
    }

    if (m_pinState)
        analogWrite(m_pin, gSettings.beepVolume);
    else
        analogWrite(m_pin, 0);
}

void Beeper::stop() {
    m_state = State::off;
    analogWrite(m_pinState, 0);
}
