#include <main.h>

#include <unity.h>

#include "../VirtEnv.h"
#include "Tools.h"

void setUp() {
    gCurrentTime = 0;
    gSettings = kDefaultSettings;
    gTimer = Timer{};
    gEncoder.clear();
    gDisplay.reset();
    gScrollableContent.reset();
    gModeBtn = ButtonT<MODE_BTN>{};
    gStartBtn = ButtonT<START_BTN>{};
    gEncoderBtn = ButtonT<ENCODER_BTN>{};
    gViewBtn = ButtonT<VIEW_BTN>{};
}

void tearDown() {}

void checkScenarioGeneric() {
    setup_();
    loop_();

    TEST_DISPLAY("F Stop test", "Init t:2");

    gEncoder.emulRetTime(42_ts);
    loop_();

    TEST_DISPLAY("F Stop test", "Init t:4.2");

    gModeBtn.emulClick();
    loop_();
    gModeBtn.emulClick();
    loop_();

    TEST_DISPLAY("Run     8.4 16.8", "34 67 134 269");

    gCurrentTime += 500;
    loop_();
    TEST_DISPLAY("Run 4.2 8.4 16.8", "34 67 134 269");

    gModeBtn.emulHold();
    gViewBtn.emulHold();
    loop_();
    TEST_DISPLAY("Lag time", "0");

    loop_();
    TEST_DISPLAY("Lag time", "0");

    gModeBtn.emulHold();
    gViewBtn.emulHold();
    loop_();
    TEST_DISPLAY("Run     8.4 16.8", "34 67 134 269");

    gModeBtn.emulPress();
    gEncoder.emulTurn(1);
    loop_();
    gEncoder.emulTurn(1);
    loop_();
    TEST_DISPLAY("Printing", "");

    loop_();
    TEST_DISPLAY("Printing", "");

    gEncoder.emulTurn(-1);
    loop_();
    gEncoder.emulTurn(-1);
    loop_();
    TEST_DISPLAY("F Stop test", "");
    gModeBtn.emulRelease();
    loop_();
    TEST_DISPLAY("Run     8.4 16.8", "34 67 134 269");

    gViewBtn.emulClick();
    loop_();
    TEST_DISPLAY("View", "Auto stop: 180");
    TEST_ASSERT(gRelayVal);

    gCurrentTime += 2000;
    loop_();
    TEST_DISPLAY("View", "Auto stop: 178");
    TEST_ASSERT(gRelayVal);
    TEST_ASSERT(gBuzzerVal == BEEP_VOLUME_SILENT);

    gViewBtn.emulClick();
    loop_();
    TEST_DISPLAY("Run     8.4 16.8", "34 67 134 269");
    TEST_ASSERT(!gRelayVal);

    gStartBtn.emulClick();
    loop_();
    TEST_DISPLAY("Run Lag 8.4 16.8", "34 67 134 269");
    TEST_ASSERT_EQUAL(gBeeper.state(), Beeper::State::single);
    TEST_ASSERT(gRelayVal);

    gCurrentTime += 2000;
    loop_();
    TEST_DISPLAY("Run 2.2 8.4 16.8", "34 67 134 269");
    TEST_ASSERT_EQUAL(gBeeper.state(), Beeper::State::on);
    TEST_ASSERT(gRelayVal);

    gEncoderBtn.emulHold();
    loop_();
    TEST_DISPLAY("Run     8.4 16.8", "34 67 134 269");
    TEST_ASSERT(!gRelayVal);
    TEST_ASSERT_EQUAL(gBeeper.state(), Beeper::State::off);
}

