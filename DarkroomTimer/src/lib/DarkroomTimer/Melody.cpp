#include "Melody.h"

#include "Tools.h"

class MelodyPlayer : public Melody {
public:
    MelodyPlayer(const uint16_t* melody, uint8_t size) : m_melody(melody), m_size(size) {}

    void init() override {
        m_melodyPhase = 0;
        m_timer = gMillis() + m_melody[0];
    }

    bool tick() override {
        uint32_t currentTime = gMillis();

        if (m_timer <= currentTime) {
            ++m_melodyPhase;

            if (end())
                return false;

            m_timer = currentTime + m_melody[m_melodyPhase];
        }

        return !(m_melodyPhase & 1);
    }

    bool end() const override { return m_melodyPhase >= m_size; }

private:
    const uint16_t* m_melody;
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
    uint8_t m_melodyPhase;
    uint32_t m_timer;
};

constexpr uint16_t kNiceMelody[] = { 250, 250, 120, 120, 60, 60, 400, 800 };

uint16_t kHotlineMelody[] = {
    39,  211, 70, 70,  39, 47, 63, 211, 63, 78, 31, 78, 39,  102, 31, 78,  63,  78,  148, 102, 39,  320, 39,
    102, 39,  70, 70,  70, 47, 47, 63,  94, 31, 78, 63, 78,  47,  63, 47,  94,  31,  78,  63,  78,  141, 109,
    31,  78,  78, 63,  31, 78, 47, 219, 63, 63, 31, 63, 63,  94,  31, 78,  78,  63,  31,  78,  31,  109, 31,
    78,  63,  78, 156, 94, 31, 78, 78,  63, 31, 78, 47, 203, 63,  78, 125, 234, 281, 219, 266, 234, 250, 10,
};

#define ArraySize(array) (sizeof(array) / sizeof((array)[0]))

Melody* Melody::getMelody(Name name) {
    switch (name) {
    case Name::alarm:
        return new AlarmMelody;
    case Name::nice:
        return new MelodyPlayer(kNiceMelody, ArraySize(kNiceMelody));
    case Name::hotline:
        return new MelodyPlayer(kHotlineMelody, ArraySize(kHotlineMelody));
    }

    return nullptr;
}

const char* Melody::getMelodyName(Name name) {
    switch (name) {
    case Name::alarm:
        return "alarm";
    case Name::nice:
        return "nice";
    case Name::hotline:
        return "Hotline Miami";
    }

    return nullptr;
}
