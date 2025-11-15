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

void checkExpertFStopTest() {
    setup_();
    loop_();

    TEST_DISPLAY("Xprt F Stop test", "Local tests:off");

    gEncoder.emulTurns(1);
    loop_();
    TEST_DISPLAY("Xprt F Stop test", "Local tests:on");

    gEncoder.emulTurns(1);
    loop_();
    TEST_DISPLAY("Xprt F Stop test", "Local tests:on");

    gEncoder.emulTurns(-1);
    loop_();
    TEST_DISPLAY("Xprt F Stop test", "Local tests:off");

    gEncoder.emulTurns(-1);
    loop_();
    TEST_DISPLAY("Xprt F Stop test", "Local tests:off");

    gModeBtn.emulClick();
    loop_();

    TEST_DISPLAY("Xprt F Stop test", "Preflash t:0");
    gEncoder.emulRetTime(105_ts);
    loop_();
    TEST_DISPLAY("Xprt F Stop test", "Preflash t:10.5");

    gModeBtn.emulClick();
    loop_();
    TEST_DISPLAY("Xprt F Stop test", "Init t:2");

    gEncoder.emulRetTime(35_ts);
    loop_();
    TEST_DISPLAY("Xprt F Stop test", "Init t:3.5");

    gModeBtn.emulClick();
    loop_();
    TEST_DISPLAY("Xprt F Stop test", "F stop: 1/1");

    gEncoder.emulTurns(1);
    loop_();
    TEST_DISPLAY("Xprt F Stop test", "F stop: 1/2");

    gModeBtn.emulClick();
    loop_();
    TEST_DISPLAY(kPlaySym "  ntf 3.5 4.9 7", "9.9 14 19.8 28");

    gStartBtn.emulClick();
    loop_();
    TEST_DISPLAY(kPlaySym "  Lag 3.5 4.9 7", "9.9 14 19.8 28");

    gCurrentTime += 15000;
    loop_();
    TEST_DISPLAY(kPlaySym " 10.5     4.9 7", "9.9 14 19.8 28");
    TEST_ASSERT(gTimer.justStopped());
    TEST_ASSERT(gBeeper.state() == Beeper::State::alarm);

    gStartBtn.emulClick();
    loop_();
    gCurrentTime += 3500;
    loop_();
    TEST_ASSERT(gTimer.justStopped());

    gStartBtn.emulClick();
    loop_();
    gCurrentTime += 4900;
    loop_();
    TEST_ASSERT(gTimer.justStopped());
    TEST_DISPLAY(kPlaySym " 10.5 3.5 4.9", "    9.9 14 19.8");

    gEncoderBtn.emulHold();
    loop_();
    TEST_DISPLAY(kPlaySym "  ntf 3.5 4.9 7", "9.9 14 19.8 28");

    gModeBtn.emulClick();
    loop_();
    TEST_DISPLAY("Xprt F Stop test", "Local tests:off");

    gEncoder.emulTurns(1);
    loop_();
    TEST_DISPLAY("Xprt F Stop test", "Local tests:on");

    gModeBtn.emulClick();
    loop_();
    gModeBtn.emulClick();
    loop_();
    gModeBtn.emulClick();
    loop_();
    gModeBtn.emulClick();
    loop_();
    TEST_DISPLAY(kPlaySym "  ntf 3.5 4.9 7", "9.9 14 19.8 28");

    gStartBtn.emulClick();
    loop_();
    gCurrentTime += 15000;
    loop_();
    TEST_DISPLAY(kPlaySym " 10.5     4.9 7", "9.9 14 19.8 28");
    TEST_ASSERT(gTimer.justStopped());
    TEST_ASSERT(gBeeper.state() == Beeper::State::alarm);

    gStartBtn.emulClick();
    loop_();
    gCurrentTime += 3500;
    loop_();
    TEST_ASSERT(gTimer.justStopped());

    gStartBtn.emulClick();
    loop_();
    gCurrentTime += 8400;
    loop_();
    TEST_ASSERT(gTimer.justStopped());

    gStartBtn.emulClick();
    loop_();
    gCurrentTime += 15400;
    loop_();
    TEST_ASSERT(gTimer.justStopped());

    gModeBtn.emulClick();
    loop_();
    gModeBtn.emulClick();
    loop_();
    TEST_DISPLAY("Xprt F Stop test", "Preflash t:10.5");

    gEncoder.emulRetTime(0_s);
    loop_();
    TEST_DISPLAY("Xprt F Stop test", "Preflash t:0");

    gModeBtn.emulClick();
    loop_();
    gModeBtn.emulClick();
    loop_();
    gModeBtn.emulClick();
    loop_();
    TEST_DISPLAY(kPlaySym "     4.9 7 9.9", "14 19.8 28 40 56");

    gStartBtn.emulClick();
    loop_();
    gCurrentTime += 3500;
    loop_();
    TEST_ASSERT(gTimer.justStopped());
    TEST_ASSERT(gBeeper.state() == Beeper::State::off);
    TEST_DISPLAY(kPlaySym " 3.5     7 9.9", "14 19.8 28 40 56");

    gStartBtn.emulClick();
    loop_();
    gCurrentTime += 8400;
    loop_();
    TEST_ASSERT(gTimer.justStopped());

    gStartBtn.emulClick();
    loop_();
    gCurrentTime += 15400;
    loop_();
    TEST_ASSERT(gTimer.justStopped());
}

