#include <unity.h>

#include <Hardware.h>

#include "../Tools.h"

void setup_();
void unsetup_();
void loop_();

void setUp() {
    gCurrentTime = 0;
    gSettings = kDefaultSettings;
    gSettings.updateEEPROM();
    gTimer = Timer{};
    gEncoder.clear();
    gDisplay.reset();
    gScrollableContent.reset();
    gModeBtn = ButtonT<MODE_BTN_PIN>{};
    gStartBtn = ButtonT<START_BTN_PIN>{};
    gEncoderBtn = ButtonT<ENCODER_BTN_PIN>{};
    gViewBtn = ButtonT<VIEW_BTN_PIN>{};
}

void tearDown() {
    unsetup_();
}

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

    TEST_DISPLAY(kPlaySym "     8.4 16.8", "34 67 134 269");

    gCurrentTime += 500;
    loop_();
    TEST_DISPLAY(kPlaySym " 4.2 8.4 16.8", "34 67 134 269");

    gModeBtn.emulHold();
    gViewBtn.emulHold();
    loop_();
    TEST_DISPLAY(kWrenchSymTop " Lag time", kWrenchSymBottom " 0");

    loop_();
    TEST_DISPLAY(kWrenchSymTop " Lag time", kWrenchSymBottom " 0");

    gModeBtn.emulHold();
    gViewBtn.emulHold();
    loop_();
    TEST_DISPLAY(kPlaySym "     8.4 16.8", "34 67 134 269");

    gModeBtn.emulPress();
    gEncoder.emulTurns(1);
    loop_();
    gEncoder.emulTurns(1);
    loop_();
    TEST_DISPLAY(kFolder3Str "Printing", "");

    loop_();
    TEST_DISPLAY(kFolder3Str "Printing", "");

    gEncoder.emulTurns(-1);
    loop_();
    gEncoder.emulTurns(-1);
    loop_();
    TEST_DISPLAY(kFolder1Str "F Stop test", "");
    gModeBtn.emulRelease();
    loop_();
    TEST_DISPLAY(kPlaySym "     8.4 16.8", "34 67 134 269");

    gViewBtn.emulClick();
    loop_();
    TEST_DISPLAY(kLampSym " View", kLampLight1Sym " Auto stop: 180");
    TEST_ASSERT(gRelayVal);

    gCurrentTime += 2000;
    loop_();
    TEST_DISPLAY(kLampSym " View", kLampLight1Sym " Auto stop: 178");
    TEST_ASSERT(gRelayVal);
    TEST_ASSERT(gBuzzerVal == BEEPER_VOLUME_SILENT);

    gViewBtn.emulClick();
    loop_();
    TEST_DISPLAY(kPlaySym "     8.4 16.8", "34 67 134 269");
    TEST_ASSERT(!gRelayVal);

    gStartBtn.emulClick();
    loop_();
    TEST_DISPLAY(kPlaySym " Lag 8.4 16.8", "34 67 134 269");
    TEST_ASSERT_EQUAL(Beeper::State::single, gBeeper.state());
    TEST_ASSERT(gRelayVal);

    gCurrentTime += 2000;
    loop_();
    TEST_DISPLAY(kPlaySym " 2.2 8.4 16.8", "34 67 134 269");
    TEST_ASSERT_EQUAL(Beeper::State::on, gBeeper.state());
    TEST_ASSERT(gRelayVal);

    gEncoderBtn.emulHold();
    loop_();
    TEST_DISPLAY(kPlaySym "     8.4 16.8", "34 67 134 269");
    TEST_ASSERT(!gRelayVal);
    TEST_ASSERT_EQUAL(Beeper::State::off, gBeeper.state());
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

    gEncoder.emulTurns(-1);
    loop_();

    TEST_DISPLAY("F Stop test", "F stop: 1/1");

    gEncoder.emulTurns(1);
    loop_();
    TEST_DISPLAY("F Stop test", "F stop: 1/2");

    gEncoder.emulTurns(1);
    loop_();
    TEST_DISPLAY("F Stop test", "F stop: 1/3");

    gEncoder.emulTurns(1);
    loop_();
    TEST_DISPLAY("F Stop test", "F stop: 1/4");

    gEncoder.emulTurns(1);
    loop_();
    TEST_DISPLAY("F Stop test", "F stop: 1/6");

    gEncoder.emulTurns(1);
    loop_();
    TEST_DISPLAY("F Stop test", "F stop: 1/12");

    gModeBtn.emulClick();
    loop_();
    TEST_DISPLAY(kPlaySym "     0 0 0 0 0", "0 0 0 0 0 0 0 0");

    gStartBtn.emulClick();
    loop_();
    TEST_DISPLAY(kPlaySym " 0     0 0 0 0", "0 0 0 0 0 0 0 0");
    TEST_ASSERT(!gRelayVal);
    TEST_ASSERT_EQUAL(Beeper::State::single, gBeeper.state());

    gEncoderBtn.emulHold();
    loop_();
    TEST_DISPLAY(kPlaySym "     0 0 0 0 0", "0 0 0 0 0 0 0 0");

    gEncoderBtn.emulRelease();
    loop_();

    // check we not fail
    for (int i = 0; i != sizeof(gModesCache) / sizeof(Time); ++i) {
        gStartBtn.emulClick();
        loop_();
        TEST_ASSERT_EQUAL(Beeper::State::single, gBeeper.state());
        gCurrentTime += 120;
        loop_();
        TEST_ASSERT_EQUAL(Beeper::State::off, gBeeper.state());
    }

    gStartBtn.emulClick();
    loop_();
    TEST_ASSERT_EQUAL(Beeper::State::off, gBeeper.state());

    gModeBtn.emulClick();
    loop_();
    gEncoder.emulRetTime(1800_s);
    loop_();
    TEST_DISPLAY("F Stop test", "Init t:1800");

    gModeBtn.emulClick();
    loop_();
    gModeBtn.emulClick();
    loop_();
    TEST_DISPLAY(kPlaySym "        1907", "2020 2141 2268");

    gEncoder.emulTurns(1);
    loop_();
    TEST_DISPLAY("2020 2141 2268", "2403 2546 2697");

    gEncoder.emulTurns(1);
    loop_();
    TEST_DISPLAY("2403 2546 2697", "2857 3027 3207");

    gEncoder.emulTurns(1);
    loop_();
    TEST_DISPLAY("2857 3027 3207", "3398 3600");

    gEncoder.emulTurns(-1);
    loop_();
    TEST_DISPLAY("2403 2546 2697", "2857 3027 3207");

    gEncoder.emulTurns(-1);
    loop_();
    TEST_DISPLAY("2020 2141 2268", "2403 2546 2697");

    // check we can start if not show printing time
    // and automatically go to current line
    gStartBtn.emulClick();
    loop_();
    TEST_DISPLAY(kPlaySym "    Lag 1907", "2020 2141 2268");

    gEncoderBtn.emulHold();
    loop_();
    TEST_DISPLAY(kPlaySym "        1907", "2020 2141 2268");

    gEncoder.emulTurns(1);
    loop_();
    TEST_DISPLAY("2020 2141 2268", "2403 2546 2697");

    gEncoder.emulTurns(1);
    loop_();
    TEST_DISPLAY("2403 2546 2697", "2857 3027 3207");

    gEncoder.emulTurns(1);
    loop_();
    TEST_DISPLAY("2857 3027 3207", "3398 3600");

    gEncoderBtn.emulHold();
    loop_();
    TEST_DISPLAY(kPlaySym "        1907", "2020 2141 2268");

    for (int i = 0; i != 13; ++i) {
        gStartBtn.emulClick();
        loop_();
        TEST_ASSERT_EQUAL(Beeper::State::single, gBeeper.state());
        gCurrentTime += 1800000;
        loop_();
        TEST_ASSERT_EQUAL(Beeper::State::off, gBeeper.state());
    }

    TEST_DISPLAY("3027 3207 3398", "3600");

    gStartBtn.emulClick();
    loop_();
    TEST_ASSERT_EQUAL(Beeper::State::off, gBeeper.state());
    TEST_ASSERT_EQUAL(gTimer.state(), Timer::State::STOPPED);

    gModeBtn.emulClick();
    loop_();

    gEncoder.emulRetTime(2_s);
    loop_();
    TEST_DISPLAY("F Stop test", "Init t:2");

    gModeBtn.emulClick();
    loop_();

    gEncoder.emulTurns(-1);
    loop_();
    gEncoder.emulTurns(-1);
    loop_();
    gEncoder.emulTurns(-1);
    loop_();
    gEncoder.emulTurns(-1);
    loop_();
    gEncoder.emulTurns(-1);
    loop_();
    gEncoder.emulTurns(-1);
    loop_();
    TEST_DISPLAY("F Stop test", "F stop: 1/1");

    gModeBtn.emulClick();
    loop_();
    TEST_DISPLAY(kPlaySym "     4 8 16 32", "64 128 256 512");

    Time testSet[] = {
        2_s, 2_s, 4_s, 8_s, 16_s, 32_s, 64_s, 128_s, 256_s, 512_s, 1024_s,
    };
    for (Time t : testSet) {
        gStartBtn.emulClick();
        loop_();
        TEST_ASSERT_EQUAL(Beeper::State::single, gBeeper.state());
        TEST_ASSERT(gRelayVal);

        gCurrentTime += t.toMillis() - 1;
        loop_();
        TEST_ASSERT_EQUAL(Beeper::State::on, gBeeper.state());
        TEST_ASSERT(gRelayVal);

        gCurrentTime += 1;
        loop_();
        TEST_ASSERT_EQUAL(Beeper::State::off, gBeeper.state());
        TEST_ASSERT(!gRelayVal);
    }

    TEST_DISPLAY("128 256 512 1024", "2048");
}

