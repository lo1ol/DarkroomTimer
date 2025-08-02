#include "EncoderWrap.h"

#ifndef PIO_UNIT_TESTING

    #include "SafeEncButton.h"

int8_t EncoderWrap::getDir() const {
    if (!m_enc->turn())
        return 0;
    return m_enc->dir();
}

bool EncoderWrap::fast() const {
    return m_enc->fast();
}

void EncoderWrap::tick() {
    m_enc->tick();
}

void EncoderWrap::clear() {
    m_enc->clear();
}
#endif

bool EncoderWrap::getInt(uint8_t& choosen, uint8_t min, uint8_t max) const {
#ifdef PIO_UNIT_TESTING
    if (m_retInt) {
        choosen = m_int;
        return true;
    }
#endif
    int8_t shift = getDir();
    if (max - min > 30 && fast())
        shift *= 5;

    if (shift == 0)
        return false;

    if (shift < 0 && (choosen - min) < -shift) {
        choosen = min;
        return false;
    } else if (shift > 0 && (max - choosen) < shift) {
        choosen = max;
        return false;
    } else {
        choosen += shift;
        return true;
    }
}

bool EncoderWrap::getTime(Time& time, bool smooth) const {
#ifdef PIO_UNIT_TESTING
    if (m_retTime) {
        time = m_time;
        return true;
    }
#endif
    Time shift{ getDir() };
    if (!smooth && fast())
        shift *= 8;

    Time oldTime = time;

    // Values are changing with accuracy 1/12 stop
    // You can check accuracy range by this formule:
    // t*(2**(1/12)-1)
    int16_t factor;
    if ((time + shift) < 10_s)
        factor = 1;
    else if ((time + shift) < 20_s)
        factor = 5;
    else if ((time + shift) < 100_s)
        factor = 10;
    else if ((time + shift) < 200_s)
        factor = 50;
    else if ((time + shift) < 1000_s)
        factor = 100;
    else
        factor = 500;

    time += shift * factor;
    time = Time((static_cast<int16_t>(time) / factor) * factor);

    if (time < 0_ts)
        time = 0_ts;

    if (time > 1800_s)
        time = 1800_s;

    return time != oldTime;
}

bool EncoderWrap::getRelTime(RelTime& time) const {
#ifdef PIO_UNIT_TESTING
    if (m_retRelTime) {
        time = m_relTime;
        return true;
    }
#endif
    uint8_t relTimeId = time.getId();
    bool res = getInt(relTimeId, 0, kMaxRelTime.getId());

    time = RelTime(relTimeId);
    return res;
}

