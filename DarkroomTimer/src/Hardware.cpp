#include "Hardware.h"

ButtonT<MODE_BTN_PIN> gModeBtn;
ButtonT<START_BTN_PIN> gStartBtn;
ButtonT<ENCODER_BTN_PIN> gEncoderBtn;
ButtonT<VIEW_BTN_PIN> gViewBtn;

Settings gSettings = Settings::load();
Timer gTimer;
Beeper gBeeper(BEEPER_PIN);
uint8_t gModesCache[32];
ScrollableContent gScrollableContent;
Lcd gLcd;
IdleShower gIdleShower;

Display gDisplay(&gLcd);
DTEncoder& gEncoder = DTEncoder::getInstance();

void setDisplayBacklight(uint8_t val) {
    gAnalogWrite(LCD_BACKLIGHT_PIN, val * LCD_BACKLIGHT_STEP);
}

uint32_t gRandomGenSeed;

void reinitRandomGenSeed() {
#ifdef PIO_UNIT_TESTING
    gRandomGenSeed = gMillis();
#else
    gRandomGenSeed = micros() ^ gAnalogRead(A6);
#endif
}

uint32_t genRandom() {
    constexpr uint32_t a = 1103515245;
    constexpr uint32_t c = 12345;
    constexpr uint32_t m = 2147483648;

    gRandomGenSeed = (a * gRandomGenSeed + c) % m;
    return gRandomGenSeed;
}

#ifdef PIO_UNIT_TESTING
int gBuzzerVal = 0;
int gBacklightVal = 0;
bool gRelayVal = 0;

uint32_t gCurrentTime = 0;

uint32_t gMillis() {
    return gCurrentTime;
}

void gAnalogWrite(uint8_t pin, int val) {
    switch (pin) {
    case BEEPER_PIN:
        gBuzzerVal = val;
        return;
    case LCD_BACKLIGHT_PIN:
        gBacklightVal = val;
        return;
    }

    assert(false);
    return;
}

void gDigitalWrite(uint8_t pin, uint8_t val) {
    switch (pin) {
    case RELAY_PIN:
        gRelayVal = val;
        return;
    }

    assert(false);
    return;
}
#endif