void checkLocalFStopTest() {
    setup_();
    loop_();

    gModeBtn.emulHold();
    gEncoder.emulTurns(-1);
    loop_();

    gModeBtn.emulHold();
    gEncoder.emulTurns(-1);
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

    gEncoder.emulTurns(-1);
    loop_();

    TEST_DISPLAY("Locl F Stop test", "F stop: 1/1");

    gEncoder.emulTurns(1);
    loop_();
    TEST_DISPLAY("Locl F Stop test", "F stop: 1/2");

    gEncoder.emulTurns(1);
    loop_();
    TEST_DISPLAY("Locl F Stop test", "F stop: 1/3");

    gEncoder.emulTurns(1);
    loop_();
    TEST_DISPLAY("Locl F Stop test", "F stop: 1/4");

    gEncoder.emulTurns(1);
    loop_();
    TEST_DISPLAY("Locl F Stop test", "F stop: 1/6");

    gEncoder.emulTurns(1);
    loop_();
    TEST_DISPLAY("Locl F Stop test", "F stop: 1/12");

    gModeBtn.emulClick();
    loop_();
    TEST_DISPLAY(kPlaySym "     0 0 0 0 0", "0 0 0 0 0 0 0 0");

    gStartBtn.emulClick();
    loop_();
    TEST_DISPLAY(kPlaySym " 0     0 0 0 0", "0 0 0 0 0 0 0 0");
    TEST_ASSERT(!gRelayVal);
    TEST_ASSERT_EQUAL(Beeper::State::single, gBeeper.state());

    gEncoderBtn.emulHold();
    loop_();
    TEST_DISPLAY(kPlaySym "     0 0 0 0 0", "0 0 0 0 0 0 0 0");

    gEncoderBtn.emulRelease();
    loop_();

    // check we not fail
    for (int i = 0; i != sizeof(gModesCache) / sizeof(Time); ++i) {
        gStartBtn.emulClick();
        loop_();
        TEST_ASSERT_EQUAL(Beeper::State::single, gBeeper.state());
        gCurrentTime += 120;
        loop_();
        TEST_ASSERT_EQUAL(Beeper::State::off, gBeeper.state());
    }

    gStartBtn.emulClick();
    loop_();
    TEST_ASSERT_EQUAL(Beeper::State::off, gBeeper.state());

    gModeBtn.emulClick();
    loop_();
    gEncoder.emulRetTime(1800_s);
    loop_();
    TEST_DISPLAY("Locl F Stop test", "Init t:1800");

    gModeBtn.emulClick();
    loop_();
    gModeBtn.emulClick();
    loop_();
    TEST_DISPLAY(kPlaySym "        1907", "2020 2141 2268");

    gEncoder.emulTurns(1);
    loop_();
    TEST_DISPLAY("2020 2141 2268", "2403 2546 2697");

    gEncoder.emulTurns(1);
    loop_();
    TEST_DISPLAY("2403 2546 2697", "2857 3027 3207");

    gEncoder.emulTurns(1);
    loop_();
    TEST_DISPLAY("2857 3027 3207", "3398 3600");

    gEncoder.emulTurns(1);
    loop_();
    TEST_DISPLAY("2857 3027 3207", "3398 3600");

    gEncoder.emulTurns(-1);
    loop_();
    TEST_DISPLAY("2403 2546 2697", "2857 3027 3207");

    gEncoder.emulTurns(-1);
    loop_();
    TEST_DISPLAY("2020 2141 2268", "2403 2546 2697");

    // check we can start if not show printing time
    // and automatically go to current line
    gStartBtn.emulClick();
    loop_();
    TEST_DISPLAY(kPlaySym "    Lag 1907", "2020 2141 2268");

    gEncoderBtn.emulHold();
    loop_();
    TEST_DISPLAY(kPlaySym "        1907", "2020 2141 2268");

    gEncoder.emulTurns(1);
    loop_();
    TEST_DISPLAY("2020 2141 2268", "2403 2546 2697");

    gEncoder.emulTurns(1);
    loop_();
    TEST_DISPLAY("2403 2546 2697", "2857 3027 3207");

    gEncoder.emulTurns(1);
    loop_();
    TEST_DISPLAY("2857 3027 3207", "3398 3600");

    gEncoderBtn.emulHold();
    loop_();
    TEST_DISPLAY(kPlaySym "        1907", "2020 2141 2268");

    for (int i = 0; i != 13; ++i) {
        gStartBtn.emulClick();
        loop_();
        TEST_ASSERT_EQUAL(Beeper::State::single, gBeeper.state());
        gCurrentTime += 3600000;
        loop_();
        TEST_ASSERT_EQUAL(Beeper::State::off, gBeeper.state());
    }

    TEST_DISPLAY("3027 3207 3398", "3600");

    gStartBtn.emulClick();
    loop_();
    TEST_ASSERT_EQUAL(Beeper::State::off, gBeeper.state());
    TEST_ASSERT_EQUAL(gTimer.state(), Timer::State::STOPPED);

    gModeBtn.emulClick();
    loop_();

    gEncoder.emulRetTime(2_s);
    loop_();
    TEST_DISPLAY("Locl F Stop test", "Init t:2");

    gModeBtn.emulClick();
    loop_();

    gEncoder.emulTurns(-1);
    loop_();
    gEncoder.emulTurns(-1);
    loop_();
    gEncoder.emulTurns(-1);
    loop_();
    gEncoder.emulTurns(-1);
    loop_();
    gEncoder.emulTurns(-1);
    loop_();
    gEncoder.emulTurns(-1);
    loop_();
    TEST_DISPLAY("Locl F Stop test", "F stop: 1/1");

    gModeBtn.emulClick();
    loop_();
    TEST_DISPLAY(kPlaySym "     4 8 16 32", "64 128 256 512");

    Time testSet[] = { 2_s, 4_s, 8_s, 16_s, 32_s, 64_s, 128_s, 256_s, 512_s, 1024_s, 2048_s };
    for (Time t : testSet) {
        gStartBtn.emulClick();
        loop_();
        TEST_ASSERT_EQUAL(Beeper::State::single, gBeeper.state());
        TEST_ASSERT(gRelayVal);

        gCurrentTime += t.toMillis() - 1;
        loop_();
        TEST_ASSERT_EQUAL(Beeper::State::on, gBeeper.state());
        TEST_ASSERT(gRelayVal);

        gCurrentTime += 1;
        loop_();
        TEST_ASSERT_EQUAL(Beeper::State::off, gBeeper.state());
        TEST_ASSERT(!gRelayVal);
    }

    TEST_DISPLAY("128 256 512 1024", "2048");
}

