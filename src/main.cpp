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
    static bool gBlockedByLog = false;

    if (gBlocked && !gBlockedByRun && !gBlockedByLog)
        return;

    if (gExtraBtn.hold()) {
        gModeProcessor->reset();
        gTimer.reset();
    }

    if ((!gBlockedByRun || gModeProcessor->supportPrintInLog()) && gExtraBtn.click())
        gBlockedByLog = !gBlockedByLog;

    if (!gBlocked && gModeSwitchBtn.click())
        gModeProcessor->switchMode();

    if (gBlockedByLog) {
        gModeProcessor->printLog();
    } else {
        gModeProcessor->process();
        gBlockedByRun = gTimer.state() == Timer::RUNNING;
    }

    gBlocked = gBlockedByRun || gBlockedByLog;
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
            setMode((ModeId)((uint8_t)((uint8_t)gModeId + dir) % (uint8_t)ModeId::last_));
            gEncoder.clear();
            gTimer.reset();
        }
    }

    processSettings();
    processView();
    processMode();
}
