#include "../ModeProcessor.h"

#include <stdint.h>

#include "../Time.h"

class PrintMode final : public ModeProcessor {
public:
    PrintMode();
    void process() override;
    void reset() override;
    void switchMode() override;

    void printLog() const;

    const char* preview() const override { return "Printing"; }

    void repaint() const override;

private:
    void appendPrintLog(const Time&);
    void resetPrintInfo();

    bool m_triggerByHold;
    Time m_printTime;
    uint8_t m_logSize = 0;
};