void checkSpltFStopTest() {
    setup_();
    loop_();

    gModeBtn.emulHold();
    gEncoder.emulTurns(1);
    loop_();
    gEncoder.emulTurns(1);
    loop_();
    gEncoder.emulTurns(1);
    loop_();
    gEncoder.emulTurns(1);
    loop_();
    gEncoder.emulTurns(1);
    loop_();
    gModeBtn.emulRelease();
    loop_();

    TEST_DISPLAY("Splt F Stop test", "First t:2");

    gEncoder.emulRetTime(32_ts);
    loop_();
    TEST_DISPLAY("Splt F Stop test", "First t:3.2");

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
    TEST_DISPLAY(kPlaySym " ntf 4 8 16 32", "64 128 256 512");

    Time testSet[] = {
        32_ts, 4_s, 4_s, 8_s, 16_s, 32_s, 64_s, 128_s, 256_s, 512_s, 1024_s,
    };
    bool first = true;
    for (Time t : testSet) {
        gStartBtn.emulClick();
        loop_();
        TEST_ASSERT_EQUAL(Beeper::State::single, gBeeper.state());
        TEST_ASSERT(gRelayVal);

        gCurrentTime += t.toMillis() - 1;
        loop_();
        TEST_ASSERT_EQUAL(Beeper::State::on, gBeeper.state());
        TEST_ASSERT(gRelayVal);

        gCurrentTime += 1;
        loop_();
        TEST_ASSERT_EQUAL((first ? Beeper::State::alarm : Beeper::State::off), gBeeper.state());
        TEST_ASSERT(!gRelayVal);

        first = false;
    }

    gStartBtn.emulClick();
    loop_();
    TEST_ASSERT_EQUAL(Beeper::State::off, gBeeper.state());
}
void checkLinearTest() {
    setup_();
    loop_();

    gModeBtn.emulHold();
    gEncoder.emulTurns(1);
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
    TEST_DISPLAY(kPlaySym "     8 13 18 23", "28 33 38 43 48");

    for (int i = 0; i != sizeof(gModesCache) / sizeof(Time); ++i) {
        gStartBtn.emulClick();
        loop_();
        TEST_ASSERT_EQUAL(Beeper::State::single, gBeeper.state());
        TEST_ASSERT(gRelayVal);

        gCurrentTime += !i ? 2999 : 4999;
        loop_();
        TEST_ASSERT_EQUAL(Beeper::State::on, gBeeper.state());
        TEST_ASSERT(gRelayVal);

        gCurrentTime += 1;
        loop_();
        TEST_ASSERT_EQUAL(Beeper::State::off, gBeeper.state());
        TEST_ASSERT(!gRelayVal);
    }

    gStartBtn.emulClick();
    loop_();
    TEST_ASSERT_EQUAL(Beeper::State::off, gBeeper.state());
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
    TEST_DISPLAY(kPlaySym "        1900", "2000 2100 2200");

    gEncoder.emulTurns(1);
    loop_();
    TEST_DISPLAY("2000 2100 2200", "2300 2400 2500");

    gEncoder.emulTurns(1);
    loop_();
    TEST_DISPLAY("2300 2400 2500", "2600 2700 2800");

    gEncoder.emulTurns(1);
    loop_();
    TEST_DISPLAY("2600 2700 2800", "2900 3000 3100");

    gEncoder.emulTurns(1);
    loop_();
    TEST_DISPLAY("2900 3000 3100", "3200 3300");

    gEncoder.emulTurns(1);
    loop_();
    TEST_DISPLAY("2900 3000 3100", "3200 3300");

    gEncoder.emulTurns(-1);
    loop_();
    TEST_DISPLAY("2600 2700 2800", "2900 3000 3100");

    // check we can start if not show printing time
    // and automatically go to current line
    gStartBtn.emulClick();
    loop_();
    TEST_DISPLAY(kPlaySym "    Lag 1900", "2000 2100 2200");
    TEST_ASSERT(gRelayVal);
    TEST_ASSERT_EQUAL(gTimer.state(), Timer::State::RUNNING);
}

void checkLocalLinearTest() {
    setup_();
    loop_();

    gModeBtn.emulHold();
    gEncoder.emulTurns(-1);
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
    TEST_DISPLAY(kPlaySym "     8 13 18 23", "28 33 38 43 48");

    for (uint32_t i = 0; i != sizeof(gModesCache) / sizeof(Time); ++i) {
        gStartBtn.emulClick();
        loop_();
        TEST_ASSERT_EQUAL(Beeper::State::single, gBeeper.state());
        TEST_ASSERT(gRelayVal);

        gCurrentTime += 2999 + 5000 * i;
        loop_();
        TEST_ASSERT_EQUAL(Beeper::State::on, gBeeper.state());
        TEST_ASSERT(gRelayVal);

        gCurrentTime += 1;
        loop_();
        TEST_ASSERT_EQUAL(Beeper::State::off, gBeeper.state());
        TEST_ASSERT(!gRelayVal);
    }

    gStartBtn.emulClick();
    loop_();
    TEST_ASSERT_EQUAL(Beeper::State::off, gBeeper.state());
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
    TEST_DISPLAY(kPlaySym "        1900", "2000 2100 2200");

    gEncoder.emulTurns(1);
    loop_();
    TEST_DISPLAY("2000 2100 2200", "2300 2400 2500");

    gEncoder.emulTurns(1);
    loop_();
    TEST_DISPLAY("2300 2400 2500", "2600 2700 2800");

    gEncoder.emulTurns(1);
    loop_();
    TEST_DISPLAY("2600 2700 2800", "2900 3000 3100");

    gEncoder.emulTurns(1);
    loop_();
    TEST_DISPLAY("2900 3000 3100", "3200 3300");

    gEncoder.emulTurns(1);
    loop_();
    TEST_DISPLAY("2900 3000 3100", "3200 3300");

    gEncoder.emulTurns(-1);
    loop_();
    TEST_DISPLAY("2600 2700 2800", "2900 3000 3100");

    // check we can start if not show printing time
    // and automatically go to current line
    gStartBtn.emulClick();
    loop_();
    TEST_DISPLAY(kPlaySym "    Lag 1900", "2000 2100 2200");
    TEST_ASSERT(gRelayVal);
    TEST_ASSERT_EQUAL(gTimer.state(), Timer::State::RUNNING);
}

void checkSpltLinearTest() {
    setup_();
    loop_();

    gModeBtn.emulHold();
    gEncoder.emulTurns(1);
    loop_();
    gEncoder.emulTurns(1);
    loop_();
    gEncoder.emulTurns(1);
    loop_();
    gEncoder.emulTurns(1);
    loop_();
    gEncoder.emulTurns(1);
    loop_();
    gEncoder.emulTurns(1);
    loop_();

    gModeBtn.emulRelease();
    loop_();
    TEST_DISPLAY("Splt linear test", "First t:2");

    gEncoder.emulRetTime(63_ts);
    loop_();
    TEST_DISPLAY("Splt linear test", "First t:6.3");

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
    TEST_DISPLAY(kPlaySym " ntf 3 8 13 18", "23 28 33 38 43");

    for (int i = 0; i != sizeof(gModesCache) / sizeof(Time); ++i) {
        gStartBtn.emulClick();
        loop_();
        TEST_ASSERT_EQUAL(Beeper::State::single, gBeeper.state());
        TEST_ASSERT(gRelayVal);

        if (i == 0)
            gCurrentTime += 6299;
        else if (i == 1)
            gCurrentTime += 2999;
        else
            gCurrentTime += 4999;

        loop_();
        TEST_ASSERT_EQUAL(Beeper::State::on, gBeeper.state());
        TEST_ASSERT(gRelayVal);

        gCurrentTime += 1;
        loop_();
        TEST_ASSERT_EQUAL((i ? Beeper::State::off : Beeper::State::alarm), gBeeper.state());
        TEST_ASSERT(!gRelayVal);
    }

    gStartBtn.emulClick();
    loop_();
    TEST_ASSERT_EQUAL(Beeper::State::off, gBeeper.state());
    TEST_ASSERT(!gRelayVal);
}

