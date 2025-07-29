#include "../ModeProcessor.h"

#include <stdint.h>

#include "../Time.h"
#include "../TimeTable.h"

class PrintMode final : public ModeProcessor {
public:
    PrintMode();
    void process() override;
    void reset() override;
    void switchMode() override;

    void repaint() override;

private:
    void appendPrintLog(const Time&);
    void resetPrintInfo();

    bool m_triggerByHold;
    Time m_printTime;
    uint8_t m_logSize = 0;
    bool m_showLog = false;

    TimeTable m_timeTable;
};