void checkFStopTest() {
    setup_();
    loop_();

    TEST_DISPLAY("F Stop test", "Init t:2");

    gEncoder.emulRetTime(0_ts);
    loop_();

    TEST_DISPLAY("F Stop test", "Init t:0");

    gModeBtn.emulClick();
    loop_();

    TEST_DISPLAY("F Stop test", "F stop: 1/1");

    gEncoder.emulTurn(1);
    loop_();

    TEST_DISPLAY("F Stop test", "F stop: 1/1");

    gEncoder.emulTurn(-1);
    loop_();
    TEST_DISPLAY("F Stop test", "F stop: 1/2");

    gEncoder.emulTurn(-1);
    loop_();
    TEST_DISPLAY("F Stop test", "F stop: 1/3");

    gEncoder.emulTurn(-1);
    loop_();
    TEST_DISPLAY("F Stop test", "F stop: 1/4");

    gEncoder.emulTurn(-1);
    loop_();
    TEST_DISPLAY("F Stop test", "F stop: 1/6");

    gEncoder.emulTurn(-1);
    loop_();
    TEST_DISPLAY("F Stop test", "F stop: 1/12");

    gModeBtn.emulClick();
    loop_();
    TEST_DISPLAY("Run     0 0 0 0", "0 0 0 0 0 0 0 0");

    gStartBtn.emulClick();
    loop_();
    TEST_DISPLAY("Run 0     0 0 0", "0 0 0 0 0 0 0 0");
    TEST_ASSERT(!gRelayVal);
    TEST_ASSERT_EQUAL(gBeeper.state(), Beeper::State::single);

    gEncoderBtn.emulHold();
    loop_();
    TEST_DISPLAY("Run     0 0 0 0", "0 0 0 0 0 0 0 0");

    gEncoderBtn.emulRelease();
    loop_();

    // check we not fail
    for (int i = 0; i != sizeof(gModesCache) / sizeof(Time); ++i) {
        gStartBtn.emulClick();
        loop_();
        TEST_ASSERT_EQUAL(gBeeper.state(), Beeper::State::single);
        gCurrentTime += 120;
        loop_();
        TEST_ASSERT_EQUAL(gBeeper.state(), Beeper::State::off);
    }

    gStartBtn.emulClick();
    loop_();
    TEST_ASSERT_EQUAL(gBeeper.state(), Beeper::State::off);

    gModeBtn.emulClick();
    loop_();
    gEncoder.emulRetTime(1800_s);
    loop_();
    TEST_DISPLAY("F Stop test", "Init t:1800");

    gModeBtn.emulClick();
    loop_();
    gModeBtn.emulClick();
    loop_();
    TEST_DISPLAY("Run        1907", "2020 2141 2268");

    gEncoder.emulTurn(1);
    loop_();
    TEST_DISPLAY("2020 2141 2268", "2403 2546 2697");

    gEncoder.emulTurn(1);
    loop_();
    TEST_DISPLAY("2403 2546 2697", "2857 3027 3207");

    gEncoder.emulTurn(1);
    loop_();
    TEST_DISPLAY("2403 2546 2697", "2857 3027 3207");

    gEncoder.emulTurn(-1);
    loop_();
    TEST_DISPLAY("2020 2141 2268", "2403 2546 2697");

    // check can't start if not show printing time
    gStartBtn.emulClick();
    loop_();
    TEST_DISPLAY("2020 2141 2268", "2403 2546 2697");

    gEncoderBtn.emulHold();
    loop_();
    TEST_DISPLAY("Run        1907", "2020 2141 2268");

    for (int i = 0; i != 11; ++i) {
        gStartBtn.emulClick();
        loop_();
        TEST_ASSERT_EQUAL(gBeeper.state(), Beeper::State::single);
        gCurrentTime += 1800000;
        loop_();
        TEST_ASSERT_EQUAL(gBeeper.state(), Beeper::State::off);
    }

    TEST_DISPLAY("2403 2546 2697", "2857 3027 3207");

    gStartBtn.emulClick();
    loop_();
    TEST_ASSERT_EQUAL(gBeeper.state(), Beeper::State::off);
    TEST_ASSERT_EQUAL(gTimer.state(), Timer::State::STOPPED);

    gModeBtn.emulClick();
    loop_();

    gEncoder.emulRetTime(2_s);
    loop_();
    TEST_DISPLAY("F Stop test", "Init t:2");

    gModeBtn.emulClick();
    loop_();

    gEncoder.emulTurn(1);
    loop_();
    gEncoder.emulTurn(1);
    loop_();
    gEncoder.emulTurn(1);
    loop_();
    gEncoder.emulTurn(1);
    loop_();
    gEncoder.emulTurn(1);
    loop_();
    gEncoder.emulTurn(1);
    loop_();
    TEST_DISPLAY("F Stop test", "F stop: 1/1");

    gModeBtn.emulClick();
    loop_();
    TEST_DISPLAY("Run     4 8 16", "32 64 128 256");

    Time testSet[] = {
        2_s, 2_s, 4_s, 8_s, 16_s, 32_s, 64_s, 128_s, 256_s, 512_s, 1024_s,
    };
    for (Time t : testSet) {
        gStartBtn.emulClick();
        loop_();
        TEST_ASSERT_EQUAL(gBeeper.state(), Beeper::State::single);
        TEST_ASSERT(gRelayVal);

        gCurrentTime += t.toMillis() - 1;
        loop_();
        TEST_ASSERT_EQUAL(gBeeper.state(), Beeper::State::on);
        TEST_ASSERT(gRelayVal);

        gCurrentTime += 1;
        loop_();
        TEST_ASSERT_EQUAL(gBeeper.state(), Beeper::State::off);
        TEST_ASSERT(!gRelayVal);
    }

    TEST_DISPLAY("64 128 256 512", "1024 2048");
}

