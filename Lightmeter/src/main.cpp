#include "common/Communication/ComData.h"
#include "common/Communication/ComSlave.h"

#include "common/SafeEncButton.h"

#include <gio/gio.h>

#include <math.h>

#include "Hardware.h"
#include "Lightmeter.h"

namespace Comm = Communication;

namespace {
Time calcSuggestedTime(const Comm::BaseTime& baseTime, uint16_t logD) {
    Time res = baseTime.baseTime * pow(2, static_cast<int16_t>(logD - baseTime.baseLogD) / 30.10299957f);
    res.round_();
    return res;
}
} // namespace

void setup() {
    Serial.begin(9600);
}

void loop() {
    static Comm::ComSlave gCon(3, 2);
    static Lightmeter gLightmeter{};

    static bool gStarted = false;
    static Comm::LightDesc gRelColor = {};
    static bool gAbs = true;
    static Comm::BaseTime gBaseTime = gSettings.bwBaseTime;

    gBtn.tick();

    Comm::LightDesc curLightDesc;
    if (gStarted)
        curLightDesc = gLightmeter.getSmoothLightDesc();

    if (gStarted && gBtn.click() && gLightmeter.getMode() == Lightmeter::Mode::BW) {
        gRelColor = curLightDesc;
        gAbs = false;
    }

    if (gBtn.hold()) {
        if (!gStarted && gCon.lastCommunicationTimeMs() < 100) {
            gCon.reset();
            gCon.sendData(Comm::LmCmd::askStart);
        }

        gAbs = true;
    }

    Comm::MainCmd inCmd;

    if (gCon.readData(inCmd)) {
        gStarted = true;
        switch (inCmd) {
        case Comm::MainCmd::end:
            gStarted = false;
            gAbs = true;
            gCon.reset();
            break;
        case Comm::MainCmd::switchBW:
            gLightmeter.switchMode(Lightmeter::Mode::BW);
            gBaseTime = gSettings.bwBaseTime;
            gAbs = true;
            break;
        case Comm::MainCmd::switchColor:
            gLightmeter.switchMode(Lightmeter::Mode::Color);
            gBaseTime = gSettings.colorBaseTime;
            gAbs = true;
            break;
        case Comm::MainCmd::sendBaseTime:
            gCon.sendData(gBaseTime);
            break;
        case Comm::MainCmd::setBaseTime:
            gCon.readData(gBaseTime);
            switch (gLightmeter.getMode()) {
            case Lightmeter::Mode::BW:
                gSettings.bwBaseTime = gBaseTime;
                break;
            case Lightmeter::Mode::Color:
                gSettings.colorBaseTime = gBaseTime;
                break;
            }

            gSettings.updateEEPROM();
            break;
        case Comm::MainCmd::sendLightMeasure: {
            Comm::LightMeasure measure{
                .lightDesc = curLightDesc,
                .suggestedTime = calcSuggestedTime(gBaseTime, curLightDesc.logD),
                .abs = gAbs,
            };

            if (!gAbs)
                curLightDesc -= gRelColor;

            gCon.sendData(measure);
        } break;
        }
    }
}
