#include "ModeProcessor.h"

#include <stdint.h>

class PrintMode final : public ModeProcessor {
public:
    PrintMode();
    void process() override;
    void reset() override;

private:
    uint32_t m_printTime;
};
