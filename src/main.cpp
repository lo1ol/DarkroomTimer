#include "Tools.h"

#include "ModeProcessor.h"

#include "Modes/FStopTestMode.h"
#include "Modes/LinearTestMode.h"
#include "Modes/MaskMode.h"
#include "Modes/PrintMode.h"

#include "SettingsSetter.h"

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

VirtButton gSettingBtn;
SettingsSetter* gSettingsSetter = nullptr;

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

    static bool gRelayState = LOW;

    // ignore if both btn was clicked
    if (gSettingBtn.click())
        return;

    if (gRelayState == LOW && gTimer.state() != Timer::RUNNING && gSettingBtn.hold()) {
        if (gSettingsSetter) {
            delete gSettingsSetter;
            gSettingsSetter = nullptr;
        } else {
            gSettingsSetter = new SettingsSetter;
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

            gDisplay[0] << "View";

            return;
        }

        if (gModeSwitchBtn.click())
            setMode((ModeId)(((int)gModeId + 1) % (int)ModeId::last_));
    }

    gModeProcessor->process();

    if (gExtraBtn.hold())
        gModeProcessor->reset();
}
