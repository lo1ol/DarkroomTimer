#include "Tools.h"

#include "ModeProcessor.h"

#include "Modes/FStopTestMode.h"
#include "Modes/LinearTestMode.h"
#include "Modes/MaskMode.h"
#include "Modes/PrintMode.h"

#include "SettingsSetter.h"

enum class ModeId : uint8_t { testFStops, testLinear, print, mask, last_ };

ModeId gModeId;
ModeProcessor* gModeProcessor = nullptr;

void setMode(ModeId modeId) {
    if (gModeProcessor)
        delete (gModeProcessor);

    gModeId = modeId;

    switch (gModeId) {
    case ModeId::testFStops:
        gModeProcessor = new FStopTestMode();
        break;
    case ModeId::testLinear:
        gModeProcessor = new LinearTestMode();
        break;
    case ModeId::print:
        gModeProcessor = new PrintMode();
        break;
    case ModeId::mask:
        gModeProcessor = new MaskMode();
        break;
    }
}

bool gBlocked = false;

VirtButton gSettingBtn;
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

    if (gViewState == HIGH) {
        if (gSettings.autoFinishViewMinutes != 0 && gViewModeTurnOffTime < millis()) {
            gViewState = LOW;
            digitalWrite(RELAY, gViewState);
        }

        gDisplay[0] << "View";
    }

    gBlocked = gViewState;
}

void processMode() {
    static bool gBlockedByRun = false;
    static bool gInLog = false;

    if (gBlocked && !gBlockedByRun)
        return;

    if (gExtraBtn.hold()) {
        gInLog = false;
        gModeProcessor->reset();
        gTimer.reset();
    }

    if (gExtraBtn.click()) {
        if (gModeProcessor->canSwitchView())
            gModeProcessor->switchView();
        else if (!gBlockedByRun)
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
    gTimer.setup();

    setupEncoder();

    setMode(ModeId::testFStops);

    pinMode(BACKLIGHT, OUTPUT);
    analogWrite(BACKLIGHT, gSettings.backlight);

    if (gSettings.startWithSettings)
        gSettingsSetter = new SettingsSetter;
}

void loop() {
    gTimer.tick();
    gEncoder.tick();
    gStartBtn.tick();
    gExtraBtn.tick();
    gViewBtn.tick();
    gModeSwitchBtn.tick();
    gSettingBtn.tick(gViewBtn, gModeSwitchBtn);
    gDisplay.tick();

    if (!gBlocked && gModeSwitchBtn.pressing()) {
        int8_t dir = getEncoderDir();
        if (dir) {
            setMode(ADD_TO_ENUM(ModeId, gModeId, dir));
            gEncoder.clear();
            gTimer.reset();
        }
    }

    processSettings();
    processView();
    processMode();
}
