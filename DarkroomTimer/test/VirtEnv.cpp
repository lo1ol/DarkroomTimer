#include "VirtEnv.h"

LiquidCrystalWrapMock gLcdWrapMock;

int gBuzzerVal = 0;
bool gRelayVal = 0;

decltype(gMillis()) gCurrentTime = 0;

decltype(gMillis()) mockMillis() {
    return gCurrentTime;
}

void mockAnalogWrite(uint8_t pin, int val) {
    switch (pin) {
    case BEEPER:
        gBuzzerVal = val;
        return;
    }

    assert(false);
    return;
}

void mockDigitalWrite(uint8_t pin, uint8_t val) {
    switch (pin) {
    case RELAY:
        gRelayVal = val;
        return;
    }

    assert(false);
    return;
}
