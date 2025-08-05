#pragma once

#include <Hardware.h>

#include "LiquidCrystalWrapMock.h"

extern LiquidCrystalWrapMock gLcdWrapMock;

extern decltype(gMillis()) gCurrentTime;
decltype(gMillis()) mockMillis();

extern int gBuzzerVal;
extern int gBacklightVal;
extern bool gRelayVal;

void mockAnalogWrite(uint8_t pin, int val);
void mockDigitalWrite(uint8_t pin, uint8_t val);

