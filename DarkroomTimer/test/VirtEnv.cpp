#include "VirtEnv.h"

LiquidCrystalWrapMock gLcdWrapMock;

int gBuzzerVal = 0;
bool gRelayVal = 0;

decltype(gMillis()) gCurrentTime = 0;

decltype(gMillis()) myMillis() {
    return gCurrentTime;
}

void myAnalogWrite(uint8_t pin, int val) {
    switch (pin) {
    case BEEPER:
        gBuzzerVal = val;
        return;
    }

    assert(false);
    return;
}

void myDigitalWrite(uint8_t pin, uint8_t val) {
    switch (pin) {
    case RELAY:
        gRelayVal = val;
        return;
    }

    assert(false);
    return;
}

void initVirtEnv() {
    gLcdWrapMock = LiquidCrystalWrapMock{};
    gDisplay = Display{ &gLcdWrapMock };
    gMillis = &myMillis;
    gAnalogWrite = &myAnalogWrite;
    gDigitalWrite = &myDigitalWrite;

    gCurrentTime = 0;
    gRelayVal = 0;
    gBuzzerVal = 0;
    gSettings.lagTime = 0_ts;
    gTimer = Timer{};
    gBeeper.stop();
}
