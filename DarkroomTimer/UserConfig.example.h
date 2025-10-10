// This file could be used to change configuration of project

// To change the configuration create a copy of this file
// and name it 'UserConfig.h'

// Leave there only that values that you want to change

/* LCD connection type
 * Possiable values:
 *    LCD_VERSION_DIRECT -- LCD pins connected directly to Arduino
 *    LCD_VERSION_I2C -- LCD connected to Arduino with I2C adapter (SDA -> A4, SCL -> A5)
 *
 * */
#define LCD_VERSION LCD_VERSION_DIRECT

/* LCD pin values for LCD_VERSION_DIRECT connection*/
#define LCD_RS_PIN 12
#define LCD_EN_PIN 11
#define LCD_D4_PIN 9
#define LCD_D5_PIN 8
#define LCD_D6_PIN 7
#define LCD_D7_PIN 6

/* Button pin values */
#define MODE_BTN_PIN A0
#define VIEW_BTN_PIN A2
#define START_BTN_PIN A1
#define ENCODER_BTN_PIN A3

/* Buzzer pin
 *
 * WARNING: Unrecommended to change
 * */
#define BEEPER_PIN 10

/* LCD backlight pin */
#define LCD_BACKLIGHT_PIN 5

/* SSR controlling pin
 * Note: Before 0.5.2 version A4 value was used
 * */
#define RELAY_PIN 4

/* Encoder rotary pins
 *
 * WARNING: You shouldn't change them,
 * otherwise encoder could stop work properly.
 * You allowed only swap them, to change encoder direction
 */
#define ENCODER_DT_PIN 2
#define ENCODER_CLK_PIN 3

/* Time on which encoder starts to move faster.
 * Encreasing of this values make encoder move faster, but less controllable
 */
#define ENCODER_FAST_TIMEOUT 22
/* Time on which encoder starts to move even faster */
#define ENCODER_FAST_FAST_TIMEOUT 6

/* Value on which LCD backight changing.
 * Maximum value is 25
 * If you are not using acrylic cover on LCD display,
 * than it's recommended to decrease this value
 */
#define LCD_BACKLIGHT_STEP 25

/* Beeper min volume */
#define BEEPER_VOLUME_MIN 27
/* Values on which beeper volume changing */
#define BEEPER_VOLUME_STEP 2

/* Beeper silent volume.
 * If you hear beeper in turned off state, than set this value to 0
 * If you hear some cracking noises, when beeper is turned on, than set this value to 1
 * It's recommended set it to 1 for high values of BEEPER_VOLUME_MIN (>=8)
 * And to 0 for low values of BEEPER_VOLUME_MIN (<8)
 */
#define BEEPER_VOLUME_SILENT 1

/* Set timer modes and their order
 * Place modes that you don't need after 'last_' element,
 * also you can rearrange modes.
 * Notes:
 *   1. you have to specify all modes, even not used (located after 'last_' elem)
 *   2. you have to put comma and backslash after each elem (except last elem)
 *
 * Example:
 *   // this config disable local test modes and rel mask modes
 *   // also it change standard order of modes
 *   #define TIMER_MODES \
 *       testFStops,     \
 *       testLinear,     \
 *       splitLinear,    \
 *       splitFStops,    \
 *       mask,           \
 *       splitMask,      \
 *       print,          \
 *       last_,          \
 *       localFStops,    \
 *       localLinear,    \
 *       relMask,        \
 *       splitRelMask,
 */
// clang-format off
#define TIMER_MODES \
    testFStops,     \
    testLinear,     \
    print,          \
    mask,           \
    relMask,        \
    splitFStops,    \
    splitLinear,    \
    splitMask,      \
    splitRelMask,   \
    localFStops,    \
    localLinear,    \
    last_,
// clang-format on
