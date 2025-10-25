#include "Beeper.h"

#include "Hardware.h"

Beeper::~Beeper() {
    if (m_melody)
        m_melody->~Melody();
}

void Beeper::tick() {
    // you couldn't change volume at runtime
    uint32_t currentTime = gMillis();
    switch (m_state) {
    case State::off:
        break;
    case State::on:
        if (m_timer <= currentTime) {
            setPinState(!m_pinState);
            if (m_pinState)
                m_timer += 100;
            else
                m_timer += 900;
        }
        break;
    case State::single:
        if (m_timer <= currentTime)
            stop();
        break;
    case State::alarm:
        processAlarm();
        break;
    }
}

void Beeper::setup() {
    gPinMode(m_pin, OUTPUT);
    setPinState(false, true);
    setMelody(gSettings.melody);
}

void Beeper::beep() {
    m_state = State::single;
    m_timer = gMillis() + 100;
    setPinState(true, true);
}

void Beeper::start() {
    m_state = State::on;
    m_timer = gMillis() + 100;
    setPinState(true, true);
}

void Beeper::stop() {
    m_state = State::off;
    setPinState(false, true);
}

void Beeper::alarm() {
    m_state = State::alarm;
    m_melody->init();
    processAlarm();
    // in case if volume was updated, but we were runned before
    setPinState(m_pinState, true);
}

void Beeper::processAlarm() {
    setPinState(m_melody->tick());
    if (!m_melody->end())
        return;

    stop();
}

void Beeper::setPinState(bool pinState, bool force) {
    if (m_pinState == pinState && !force)
        return;

    m_pinState = pinState;

    if (pinState)
        gAnalogWrite(m_pin, BEEPER_VOLUME_MIN + (gSettings.beepVolume - 1) * BEEPER_VOLUME_STEP);
    else
        // setting volume to BEEPER_VOLUME_SILENT instead of 0 reduce bad noise on low volumes
        gAnalogWrite(m_pin, BEEPER_VOLUME_SILENT);
}

void Beeper::setMelody(Melody::Name melodyName) {
    stop();

    if (m_melody)
        m_melody->~Melody();

    m_melody = Melody::getMelody(melodyName);
}

#ifdef PIO_UNIT_TESTING
void Beeper::setMelody(Melody* melody) {
    stop();

    if (m_melody)
        m_melody->~Melody();

    m_melody = melody;
}
#endif
