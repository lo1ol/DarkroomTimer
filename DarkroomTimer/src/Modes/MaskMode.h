#include "../ModeProcessor.h"

#include <stdint.h>

#include "../Time.h"

class MaskMode final : public ModeProcessor {
    enum class Step { setNum, setMasks, run, last_ };

public:
    MaskMode(uint8_t filterNum);
    void process() override;
    void reset() override;
    void switchMode() override;

    void repaint() const override;

private:
    void processRun();
    void processSetMasks();

    void setCurrentMask(uint8_t filter, uint8_t mask);
    void moveCurrentMask(int8_t dir);

    Step m_step;
    uint8_t m_filterNum;
    uint8_t m_numberOfMasks[2];
    uint8_t m_currentFilter;
    uint32_t m_notifyMask;
    uint8_t m_currentMask;
};
