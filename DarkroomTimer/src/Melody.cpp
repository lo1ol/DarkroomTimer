#include "Melody.h"

#include "Hardware.h"

#ifdef DT_NATIVE
    #define READ_PROGMEM(array, id) (array[id])
#else
    #define READ_PROGMEM(array, id) (pgm_read_byte(&array[id]))
#endif

class MelodyPlayer : public Melody {
public:
    MelodyPlayer(const uint8_t* melody, uint8_t size) : m_melody(melody), m_size(size) {}

    void init() override {
        m_melodyPhase = 0;
        m_timer = gMillis() + READ_PROGMEM(m_melody, 0) * 4;
    }

    bool tick() override {
        uint32_t currentTime = gMillis();

        if (m_timer <= currentTime) {
            ++m_melodyPhase;

            if (end())
                return false;

            m_timer = currentTime + READ_PROGMEM(m_melody, m_melodyPhase) * 4;
        }

        return !(m_melodyPhase & 1);
    }

    bool end() const override { return m_melodyPhase >= m_size; }

private:
    const uint8_t* m_melody;
    uint8_t m_size;
    uint8_t m_melodyPhase = 0;
    uint32_t m_timer = 0;
};

class AlarmMelody : public Melody {
public:
    void init() override {
        m_melodyPhase = 0;
        m_timer = gMillis() + 60;
    }

    bool tick() override {
        uint32_t currentTime = gMillis();

        if (m_timer < currentTime) {
            ++m_melodyPhase;
            if (m_melodyPhase % 20 == 19)
                m_timer = currentTime + 200;
            else
                m_timer = currentTime + 50;
        }

        return !(m_melodyPhase & 1);
    }

    bool end() const override { return m_melodyPhase >= 20; }

private:
    uint8_t m_melodyPhase = 0;
    uint32_t m_timer = 0;
};

constexpr uint8_t kNiceMelody[] PROGMEM = { 62, 62, 30, 30, 15, 15, 100, 200 };

constexpr uint8_t kHotlineMelody[] PROGMEM = {
    9,  52, 17, 17, 9,  11, 15, 52, 15, 19, 7,  19, 9,  25, 7,  19, 15, 19, 37, 25, 9,  80, 9,
    25, 9,  17, 17, 17, 11, 11, 15, 23, 7,  19, 15, 19, 11, 15, 11, 23, 7,  19, 15, 19, 35, 27,
    7,  19, 19, 15, 7,  19, 11, 54, 15, 15, 7,  15, 15, 23, 7,  19, 19, 15, 7,  19, 7,  27, 7,
    19, 15, 19, 39, 23, 7,  19, 19, 15, 7,  19, 11, 50, 15, 19, 31, 58, 70, 54, 66, 58, 62, 2,
};

#define ArraySize(array) (sizeof(array) / sizeof((array)[0]))

Melody* Melody::getMelody(Name name) {
    static uint8_t gMelodyBuf[sizeof(MelodyPlayer)];

    switch (name) {
    case Name::alarm:
        return new (gMelodyBuf) AlarmMelody;
    case Name::nice:
        return new (gMelodyBuf) MelodyPlayer(kNiceMelody, ArraySize(kNiceMelody));
    case Name::hotline:
        return new (gMelodyBuf) MelodyPlayer(kHotlineMelody, ArraySize(kHotlineMelody));
    case Name::last_:
        break;
    }

    assert(false);
    return nullptr;
}

const __FlashStringHelper* Melody::getMelodyName(Name name) {
    switch (name) {
    case Name::alarm:
        return F("Alarm");
    case Name::nice:
        return F("Nice");
    case Name::hotline:
        return F("Hotline");
    case Name::last_:
        break;
    }

    assert(false);
    return nullptr;
}