void checkExpertLinearTest() {
    setup_();
    loop_();

    gModeBtn.emulHold();
    gEncoder.emulTurns(1);
    loop_();
    gModeBtn.emulRelease();
    loop_();

    TEST_DISPLAY("Xprt linear test", "Local tests:off");

    gEncoder.emulTurns(1);
    loop_();
    TEST_DISPLAY("Xprt linear test", "Local tests:on");

    gEncoder.emulTurns(1);
    loop_();
    TEST_DISPLAY("Xprt linear test", "Local tests:on");

    gEncoder.emulTurns(-1);
    loop_();
    TEST_DISPLAY("Xprt linear test", "Local tests:off");

    gEncoder.emulTurns(-1);
    loop_();
    TEST_DISPLAY("Xprt linear test", "Local tests:off");

    gModeBtn.emulClick();
    loop_();

    TEST_DISPLAY("Xprt linear test", "Preflash t:0");
    gEncoder.emulRetTime(105_ts);
    loop_();
    TEST_DISPLAY("Xprt linear test", "Preflash t:10.5");

    gModeBtn.emulClick();
    loop_();
    TEST_DISPLAY("Xprt linear test", "Init t:8");

    gEncoder.emulRetTime(35_ts);
    loop_();
    TEST_DISPLAY("Xprt linear test", "Init t:3.5");

    gModeBtn.emulClick();
    loop_();
    TEST_DISPLAY("Xprt linear test", "Step t:2");

    gEncoder.emulRetTime(6_s);
    loop_();
    TEST_DISPLAY("Xprt linear test", "Step t:6");

    gModeBtn.emulClick();
    loop_();
    TEST_DISPLAY(kPlaySym "  ntf 3.5 9.5", "15.5 22 28 34 40");

    gStartBtn.emulClick();
    loop_();
    TEST_DISPLAY(kPlaySym "  Lag 3.5 9.5", "15.5 22 28 34 40");

    gCurrentTime += 15000;
    loop_();
    TEST_DISPLAY(kPlaySym " 10.5     9.5", "15.5 22 28 34 40");
    TEST_ASSERT(gTimer.justStopped());
    TEST_ASSERT(gBeeper.state() == Beeper::State::alarm);

    gStartBtn.emulClick();
    loop_();
    gCurrentTime += 3500;
    loop_();
    TEST_ASSERT(gTimer.justStopped());

    gStartBtn.emulClick();
    loop_();
    gCurrentTime += 6000;
    loop_();
    TEST_ASSERT(gTimer.justStopped());
    TEST_DISPLAY(kPlaySym " 10.5 3.5 9.5", "     22 28 34 40");

    gEncoderBtn.emulHold();
    loop_();
    TEST_DISPLAY(kPlaySym "  ntf 3.5 9.5", "15.5 22 28 34 40");

    gModeBtn.emulClick();
    loop_();
    TEST_DISPLAY("Xprt linear test", "Local tests:off");

    gEncoder.emulTurns(1);
    loop_();
    TEST_DISPLAY("Xprt linear test", "Local tests:on");

    gModeBtn.emulClick();
    loop_();
    gModeBtn.emulClick();
    loop_();
    gModeBtn.emulClick();
    loop_();
    gModeBtn.emulClick();
    loop_();
    TEST_DISPLAY(kPlaySym "  ntf 3.5 9.5", "15.5 22 28 34 40");

    gStartBtn.emulClick();
    loop_();
    gCurrentTime += 15000;
    loop_();
    TEST_DISPLAY(kPlaySym " 10.5     9.5", "15.5 22 28 34 40");
    TEST_ASSERT(gTimer.justStopped());
    TEST_ASSERT(gBeeper.state() == Beeper::State::alarm);

    gStartBtn.emulClick();
    loop_();
    gCurrentTime += 3500;
    loop_();
    TEST_ASSERT(gTimer.justStopped());

    gStartBtn.emulClick();
    loop_();
    gCurrentTime += 9500;
    loop_();
    TEST_ASSERT(gTimer.justStopped());

    gStartBtn.emulClick();
    loop_();
    gCurrentTime += 15500;
    loop_();
    TEST_ASSERT(gTimer.justStopped());

    gModeBtn.emulClick();
    loop_();
    gModeBtn.emulClick();
    loop_();
    TEST_DISPLAY("Xprt linear test", "Preflash t:10.5");

    gEncoder.emulRetTime(0_s);
    loop_();
    TEST_DISPLAY("Xprt linear test", "Preflash t:0");

    gModeBtn.emulClick();
    loop_();
    gModeBtn.emulClick();
    loop_();
    gModeBtn.emulClick();
    loop_();
    TEST_DISPLAY(kPlaySym "     9.5 15.5", "22 28 34 40 46");

    gStartBtn.emulClick();
    loop_();
    gCurrentTime += 3500;
    loop_();
    TEST_ASSERT(gTimer.justStopped());
    TEST_ASSERT(gBeeper.state() == Beeper::State::off);
    TEST_DISPLAY(kPlaySym " 3.5     15.5", "22 28 34 40 46");

    gStartBtn.emulClick();
    loop_();
    gCurrentTime += 9500;
    loop_();
    TEST_ASSERT(gTimer.justStopped());

    gStartBtn.emulClick();
    loop_();
    gCurrentTime += 15500;
    loop_();
    TEST_ASSERT(gTimer.justStopped());
}

int main() {
    UNITY_BEGIN();

    RUN_TEST(checkExpertFStopTest);
    RUN_TEST(checkExpertLinearTest);
    UNITY_END();
}
