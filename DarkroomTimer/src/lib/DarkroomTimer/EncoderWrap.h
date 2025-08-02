#pragma once

#include <stdint.h>

#include "RelTime.h"
#include "Time.h"

class EncButton;

class EncoderWrap {
public:
#ifndef PIO_UNIT_TESTING
    EncoderWrap(EncButton* enc) : m_enc(enc) {}

    void tick();
    void clear();

    int8_t getDir() const;
#endif

    bool getInt(uint8_t& choosen, uint8_t min, uint8_t max) const;
    bool getTime(Time& time, bool smooth = false) const;
    bool getRelTime(RelTime& time) const;

#ifndef PIO_UNIT_TESTING
private:
    bool fast() const;

    EncButton* m_enc = nullptr;
#else

    EncoderWrap() {}

    void emulTurn(int8_t dir) { m_reqDir = dir; }
    void emulTurnFast(int8_t dir) {
        m_reqDir = dir;
        m_reqFast = true;
    }

    void emulRetInt(int v) {
        m_reqRetInt = true;
        m_int = v;
    }

    void emulRetTime(const Time& v) {
        m_reqRetTime = true;
        m_time = v;
    }

    void emulRetRelTime(const RelTime& v) {
        m_reqRetRelTime = true;
        m_relTime = v;
    }

    int8_t getDir() const { return m_dir; }
    bool fast() const { return m_fast; }

    void clear() {
        m_fast = false;
        m_dir = false;
        m_retInt = false;
        m_retTime = false;
        m_retRelTime = false;
    }

    void tick() {
        clear();

        m_dir = m_reqDir;
        m_fast = m_reqFast;
        m_retInt = m_reqRetInt;
        m_retTime = m_reqRetTime;
        m_retRelTime = m_reqRetRelTime;

        m_reqDir = 0;
        m_reqFast = false;
        m_reqRetInt = false;
        m_reqRetTime = false;
        m_reqRetRelTime = false;
    }

private:
    int8_t m_dir = 0;
    bool m_fast = false;
    bool m_retInt = false;
    bool m_retTime = false;
    bool m_retRelTime = false;

    int m_int = 0;
    Time m_time{};
    RelTime m_relTime{};

    int8_t m_reqDir = 0;
    bool m_reqFast = false;
    bool m_reqRetInt = false;
    bool m_reqRetTime = false;
    bool m_reqRetRelTime = false;
#endif
};
