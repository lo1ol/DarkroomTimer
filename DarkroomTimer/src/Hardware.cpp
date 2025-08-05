#include "Hardware.h"

#include <LiquidCrystal.h>

ButtonT<MODE_BTN> gModeBtn;
ButtonT<START_BTN> gStartBtn;
ButtonT<ENCODER_BTN> gEncoderBtn;
ButtonT<VIEW_BTN> gViewBtn;

Settings gSettings = Settings::load();
Timer gTimer;
Beeper gBeeper(BEEPER);
uint8_t gModesCache[32];
ScrollableContent gScrollableContent;

#ifndef PIO_UNIT_TESTING
LiquidCrystal gLcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);
LiquidCrystalWrap gLcdWrap(gLcd);

static EncButton gEncButton(ENCODER_DT, ENCODER_CLK);
void isr() {
    gEncButton.tickISR();
}

EncButton* getEncoder() {
    attachInterrupt(0, isr, CHANGE);
    attachInterrupt(1, isr, CHANGE);
    gEncButton.setEncISR(true);
    return &gEncButton;
}

EncoderWrap gEncoder(getEncoder());

#else

LiquidCrystalWrap gLcdWrap;
EncoderWrap gEncoder;

int gBuzzerVal = 0;
int gBacklightVal = 0;
bool gRelayVal = 0;

uint32_t gCurrentTime = 0;

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
#endif

Display gDisplay(&gLcdWrap);

void setDisplayBacklight(uint8_t val) {
    gAnalogWrite(BACKLIGHT, val * BACKLIGHT_STEP);
}
