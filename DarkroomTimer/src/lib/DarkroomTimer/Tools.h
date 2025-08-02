#pragma once

#include <Arduino.h>

#include "SafeEncButton.h"

#include "Beeper.h"
#include "Config.h"
#include "Display.h"
#include "EncoderWrap.h"
#include "RelTimeTable.h"
#include "ScrollableContent.h"
#include "Settings.h"
#include "Time.h"
#include "TimeTable.h"
#include "Timer.h"

#define ADD_TO_ENUM(enumName, current, num) \
    (enumName)(((uint8_t)(current) + (uint8_t)enumName::last_ + num) % (uint8_t)enumName::last_)

void alignStr(char* buf, uint8_t align);

extern EncoderWrap gEncoder;
extern ButtonT<MODE_BTN> gModeBtn;
extern ButtonT<START_BTN> gStartBtn;
extern ButtonT<ENCODER_BTN> gEncoderBtn;
extern ButtonT<VIEW_BTN> gViewBtn;

extern Timer gTimer;
extern Settings gSettings;
extern Display gDisplay;
extern Beeper gBeeper;
extern uint8_t gModesCache[32];
extern ScrollableContent gScrollableContent;

#ifdef PIO_UNIT_TESTING
uint32_t gMillis();
void gDigitalWrite(uint8_t, uint8_t);
void gAnalogWrite(uint8_t, int);
#else
    #define gMillis millis
    #define gDigitalWrite digitalWrite
    #define gAnalogWrite analogWrite
#endif
