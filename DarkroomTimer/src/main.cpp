#include "Config.h"
#include "Hardware.h"
#include "Utils.h"

#include "ModeProcessor.h"

#include "Modes/FStopTestMode.h"
#include "Modes/LinearTestMode.h"
#include "Modes/MaskMode.h"
#include "Modes/PrintMode.h"
#include "Modes/RelMaskMode.h"

#include "SettingsSetter.h"

enum class ModeId : uint8_t { TIMER_MODES };

static_assert(static_cast<int>(ModeId::last_) && "TIMER_MODES couldn't be empty");

ModeId gModeId;
static ModeId gNewModeId = gModeId;
ModeProcessor* gModeProcessor = nullptr;

[[nodiscard]] const char* getPreview(ModeId modeId) {
    switch (modeId) {
#define RET_MODE_STR(id, str)            \
    case ModeId::id:                     \
        if (ModeId::id >= ModeId::last_) \
            return nullptr;              \
        return str;

        RET_MODE_STR(testFStops, "F Stop test");
        RET_MODE_STR(testLinear, "Linear test");
        RET_MODE_STR(print, "Printing");
        RET_MODE_STR(mask, "Mask printing");
        RET_MODE_STR(relMask, "Rel mask print");
        RET_MODE_STR(splitFStops, "Splt F Stop test");
        RET_MODE_STR(splitLinear, "Splt linear test");
        RET_MODE_STR(splitMask, "Splt linear test");
        RET_MODE_STR(splitRelMask, "Splt rel mask");
        RET_MODE_STR(localFStops, "Locl F Stop test");
        RET_MODE_STR(localLinear, "Locl linear test");
#undef RET_MODE_STR
    case ModeId::last_:
        assert(false);
        return nullptr;
    }

    return nullptr;
}

void setMode(ModeId modeId) {
    if (gModeProcessor)
        gModeProcessor->~ModeProcessor();

    gModeId = modeId;
    gNewModeId = modeId;

    // try not use heap at all
    // to reduce firmware sice
    static uint8_t gModeBuf[sizeof(MaskMode)];

    switch (gModeId) {
#define SET_MODE_IMPL(id, impl)               \
    case ModeId::id:                          \
        if (ModeId::id >= ModeId::last_)      \
            return;                           \
        gModeProcessor = new (gModeBuf) impl; \
        break;

        SET_MODE_IMPL(testFStops, FStopTestMode(FStopTestMode::Generic));
        SET_MODE_IMPL(testLinear, LinearTestMode(LinearTestMode::Generic));
        SET_MODE_IMPL(print, PrintMode());
        SET_MODE_IMPL(mask, MaskMode(1));
        SET_MODE_IMPL(relMask, RelMaskMode(1));
        SET_MODE_IMPL(splitFStops, FStopTestMode(FStopTestMode::SplitGrade));
        SET_MODE_IMPL(splitLinear, LinearTestMode(LinearTestMode::SplitGrade));
        SET_MODE_IMPL(splitMask, MaskMode(2));
        SET_MODE_IMPL(splitRelMask, RelMaskMode(2));
        SET_MODE_IMPL(localFStops, FStopTestMode(FStopTestMode::Local));
        SET_MODE_IMPL(localLinear, LinearTestMode(LinearTestMode::Local));
#undef SET_MODE_IMPL
    case ModeId::last_:
        assert(false);
        return;
    }
}

bool gBlocked = false;

VirtButton gSettingBtn;

void processModeSwitch() {
    static bool gBlockedByPreview = false;

    if (gBlocked && !gBlockedByPreview)
        return;

    if (!gModeBtn.pressing()) {
        if (gBlockedByPreview) {
            if (gNewModeId != gModeId)
                setMode(gNewModeId);

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

        gNewModeId = ADD_TO_ENUM(ModeId, gNewModeId, dir);
        gEncoder.clear();
        gTimer.reset();
    }

    if (gBlockedByPreview) {
        gDisplay.reset();
        gDisplay[0] << getPreview(gNewModeId);
    }

    gBlocked = gBlockedByPreview;
}

SettingsSetter* gSettingsSetter = nullptr;
static uint8_t gSettingsSetterBuf[sizeof(SettingsSetter)];

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
            gSettingsSetter->~SettingsSetter();
            gSettingsSetter = nullptr;
            gModeProcessor->repaint();
        } else {
            gSettingsSetter = new (gSettingsSetterBuf) SettingsSetter;
        }
    }

    if (gSettingsSetter)
        gSettingsSetter->process();

    gBlocked = gSettingsSetter;
}

void processView() {
    static bool gViewState = false;

    if (gBlocked && !gViewState)
        return;

    static uint32_t gViewModeTurnOffTime;

    if (gViewBtn.click()) {
        gViewState = !gViewState;
        gDigitalWrite(RELAY_PIN, gViewState);
        gViewModeTurnOffTime = gMillis() + gSettings.autoFinishViewMinutes * 60000L;

        if (gViewState) {
            gDisplay.reset();
            gDisplay[0] << "View";
        } else {
            gModeProcessor->repaint();
        }
    }

    gBlocked = gViewState;

    if (!gViewState)
        return;

    gDisplay[1].reset();
    if (!gSettings.autoFinishViewMinutes) {
        gDisplay[1] << "Auto stop is off";
        return;
    }

    auto curTime = gMillis();

    if (curTime >= gViewModeTurnOffTime) {
        gBlocked = gViewState = false;
        gDigitalWrite(RELAY_PIN, gViewState);
        gModeProcessor->repaint();
    } else {
        char str[DISPLAY_COLS] = "";
        itoa((gViewModeTurnOffTime - 1 - curTime) / 1000 + 1, str, 10);
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

#ifndef PIO_UNIT_TESTING
void setup() {
    gLcd.init();
#else
void setup_() {
#endif
    gBeeper.setup();

    pinMode(LCD_BACKLIGHT_PIN, OUTPUT);
    setDisplayBacklight(gSettings.backlight);
    pinMode(RELAY_PIN, OUTPUT);
    gDigitalWrite(RELAY_PIN, 0);

    setMode(static_cast<ModeId>(0));
    gModeProcessor->repaint();

    if (gSettings.startWithSettings)
        gSettingsSetter = new (gSettingsSetterBuf) SettingsSetter;
    return;
}

#ifndef PIO_UNIT_TESTING
void loop() {
#else
void loop_() {
#endif
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

#ifdef PIO_UNIT_TESTING
void unsetup_() {
    if (gSettingsSetter) {
        gSettingsSetter->~SettingsSetter();
        gSettingsSetter = nullptr;
    }
    if (gModeProcessor) {
        gModeProcessor->~ModeProcessor();
        gModeProcessor = nullptr;
    }
}
#endif
