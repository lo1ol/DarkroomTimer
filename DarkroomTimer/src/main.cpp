#pragma GCC diagnostic error "-Wswitch"

#include "Config.h"
#include "CustomChars.h"
#include "Hardware.h"
#include "Utils.h"

#include "ModeProcessor.h"

#include "Modes/FStopTestMode.h"
#include "Modes/LinearTestMode.h"
#include "Modes/MaskMode.h"
#include "Modes/PrintMode.h"
#include "Modes/RelMaskMode.h"

#include "SettingsSetter.h"

#define MODE_DESCS                                                                               \
    MODE_DESC(fStopTest, F("F Stop test"), FStopTestMode(TestMode::Generic))                     \
    MODE_DESC(splitGradeFStopTest, F("Splt F Stop test"), FStopTestMode(TestMode::SplitGrade))   \
    MODE_DESC(localizedFStopTest, F("Locl F Stop test"), FStopTestMode(TestMode::Local))         \
    MODE_DESC(expertFStopTest, F("Xprt F Stop test"), FStopTestMode(TestMode::Expert))           \
    MODE_DESC(linearTest, F("Linear test"), LinearTestMode(TestMode::Generic))                   \
    MODE_DESC(splitGradeLinearTest, F("Splt linear test"), LinearTestMode(TestMode::SplitGrade)) \
    MODE_DESC(localizedLinearTest, F("Locl linear test"), LinearTestMode(TestMode::Local))       \
    MODE_DESC(expertLinearTest, F("Xprt linear test"), LinearTestMode(TestMode::Expert))         \
    MODE_DESC(print, F("Printing"), PrintMode())                                                 \
    MODE_DESC(mask, F("Mask printing"), MaskMode(1))                                             \
    MODE_DESC(splitGradeMask, F("Splt mask"), MaskMode(2))                                       \
    MODE_DESC(relMask, F("Rel mask print"), RelMaskMode(1))                                      \
    MODE_DESC(splitGradeRelMask, F("Splt rel mask"), RelMaskMode(2))

enum class ModeId : uint8_t { TIMER_MODES };

static_assert(static_cast<int>(ModeId::last_), "TIMER_MODES couldn't be empty");

ModeId gModeId;
static ModeId gNewModeId = gModeId;
ModeProcessor* gModeProcessor = nullptr;

[[nodiscard]] const __FlashStringHelper* getPreview(ModeId modeId) {
    switch (modeId) {
#define MODE_DESC(id, str, _)            \
    case ModeId::id:                     \
        if (ModeId::id >= ModeId::last_) \
            return nullptr;              \
        return str;

        MODE_DESCS
#undef MODE_DESC
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
#define MODE_DESC(id, _, impl)                \
    case ModeId::id:                          \
        if (ModeId::id >= ModeId::last_)      \
            return;                           \
        gModeProcessor = new (gModeBuf) impl; \
        break;

        MODE_DESCS
#undef MODE_DESC
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

    bool needUpdate = false;

    if (gModeBtn.holding() && !gSettingBtn.pressing()) {
        if (!gBlockedByPreview)
            needUpdate = true;

        gBlockedByPreview = true;
    }

    int8_t dir = gEncoder.getDir();
    if (dir) {
        needUpdate = true;
        gModeBtn.skipEvents();

        gBlocked = gBlockedByPreview = true;

        gNewModeId = ADD_TO_ENUM(ModeId, gNewModeId, dir);
        gEncoder.clear();
        gTimer.reset();
    }

    if (gBlockedByPreview && needUpdate) {
        gDisplay.reset();
        gDisplay[0].printWithAnimation(getPreview(gNewModeId), 200);
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
            gDisplay[0] << F(kLampSymTop "View");
        } else {
            gModeProcessor->repaint();
        }
    }

    gBlocked = gViewState;

    if (!gViewState)
        return;

    gDisplay[1].reset();
    gDisplay[1] << F(kLampSymBottom);
    if (!gSettings.autoFinishViewMinutes) {
        gDisplay[1] << F("Auto stop is off");
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
        gDisplay[1] << F("Auto stop: ") << str;
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

    gDisplay.setupCharset(Charset::Main);

#ifndef PIO_UNIT_TESTING
    #if STARTUP_LOGO_VERSION != LOGO_DISABLED
    gDisplay.playLogo();
    #endif
#endif

    gBeeper.setup();

    gPinMode(LCD_BACKLIGHT_PIN, OUTPUT);
    setDisplayBacklight(gSettings.backlight);
    gPinMode(RELAY_PIN, OUTPUT);
    gDigitalWrite(RELAY_PIN, 0);

    setMode(static_cast<ModeId>(0));
    gModeProcessor->repaint();

    if (gSettings.startWith == StartWith::Settings)
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
