#include "../ModeProcessor.h"

#include <stdint.h>

#include "../Time.h"

class RelMaskMode final : public ModeProcessor {
    enum class Step { setNum, setMasks, run, last_ };

public:
    RelMaskMode(uint8_t filterNum);
    void process() override;
    void reset() override;
    void switchMode() override;

    void repaint() const override;

private:
    void processRun();
    void processSetMasks();

    void setCurrentMask(uint8_t filter, uint8_t mask);
    void moveCurrentMask(int8_t dir);

    void printTimeExplicit() const;

    Time getStepTime();

    Step m_step;
    uint8_t m_numberOfMasks[2];
    uint8_t m_currentMask;
    uint8_t m_currentFilter;
    uint8_t m_filterNum;

    bool m_viewSecs;
};