void checkPrintMode() {
    setup_();
    loop_();

    gModeBtn.emulHold();
    gEncoder.emulTurns(1);
    loop_();
    gEncoder.emulTurns(1);
    loop_();

    gModeBtn.emulRelease();
    loop_();
    TEST_DISPLAY("Prnt CLK T:0", kPlaySym " 8          0.0");

    gEncoder.emulRetTime(0_s);
    loop_();
    TEST_DISPLAY("Prnt CLK T:0", kPlaySym " 0          0.0");

    gModeBtn.emulClick();
    loop_();
    TEST_DISPLAY("Prnt HLD T:0", kPlaySym " 0          0.0");

    gModeBtn.emulClick();
    loop_();
    TEST_DISPLAY("Prnt CLK T:0", kPlaySym " 0          0.0");

    gStartBtn.emulClick();
    loop_();
    TEST_DISPLAY("Prnt CLK T:0", kPlaySym " 0          0.0");
    TEST_ASSERT_EQUAL(Beeper::State::single, gBeeper.state());
    TEST_ASSERT(!gRelayVal);

    gEncoder.emulRetTime(1800_s);
    loop_();
    TEST_DISPLAY("Prnt CLK T:0", kPlaySym " 1800       0.0");
    TEST_ASSERT_EQUAL(Beeper::State::single, gBeeper.state());
    TEST_ASSERT(!gRelayVal);

    gSettings.lagTime = 3_ts;
    gStartBtn.emulClick();
    loop_();
    TEST_DISPLAY("Prnt CLK T:0", kPlaySym " Lag        0.0");
    TEST_ASSERT_EQUAL(Beeper::State::single, gBeeper.state());
    TEST_ASSERT(gRelayVal);

    gCurrentTime += 200;
    loop_();
    TEST_DISPLAY("Prnt CLK T:0", kPlaySym " Lag        0.0");
    TEST_ASSERT_EQUAL(Beeper::State::off, gBeeper.state());
    TEST_ASSERT(gRelayVal);

    gCurrentTime += 7600;
    loop_();
    TEST_DISPLAY("Prnt CLK T:7.5", kPlaySym " 1792.5     7.5");
    TEST_ASSERT_EQUAL(Beeper::State::on, gBeeper.state());
    TEST_ASSERT(gRelayVal);

    gStartBtn.emulPress();
    loop_();
    TEST_DISPLAY("Prnt CLK T:7.5", kPauseSym " 1792.5     7.5");
    TEST_ASSERT_EQUAL(Beeper::State::off, gBeeper.state());
    TEST_ASSERT(!gRelayVal);

    gStartBtn.emulRelease();
    loop_();
    TEST_DISPLAY("Prnt CLK T:7.5", kPauseSym " 1792.5     7.5");
    TEST_ASSERT_EQUAL(Beeper::State::off, gBeeper.state());
    TEST_ASSERT(!gRelayVal);

    gModeBtn.emulClick();
    loop_();
    TEST_DISPLAY("Prnt HLD T:7.5", kPauseSym " 1792.5     7.5");
    TEST_ASSERT_EQUAL(Beeper::State::off, gBeeper.state());
    TEST_ASSERT(!gRelayVal);

    gStartBtn.emulClick();
    loop_();
    TEST_DISPLAY("Prnt HLD T:7.5", kPauseSym " 1792.5     7.5");
    TEST_ASSERT_EQUAL(Beeper::State::off, gBeeper.state());
    TEST_ASSERT(!gRelayVal);

    gStartBtn.emulPress();
    loop_();
    TEST_DISPLAY("Prnt HLD T:7.5", kPlaySym " Lag        7.5");
    TEST_ASSERT_EQUAL(Beeper::State::single, gBeeper.state());
    TEST_ASSERT(gRelayVal);

    gCurrentTime += 300;
    gStartBtn.emulRelease();
    loop_();
    TEST_DISPLAY("Prnt HLD T:7.5", kPauseSym " 1792.5     7.5");
    TEST_ASSERT_EQUAL(Beeper::State::off, gBeeper.state());
    TEST_ASSERT(!gRelayVal);

    gStartBtn.emulPress();
    loop_();
    TEST_DISPLAY("Prnt HLD T:7.5", kPlaySym " Lag        7.5");
    TEST_ASSERT_EQUAL(Beeper::State::single, gBeeper.state());
    TEST_ASSERT(gRelayVal);

    gCurrentTime += 7000;
    loop_();
    TEST_DISPLAY("Prnt HLD T:14.2", kPlaySym " 1785.8     6.7");
    TEST_ASSERT_EQUAL(Beeper::State::on, gBeeper.state());
    TEST_ASSERT(gRelayVal);

    gCurrentTime += 1785799;
    loop_();
    TEST_DISPLAY("Prnt HLD T:1799.", kPlaySym " 0       1792.5");
    TEST_ASSERT_EQUAL(Beeper::State::on, gBeeper.state());
    TEST_ASSERT(gRelayVal);

    gCurrentTime += 1;
    loop_();
    TEST_DISPLAY("Prnt HLD T:1800", kPlaySym " 1800    1792.5");
    TEST_ASSERT_EQUAL(Beeper::State::off, gBeeper.state());
    TEST_ASSERT(!gRelayVal);

    gCurrentTime += 100;
    loop_();
    TEST_DISPLAY("Prnt HLD T:1800", kPlaySym " 1800    1792.5");
    TEST_ASSERT_EQUAL(Beeper::State::off, gBeeper.state());
    TEST_ASSERT(!gRelayVal);

    gStartBtn.emulRelease();
    gCurrentTime += 100;
    loop_();
    TEST_DISPLAY("Prnt HLD T:1800", kPlaySym " 1800    1792.5");
    TEST_ASSERT_EQUAL(Beeper::State::off, gBeeper.state());
    TEST_ASSERT(!gRelayVal);

    gEncoderBtn.emulClick();
    loop_();
    TEST_DISPLAY("Log 7.5 1793", "");
    TEST_ASSERT_EQUAL(Beeper::State::off, gBeeper.state());
    TEST_ASSERT(!gRelayVal);

    gStartBtn.emulClick();
    loop_();
    TEST_DISPLAY("Log 7.5 1793", "");
    TEST_ASSERT_EQUAL(Beeper::State::off, gBeeper.state());
    TEST_ASSERT(!gRelayVal);

    gModeBtn.emulClick();
    loop_();
    TEST_DISPLAY("Log 7.5 1793", "");
    TEST_ASSERT_EQUAL(Beeper::State::off, gBeeper.state());
    TEST_ASSERT(!gRelayVal);

    gEncoderBtn.emulHold();
    loop_();
    TEST_DISPLAY("Prnt HLD T:0", kPlaySym " 1800       0.0");

    gModeBtn.emulClick();
    loop_();
    TEST_DISPLAY("Prnt CLK T:0", kPlaySym " 1800       0.0");

    gSettings.lagTime = 0_s;

    for (int i = 1; i != 11; ++i) {
        gStartBtn.emulClick();
        loop_();

        gCurrentTime += i * 1000;
        gStartBtn.emulClick();
        loop_();
    }

    TEST_DISPLAY("Prnt CLK T:55", kPauseSym " 1745      10.0");

    gEncoderBtn.emulClick();
    loop_();
    TEST_DISPLAY("Log 1 2 3 4 5 6", "7 8 9 10");

    gEncoderBtn.emulHold();
    loop_();
    TEST_DISPLAY("Prnt CLK T:0", kPlaySym " 1800       0.0");

    gEncoderBtn.emulClick();
    loop_();
    TEST_DISPLAY("Log 1 2 3 4 5 6", "7 8 9 10");
}

