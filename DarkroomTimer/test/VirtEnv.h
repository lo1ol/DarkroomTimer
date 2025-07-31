#pragma once

#include <Tools.h>

#include "LiquidCrystalWrapMock.h"

extern LiquidCrystalWrapMock gLcdWrapMock;
extern decltype(gMillis()) gCurrentTime;

void initVirtEnv();
