#include "ModeProcessor.h"

#include <stddef.h>

class MaskMode final : public ModeProcessor {
    enum class Step { setNum, setMasks, log, run, last_ };

public:
    MaskMode();
    void process() override;

private:
    Step m_step;
    size_t m_numberOfMasks;
    size_t m_currentMask;
};
