#include "ModeProcessor.h"

#include <stdint.h>

class SetLagTimeMode final : public ModeProcessor {
public:
    SetLagTimeMode();
    ~SetLagTimeMode();

    static uint16_t getLagTime();

    void process() override;
    void reset() override {}

private:
    uint16_t m_lagTime;
};
