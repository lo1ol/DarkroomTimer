#pragma once

#include <stdint.h>

#include "Melody.h"

class Beeper {
public:
    explicit Beeper(uint8_t pin) : m_pin(pin) {}

    enum class State { off, on, single, alarm };

    void beep();
    void alarm();

    // start beep every second
    void start();
    void stop();

    void setup();
    void tick();

    void setMelody(Melody::Name);

    State state() const { return m_state; }

#ifndef PIO_UNIT_TESTING
private:
#endif
    void setMelody(Melody*);
    void processAlarm();
    void processPin() const;

    bool m_pinState = false;
    State m_state = State::off;
    uint8_t m_pin;
    uint32_t m_timer = 0;
    Melody* m_melody = nullptr;
};
