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

void getFormatedTime(uint32_t ms, char* buf, bool accurate = true);

inline void concat(char* dst, const char* src) {
    strcpy(dst + strlen(dst), src);
}

inline void concatInt(char* dst, int value) {
    itoa(value, dst + strlen(dst), 10);
}

inline void concatTime(char* dst, uint32_t ms, bool accurate = true) {
    getFormatedTime(ms, dst + strlen(dst), accurate);
}

void printFormatedLine(const char* line, int pos);
void printFormatedTime(const char* prefix, uint32_t ms);
void printTimeLog(const char* prefix, uint32_t (*timeGetter)(uint8_t), uint8_t maxIndex);

void getInt(uint8_t& choosen, uint8_t min, uint8_t max);
void getTime(uint32_t& time);

void setupEncoder();

extern LiquidCrystal gLcd;
extern EncButton gEncoder;
extern Button gModeSwitchBtn;
extern Button gStartBtn;
extern Button gExtraBtn;
extern Button gViewBtn;
extern Timer gTimer;
