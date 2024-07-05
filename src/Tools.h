#pragma once

#define EB_FAST_TIME 13
#define EB_HOLD_TIME 500

#include <EncButton.h>

#include "Display.h"
#include "Settings.h"
#include "Time.h"
#include "Timer.h"

#define ADD_TO_ENUM(enumName, current, num) \
    (enumName)(((uint8_t)(current) + (uint8_t)enumName::last_ + num) % (uint8_t)enumName::last_)

int8_t getEncoderDir();
bool getInt(uint8_t& choosen, uint8_t min, uint8_t max);
void getTime(Time& time, bool smooth = false);

void setupEncoder();

extern EncButton gEncoder;
extern Button gModeSwitchBtn;
extern Button gStartBtn;
extern Button gExtraBtn;
extern Button gViewBtn;
extern Timer gTimer;
extern Settings gSettings;
extern Display gDisplay;
