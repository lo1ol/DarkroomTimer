#include "../ModeProcessor.h"

#include <stdint.h>

#include "../Time.h"

class PrintMode final : public ModeProcessor {
public:
    PrintMode();
    void process() override;
    void reset() override;
    void printLog() const override {}

private:
    Time m_printTime;
    bool m_triggerByHold;
};
