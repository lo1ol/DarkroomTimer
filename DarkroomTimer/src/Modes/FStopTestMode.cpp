#include "FStopTestMode.h"

#include "../Hardware.h"
#include "../Utils.h"

namespace {
constexpr uint8_t kFStopPartVarinatns[] = { 1, 2, 3, 4, 6, 12 };
} // namespace

FStopTestMode::FStopTestMode(SubMode subMode) : TestMode(subMode) {
    m_initTime = 2_s;
    m_FStopPartId = 0;
}

bool FStopTestMode::handleStepSet() {
    return gEncoder.getInt(m_FStopPartId, 0, sizeof(kFStopPartVarinatns) - 1);
}

void FStopTestMode::printStep() const {
    gDisplay[1] << F("F stop: 1/") << kFStopPartVarinatns[m_FStopPartId];
}

Time FStopTestMode::getStepTime_(uint8_t step) const {
    float stopPart = kFStopPartVarinatns[m_FStopPartId];
    return m_initTime * (fast2pow(step / stopPart) - fast2pow((step - 1) / stopPart));
}

Time FStopTestMode::getStepTotalTime_(uint8_t step) const {
    float stopPart = kFStopPartVarinatns[m_FStopPartId];
    return m_initTime * fast2pow(step / stopPart);
}

const __FlashStringHelper* FStopTestMode::header() const {
    switch (kSubMode) {
    case Generic:
        return F("F Stop test");
    case SplitGrade:
        return F("Splt F Stop test");
    case Local:
        return F("Locl F Stop test");
    case Expert:
        return F("Xprt F Stop test");
    }

    assert(false);
    return F("");
}