void checkLocalFStopTest() {
    setup_();
    loop_();

    gModeBtn.emulHold();
    gEncoder.emulTurn(-1);
    loop_();

    gModeBtn.emulHold();
    gEncoder.emulTurn(-1);
    loop_();

    gModeBtn.emulRelease();
    loop_();
    TEST_DISPLAY("Locl F Stop test", "Init t:2");

    gEncoder.emulRetTime(0_ts);
    loop_();

    TEST_DISPLAY("Locl F Stop test", "Init t:0");

    gModeBtn.emulClick();
    loop_();

    TEST_DISPLAY("Locl F Stop test", "F stop: 1/1");

    gEncoder.emulTurn(1);
    loop_();

    TEST_DISPLAY("Locl F Stop test", "F stop: 1/1");

    gEncoder.emulTurn(-1);
    loop_();
    TEST_DISPLAY("Locl F Stop test", "F stop: 1/2");

    gEncoder.emulTurn(-1);
    loop_();
    TEST_DISPLAY("Locl F Stop test", "F stop: 1/3");

    gEncoder.emulTurn(-1);
    loop_();
    TEST_DISPLAY("Locl F Stop test", "F stop: 1/4");

    gEncoder.emulTurn(-1);
    loop_();
    TEST_DISPLAY("Locl F Stop test", "F stop: 1/6");

    gEncoder.emulTurn(-1);
    loop_();
    TEST_DISPLAY("Locl F Stop test", "F stop: 1/12");

    gModeBtn.emulClick();
    loop_();
    TEST_DISPLAY("Run     0 0 0 0", "0 0 0 0 0 0 0 0");

    gStartBtn.emulClick();
    loop_();
    TEST_DISPLAY("Run 0     0 0 0", "0 0 0 0 0 0 0 0");
    TEST_ASSERT(!gRelayVal);
    TEST_ASSERT_EQUAL(gBeeper.state(), Beeper::State::single);

    gEncoderBtn.emulHold();
    loop_();
    TEST_DISPLAY("Run     0 0 0 0", "0 0 0 0 0 0 0 0");

    gEncoderBtn.emulRelease();
    loop_();

    // check we not fail
    for (int i = 0; i != sizeof(gModesCache) / sizeof(Time); ++i) {
        gStartBtn.emulClick();
        loop_();
        TEST_ASSERT_EQUAL(gBeeper.state(), Beeper::State::single);
        gCurrentTime += 120;
        loop_();
        TEST_ASSERT_EQUAL(gBeeper.state(), Beeper::State::off);
    }

    gStartBtn.emulClick();
    loop_();
    TEST_ASSERT_EQUAL(gBeeper.state(), Beeper::State::off);

    gModeBtn.emulClick();
    loop_();
    gEncoder.emulRetTime(1800_s);
    loop_();
    TEST_DISPLAY("Locl F Stop test", "Init t:1800");

    gModeBtn.emulClick();
    loop_();
    gModeBtn.emulClick();
    loop_();
    TEST_DISPLAY("Run        1907", "2020 2141 2268");

    gEncoder.emulTurn(1);
    loop_();
    TEST_DISPLAY("2020 2141 2268", "2403 2546 2697");

    gEncoder.emulTurn(1);
    loop_();
    TEST_DISPLAY("2403 2546 2697", "2857 3027 3207");

    gEncoder.emulTurn(1);
    loop_();
    TEST_DISPLAY("2403 2546 2697", "2857 3027 3207");

    gEncoder.emulTurn(-1);
    loop_();
    TEST_DISPLAY("2020 2141 2268", "2403 2546 2697");

    // check can't start if not show printing time
    gStartBtn.emulClick();
    loop_();
    TEST_DISPLAY("2020 2141 2268", "2403 2546 2697");

    gEncoderBtn.emulHold();
    loop_();
    TEST_DISPLAY("Run        1907", "2020 2141 2268");

    for (int i = 0; i != 11; ++i) {
        gStartBtn.emulClick();
        loop_();
        TEST_ASSERT_EQUAL(gBeeper.state(), Beeper::State::single);
        gCurrentTime += 3208000;
        loop_();
        TEST_ASSERT_EQUAL(gBeeper.state(), Beeper::State::off);
    }

    TEST_DISPLAY("2403 2546 2697", "2857 3027 3207");

    gStartBtn.emulClick();
    loop_();
    TEST_ASSERT_EQUAL(gBeeper.state(), Beeper::State::off);
    TEST_ASSERT_EQUAL(gTimer.state(), Timer::State::STOPPED);

    gModeBtn.emulClick();
    loop_();

    gEncoder.emulRetTime(2_s);
    loop_();
    TEST_DISPLAY("Locl F Stop test", "Init t:2");

    gModeBtn.emulClick();
    loop_();

    gEncoder.emulTurn(1);
    loop_();
    gEncoder.emulTurn(1);
    loop_();
    gEncoder.emulTurn(1);
    loop_();
    gEncoder.emulTurn(1);
    loop_();
    gEncoder.emulTurn(1);
    loop_();
    gEncoder.emulTurn(1);
    loop_();
    TEST_DISPLAY("Locl F Stop test", "F stop: 1/1");

    gModeBtn.emulClick();
    loop_();
    TEST_DISPLAY("Run     4 8 16", "32 64 128 256");

    Time testSet[] = { 2_s, 4_s, 8_s, 16_s, 32_s, 64_s, 128_s, 256_s, 512_s, 1024_s, 2048_s };
    for (Time t : testSet) {
        gStartBtn.emulClick();
        loop_();
        TEST_ASSERT_EQUAL(gBeeper.state(), Beeper::State::single);
        TEST_ASSERT(gRelayVal);

        gCurrentTime += t.toMillis() - 1;
        loop_();
        TEST_ASSERT_EQUAL(gBeeper.state(), Beeper::State::on);
        TEST_ASSERT(gRelayVal);

        gCurrentTime += 1;
        loop_();
        TEST_ASSERT_EQUAL(gBeeper.state(), Beeper::State::off);
        TEST_ASSERT(!gRelayVal);
    }

    TEST_DISPLAY("64 128 256 512", "1024 2048");
}

