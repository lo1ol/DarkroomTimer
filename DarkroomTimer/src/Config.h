#pragma once

#include <assert.h>

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

#define TIMER_FIRMWARE_VERSION "0.4.3"
