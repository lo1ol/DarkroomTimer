#include "../ModeProcessor.h"

#include <stdint.h>

#include "../Time.h"

class PrintMode final : public ModeProcessor {
public:
    PrintMode();
    void process() override;
    void reset() override;
    void printLog() const override;
    void switchMode() override;

private:
    void appendPrintLog(const Time&);
    void resetPrintInfo();

    bool m_triggerByHold;
    Time m_printTime;
    uint8_t m_logSize = 0;
    Time m_printLog[13];
};