void checkSpltFStopTest() {
    setup_();
    loop_();

    gModeBtn.emulHold();
    gEncoder.emulTurn(1);
    loop_();
    gEncoder.emulTurn(1);
    loop_();
    gEncoder.emulTurn(1);
    loop_();
    gEncoder.emulTurn(1);
    loop_();
    gEncoder.emulTurn(1);
    loop_();
    gModeBtn.emulRelease();
    loop_();

    TEST_DISPLAY("Splt F Stop test", "Base t:2");

    gEncoder.emulRetTime(32_ts);
    loop_();
    TEST_DISPLAY("Splt F Stop test", "Base t:3.2");

    gModeBtn.emulClick();
    loop_();

    gEncoder.emulRetTime(4_s);
    loop_();

    TEST_DISPLAY("Splt F Stop test", "Init t:4");

    gModeBtn.emulClick();
    loop_();

    TEST_DISPLAY("Splt F Stop test", "F stop: 1/1");

    gModeBtn.emulClick();
    loop_();
    TEST_DISPLAY("Run ntf 4 8 16", "32 64 128 256");

    Time testSet[] = {
        32_ts, 4_s, 4_s, 8_s, 16_s, 32_s, 64_s, 128_s, 256_s, 512_s, 1024_s,
    };
    bool first = true;
    for (Time t : testSet) {
        gStartBtn.emulClick();
        loop_();
        TEST_ASSERT_EQUAL(gBeeper.state(), Beeper::State::single);
        TEST_ASSERT(gRelayVal);

        gCurrentTime += t.toMillis() - 1;
        loop_();
        TEST_ASSERT_EQUAL(gBeeper.state(), Beeper::State::on);
        TEST_ASSERT(gRelayVal);

        gCurrentTime += 1;
        loop_();
        TEST_ASSERT_EQUAL(gBeeper.state(), (first ? Beeper::State::alarm : Beeper::State::off));
        TEST_ASSERT(!gRelayVal);

        first = false;
    }

    gStartBtn.emulClick();
    loop_();
    TEST_ASSERT_EQUAL(gBeeper.state(), Beeper::State::off);
}
void checkLinearTest() {
    setup_();
    loop_();

    gModeBtn.emulHold();
    gEncoder.emulTurn(1);
    loop_();

    gModeBtn.emulRelease();
    loop_();
    TEST_DISPLAY("Linear test", "Init t:8");

    gEncoder.emulRetTime(3_s);
    loop_();
    TEST_DISPLAY("Linear test", "Init t:3");

    gModeBtn.emulClick();
    loop_();
    TEST_DISPLAY("Linear test", "Step t:2");

    gEncoder.emulRetTime(5_s);
    loop_();
    TEST_DISPLAY("Linear test", "Step t:5");

    gModeBtn.emulClick();
    loop_();
    TEST_DISPLAY("Run     8 13 18", "23 28 33 38 43");

    for (int i = 0; i != sizeof(gModesCache) / sizeof(Time); ++i) {
        gStartBtn.emulClick();
        loop_();
        TEST_ASSERT_EQUAL(gBeeper.state(), Beeper::State::single);
        TEST_ASSERT(gRelayVal);

        gCurrentTime += !i ? 2999 : 4999;
        loop_();
        TEST_ASSERT_EQUAL(gBeeper.state(), Beeper::State::on);
        TEST_ASSERT(gRelayVal);

        gCurrentTime += 1;
        loop_();
        TEST_ASSERT_EQUAL(gBeeper.state(), Beeper::State::off);
        TEST_ASSERT(!gRelayVal);
    }

    gStartBtn.emulClick();
    loop_();
    TEST_ASSERT_EQUAL(gBeeper.state(), Beeper::State::off);
    TEST_ASSERT(!gRelayVal);

    gModeBtn.emulClick();
    loop_();

    gEncoder.emulRetTime(1800_s);
    loop_();
    TEST_DISPLAY("Linear test", "Init t:1800");

    gModeBtn.emulClick();
    loop_();

    gEncoder.emulRetTime(100_s);
    loop_();
    TEST_DISPLAY("Linear test", "Step t:100");

    gModeBtn.emulClick();
    loop_();
    TEST_DISPLAY("Run        1900", "2000 2100 2200");

    gEncoder.emulTurn(1);
    loop_();
    TEST_DISPLAY("2000 2100 2200", "2300 2400 2500");

    gEncoder.emulTurn(1);
    loop_();
    TEST_DISPLAY("2300 2400 2500", "2600 2700 2800");

    gEncoder.emulTurn(1);
    loop_();
    TEST_DISPLAY("2600 2700 2800", "2900 3000 3100");

    gEncoder.emulTurn(1);
    loop_();
    TEST_DISPLAY("2900 3000 3100", "3200");

    gEncoder.emulTurn(1);
    loop_();
    TEST_DISPLAY("2900 3000 3100", "3200");

    gEncoder.emulTurn(-1);
    loop_();
    TEST_DISPLAY("2600 2700 2800", "2900 3000 3100");

    // check can't start if not show printing time
    gStartBtn.emulClick();
    loop_();
    TEST_DISPLAY("2600 2700 2800", "2900 3000 3100");
    TEST_ASSERT(!gRelayVal);
    TEST_ASSERT_EQUAL(gTimer.state(), Timer::State::STOPPED);
}

