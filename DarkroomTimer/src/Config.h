#pragma once

#include <assert.h>

#include <Arduino.h>

#define LCD_RS 12
#define LCD_EN 11
#define LCD_D4 9
#define LCD_D5 8
#define LCD_D6 7
#define LCD_D7 6

#define MODE_BTN A0
#define VIEW_BTN A2
#define START_BTN A1
#define ENCODER_BTN A3

// You shouldn't change this pins
// Otherwise encoder could stop to work.
// You allowed only swap them, to change encoder direction
#define ENCODER_DT 2
#define ENCODER_CLK 3

#define BEEPER 10
#define BACKLIGHT 5
#define RELAY A4

#define DISPLAY_ROWS 2
#define DISPLAY_COLS 16

#define MIN_BEEP_VOLUME 27
#define BEEP_VOLUME_STEP 2
#define BACKLIGHT_STEP 25

// On some buzzers it's better to set 1 instead of 0 for silence
// Setting volume to 0 can produce some sound noize and cracks
#define BEEP_VOLUME_SILENT (MIN_BEEP_VOLUME < 8 ? 0 : 1)
#define MAX_BEEP_VOLUME MIN_BEEP_VOLUME + (BEEP_VOLUME_STEP * 9)

static_assert(MAX_BEEP_VOLUME <= 255);
static_assert(BACKLIGHT_STEP <= 25);

// Time on which encoder starts to move faster
#define ENCODER_FAST_TIMEOUT 11

#define TIMER_FIRMWARE_VERSION "0.5.0"

#if ENCODER_DT == 2 && ENCODER_CLK == 3
    #define ENCODER_DIRECTION (1)
#elif ENCODER_DT == 3 && ENCODER_CLK == 2
    #define ENCODER_DIRECTION (-1)
#else
static_assert(false && "Bad values for ENCODER_DT and ENCODER_CLK")
#endif
