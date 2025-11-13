#define LCD_VERSION LCD_VERSION_I2C

#define LCD_RS_PIN 12
#define LCD_EN_PIN 11
#define LCD_D4_PIN 9
#define LCD_D5_PIN 8
#define LCD_D6_PIN 7
#define LCD_D7_PIN 6

#define MODE_BTN_PIN A0
#define VIEW_BTN_PIN A2
#define START_BTN_PIN A1
#define ENCODER_BTN_PIN A3

#define BEEPER_PIN 10

#define LCD_BACKLIGHT_PIN 5

#define RELAY_PIN 5

#define ENCODER_CLK_PIN 3
#define ENCODER_DT_PIN 2

#define ENCODER_FAST_TIMEOUT 10
#define ENCODER_FAST_FAST_TIMEOUT 5

#define LCD_BACKLIGHT_STEP 10

#define BEEPER_VOLUME_MIN 20
#define BEEPER_VOLUME_STEP 1

#define BEEPER_VOLUME_SILENT 0

#define STARTUP_LOGO_VERSION LOGO_DISABLED

// clang-format off
#define TIMER_MODES       \
    fStopTest,            \
    linearTest,           \
    print,                \
    mask,                 \
    splitGradeMask,       \
    expertFStopTest,      \
    expertLinearTest,     \
    last_,                \
    localizedFStopTest,   \
    localizedLinearTest,  \
    splitGradeFStopTest,  \
    splitGradeLinearTest, \
    relMask,              \
    splitGradeRelMask
// clang-format on