void checkLocalLinearTest() {
    setup_();
    loop_();

    gModeBtn.emulHold();
    gEncoder.emulTurn(-1);
    loop_();

    gModeBtn.emulRelease();
    loop_();
    TEST_DISPLAY("Locl linear test", "Init t:8");

    gEncoder.emulRetTime(3_s);
    loop_();
    TEST_DISPLAY("Locl linear test", "Init t:3");

    gModeBtn.emulClick();
    loop_();
    TEST_DISPLAY("Locl linear test", "Step t:2");

    gEncoder.emulRetTime(5_s);
    loop_();
    TEST_DISPLAY("Locl linear test", "Step t:5");

    gModeBtn.emulClick();
    loop_();
    TEST_DISPLAY("Run     8 13 18", "23 28 33 38 43");

    for (uint32_t i = 0; i != sizeof(gModesCache) / sizeof(Time); ++i) {
        gStartBtn.emulClick();
        loop_();
        TEST_ASSERT_EQUAL(gBeeper.state(), Beeper::State::single);
        TEST_ASSERT(gRelayVal);

        gCurrentTime += 2999 + 5000 * i;
        loop_();
        TEST_ASSERT_EQUAL(gBeeper.state(), Beeper::State::on);
        TEST_ASSERT(gRelayVal);

        gCurrentTime += 1;
        loop_();
        TEST_ASSERT_EQUAL(gBeeper.state(), Beeper::State::off);
        TEST_ASSERT(!gRelayVal);
    }

    gStartBtn.emulClick();
    loop_();
    TEST_ASSERT_EQUAL(gBeeper.state(), Beeper::State::off);
    TEST_ASSERT(!gRelayVal);

    gModeBtn.emulClick();
    loop_();

    gEncoder.emulRetTime(1800_s);
    loop_();
    TEST_DISPLAY("Locl linear test", "Init t:1800");

    gModeBtn.emulClick();
    loop_();

    gEncoder.emulRetTime(100_s);
    loop_();
    TEST_DISPLAY("Locl linear test", "Step t:100");

    gModeBtn.emulClick();
    loop_();
    TEST_DISPLAY("Run        1900", "2000 2100 2200");

    gEncoder.emulTurn(1);
    loop_();
    TEST_DISPLAY("2000 2100 2200", "2300 2400 2500");

    gEncoder.emulTurn(1);
    loop_();
    TEST_DISPLAY("2300 2400 2500", "2600 2700 2800");

    gEncoder.emulTurn(1);
    loop_();
    TEST_DISPLAY("2600 2700 2800", "2900 3000 3100");

    gEncoder.emulTurn(1);
    loop_();
    TEST_DISPLAY("2900 3000 3100", "3200");

    gEncoder.emulTurn(1);
    loop_();
    TEST_DISPLAY("2900 3000 3100", "3200");

    gEncoder.emulTurn(-1);
    loop_();
    TEST_DISPLAY("2600 2700 2800", "2900 3000 3100");

    // check can't start if not show printing time
    gStartBtn.emulClick();
    loop_();
    TEST_DISPLAY("2600 2700 2800", "2900 3000 3100");
    TEST_ASSERT(!gRelayVal);
    TEST_ASSERT_EQUAL(gTimer.state(), Timer::State::STOPPED);
}

