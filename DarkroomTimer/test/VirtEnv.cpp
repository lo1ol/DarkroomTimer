#include "VirtEnv.h"

#include <Hardware.h>

LiquidCrystalWrapMock gLcdWrapMock;
Display gDisplay(&gLcdWrapMock);

int gBuzzerVal = 0;
int gBacklightVal = 0;
bool gRelayVal = 0;

decltype(gMillis()) gCurrentTime = 0;

uint32_t gMillis() {
    return gCurrentTime;
}

void gAnalogWrite(uint8_t pin, int val) {
    switch (pin) {
    case BEEPER:
        gBuzzerVal = val;
        return;
    case BACKLIGHT:
        gBacklightVal = val;
        return;
    }

    assert(false);
    return;
}

void gDigitalWrite(uint8_t pin, uint8_t val) {
    switch (pin) {
    case RELAY:
        gRelayVal = val;
        return;
    }

    assert(false);
    return;
}
