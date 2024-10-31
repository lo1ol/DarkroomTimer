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

    void repaint() const override;

private:
    void processRun();
    void processSetMasks();

    void setCurrentMask(uint8_t);

    Step m_step;
    uint8_t m_numberOfMasks;
    uint32_t m_currentMask;

    uint16_t m_notifyMask;
};
