#include "Hardware.h"

#include <LiquidCrystal.h>
#include "LiquidCrystalWrap.h"

ButtonT<MODE_BTN> gModeBtn;
ButtonT<START_BTN> gStartBtn;
ButtonT<ENCODER_BTN> gEncoderBtn;
ButtonT<VIEW_BTN> gViewBtn;

Timer gTimer;
Settings gSettings = Settings::load();
Beeper gBeeper(BEEPER);
uint8_t gModesCache[32];
ScrollableContent gScrollableContent;

#ifndef PIO_UNIT_TESTING
LiquidCrystal gLcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);
LiquidCrystalWrap gLcdWrap(&gLcd);
Display gDisplay(&gLcdWrap);

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
EncoderWrap gEncoder;
#endif

void setDisplayBacklight(uint8_t val) {
    gAnalogWrite(BACKLIGHT, val * BACKLIGHT_STEP);
}
