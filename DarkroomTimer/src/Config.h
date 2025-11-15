#pragma once

// WARNING: Don't change values here
// Create ../UserConfig.h file to redefine values from here

#include <assert.h>

#include <Arduino.h>

#define LCD_VERSION_DIRECT 0
#define LCD_VERSION_I2C 1

#ifdef PIO_UNIT_TESTING
    #if __has_include(<UserConfig.h>)
        #include <UserConfig.h>
    #endif
#else
    // Use user config values if exists
    #if __has_include("../UserConfig.h")
        #include "../UserConfig.h"
    #endif
#endif

#ifndef LCD_VERSION
    #define LCD_VERSION LCD_VERSION_DIRECT
#endif

static_assert(LCD_VERSION | 1, "Bad LCD_VERSION");

#ifndef LCD_RS_PIN
    #define LCD_RS_PIN 12
#endif
#ifndef LCD_EN_PIN
    #define LCD_EN_PIN 11
#endif
#ifndef LCD_D4_PIN
    #define LCD_D4_PIN 9
#endif
#ifndef LCD_D5_PIN
    #define LCD_D5_PIN 8
#endif
#ifndef LCD_D6_PIN
    #define LCD_D6_PIN 7
#endif
#ifndef LCD_D7_PIN
    #define LCD_D7_PIN 6
#endif

#ifndef MODE_BTN_PIN
    #define MODE_BTN_PIN A0
#endif
#ifndef VIEW_BTN_PIN
    #define VIEW_BTN_PIN A2
#endif
#ifndef START_BTN_PIN
    #define START_BTN_PIN A1
#endif
#ifndef ENCODER_BTN_PIN
    #define ENCODER_BTN_PIN A3
#endif

#ifndef BEEPER_PIN
    #define BEEPER_PIN 10
#endif
#ifndef LCD_BACKLIGHT_PIN
    #define LCD_BACKLIGHT_PIN 5
#endif
#ifndef RELAY_PIN
    #define RELAY_PIN 4
#endif

#ifndef ENCODER_DT_PIN
    #define ENCODER_DT_PIN 2
#endif
#ifndef ENCODER_CLK_PIN
    #define ENCODER_CLK_PIN 3
#endif

#if ENCODER_DT_PIN == 2 && ENCODER_CLK_PIN == 3
    #define ENCODER_DIRECTION (1)
#elif ENCODER_DT_PIN == 3 && ENCODER_CLK_PIN == 2
    #define ENCODER_DIRECTION (-1)
#else
static_assert(false && "Bad values for ENCODER_DT_PIN and ENCODER_CLK_PIN");
#endif

#ifndef ENCODER_FAST_TIMEOUT
    #define ENCODER_FAST_TIMEOUT 22
#endif
#ifndef ENCODER_FAST_FAST_TIMEOUT
    #define ENCODER_FAST_FAST_TIMEOUT 6
#endif

#ifndef LCD_BACKLIGHT_STEP
    #define LCD_BACKLIGHT_STEP 25
#endif

static_assert(LCD_BACKLIGHT_STEP <= 25 && 0 < LCD_BACKLIGHT_STEP);

#ifndef BEEPER_VOLUME_MIN
    #define BEEPER_VOLUME_MIN 27
#endif
#ifndef BEEPER_VOLUME_STEP
    #define BEEPER_VOLUME_STEP 2
#endif

// On some buzzers it's better to set 1 instead of 0 for silence
// Setting volume to 0 can produce some sound noize and cracks
#ifndef BEEPER_VOLUME_SILENT
    #define BEEPER_VOLUME_SILENT (BEEPER_VOLUME_MIN < 8 ? 0 : 1)
#endif

static_assert((BEEPER_VOLUME_MIN + (BEEPER_VOLUME_STEP * 9)) <= 255);

#define DISPLAY_ROWS 2
#define DISPLAY_COLS 16

#define LOGO_DISABLED 0
#define LOGO_MATRIX 1
#define LOGO_SIMPLE 2

#ifndef STARTUP_LOGO_VERSION
    #define STARTUP_LOGO_VERSION LOGO_MATRIX
#endif

static_assert(STARTUP_LOGO_VERSION | 1, "Bad STARTUP_LOGO_VERSION");

#define TIMER_FIRMWARE_VERSION "0.6.0"

#ifndef TIMER_MODES
// clang-format off
    #define TIMER_MODES       \
        fStopTest,            \
        linearTest,           \
        print,                \
        mask,                 \
        relMask,              \
        splitGradeFStopTest,  \
        splitGradeLinearTest, \
        splitGradeMask,       \
        splitGradeRelMask,    \
        localizedFStopTest,   \
        localizedLinearTest,  \
        last_,                \
        expertFStopTest,      \
        expertLinearTest,
// clang-format on
#endif
