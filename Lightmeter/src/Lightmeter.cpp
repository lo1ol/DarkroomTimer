#include "Lightmeter.h"

#include <Arduino.h>

#include <math.h>

namespace Comm = Communication;

namespace {
float fastLog10(float x) {
    int exponent;
    float value = frexpf(fabsf(x), &exponent);
    float y = 1.23149591368684f;
    y *= value;
    y += -4.11852516267426f;
    y *= value;
    y += 6.02197014179219f;
    y *= value;
    y += -3.13396450166353f;
    y += exponent;
    return y * 0.3010299956639812f;
}
} // namespace

Comm::LightDesc Lightmeter::getSingleColorMeasure() {
    uint16_t chK415 = random(10944 / 100);
    uint16_t chK445 = random(19857 / 100);
    uint16_t chK480 = random(24706 / 100);
    uint16_t chK515 = random(31850 / 100) / 20;
    uint16_t chK555 = random(38206 / 100) / 20;
    uint16_t chK590 = random(42728 / 100) / 20;
    uint16_t chK630 = random(49609 / 100) * 30;
    uint16_t chK680 = random(65535 / 100) * 30;

    // how much light we got
    float red = max(0, -0.0812698108293189f * chK590 + 0.3889645588671796f * chK630 + 0.6435851049073148f * chK680);
    float green = max(0, 1.3625505596392686f * chK415 + 0.7230896797200832f * chK445 + 0.7186750494198619f * chK480 +
                             1.1292803233662263f * chK515 + 1.7586387114631004f * chK555 +
                             0.012904432878220122f * chK590 + -0.013767720765582843f * chK630);
    float blue = max(0, 3.4400423464479277f * chK415 + 1.9093843426914159f * chK445 + 2.997503714014938f * chK480 +
                            0.4701350013652125f * chK515 + -0.11413907320069276f * chK555);

    // density of each channel
    red = fastLog10(0xffffff / red) * 100;
    green = fastLog10(0xffffff / green) * 100;
    blue = fastLog10(0xffffff / blue) * 100;

    // dencity of all channel shown relative to red
    Comm::LightDesc lightDesc;
    lightDesc.logD = round(red);
    lightDesc.logBlue = round(blue - red);
    lightDesc.logGreen = round(green - red);

    return lightDesc;
}

void Lightmeter::switchMode(Mode mode) {
    m_mode = mode;
}

Comm::LightDesc Lightmeter::getSmoothLightDesc() {
    // check data avaliable
    if (true) {
        Comm::LightDesc currentMeasure;
        switch (m_mode) {
        case Mode::BW:
            // TODO
            break;
        case Mode::Color:
            currentMeasure = getSingleColorMeasure();
            break;
        }

        m_measureSum += currentMeasure - m_measures[m_measureNum];
        m_measures[m_measureNum] = currentMeasure;
        m_measureNum = (m_measureNum + 1) % kMeasureAverageCnt;
    }

    return m_measureSum / kMeasureAverageCnt;
}
