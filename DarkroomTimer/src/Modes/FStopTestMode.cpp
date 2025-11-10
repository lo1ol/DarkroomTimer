#include "FStopTestMode.h"

#include "../Hardware.h"

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
    gDisplay[1] << "F stop: 1/" << kFStopPartVarinatns[m_FStopPartId];
}

Time FStopTestMode::getStepTime_(uint8_t step) const {
    float stopPart = kFStopPartVarinatns[m_FStopPartId];
    return m_initTime * (pow(2, step / stopPart) - pow(2, (step - 1) / stopPart));
}

Time FStopTestMode::getStepTotalTime_(uint8_t step) const {
    float stopPart = kFStopPartVarinatns[m_FStopPartId];
    return m_initTime * pow(2, step / stopPart);
}

const char* FStopTestMode::header() const {
    if (kSubMode == SplitGrade)
        return "Splt F Stop test";
    if (kSubMode == Local)
        return "Locl F Stop test";
    return "F Stop test";
}
