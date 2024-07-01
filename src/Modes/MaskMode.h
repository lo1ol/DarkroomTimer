#include "../ModeProcessor.h"

#include <stdint.h>

#include "../Time.h"

class MaskMode final : public ModeProcessor {
    enum class Step { setNum, setMasks, run, last_ };

public:
    MaskMode();
    void process() override;
    void reset() override;
    void printLog() const override;

private:
    static constexpr uint8_t gMasksMaxNumber = 10;

    Step m_step;
    uint8_t m_numberOfMasks;
    uint8_t m_currentMask;

    Time m_masks[gMasksMaxNumber];
};
