#include "VirtEnv.h"

LiquidCrystalWrapMock gLcdWrapMock;

decltype(gMillis()) gCurrentTime = 0;

decltype(gMillis()) myMillis() {
    return gCurrentTime;
}

void initVirtEnv() {
    gLcdWrapMock = LiquidCrystalWrapMock{};
    gDisplay = Display{ &gLcdWrapMock };
    gCurrentTime = 0;
    gMillis = &myMillis;
}
