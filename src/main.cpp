#include "Tools.h"

#include "Config.h"

#include "ModeProcessor.h"

#include "Modes/FStopTestMode.h"
#include "Modes/LinearTestMode.h"
#include "Modes/MaskMode.h"
#include "Modes/PrintMode.h"

#include "SettingsSetter.h"

enum class ModeId : uint8_t { testFStops, testLinear, print, mask, splitFStops, splitLinear, last_ };

ModeId gModeId;
ModeProcessor* gModeProcessor = nullptr;

void setMode(ModeId modeId) {
    if (gModeProcessor)
        delete (gModeProcessor);

    gModeId = modeId;

    switch (gModeId) {
    case ModeId::testFStops:
        gModeProcessor = new FStopTestMode(false);
        break;
    case ModeId::testLinear:
        gModeProcessor = new LinearTestMode(false);
        break;
    case ModeId::print:
        gModeProcessor = new PrintMode();
        break;
    case ModeId::mask:
        gModeProcessor = new MaskMode();
        break;
    case ModeId::splitFStops:
        gModeProcessor = new FStopTestMode(true);
        break;
    case ModeId::splitLinear:
        gModeProcessor = new LinearTestMode(true);
        break;
    }
}

bool gBlocked = false;

static bool gInLog = false;
VirtButton gSettingBtn;

void processModeSwitch() {
    static bool gBlockedByPreview = false;

    if (gBlocked && !gBlockedByPreview)
        return;

    if (!gModeSwitchBtn.pressing()) {
        gBlocked = gBlockedByPreview = false;
        return;
    }

    if (gModeSwitchBtn.holding() && !gSettingBtn.pressing())
        gBlockedByPreview = true;

    int8_t dir = getEncoderDir();
    if (dir) {
        gModeSwitchBtn.skipEvents();

        gBlocked = gBlockedByPreview = true;

        setMode(ADD_TO_ENUM(ModeId, gModeId, dir));
        gEncoder.clear();
        gTimer.reset();
        gInLog = false;
    }

    if (gBlockedByPreview)
        gDisplay[0] << gModeProcessor->preview();

    gBlocked = gBlockedByPreview;
}

SettingsSetter* gSettingsSetter = nullptr;

void processSettings() {
    if (gBlocked && !gSettingsSetter)
        return;

    // ignore if both btn was clicked
    if (gSettingBtn.click())
        return;

    if (gSettingBtn.hold()) {
        if (gSettingsSetter) {
            if (!gSettingsSetter->couldBeClosed())
                return;
            delete gSettingsSetter;
            gSettingsSetter = nullptr;
        } else {
            gSettingsSetter = new SettingsSetter;
        }
    }

    if (gSettingsSetter)
        gSettingsSetter->process();

    gBlocked = gSettingsSetter;
}

void processView() {
    static bool gViewState = LOW;

    if (gBlocked && !gViewState)
        return;

    static uint32_t gViewModeTurnOffTime;

    if (gViewBtn.click()) {
        gViewState = !gViewState;
        digitalWrite(RELAY, gViewState);
        gViewModeTurnOffTime = millis() + gSettings.autoFinishViewMinutes * 60000L;
    }

    gBlocked = gViewState;

    if (gViewState != HIGH)
        return;

    gDisplay[0] << "View";

    if (!gSettings.autoFinishViewMinutes) {
        gDisplay[1] << "Auto stop is off";
        return;
    }

    Time beforeStop = Time::fromMillis(gViewModeTurnOffTime - millis());

    if (beforeStop < 0_ts) {
        gBlocked = gViewState = LOW;
        digitalWrite(RELAY, gViewState);
    } else {
        char str[DISPLAY_COLS] = "";
        beforeStop.getFormatedTime(str, false);
        gDisplay[1] << "Auto stop: " << str;
    }
}

void processMode() {
    static bool gBlockedByRun = false;

    if (gBlocked && !gBlockedByRun)
        return;

    if (gExtraBtn.hold()) {
        gInLog = false;
        gModeProcessor->reset();
        gTimer.reset();
    }

    if (gExtraBtn.click()) {
        if (gModeProcessor->canSwitchView()) {
            gDisplay.resetBlink();
            gModeProcessor->switchView();
        } else if (!gBlockedByRun)
            gInLog = !gInLog;
    }

    if (!gBlocked && gModeSwitchBtn.click()) {
        if (gInLog)
            gInLog = false;
        else
            gModeProcessor->switchMode();
    }

    if (gInLog) {
        gModeProcessor->printLog();
    } else {
        gModeProcessor->process();
    }

    gBlocked = gBlockedByRun = gTimer.state() == Timer::RUNNING;
}

void setup() {
    setupEncoder();
    gBeeper.setup();

    setMode(ModeId::testFStops);

    pinMode(BACKLIGHT, OUTPUT);
    analogWrite(BACKLIGHT, gSettings.backlight);
    pinMode(RELAY, OUTPUT);
    analogWrite(RELAY, 0);

    if (gSettings.startWithSettings)
        gSettingsSetter = new SettingsSetter;
}

void loop() {
    gTimer.tick();
    gBeeper.tick();
    gEncoder.tick();
    gStartBtn.tick();
    gExtraBtn.tick();
    gViewBtn.tick();
    gModeSwitchBtn.tick();
    gSettingBtn.tick(gViewBtn, gModeSwitchBtn);
    gDisplay.tick();

    if (gBeeper.blocking())
        return;

    processModeSwitch();
    processSettings();
    processView();
    processMode();
}
