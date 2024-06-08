#pragma once

#define EB_FAST_TIME 13
#define EB_HOLD_TIME 500

#include <EncButton.h>
#include <LiquidCrystal.h>

#include "Timer.h"

#define LCD_RS 8
#define LCD_EN 9
#define LCD_D4 5
#define LCD_D5 4
#define LCD_D6 3
#define LCD_D7 2

#define MAX_SYMS_PER_LINE 16

#define MODE_SWITCH_BTN A5
#define VIEW_BTN A1
#define START_BTN A0
#define EXTRA_BTN A2

#define ENCODER_DT 11
#define ENCODER_CLK 12

#define BEEPER 10
#define RELAY A4

void getFormatedTime(unsigned long ms, char* buf, bool accurate = true);

void printFormatedLine(const char* line, int pos);
void printFormatedTime(const char* prefix, unsigned long ms);
void printTimeLog(const char* prefix, unsigned long (*timeGetter)(size_t), size_t maxIndex);

void getInt(size_t& choosen, size_t min, size_t max);
void getTime(unsigned long& time);

void setupEncoder();

extern LiquidCrystal gLcd;
extern EncButton gEncoder;
extern Button gModeSwitchBtn;
extern Button gStartBtn;
extern Button gExtraBtn;
extern Button gViewBtn;
extern Timer gTimer;