void checkMaskMode() {
    setup_();
    loop_();

    gModeBtn.emulHold();
    gEncoder.emulTurns(1);
    loop_();
    gEncoder.emulTurns(1);
    loop_();
    gEncoder.emulTurns(1);
    loop_();

    gModeBtn.emulRelease();
    loop_();
    TEST_DISPLAY("Mask printing", "Mask num: 1");

    gEncoder.emulRetInt(15);
    loop_();
    TEST_DISPLAY("Mask printing", "Mask num: 15");

    gModeBtn.emulClick();
    loop_();
    TEST_DISPLAY(kSheetSym "     0 0 0 0 0", "0 0 0 0 0 0 0 0");

    Time times[] = { 1_ts,     10_ts, 30_ts, 2_ts, 60_ts, 1800_s, kBadTime, 32_s,
                     kBadTime, 70_ts, 30_ts, 0_ts, 8_ts,  19_ts,  1800_s,   17_ts };
    static_assert(sizeof(times) / sizeof(times[0]) == 16);

    bool needGoNext = false;
    for (auto t : times) {
        // pass setting of time
        // and check fast trevel
        if (t == kBadTime) {
            gEncoderBtn.emulPress();
            gEncoder.emulTurns(1);
            loop_();
            gEncoder.emulTurns(1);
            loop_();
            gEncoderBtn.emulRelease();
            loop_();
            needGoNext = false;
            continue;
        }

        if (needGoNext) {
            gEncoderBtn.emulClick();
            loop_();
        }
        needGoNext = true;
        gEncoder.emulRetTime(t);
        loop_();

        if (t == 30_ts) {
            // set notify
            gStartBtn.emulClick();
            loop_();
        }
    }

    TEST_DISPLAY("0 0.8 1.9 1800", "1.7");

    gEncoderBtn.emulClick();
    loop_();
    TEST_DISPLAY(kSheetSym "     1 3 0.2 6", "1800 0 32 0 7 3");

    gModeBtn.emulClick();
    loop_();
    TEST_DISPLAY(kPlaySym "     1 3 0.2 6", "1800 0 32 0 7 3");

    for (auto t : times) {
        gStartBtn.emulClick();
        loop_();
        TEST_ASSERT_EQUAL(Beeper::State::single, gBeeper.state());

        if (t == kBadTime || t == 0_s) {
            TEST_ASSERT(!gRelayVal);
            continue;
        }

        gCurrentTime += t.toMillis() - 1;
        loop_();
        TEST_ASSERT(gRelayVal);
        if (t < 2_ts)
            TEST_ASSERT_EQUAL(Beeper::State::single, gBeeper.state());
        else if (t < 11_ts)
            TEST_ASSERT_EQUAL(Beeper::State::off, gBeeper.state());
        else
            TEST_ASSERT_EQUAL(Beeper::State::on, gBeeper.state());

        gCurrentTime += 1;
        loop_();
        TEST_ASSERT(!gRelayVal);
        // check notify
        TEST_ASSERT_EQUAL(((t == 30_ts) ? Beeper::State::alarm : Beeper::State::off), gBeeper.state());
    }

    TEST_DISPLAY("0 0.8 1.9 1800", "1.7     Finished");
    gStartBtn.emulClick();
    loop_();
    TEST_ASSERT_EQUAL(Beeper::State::off, gBeeper.state());
    TEST_ASSERT(!gRelayVal);

    // Scroll didn't work on finished
    gEncoder.emulTurns(-1);
    loop_();
    TEST_DISPLAY("0 0.8 1.9 1800", "1.7     Finished");

    gEncoderBtn.emulHold();
    loop_();
    TEST_DISPLAY(kPlaySym "     1 3 0.2 6", "1800 0 32 0 7 3");

    gEncoder.emulTurns(1);
    loop_();
    TEST_DISPLAY("1800 0 32 0 7 3", "0 0.8 1.9 1800");

    // could start even if not see printing time
    gStartBtn.emulClick();
    loop_();
    TEST_DISPLAY(kPlaySym " Lag 1 3 0.2 6", "1800 0 32 0 7 3");
}

void checkSplitMaskMode() {
    setup_();
    loop_();

    gModeBtn.emulHold();
    gEncoder.emulTurns(1);
    loop_();
    gEncoder.emulTurns(1);
    loop_();
    gEncoder.emulTurns(1);
    loop_();
    gEncoder.emulTurns(1);
    loop_();
    gEncoder.emulTurns(1);
    loop_();
    gEncoder.emulTurns(1);
    loop_();
    gEncoder.emulTurns(1);
    loop_();

    gModeBtn.emulRelease();
    loop_();
    TEST_DISPLAY("Filter 1", "Mask num: 1");

    gEncoder.emulRetInt(7);
    loop_();
    TEST_DISPLAY("Filter 1", "Mask num: 7");

    gModeBtn.emulClick();
    loop_();
    TEST_DISPLAY("Filter 2", "Mask num: 1");

    gEncoder.emulRetInt(7);
    loop_();
    TEST_DISPLAY("Filter 2", "Mask num: 7");

    gModeBtn.emulClick();
    loop_();
    TEST_DISPLAY("1" kSheetSym "     0 0 0 0 0", "0 0");

    Time times[2][8] = { { 1_ts, 10_ts, 500_s, 10_ts, 40_s, 1800_s, kBadTime, 32_s },
                         { 100_ts, kBadTime, 30_ts, 0_ts, 8_ts, 19_ts, 1800_s, 17_ts } };

    bool needGoNext = false;
    for (auto& tt : times) {
        for (auto t : tt) {
            // pass setting of time
            // and check fast trevel
            if (t == kBadTime) {
                gEncoderBtn.emulPress();
                gEncoder.emulTurns(1);
                loop_();
                gEncoder.emulTurns(1);
                loop_();
                gEncoderBtn.emulRelease();
                loop_();
                needGoNext = false;
                continue;
            }

            if (needGoNext) {
                gEncoderBtn.emulClick();
                loop_();
            }

            needGoNext = true;
            gEncoder.emulRetTime(t);
            loop_();
        }
    }

    TEST_DISPLAY("2" kSheetSym " 10 0 3 0 0.8", "1.9 1800 1.7")

    gEncoderBtn.emulClick();
    loop_();
    TEST_DISPLAY("1" kSheetSym "     1 500 1", "40 1800 0 32");

    gModeBtn.emulClick();
    loop_();
    TEST_DISPLAY("1" kPlaySym "     1 500 1", "40 1800 0 32");

    int i = 0;
    for (auto& tt : times) {
        for (auto t : tt) {
            ++i;
            gStartBtn.emulClick();
            loop_();
            TEST_ASSERT_EQUAL(Beeper::State::single, gBeeper.state());

            if (t == kBadTime || t == 0_s) {
                TEST_ASSERT(!gRelayVal);
                continue;
            }

            gCurrentTime += t.toMillis() - 1;
            loop_();
            TEST_ASSERT(gRelayVal);
            if (t < 2_ts)
                TEST_ASSERT_EQUAL(Beeper::State::single, gBeeper.state());
            else if (t < 11_ts)
                TEST_ASSERT_EQUAL(Beeper::State::off, gBeeper.state());
            else
                TEST_ASSERT_EQUAL(Beeper::State::on, gBeeper.state());

            gCurrentTime += 1;
            loop_();
            TEST_ASSERT(!gRelayVal);
            // check notify
            TEST_ASSERT_EQUAL(((i == 8) ? Beeper::State::alarm : Beeper::State::off), gBeeper.state());
        }
    }

    TEST_DISPLAY("2" kPlaySym " 10 0 3 0 0.8", "1.9 1800Finished");
    gStartBtn.emulClick();
    loop_();
    TEST_ASSERT_EQUAL(Beeper::State::off, gBeeper.state());
    TEST_ASSERT(!gRelayVal);

    // Scroll didn't work on finished
    gEncoder.emulTurns(-1);
    loop_();
    TEST_DISPLAY("2" kPlaySym " 10 0 3 0 0.8", "1.9 1800Finished");

    gEncoderBtn.emulHold();
    loop_();
    TEST_DISPLAY("1" kPlaySym "     1 500 1", "40 1800 0 32");

    gEncoder.emulTurns(1);
    loop_();
    TEST_DISPLAY("40 1800 0 32", "2" kPlaySym " 10 0 3 0 0.8");

    // could start even if not see printing time
    gStartBtn.emulClick();
    loop_();
    TEST_DISPLAY("1" kPlaySym " Lag 1 500 1", "40 1800 0 32");
}

