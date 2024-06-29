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

void getTime(Time& time) {
    Time shift{ getEncoderShift() };

    int16_t factor;
    if ((time + shift) < 100_ts)
        factor = 5;
    else if ((time + shift) < 1000_ts)
        factor = 10;
    else if ((time + shift) < 1000_ts)
        factor = 50;
    else
        factor = 500;

    time += shift * factor;
    time = Time((static_cast<int16_t>(time) / factor) * factor);

    if (time < 0_ts)
        time = 0_ts;

    if (time > 18000_ts)
        time = 18000_ts;
}