void checkSpltLinearTest() {
    setup_();
    loop_();

    gModeBtn.emulHold();
    gEncoder.emulTurn(1);
    loop_();
    gEncoder.emulTurn(1);
    loop_();
    gEncoder.emulTurn(1);
    loop_();
    gEncoder.emulTurn(1);
    loop_();
    gEncoder.emulTurn(1);
    loop_();
    gEncoder.emulTurn(1);
    loop_();

    gModeBtn.emulRelease();
    loop_();
    TEST_DISPLAY("Splt linear test", "Base t:2");

    gEncoder.emulRetTime(63_ts);
    loop_();
    TEST_DISPLAY("Splt linear test", "Base t:6.3");

    gModeBtn.emulClick();
    loop_();
    TEST_DISPLAY("Splt linear test", "Init t:8");

    gEncoder.emulRetTime(3_s);
    loop_();
    TEST_DISPLAY("Splt linear test", "Init t:3");

    gModeBtn.emulClick();
    loop_();
    TEST_DISPLAY("Splt linear test", "Step t:2");

    gEncoder.emulRetTime(5_s);
    loop_();
    TEST_DISPLAY("Splt linear test", "Step t:5");

    gModeBtn.emulClick();
    loop_();
    TEST_DISPLAY("Run ntf 3 8 13", "18 23 28 33 38");

    for (int i = 0; i != sizeof(gModesCache) / sizeof(Time); ++i) {
        gStartBtn.emulClick();
        loop_();
        TEST_ASSERT_EQUAL(gBeeper.state(), Beeper::State::single);
        TEST_ASSERT(gRelayVal);

        if (i == 0)
            gCurrentTime += 6299;
        else if (i == 1)
            gCurrentTime += 2999;
        else
            gCurrentTime += 4999;

        loop_();
        TEST_ASSERT_EQUAL(gBeeper.state(), Beeper::State::on);
        TEST_ASSERT(gRelayVal);

        gCurrentTime += 1;
        loop_();
        TEST_ASSERT_EQUAL(gBeeper.state(), (i ? Beeper::State::off : Beeper::State::alarm));
        TEST_ASSERT(!gRelayVal);
    }

    gStartBtn.emulClick();
    loop_();
    TEST_ASSERT_EQUAL(gBeeper.state(), Beeper::State::off);
    TEST_ASSERT(!gRelayVal);
}
int main() {
    UNITY_BEGIN();
    RUN_TEST(checkScenarioGeneric);
    RUN_TEST(checkFStopTest);
    RUN_TEST(checkLocalFStopTest);
    RUN_TEST(checkSpltFStopTest);
    RUN_TEST(checkLinearTest);
    RUN_TEST(checkLocalLinearTest);
    RUN_TEST(checkSpltLinearTest);
    UNITY_END();
}
