#include "common/Communication/ComData.h"
#include "common/Communication/ComSlave.h"

#include "common/SafeEncButton.h"

#include <gio/gio.h>

namespace Comm = Communication;

void setup() {
    Serial.begin(9600);
}

void loop() {
    static Comm::ComSlave gCon(3, 2);

    static ButtonT<A3> gBtn;
    static bool gStarted = false;
    static Comm::ColorDesc gRelColor = {};
    static bool gAbs = true;

    Comm::ColorDesc colorDesc = {};
    colorDesc.lm = (millis() % 100000) / 100;
    colorDesc.y = (millis() % 100000) / 100 - 500;
    colorDesc.m = (millis() % 100000) / 100 - 999;
    colorDesc.c = (millis() % 100000) / 100 + 500;

    gBtn.tick();

    if (gBtn.click()) {
        if (!gStarted) {
            gCon.sendData(Comm::LmCmd::askStart);
        } else {
            gRelColor = colorDesc;
            gAbs = false;
        }
    }

    if (gBtn.hold())
        gAbs = true;

    Comm::MainCmd inCmd;

    if (gCon.readData(inCmd)) {
        gStarted = true;
        switch (inCmd) {
        case Comm::MainCmd::end:
            gStarted = false;
            gCon.reset();
            break;
        case Comm::MainCmd::sendColorInfo:
            if (!gAbs)
                colorDesc -= gRelColor;
            gCon.sendData(colorDesc);
            break;
        }
    }
}
