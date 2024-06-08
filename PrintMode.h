#include "ModeProcessor.h"

#include <stddef.h>

class PrintMode final : public ModeProcessor {
public:
    PrintMode();
    void process() override;
    void reset() override;

private:
    unsigned long m_printTime;
};
