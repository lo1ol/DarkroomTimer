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
    TEST_ASSERT(gBeeper.state() == Beeper::State::single);
    TEST_ASSERT(gRelayVal);

    gCurrentTime += 2000;
    loop_();
    TEST_DISPLAY("Run 2.2 8.4 16.8", "34 67 134 269");
    TEST_ASSERT(gBeeper.state() == Beeper::State::on);
    TEST_ASSERT(gRelayVal);

    gEncoderBtn.emulHold();
    loop_();
    TEST_DISPLAY("Run     8.4 16.8", "34 67 134 269");
    TEST_ASSERT(!gRelayVal);
    TEST_ASSERT(gBeeper.state() == Beeper::State::off);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(checkScenarioGeneric);
    UNITY_END();
}
