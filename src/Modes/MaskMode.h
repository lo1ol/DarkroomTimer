#include "../ModeProcessor.h"

#include <stdint.h>

#include "../Time.h"

class MaskMode final : public ModeProcessor {
    enum class Step { setNum, setMasks, run, last_ };

public:
    MaskMode();
    void process() override;
    void reset() override;
    void switchMode() override;

    const char* preview() const override { return "Mask printing"; }

private:
    void processRun();
    void processSetMasks();

    void printTimes() const;

    static constexpr uint8_t kMasksMaxNumber = 10;

    Step m_step;
    uint8_t m_numberOfMasks;
    uint8_t m_currentMask;

    Time m_masks[kMasksMaxNumber];
    uint16_t m_notifyMask;
};
