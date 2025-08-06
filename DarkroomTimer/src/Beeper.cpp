#include "Beeper.h"

#include "Hardware.h"

Beeper::~Beeper() {
    if (m_melody)
        delete m_melody;
}

void Beeper::tick() {
    uint32_t currentTime = gMillis();
    switch (m_state) {
    case State::off:
        break;
    case State::on:
        if (m_timer <= currentTime) {
            m_pinState = !m_pinState;
            if (m_pinState)
                m_timer += 100;
            else
                m_timer += 900;
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
    pinMode(m_pin, OUTPUT);
    gAnalogWrite(m_pin, BEEP_VOLUME_SILENT);

    m_melody = Melody::getMelody(gSettings.melody);
}

void Beeper::beep() {
    m_state = State::single;

    m_pinState = true;
    m_timer = gMillis() + 100;
    processPin();
}

void Beeper::start() {
    m_state = State::on;
    m_pinState = true;
    m_timer = gMillis() + 100;

    processPin();
}

void Beeper::stop() {
    m_state = State::off;
    m_pinState = false;
    processPin();
}

void Beeper::alarm() {
    m_state = State::alarm;

    m_melody->init();

    processAlarm();
    processPin();
}

void Beeper::processAlarm() {
    m_pinState = m_melody->tick();
    if (!m_melody->end())
        return;

    stop();
}

void Beeper::processPin() const {
    if (m_pinState)
        gAnalogWrite(m_pin, MIN_BEEP_VOLUME + (gSettings.beepVolume - 1) * BEEP_VOLUME_STEP);
    else
        // setting volume to BEEP_VOLUME_SILENT instead of 0 reduce bad noise on low volumes
        gAnalogWrite(m_pin, BEEP_VOLUME_SILENT);
}

void Beeper::setMelody(Melody::Name melodyName) {
    setMelody(Melody::getMelody(melodyName));
}

void Beeper::setMelody(Melody* melody) {
    stop();
    delete m_melody;
    m_melody = melody;
}