void checkRelMaskMode() {
    setup_();
    loop_();

    gModeBtn.emulHold();
    gEncoder.emulTurns(1);
    loop_();
    gEncoder.emulTurns(1);
    loop_();
    gEncoder.emulTurns(1);
    loop_();
    gEncoder.emulTurns(1);
    loop_();

    gModeBtn.emulRelease();
    loop_();
    TEST_DISPLAY("Rel mask print", "Mask num: 1");

    gEncoder.emulRetInt(15);
    loop_();
    TEST_DISPLAY("Rel mask print", "Mask num: 15");

    gModeBtn.emulClick();
    loop_();
    TEST_DISPLAY(kSheetSym "     0 0 0 0 0", "0 0 0 0 0 0 0 0");

    Time baseTime = 16_s;
    RelTime times[] = { RelTime(0), // not used time
                        RelTime(3),  RelTime(0),  RelTime(10), RelTime(20), kMaxRelTime,
                        kMaxRelTime, RelTime(10), RelTime(0),  RelTime(0),  RelTime(7),
                        RelTime(0),  RelTime(4),  RelTime(25), RelTime(33), RelTime(41) };

    static_assert(sizeof(times) / sizeof(times[0]) == 16);

    bool first = true;
    for (auto t : times) {
        if (first) {
            gEncoder.emulRetTime(baseTime);
            loop_();
            first = false;
            continue;
        }

        gEncoderBtn.emulClick();
        loop_();

        gEncoder.emulRetRelTime(t);
        loop_();
    }

    TEST_DISPLAY("2+1/2 2+5/12", "3+3/4");

    gEncoderBtn.emulClick();
    loop_();
    TEST_DISPLAY(kSheetSym "      2/3 0", "7/12 1+1/12 5 5");

    gModeBtn.emulClick();
    loop_();
    TEST_DISPLAY(kPlaySym "      2/3 0", "7/12 1+1/12 5 5");

    first = true;
    for (auto t : times) {
        Time printTime = baseTime;
        if (!first)
            printTime = t ^ baseTime;
        first = false;

        gStartBtn.emulClick();
        loop_();
        TEST_ASSERT_EQUAL(Beeper::State::single, gBeeper.state());

        if (!printTime) {
            TEST_ASSERT(!gRelayVal);
            continue;
        }

        gCurrentTime += printTime.toMillis() - 1;
        loop_();
        TEST_ASSERT(gRelayVal);
        if (printTime < 2_ts)
            TEST_ASSERT_EQUAL(Beeper::State::single, gBeeper.state());
        else if (printTime < 11_ts)
            TEST_ASSERT_EQUAL(Beeper::State::off, gBeeper.state());
        else
            TEST_ASSERT_EQUAL(Beeper::State::on, gBeeper.state());

        gCurrentTime += 1;
        loop_();
        TEST_ASSERT(!gRelayVal);
        // check notify
        TEST_ASSERT_EQUAL(Beeper::State::off, gBeeper.state());
    }

    TEST_DISPLAY("2+1/2 2+5/12", "3+3/4   Finished");
    gStartBtn.emulClick();
    loop_();
    TEST_ASSERT_EQUAL(Beeper::State::off, gBeeper.state());
    TEST_ASSERT(!gRelayVal);

    // Scroll didn't work on finished
    gEncoder.emulTurns(-1);
    loop_();
    TEST_DISPLAY("2+1/2 2+5/12", "3+3/4   Finished");

    gEncoderBtn.emulHold();
    loop_();
    TEST_DISPLAY(kPlaySym "      2/3 0", "7/12 1+1/12 5 5");

    gEncoder.emulTurns(1);
    loop_();
    TEST_DISPLAY("7/12 1+1/12 5 5", "7/12 0 0 5/6 0");

    // could start even if not see printing time
    gStartBtn.emulClick();
    loop_();
    TEST_DISPLAY(kPlaySym "  Lag 2/3 0", "7/12 1+1/12 5 5");

    // could switch view at run time
    gEncoderBtn.emulClick();
    loop_();
    TEST_DISPLAY(kPlaySym "  Lag 9.4 0 8", "17.9 496 496 8 0");

    // tests on overflow
    gEncoderBtn.emulHold();
    loop_();
    gEncoderBtn.emulRelease();
    loop_();

    baseTime = 1400_s;
    gModeBtn.emulClick();
    loop_();
    gModeBtn.emulClick();
    loop_();
    TEST_DISPLAY(kSheetSym "      2/3 0", "7/12 1+1/12 5 5");

    gEncoder.emulRetTime(baseTime);
    loop_();
    TEST_DISPLAY(kSheetSym " 1400.0 2/3 0", "7/12 1+1/12 ovr");

    gModeBtn.emulClick();
    loop_();

    first = true;
    for (auto t : times) {
        Time printTime = baseTime;
        if (!first)
            printTime = t ^ baseTime;
        first = false;

        gStartBtn.emulClick();
        loop_();
        TEST_ASSERT_EQUAL(Beeper::State::single, gBeeper.state());

        if (!printTime || printTime == kBadTime) {
            TEST_ASSERT(!gRelayVal);
            continue;
        }

        gCurrentTime += printTime.toMillis() - 1;
        loop_();
        TEST_ASSERT(gRelayVal);
        if (printTime < 2_ts)
            TEST_ASSERT_EQUAL(Beeper::State::single, gBeeper.state());
        else if (printTime < 11_ts)
            TEST_ASSERT_EQUAL(Beeper::State::off, gBeeper.state());
        else
            TEST_ASSERT_EQUAL(Beeper::State::on, gBeeper.state());

        gCurrentTime += 1;
        loop_();
        TEST_ASSERT(!gRelayVal);
        // check notify
        TEST_ASSERT_EQUAL(Beeper::State::off, gBeeper.state());
    }
    TEST_DISPLAY("0 1/4 ovr ovr", "ovr     Finished");
}

