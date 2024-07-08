#include "Beeper.h"

#include "Tools.h"

constexpr uint16_t kMelodySwitchTimes[] = { 250, 250, 125, 125, 60, 60, 400 };

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
    case State::melody:
        processMelody();
        break;
    }

    processPin();
}

void Beeper::setup() {
    pinMode(BEEPER, OUTPUT);
    analogWrite(BEEPER, 0);
}

void Beeper::beep() {
    m_state = State::single;

    m_pinState = true;
    m_timer = millis() + 100;
    processPin();
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

    processPin();
}

void Beeper::stop() {
    m_state = State::off;
    processPin();
}

void Beeper::melody() {
    m_state = State::melody;

    m_timer = millis() + kMelodySwitchTimes[0];
    m_melodyPhase = 0;
    m_pinState = true;

    processMelody();
    processPin();
}

void Beeper::processMelody() {
    if (m_timer > millis())
        return;

    ++m_melodyPhase;

    if (m_melodyPhase == sizeof(kMelodySwitchTimes) / sizeof(kMelodySwitchTimes[0])) {
        m_pinState = false;
        m_state = State::off;
        return;
    }

    m_pinState = !m_pinState;
    m_timer = millis() + kMelodySwitchTimes[m_melodyPhase];
}

void Beeper::processPin() const {
    if (m_pinState)
        analogWrite(m_pin, gSettings.beepVolume);
    else
        analogWrite(m_pin, 0);
}
