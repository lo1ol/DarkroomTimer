#pragma once

#include <Tools.h>

#include "LiquidCrystalWrapMock.h"

extern LiquidCrystalWrapMock gLcdWrapMock;
extern decltype(gMillis()) gCurrentTime;
extern int gBuzzerVal;
extern bool gRelayVal;

void initVirtEnv();