void checkSplitRelMaskMode() {
    setup_();
    loop_();

    gModeBtn.emulHold();
    loop_();
    gEncoder.emulTurns(-1);
    loop_();
    gEncoder.emulTurns(-1);
    loop_();
    gEncoder.emulTurns(-1);
    loop_();

    gModeBtn.emulRelease();
    loop_();
    TEST_DISPLAY("Filter 1", "Mask num: 1");

    gEncoder.emulRetInt(7);
    loop_();
    TEST_DISPLAY("Filter 1", "Mask num: 7");

    gModeBtn.emulClick();
    loop_();

    gModeBtn.emulRelease();
    loop_();
    TEST_DISPLAY("Filter 2", "Mask num: 1");

    gEncoder.emulRetInt(7);
    loop_();
    TEST_DISPLAY("Filter 2", "Mask num: 7");

    gModeBtn.emulClick();
    loop_();
    TEST_DISPLAY("1" kSheetSym "     0 0 0 0 0", "0 0");

    Time baseTimes[2] = { 16_s, 100_s };
    RelTime times[] = { RelTime(0), // not used time
                        RelTime(3),  RelTime(0), RelTime(22), RelTime(20), kMaxRelTime, kMaxRelTime, RelTime(10),
                        RelTime(0), // not used time
                        RelTime(10), RelTime(0), RelTime(7),  RelTime(0),  RelTime(25), RelTime(33), RelTime(41) };

    static_assert(sizeof(times) / sizeof(times[0]) == 16);

    int id = 0;
    for (auto t : times) {
        if (id) {
            gEncoderBtn.emulClick();
            loop_();
        }

        if (id % 8) {
            gEncoder.emulRetRelTime(t);
        } else {
            gEncoder.emulRetTime(baseTimes[id / 8]);
        }
        loop_();

        ++id;
    }

    TEST_DISPLAY("5/6 0 2+1/2", "2+5/12  3+3/4");

    gEncoderBtn.emulClick();
    loop_();
    TEST_DISPLAY("1" kSheetSym "      2/3 0", "1+7/12 1+1/12 5");

    gModeBtn.emulClick();
    loop_();
    TEST_DISPLAY("1" kPlaySym "      2/3 0", "1+7/12 1+1/12 5");

    id = 0;
    for (auto t : times) {
        ++id;
        Time printTime = baseTimes[(id - 1) / 8];
        if ((id % 8) != 1)
            printTime = t ^ printTime;

        gStartBtn.emulClick();
        loop_();
        TEST_ASSERT_EQUAL(Beeper::State::single, gBeeper.state());

        if (!printTime) {
            TEST_ASSERT(!gRelayVal);
            continue;
        }

        gCurrentTime += printTime.toMillis() - 1;
        loop_();
        TEST_ASSERT(gRelayVal);
        if (printTime < 2_ts)
            TEST_ASSERT_EQUAL(Beeper::State::single, gBeeper.state());
        else if (printTime < 11_ts)
            TEST_ASSERT_EQUAL(Beeper::State::off, gBeeper.state());
        else
            TEST_ASSERT_EQUAL(Beeper::State::on, gBeeper.state());

        gCurrentTime += 1;
        loop_();
        TEST_ASSERT(!gRelayVal);
        // check notify
        if (id == 8)
            TEST_ASSERT_EQUAL(Beeper::State::alarm, gBeeper.state());
        else
            TEST_ASSERT_EQUAL(Beeper::State::off, gBeeper.state());
    }

    TEST_DISPLAY("5/6 0 2+1/2", "2+5/12 3Finished");
    gStartBtn.emulClick();
    loop_();
    TEST_ASSERT_EQUAL(Beeper::State::off, gBeeper.state());
    TEST_ASSERT(!gRelayVal);

    // Scroll didn't work on finished
    gEncoder.emulTurns(-1);
    loop_();
    TEST_DISPLAY("5/6 0 2+1/2", "2+5/12 3Finished");

    gEncoderBtn.emulHold();
    loop_();
    TEST_DISPLAY("1" kPlaySym "      2/3 0", "1+7/12 1+1/12 5");

    gEncoder.emulTurns(1);
    loop_();
    TEST_DISPLAY("1+7/12 1+1/12 5", "5 7/12");

    // could start even if not see printing time
    gStartBtn.emulClick();
    loop_();
    TEST_DISPLAY("1" kPlaySym "  Lag 2/3 0", "1+7/12 1+1/12 5");

    // could switch view at run time
    gEncoderBtn.emulClick();
    loop_();
    TEST_DISPLAY("1" kPlaySym "  Lag 9.4 0 32", "17.9 496 496 8");

    // tests on overflow
    baseTimes[0] = 2000_s;

    gEncoderBtn.emulHold();
    loop_();
    gEncoderBtn.emulRelease();
    loop_();

    gModeBtn.emulClick();
    loop_();
    gModeBtn.emulClick();
    loop_();

    gModeBtn.emulClick();
    loop_();
    TEST_DISPLAY("1" kSheetSym "      2/3 0", "1+7/12 1+1/12 5");

    gEncoder.emulRetTime(baseTimes[0]);
    loop_();
    TEST_DISPLAY("1" kSheetSym " 2000.0 2/3 0", "ovr 1+1/12 ovr");

    gModeBtn.emulClick();
    loop_();

    gEncoder.emulRetTime(baseTimes[0]);
    loop_();
    TEST_DISPLAY("1" kPlaySym "        2/3 0", "ovr 1+1/12 ovr");

    id = 0;
    for (auto t : times) {
        ++id;
        Time printTime = baseTimes[(id - 1) / 8];
        if ((id % 8) != 1)
            printTime = t ^ printTime;

        gStartBtn.emulClick();
        loop_();
        TEST_ASSERT_EQUAL(Beeper::State::single, gBeeper.state());

        if (!printTime || printTime == kBadTime) {
            TEST_ASSERT(!gRelayVal);
            continue;
        }

        gCurrentTime += printTime.toMillis() - 1;
        loop_();
        TEST_ASSERT(gRelayVal);
        if (printTime < 2_ts)
            TEST_ASSERT_EQUAL(Beeper::State::single, gBeeper.state());
        else if (printTime < 11_ts)
            TEST_ASSERT_EQUAL(Beeper::State::off, gBeeper.state());
        else
            TEST_ASSERT_EQUAL(Beeper::State::on, gBeeper.state());

        gCurrentTime += 1;
        loop_();
        TEST_ASSERT(!gRelayVal);
        // check notify
        if (id == 8)
            TEST_ASSERT_EQUAL(Beeper::State::alarm, gBeeper.state());
        else
            TEST_ASSERT_EQUAL(Beeper::State::off, gBeeper.state());
    }
    TEST_DISPLAY("5/6 0 2+1/2", "2+5/12 3Finished");
}

void checkSettings() {
    setup_();
    loop_();

    gModeBtn.emulHold();
    gEncoder.emulTurns(1);
    loop_();
    gEncoder.emulTurns(1);
    loop_();

    gModeBtn.emulRelease();

    gEncoder.emulRetTime(1800_s);
    loop_();
    TEST_DISPLAY("Prnt CLK T:0", kPlaySym " 1800       0.0");

    gSettings.lagTime = 3_ts;
    gStartBtn.emulClick();
    loop_();
    TEST_DISPLAY("Prnt CLK T:0", kPlaySym " Lag        0.0");
    TEST_ASSERT_EQUAL(Beeper::State::single, gBeeper.state());
    TEST_ASSERT(gRelayVal);

    gCurrentTime += 7800;
    loop_();
    TEST_DISPLAY("Prnt CLK T:7.5", kPlaySym " 1792.5     7.5");
    TEST_ASSERT_EQUAL(Beeper::State::on, gBeeper.state());
    TEST_ASSERT(gRelayVal);

    // can't open settings when timer is runned
    gViewBtn.emulHold();
    gModeBtn.emulHold();
    loop_();
    TEST_DISPLAY("Prnt CLK T:7.5", kPlaySym " 1792.5     7.5");

    // check we can edit settings at pause
    gStartBtn.emulPress();
    loop_();
    TEST_DISPLAY("Prnt CLK T:7.5", kPauseSym " 1792.5     7.5");
    TEST_ASSERT_EQUAL(Beeper::State::off, gBeeper.state());
    TEST_ASSERT(!gRelayVal);

    gViewBtn.emulHold();
    gModeBtn.emulHold();
    loop_();
    TEST_DISPLAY(kWrenchSymTop " Lag time", kWrenchSymBottom " 0.3");
    TEST_ASSERT(!gRelayVal);

    gEncoder.emulRetTime(6_ts);
    loop_();
    TEST_DISPLAY(kWrenchSymTop " Lag time", kWrenchSymBottom " 0.6");
    TEST_ASSERT(!gRelayVal);

    gViewBtn.emulHold();
    gModeBtn.emulHold();
    loop_();
    TEST_DISPLAY("Prnt CLK T:7.5", kPauseSym " 1792.5     7.5");
    TEST_ASSERT(!gRelayVal);

    gStartBtn.emulClick();
    loop_();
    TEST_DISPLAY("Prnt CLK T:7.5", kPlaySym " Lag        7.5");
    TEST_ASSERT(gRelayVal);

    gCurrentTime += 600;
    loop_();
    TEST_DISPLAY("Prnt CLK T:7.5", kPlaySym " Lag        7.5");
    TEST_ASSERT(gRelayVal);

    gCurrentTime += 1;
    loop_();
    TEST_DISPLAY("Prnt CLK T:7.5", kPlaySym " 1792.5     0.0");
    TEST_ASSERT(gRelayVal);

    gStartBtn.emulClick();
    loop_();
    TEST_DISPLAY("Prnt CLK T:7.5", kPauseSym " 1792.5     0.0");
    TEST_ASSERT(!gRelayVal);

    gViewBtn.emulHold();
    gModeBtn.emulHold();
    loop_();
    TEST_DISPLAY(kWrenchSymTop " Lag time", kWrenchSymBottom " 0.6");
    TEST_ASSERT(!gRelayVal);
    gViewBtn.emulRelease();
    gModeBtn.emulRelease();
    loop_();

    gStartBtn.emulClick();
    loop_();
    TEST_DISPLAY(kWrenchSymTop " Lag time", kWrenchSymBottom " 0.6");
    TEST_ASSERT(gRelayVal);

    gCurrentTime += 599;
    loop_();
    TEST_DISPLAY(kWrenchSymTop " Lag time", kWrenchSymBottom " 0");
    TEST_ASSERT(gRelayVal);

    // can't close during setting
    gViewBtn.emulHold();
    gModeBtn.emulHold();
    loop_();
    TEST_DISPLAY(kWrenchSymTop " Lag time", kWrenchSymBottom " 0");
    TEST_ASSERT(gRelayVal);
    gViewBtn.emulRelease();
    gModeBtn.emulRelease();

    gCurrentTime += 1;
    loop_();
    TEST_DISPLAY(kWrenchSymTop " Lag time", kWrenchSymBottom " 0.6");
    TEST_ASSERT(!gRelayVal);

    gModeBtn.emulClick();
    loop_();
    TEST_DISPLAY(kWrenchSymTop " Backlight", kWrenchSymBottom " 5");

    auto backlight = gBacklightVal;
    gEncoder.emulTurns(1);
    loop_();
    TEST_DISPLAY(kWrenchSymTop " Backlight", kWrenchSymBottom " 6");
    TEST_ASSERT(backlight < gBacklightVal);

    gEncoder.emulTurns(-1);
    loop_();
    TEST_DISPLAY(kWrenchSymTop " Backlight", kWrenchSymBottom " 5");
    TEST_ASSERT(backlight == gBacklightVal);

    gEncoder.emulRetInt(7);
    loop_();
    TEST_DISPLAY(kWrenchSymTop " Backlight", kWrenchSymBottom " 7");

    gModeBtn.emulClick();
    loop_();
    TEST_DISPLAY(kWrenchSymTop " Beep volume", kWrenchSymBottom " 3");

    auto buzzerVal = gBuzzerVal;
    TEST_ASSERT(buzzerVal == BEEPER_VOLUME_SILENT);

    gEncoder.emulTurns(1);
    loop_();
    TEST_DISPLAY(kWrenchSymTop " Beep volume", kWrenchSymBottom " 4");
    TEST_ASSERT(buzzerVal < gBuzzerVal);
    buzzerVal = gBuzzerVal;

    gEncoder.emulTurns(-1);
    loop_();
    TEST_DISPLAY(kWrenchSymTop " Beep volume", kWrenchSymBottom " 3");
    TEST_ASSERT(buzzerVal > gBuzzerVal);
    TEST_ASSERT(BEEPER_VOLUME_SILENT < gBuzzerVal);

    gEncoder.emulRetInt(7);
    loop_();
    TEST_DISPLAY(kWrenchSymTop " Beep volume", kWrenchSymBottom " 7");

    gModeBtn.emulClick();
    loop_();
    TEST_ASSERT(BEEPER_VOLUME_SILENT == gBuzzerVal);
    TEST_DISPLAY(kWrenchSymTop " Auto stop view", kWrenchSymBottom " 3 minutes");

    gEncoder.emulTurns(1);
    loop_();
    TEST_DISPLAY(kWrenchSymTop " Auto stop view", kWrenchSymBottom " 4 minutes");

    gModeBtn.emulClick();
    loop_();
    TEST_DISPLAY(kWrenchSymTop " Start with", kWrenchSymBottom " Modes");

    gEncoder.emulTurns(1);
    loop_();
    TEST_DISPLAY(kWrenchSymTop " Start with", kWrenchSymBottom " Settings");

    gViewBtn.emulHold();
    gModeBtn.emulHold();
    loop_();
    gViewBtn.emulRelease();
    gModeBtn.emulRelease();

    unsetup_();
    setup_();
    loop_();

    TEST_DISPLAY(kWrenchSymTop " Lag time", kWrenchSymBottom " 0.6");

    // check fast travel
    gModeBtn.emulHold();
    loop_();
    gEncoder.emulTurns(1);
    loop_();
    gEncoder.emulTurns(1);
    loop_();
    gEncoder.emulTurns(1);
    loop_();
    gEncoder.emulTurns(1);
    loop_();
    gEncoder.emulTurns(1);
    loop_();
    gModeBtn.emulRelease();
    loop_();

    TEST_DISPLAY(kWrenchSymTop " Notify melody", kWrenchSymBottom " nice");

    gEncoder.emulTurns(-1);
    loop_();
    TEST_DISPLAY(kWrenchSymTop " Notify melody", kWrenchSymBottom " alarm");

    gModeBtn.emulClick();
    loop_();
    TEST_ASSERT_EQUAL_STRING(kWrenchSymTop " Version       ", gLcd.getLine(0));

    gModeBtn.emulClick();
    loop_();
    TEST_DISPLAY(kWrenchSymTop " Lag time", kWrenchSymBottom " 0.6");

#ifndef DT_NATIVE
    TEST_ASSERT((Settings::load() == Settings{

                                         .lagTime = 6_ts,
                                         .beepVolume = 7,
                                         .backlight = 7,
                                         .autoFinishViewMinutes = 4,
                                         .startWith = StartWith::Settings,
                                         .melody = Melody::alarm,
                                     }));
#endif

    gViewBtn.emulHold();
    gModeBtn.emulHold();
    loop_();
    gViewBtn.emulRelease();
    gModeBtn.emulRelease();
    loop_();
}

