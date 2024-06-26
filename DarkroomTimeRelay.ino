#include "Tools.h"

#include "ModeProcessor.h"

#include "FstopTestMode.h"
#include "LinearTestMode.h"
#include "MaskMode.h"
#include "PrintMode.h"
#include "SetSettingsMode.h"

enum class ModeId { testFStops, testLinear, print, mask, last_ };

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

void setup() {
    gTimer.setup();
    gLcd.begin(MAX_SYMS_PER_LINE, 2);

    setupEncoder();

    setMode(ModeId::testFStops);

    pinMode(BACKLIGHT, OUTPUT);
    analogWrite(BACKLIGHT, gSettings.backlight);
}

void loop() {
    gTimer.tick();
    gEncoder.tick();
    gStartBtn.tick();
    gExtraBtn.tick();
    gViewBtn.tick();
    gModeSwitchBtn.tick();

    static SetSettingsMode* gSettingsSetter = nullptr;
    static bool gRelayState = LOW;

    if (gRelayState == LOW && gTimer.state() != Timer::RUNNING && gViewBtn.hold() && gModeSwitchBtn.pressing()) {
        if (gSettingsSetter) {
            delete gSettingsSetter;
            gSettingsSetter = nullptr;
        } else {
            gSettingsSetter = new SetSettingsMode;
        }
    }

    if (gSettingsSetter) {
        gSettingsSetter->process();
        return;
    }

    if (gTimer.state() == Timer::STOPPED) {
        static uint32_t gViewModeTurnOnTime;

        if (gViewBtn.click()) {
            gRelayState = !gRelayState;
            digitalWrite(RELAY, gRelayState);
            if (gRelayState == HIGH)
                gViewModeTurnOnTime = millis();
        }

        if (gRelayState == HIGH) {
            if (gSettings.autoFinishViewMinutes != 0 &&
                (gViewModeTurnOnTime + gSettings.autoFinishViewMinutes * 1000L * 60) < millis()) {
                gRelayState = LOW;
                digitalWrite(RELAY, gRelayState);
            }

            return;
        }

        if (gModeSwitchBtn.click())
            setMode((ModeId)(((int)gModeId + 1) % (int)ModeId::last_));
    }

    gModeProcessor->process();

    if (gExtraBtn.hold())
        gModeProcessor->reset();
}
