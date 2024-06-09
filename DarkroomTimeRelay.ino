#include "Tools.h"

#include "ModeProcessor.h"

#include "FstopTestMode.h"
#include "LinearTestMode.h"
#include "MaskMode.h"
#include "PrintMode.h"
#include "SetLagTimeMode.h"

enum class ModeId { testFStops, testLinear, print, mask, last_, setLagTime };

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
    case ModeId::setLagTime:
        gModeProcessor = new SetLagTimeMode();
        break;
    }
}

void checkSwitchMode() {
    if (gModeSwitchBtn.click()) {
        setMode((ModeId)(((int)gModeId + 1) % (int)ModeId::last_));
    }
}

int gRelayState = LOW;

void setup() {
    gTimer.setup();
    gTimer.setLagTime(SetLagTimeMode::getLagTime());
    gLcd.begin(MAX_SYMS_PER_LINE, 2);

    setupEncoder();

    setMode(ModeId::testFStops);
}

void loop() {
    gTimer.tick();
    gEncoder.tick();
    gStartBtn.tick();
    gExtraBtn.tick();
    gViewBtn.tick();
    gModeSwitchBtn.tick();

    if (gTimer.state() == Timer::STOPPED) {
        if (gViewBtn.click()) {
            gRelayState = !gRelayState;
            digitalWrite(RELAY, gRelayState);
        }

        if (gRelayState == HIGH)
            return;

        checkSwitchMode();

        if (gViewBtn.hold() && gModeSwitchBtn.pressing()) {
            if (gModeId != ModeId::setLagTime)
                setMode(ModeId::setLagTime);
            else
                setMode(ModeId::testFStops);
        }
    }

    gModeProcessor->process();

    if (gExtraBtn.hold())
        gModeProcessor->reset();
}