void checkView() {
    setup_();
    loop_();

    gModeBtn.emulHold();
    gEncoder.emulTurns(1);
    loop_();
    gEncoder.emulTurns(1);
    loop_();

    gModeBtn.emulRelease();

    gEncoder.emulRetTime(1800_s);
    loop_();
    TEST_DISPLAY("Prnt CLK T:0", kPlaySym " 1800       0.0");

    gSettings.lagTime = 3_ts;
    gStartBtn.emulClick();
    loop_();
    TEST_DISPLAY("Prnt CLK T:0", kPlaySym " Lag        0.0");
    TEST_ASSERT_EQUAL(Beeper::State::single, gBeeper.state());
    TEST_ASSERT(gRelayVal);

    gCurrentTime += 7800;
    loop_();
    TEST_DISPLAY("Prnt CLK T:7.5", kPlaySym " 1792.5     7.5");
    TEST_ASSERT_EQUAL(Beeper::State::on, gBeeper.state());
    TEST_ASSERT(gRelayVal);

    // can't start view
    gViewBtn.emulClick();
    loop_();
    TEST_DISPLAY("Prnt CLK T:7.5", kPlaySym " 1792.5     7.5");

    // check we can view at pause
    gStartBtn.emulPress();
    loop_();
    TEST_DISPLAY("Prnt CLK T:7.5", kPauseSym " 1792.5     7.5");
    TEST_ASSERT_EQUAL(Beeper::State::off, gBeeper.state());
    TEST_ASSERT(!gRelayVal);

    gViewBtn.emulClick();
    loop_();
    TEST_DISPLAY(kLampSym " View", kLampLight1Sym " Auto stop: 180");
    TEST_ASSERT(gRelayVal);

    gViewBtn.emulClick();
    loop_();
    TEST_DISPLAY("Prnt CLK T:7.5", kPauseSym " 1792.5     7.5");
    TEST_ASSERT(!gRelayVal);

    gViewBtn.emulClick();
    loop_();
    TEST_DISPLAY(kLampSym " View", kLampLight1Sym " Auto stop: 180");
    TEST_ASSERT(gRelayVal);

    gCurrentTime += 179999;
    loop_();
    TEST_DISPLAY(kLampSym " View", kLampLight6Sym " Auto stop: 1");
    TEST_ASSERT(gRelayVal);

    gCurrentTime += 1;
    loop_();
    TEST_DISPLAY("Prnt CLK T:7.5", kPauseSym " 1792.5     7.5");
    TEST_ASSERT(!gRelayVal);

    gSettings.autoFinishViewMinutes = 4;
    gViewBtn.emulClick();
    loop_();
    TEST_DISPLAY(kLampSym " View", kLampLight1Sym " Auto stop: 240");
    TEST_ASSERT(gRelayVal);

    gViewBtn.emulClick();
    loop_();
    TEST_DISPLAY("Prnt CLK T:7.5", kPauseSym " 1792.5     7.5");
    TEST_ASSERT(!gRelayVal);

    gSettings.autoFinishViewMinutes = 0;
    gViewBtn.emulClick();
    loop_();
    TEST_DISPLAY(kLampSym " View", kLampLight1Sym " Auto stop off");
    TEST_ASSERT(gRelayVal);

    gCurrentTime += 1000000;
    loop_();
    TEST_DISPLAY(kLampSym " View", kLampLight1Sym " Auto stop off");
    TEST_ASSERT(gRelayVal);

    gViewBtn.emulClick();
    loop_();
    TEST_DISPLAY("Prnt CLK T:7.5", kPauseSym " 1792.5     7.5");
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

    RUN_TEST(checkPrintMode);

    RUN_TEST(checkMaskMode);
    RUN_TEST(checkSplitMaskMode);

    RUN_TEST(checkRelMaskMode);
    RUN_TEST(checkSplitRelMaskMode);

    RUN_TEST(checkSettings);
    RUN_TEST(checkView);
    UNITY_END();
}
