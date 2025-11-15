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

namespace {
constexpr const char kFStopTestStr[] PROGMEM = "F Stop test";
constexpr const char kLinearTestStr[] PROGMEM = "Linear test";
constexpr const char kPrintingStr[] PROGMEM = "Printing";
constexpr const char kMaskPrinting[] PROGMEM = "Mask printing";
constexpr const char kRelMaskPrinting[] PROGMEM = "Rel mask print";

constexpr const char kSplitGradeStr[] PROGMEM = "Split grade";
constexpr const char kLocalizedStr[] PROGMEM = "Localized";
constexpr const char kExpertStr[] PROGMEM = "Expert";
} // namespace

#define MODE_DESCS                                                                                        \
    MODE_DESC(fStopTest, kFStopTestStr, nullptr, FStopTestMode(TestMode::Generic))                        \
    MODE_DESC(splitGradeFStopTest, kFStopTestStr, kSplitGradeStr, FStopTestMode(TestMode::SplitGrade))    \
    MODE_DESC(localizedFStopTest, kFStopTestStr, kLocalizedStr, FStopTestMode(TestMode::Local))           \
    MODE_DESC(expertFStopTest, kFStopTestStr, kExpertStr, FStopTestMode(TestMode::Expert))                \
    MODE_DESC(linearTest, kLinearTestStr, nullptr, LinearTestMode(TestMode::Generic))                     \
    MODE_DESC(splitGradeLinearTest, kLinearTestStr, kSplitGradeStr, LinearTestMode(TestMode::SplitGrade)) \
    MODE_DESC(localizedLinearTest, kLinearTestStr, kLocalizedStr, LinearTestMode(TestMode::Local))        \
    MODE_DESC(expertLinearTest, kLinearTestStr, kExpertStr, LinearTestMode(TestMode::Expert))             \
    MODE_DESC(print, kPrintingStr, nullptr, PrintMode())                                                  \
    MODE_DESC(mask, kMaskPrinting, nullptr, MaskMode(1))                                                  \
    MODE_DESC(splitGradeMask, kMaskPrinting, kSplitGradeStr, MaskMode(2))                                 \
    MODE_DESC(relMask, kRelMaskPrinting, nullptr, RelMaskMode(1))                                         \
    MODE_DESC(splitGradeRelMask, kRelMaskPrinting, kSplitGradeStr, RelMaskMode(2))

enum class ModeId : uint8_t { TIMER_MODES };

static_assert(static_cast<int>(ModeId::last_), "TIMER_MODES couldn't be empty");

ModeId gModeId;
static ModeId gNewModeId = gModeId;
ModeProcessor* gModeProcessor = nullptr;

[[nodiscard]] const __FlashStringHelper* getMainPreview(ModeId modeId) {
    switch (modeId) {
#define MODE_DESC(id, str, _1, _2)       \
    case ModeId::id:                     \
        if (ModeId::id >= ModeId::last_) \
            return nullptr;              \
        return (const __FlashStringHelper*)(str);

        MODE_DESCS
#undef MODE_DESC
    case ModeId::last_:
        assert(false);
        return nullptr;
    }

    return nullptr;
}

[[nodiscard]] const __FlashStringHelper* getSubPreview(ModeId modeId) {
    switch (modeId) {
#define MODE_DESC(id, _1, str, _2)       \
    case ModeId::id:                     \
        if (ModeId::id >= ModeId::last_) \
            return nullptr;              \
        return (const __FlashStringHelper*)(str);

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
#define MODE_DESC(id, _1, _2, impl)           \
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
            gDisplay.setupCharset(Charset::Main);
            if (gNewModeId != gModeId)
                setMode(gNewModeId);

            gTimer.reset();
            gModeProcessor->repaint();
        }
        gBlocked = gBlockedByPreview = false;
        return;
    }

    bool needUpdate = false;

    if (gModeBtn.holding() && !gSettingBtn.pressing()) {
        if (!gBlockedByPreview) {
            gDisplay.setupCharset(Charset::ModePreview);
            needUpdate = true;
        }

        gBlockedByPreview = true;
    }

    int8_t dir = gEncoder.getDir();
    if (dir) {
        needUpdate = true;
        gModeBtn.skipEvents();

        gBlocked = gBlockedByPreview = true;

        gNewModeId = ADD_TO_ENUM(ModeId, gNewModeId, dir);
        gEncoder.clear();
    }

    gBlocked = gBlockedByPreview;

    if (!gBlockedByPreview || !needUpdate)
        return;

    gDisplay.reset();
    switch (static_cast<uint8_t>(gNewModeId) % 4) {
    case 0:
        gDisplay[0] << F(kFolder1Str);
        break;
    case 1:
        gDisplay[0] << F(kFolder2Str);
        break;
    case 2:
        gDisplay[0] << F(kFolder3Str);
        break;
    case 3:
        gDisplay[0] << F(kFolder4Str);
        break;
    }

    gDisplay[0] << getMainPreview(gNewModeId);

    auto subPreview = getSubPreview(gNewModeId);
    if (subPreview)
        gDisplay[1] << F(" " kSubFolderPointerSym) << subPreview;
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

    static uint32_t gViewModeStartTime;

    if (gViewBtn.click()) {
        gViewState = !gViewState;
        gDigitalWrite(RELAY_PIN, gViewState);
        gViewModeStartTime = gMillis();

        if (gViewState) {
            gDisplay.reset();
            gDisplay.setupCharset(Charset::View);
            gDisplay[0] << F(kLampSym " View");
        } else {
            gDisplay.setupCharset(Charset::Main);
            gModeProcessor->repaint();
        }
    }

    gBlocked = gViewState;

    if (!gViewState)
        return;

    gDisplay[1].reset();

    uint32_t passedTime = gMillis() - gViewModeStartTime;
    uint8_t lampAnimStage = (passedTime % 1000) / 167;
    char animStr[2] = kLampLight1Sym;
    animStr[0] += lampAnimStage;
    gDisplay[1] << animStr << " ";

    if (!gSettings.autoFinishViewMinutes) {
        gDisplay[1] << F("Auto stop is off");
        return;
    }

    if (passedTime >= gSettings.autoFinishViewMinutes * 60000L) {
        gBlocked = gViewState = false;
        gDigitalWrite(RELAY_PIN, gViewState);
        gDisplay.setupCharset(Charset::Main);
        gModeProcessor->repaint();
        return;
    }

    uint32_t leftTime = gSettings.autoFinishViewMinutes * 60000L - passedTime;

    char str[DISPLAY_COLS] = "";
    itoa((leftTime - 1) / 1000 + 1, str, 10);
    gDisplay[1] << F("Auto stop: ") << str;
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
