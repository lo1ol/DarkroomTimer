#pragma once

#define EB_FAST_TIME 13
#define EB_HOLD_TIME 500

#include <EncButton.h>

#include "Display.h"
#include "Settings.h"
#include "Timer.h"

bool getInt(uint8_t& choosen, uint8_t min, uint8_t max);
void getTime(uint32_t& time);

void setupEncoder();

extern EncButton gEncoder;
extern Button gModeSwitchBtn;
extern Button gStartBtn;
extern Button gExtraBtn;
extern Button gViewBtn;
extern Timer gTimer;
extern Settings gSettings;
extern Display gDisplay;
