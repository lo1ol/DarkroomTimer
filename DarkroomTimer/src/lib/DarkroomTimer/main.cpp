#include "Tools.h"

#include "Config.h"

#include "ModeProcessor.h"

#include "Modes/FStopTestMode.h"
#include "Modes/LinearTestMode.h"
#include "Modes/MaskMode.h"
#include "Modes/PrintMode.h"
#include "Modes/RelMaskMode.h"

#include "SettingsSetter.h"

enum class ModeId : uint8_t {
    testFStops,
    testLinear,
    print,
    mask,
    relMask,
    splitFStops,
    splitLinear,
    splitMask,
    splitRelMask,
    localFStops,
    localLinear,
    last_
};

ModeId gModeId;
ModeProcessor* gModeProcessor = nullptr;

const char* getPreview(ModeId modeId) {
    switch (modeId) {
    case ModeId::testFStops:
        return "F Stop test";
    case ModeId::testLinear:
        return "Linear test";
    case ModeId::print:
        return "Printing";
    case ModeId::mask:
        return "Mask printing";
    case ModeId::relMask:
        return "Rel mask print";
    case ModeId::splitFStops:
        return "Splt F Stop test";
    case ModeId::splitLinear:
        return "Splt linear test";
    case ModeId::splitMask:
        return "Splt mask";
    case ModeId::splitRelMask:
        return "Splt rel mask";
    case ModeId::localFStops:
        return "Locl F Stop test";
    case ModeId::localLinear:
        return "Locl linear test";
    }

    return "";
}

void setMode(ModeId modeId) {
    if (gModeProcessor)
        delete (gModeProcessor);

    gModeId = modeId;

    switch (gModeId) {
    case ModeId::testFStops:
        gModeProcessor = new FStopTestMode(FStopTestMode::Generic);
        break;
    case ModeId::testLinear:
        gModeProcessor = new LinearTestMode(LinearTestMode::Generic);
        break;
    case ModeId::print:
        gModeProcessor = new PrintMode();
        break;
    case ModeId::mask:
        gModeProcessor = new MaskMode(1);
        break;
    case ModeId::relMask:
        gModeProcessor = new RelMaskMode(1);
        break;
    case ModeId::splitFStops:
        gModeProcessor = new FStopTestMode(FStopTestMode::SplitGrade);
        break;
    case ModeId::splitLinear:
        gModeProcessor = new LinearTestMode(LinearTestMode::SplitGrade);
        break;
    case ModeId::splitMask:
        gModeProcessor = new MaskMode(2);
        break;
    case ModeId::splitRelMask:
        gModeProcessor = new RelMaskMode(2);
        break;
    case ModeId::localFStops:
        gModeProcessor = new FStopTestMode(FStopTestMode::Local);
        break;
    case ModeId::localLinear:
        gModeProcessor = new LinearTestMode(LinearTestMode::Local);
        break;
    }
}

bool gBlocked = false;

VirtButton gSettingBtn;

void processModeSwitch() {
    static bool gBlockedByPreview = false;
    static ModeId gNewMode = gModeId;

    if (gBlocked && !gBlockedByPreview)
        return;

    if (!gModeBtn.pressing()) {
        if (gBlockedByPreview) {
            if (gNewMode != gModeId)
                setMode(gNewMode);
            else
                gModeProcessor->repaint();
        }
        gBlocked = gBlockedByPreview = false;
        return;
    }

    if (gModeBtn.holding() && !gSettingBtn.pressing())
        gBlockedByPreview = true;

    int8_t dir = gEncoder.getDir();
    if (dir) {
        gModeBtn.skipEvents();

        gBlocked = gBlockedByPreview = true;

        gNewMode = ADD_TO_ENUM(ModeId, gNewMode, dir);
        gEncoder.clear();
        gTimer.reset();
    }

    if (gBlockedByPreview) {
        gDisplay.reset();
        gDisplay[0] << getPreview(gNewMode);
    }

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
            gModeProcessor->repaint();
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
        gDigitalWrite(RELAY, gViewState);
        gViewModeTurnOffTime = gMillis() + gSettings.autoFinishViewMinutes * 60000L;

        if (gViewState == LOW)
            gModeProcessor->repaint();
    }

    gBlocked = gViewState;

    if (gViewState != HIGH)
        return;

    gDisplay.reset();
    gDisplay[0] << "View";

    if (!gSettings.autoFinishViewMinutes) {
        gDisplay[1] << "Auto stop is off";
        return;
    }

    Time beforeStop = Time::fromMillis(gViewModeTurnOffTime - gMillis());

    if (beforeStop < 0_ts) {
        gBlocked = gViewState = LOW;
        gDigitalWrite(RELAY, gViewState);
        gModeProcessor->repaint();
    } else {
        char str[DISPLAY_COLS] = "";
        beforeStop = (beforeStop / 10) * 10 + 1_s;
        beforeStop.getFormatedTime(str, false);
        gDisplay[1] << "Auto stop: " << str;
    }
}

void processMode() {
    static bool gBlockedByRun = false;

    if (gBlocked && !gBlockedByRun)
        return;

    if (gEncoderBtn.hold()) {
        gModeProcessor->reset();
        gTimer.reset();
        gModeProcessor->repaint();
    }

    if (!gBlocked && gModeBtn.click())
        gModeProcessor->switchMode();

    gModeProcessor->process();

    gBlocked = gBlockedByRun = gTimer.state() == Timer::RUNNING;
}

void setup_() {
    gBeeper.setup();

    setMode(ModeId::testFStops);

    pinMode(BACKLIGHT, OUTPUT);
    gAnalogWrite(BACKLIGHT, gSettings.backlight);
    pinMode(RELAY, OUTPUT);
    gDigitalWrite(RELAY, 0);

    if (gSettings.startWithSettings)
        gSettingsSetter = new SettingsSetter;
}

void loop_() {
    gTimer.tick();
    gBeeper.tick();
    gEncoder.tick();
    gStartBtn.tick();
    gEncoderBtn.tick();
    gViewBtn.tick();
    gModeBtn.tick();
    gSettingBtn.tick(gViewBtn, gModeBtn);

    processModeSwitch();
    processSettings();
    processView();
    processMode();

    gDisplay.tick();
}
