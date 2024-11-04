#pragma once

#include "SafeEncButton.h"

#include "Beeper.h"
#include "Config.h"
#include "Display.h"
#include "RelTimeTable.h"
#include "ScrollableContent.h"
#include "Settings.h"
#include "Time.h"
#include "TimeTable.h"
#include "Timer.h"

#define ADD_TO_ENUM(enumName, current, num) \
    (enumName)(((uint8_t)(current) + (uint8_t)enumName::last_ + num) % (uint8_t)enumName::last_)

int8_t getEncoderDir();
bool getInt(uint8_t& choosen, uint8_t min, uint8_t max);
bool getTime(Time& time, bool smooth = false);
bool getRelTime(RelTime& time);

void alignStr(char* buf, uint8_t align);

void setupEncoder();

extern EncButton gEncoder;
extern ButtonT<MODE_SWITCH_BTN> gModeSwitchBtn;
extern ButtonT<START_BTN> gStartBtn;
extern ButtonT<EXTRA_BTN> gExtraBtn;
extern ButtonT<VIEW_BTN> gViewBtn;
extern Timer gTimer;
extern Settings gSettings;
extern Display gDisplay;
extern Beeper gBeeper;
extern TimeTable gTimeTable[2];
extern RelTimeTable gRelTimeTable[2];
extern ScrollableContent gScrollableContent;
