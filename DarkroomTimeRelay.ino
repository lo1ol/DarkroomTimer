#include "Tools.h"

#include "ModeProcessor.h"

#include "FstopTestMode.h"
#include "LinearTestMode.h"
#include "MaskMode.h"
#include "PrintMode.h"

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

Button gModeSwitchBtn(MODE_SWITCH_BTN);

void checkSwitchMode() {
    if (gModeSwitchBtn.pressed()) {
        setMode((ModeId)(((int)gModeId + 1) % (int)ModeId::last_));
    }
}

void setup() {
    Serial.begin(9600);
    gModeSwitchBtn.begin();
    gStartBtn.begin();
    gExtraBtn.begin();
    gTimer.setup();
    gLcd.begin(MAX_SYMS_PER_LINE, 2);

    setMode(ModeId::testFStops);
}

void loop() {
    gTimer.tick();
    gEncoder.tick();

    if (gTimer.state() == Timer::STOPPED)
        checkSwitchMode();

    gModeProcessor->process();
}
