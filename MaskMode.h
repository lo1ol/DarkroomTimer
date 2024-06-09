#include "ModeProcessor.h"

#include <stdint.h>

class MaskMode final : public ModeProcessor {
    enum class Step { setNum, setMasks, log, run, last_ };

public:
    MaskMode();
    void process() override;
    void reset() override;

private:
    Step m_step;
    uint8_t m_numberOfMasks;
    uint8_t m_currentMask;
};
