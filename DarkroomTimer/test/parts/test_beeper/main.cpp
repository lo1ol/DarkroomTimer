#include <Arduino.h>
#include <unity.h>

#include <Beeper.h>
#include <Config.h>
#include <Melody.h>

#include "../../VirtEnv.h"

#define DOWN_VOLUME BEEP_VOLUME_SILENT
#define UP_VOLUME (BEEP_VOLUME_SILENT + 30)

class SimpleMelody : public Melody {
public:
    void init() override {
        m_melodyPhase = 0;
        m_timer = gMillis() + 1000;
    }

    bool tick() override {
        uint32_t currentTime = gMillis();

        if (m_timer <= currentTime) {
            ++m_melodyPhase;
        }

        return m_melodyPhase == 0;
    }

    bool end() const override { return m_melodyPhase; }

private:
    uint8_t m_melodyPhase;
    uint32_t m_timer;
};

SimpleMelody gSimpleMelody;

void setUp() {
    gMillis = &mockMillis;
    gAnalogWrite = &mockAnalogWrite;
    gCurrentTime = 0;
    gBeeper.stop();
    gSettings.beepVolume = UP_VOLUME;
    gBeeper.setMelody(&gSimpleMelody);
}

void tearDown() {}

void checkBeeperGeneric() {
    TEST_ASSERT_EQUAL(DOWN_VOLUME, gBuzzerVal);
    TEST_ASSERT_EQUAL(Beeper::State::off, gBeeper.state());
    gBeeper.beep();
    TEST_ASSERT_EQUAL(UP_VOLUME, gBuzzerVal);
    TEST_ASSERT_EQUAL(Beeper::State::single, gBeeper.state());

    gCurrentTime += 50;
    TEST_ASSERT_EQUAL(UP_VOLUME, gBuzzerVal);
    TEST_ASSERT_EQUAL(Beeper::State::single, gBeeper.state());
    gBeeper.tick();
    TEST_ASSERT_EQUAL(UP_VOLUME, gBuzzerVal);
    TEST_ASSERT_EQUAL(Beeper::State::single, gBeeper.state());

    gCurrentTime += 300;
    TEST_ASSERT_EQUAL(UP_VOLUME, gBuzzerVal);
    TEST_ASSERT_EQUAL(Beeper::State::single, gBeeper.state());
    gBeeper.tick();
    TEST_ASSERT_EQUAL(DOWN_VOLUME, gBuzzerVal);
    TEST_ASSERT_EQUAL(Beeper::State::off, gBeeper.state());

    gBeeper.beep();
    gBeeper.start();
    TEST_ASSERT_EQUAL(UP_VOLUME, gBuzzerVal);
    TEST_ASSERT_EQUAL(Beeper::State::on, gBeeper.state());
    gBeeper.stop();
    TEST_ASSERT_EQUAL(DOWN_VOLUME, gBuzzerVal);
    TEST_ASSERT_EQUAL(Beeper::State::off, gBeeper.state());

    gBeeper.beep();
    gBeeper.alarm();
    TEST_ASSERT_EQUAL(UP_VOLUME, gBuzzerVal);
    TEST_ASSERT_EQUAL(Beeper::State::alarm, gBeeper.state());
    gBeeper.stop();
    TEST_ASSERT_EQUAL(DOWN_VOLUME, gBuzzerVal);
    TEST_ASSERT_EQUAL(Beeper::State::off, gBeeper.state());

    gBeeper.start();
    TEST_ASSERT_EQUAL(UP_VOLUME, gBuzzerVal);
    TEST_ASSERT_EQUAL(Beeper::State::on, gBeeper.state());
    gBeeper.alarm();
    TEST_ASSERT_EQUAL(UP_VOLUME, gBuzzerVal);
    TEST_ASSERT_EQUAL(Beeper::State::alarm, gBeeper.state());
    gBeeper.stop();
    TEST_ASSERT_EQUAL(DOWN_VOLUME, gBuzzerVal);
    TEST_ASSERT_EQUAL(Beeper::State::off, gBeeper.state());

    gBeeper.start();
    gBeeper.beep();
    TEST_ASSERT_EQUAL(UP_VOLUME, gBuzzerVal);
    TEST_ASSERT_EQUAL(Beeper::State::single, gBeeper.state());
    gBeeper.stop();
    TEST_ASSERT_EQUAL(DOWN_VOLUME, gBuzzerVal);
    TEST_ASSERT_EQUAL(Beeper::State::off, gBeeper.state());

    gBeeper.start();
    TEST_ASSERT_EQUAL(UP_VOLUME, gBuzzerVal);

    for (int i = 0; i != 5; ++i) {
        gCurrentTime += 200;
        gBeeper.tick();
        TEST_ASSERT_EQUAL(DOWN_VOLUME, gBuzzerVal);
        gCurrentTime += 800;
        gBeeper.tick();
        TEST_ASSERT_EQUAL(UP_VOLUME, gBuzzerVal);
    }

    gBeeper.start();
    TEST_ASSERT_EQUAL(UP_VOLUME, gBuzzerVal);
    gCurrentTime += 1050;
    gBeeper.tick();
    TEST_ASSERT_EQUAL(DOWN_VOLUME, gBuzzerVal);
    gBeeper.tick();
    TEST_ASSERT_EQUAL(UP_VOLUME, gBuzzerVal);
    gBeeper.tick();
    TEST_ASSERT_EQUAL(UP_VOLUME, gBuzzerVal);
    gBeeper.stop();

    gBeeper.stop();
    TEST_ASSERT_EQUAL(DOWN_VOLUME, gBuzzerVal);
    TEST_ASSERT_EQUAL(Beeper::State::off, gBeeper.state());

    gBeeper.alarm();
    TEST_ASSERT_EQUAL(UP_VOLUME, gBuzzerVal);
    TEST_ASSERT_EQUAL(Beeper::State::alarm, gBeeper.state());
    gCurrentTime += 999;
    gBeeper.tick();
    TEST_ASSERT_EQUAL(UP_VOLUME, gBuzzerVal);
    TEST_ASSERT_EQUAL(Beeper::State::alarm, gBeeper.state());

    gCurrentTime += 1;
    gBeeper.tick();
    TEST_ASSERT_EQUAL(DOWN_VOLUME, gBuzzerVal);
    TEST_ASSERT_EQUAL(Beeper::State::off, gBeeper.state());
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(checkBeeperGeneric);
    UNITY_END();
}
