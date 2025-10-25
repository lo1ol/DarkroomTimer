#pragma once

#include "common/SafeEncButton.h"

#ifndef PIO_UNIT_TESTING
    #include <GyverIO.h>
#endif

#include "Beeper.h"
#include "Config.h"
#include "DTEncoder.h"
#include "Display.h"
#include "Lcd.h"
#include "ScrollableContent.h"
#include "Settings.h"
#include "Timer.h"

void setDisplayBacklight(uint8_t val);

extern DTEncoder& gEncoder;
extern ButtonT<MODE_BTN_PIN> gModeBtn;
extern ButtonT<START_BTN_PIN> gStartBtn;
extern ButtonT<ENCODER_BTN_PIN> gEncoderBtn;
extern ButtonT<VIEW_BTN_PIN> gViewBtn;

extern Lcd gLcd;
extern Display gDisplay;

extern Timer gTimer;
extern Settings gSettings;
extern Beeper gBeeper;
extern uint8_t gModesCache[32];
extern ScrollableContent gScrollableContent;

#ifdef PIO_UNIT_TESTING
uint32_t gMillis();
void gDigitalWrite(uint8_t, uint8_t);
void gAnalogWrite(uint8_t, int);

extern uint32_t gCurrentTime;

extern int gBuzzerVal;
extern int gBacklightVal;
extern bool gRelayVal;
#else
    #define gMillis millis
    #define gDigitalRead gio::read
    #define gDigitalWrite gio::write
    #define gPinMode gio::mode
    #define gAnalogWrite analogWrite
#endif
