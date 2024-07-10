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
    case State::alarm:
        processAlarm();
        break;
    }

    processPin();
}

void Beeper::setup() {
    pinMode(BEEPER, OUTPUT);
    analogWrite(BEEPER, 0);

    m_melody = Melody::getMelody(Melody::nice);
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
    m_pinState = false;
    processPin();
}

void Beeper::alarm(const char* notification) {
    m_state = State::alarm;

    m_melody->init();
    m_notification = notification;

    processAlarm();
    processPin();
}

void Beeper::processAlarm() {
    if (blocking()) {
        gDisplay[0] << m_notification;
        gDisplay[1] << "Click start btn";
        // for safety support click on mode btn
        if (gStartBtn.click() || gModeSwitchBtn.click()) {
            stop();
            gStartBtn.clear();
            gModeSwitchBtn.clear();
            return;
        }
    }

    m_pinState = m_melody->tick();
    if (!m_melody->end())
        return;

    if (blocking())
        m_melody->init();
    else
        stop();
}

void Beeper::processPin() const {
    if (m_pinState)
        analogWrite(m_pin, gSettings.beepVolume);
    else
        analogWrite(m_pin, 0);
}

bool Beeper::blocking() const {
    return m_notification && gSettings.confirmAlarm && m_state == State::alarm;
}

void Beeper::setMelody(Melody::Name melodyName) {
    delete m_melody;
    m_melody = Melody::getMelody(melodyName);
    if (m_state == State::alarm)
        alarm(m_notification);
}
