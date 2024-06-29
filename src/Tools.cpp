#include "Tools.h"

#include <string.h>

EncButton gEncoder(ENCODER_DT, ENCODER_CLK);
Button gModeSwitchBtn(MODE_SWITCH_BTN);
Button gStartBtn(START_BTN);
Button gExtraBtn(EXTRA_BTN);
Button gViewBtn(VIEW_BTN);
Timer gTimer(BEEPER, RELAY);
Settings gSettings;
Display gDisplay(LiquidCrystal(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7));

void isr() {
    gEncoder.tickISR();
}

void setupEncoder() {
    attachInterrupt(0, isr, CHANGE);
    attachInterrupt(1, isr, CHANGE);
    gEncoder.setEncISR(true);
}

int8_t getEncoderShift() {
    if (!gEncoder.turn())
        return 0;
    return gEncoder.dir() * (gEncoder.fast() ? 5 : 1);
}

bool getInt(uint8_t& choosen, uint8_t min, uint8_t max) {
    int8_t shift = getEncoderShift();

    if (shift == 0)
        return false;

    if (shift < 0 && (choosen - min) < -shift) {
        choosen = min;
        return false;
    } else if (shift > 0 && (max - choosen) < shift) {
        choosen = max;
        return false;
    } else {
        choosen += shift;
        return true;
    }
}

void getTime(uint32_t& time) {
    int32_t shift = getEncoderShift();

    uint16_t factor;
    if ((time + shift) < 10 * 1000L)
        factor = 500;
    else if ((time + shift) < 100 * 1000L)
        factor = 1000;
    else if ((time + shift) < 1000 * 1000L)
        factor = 5000;
    else
        factor = 50000;

    shift *= factor;

    if (shift < 0 && time < static_cast<uint32_t>(-shift)) {
        time = 0;
    } else {
        time += shift;
        time -= time % factor;
    }

    // use only whole number of 100 milliseconds
    time = (time / 100) * 100;

    if (time > 1000 * 10000L)
        time = 1000 * 10000L;
}
