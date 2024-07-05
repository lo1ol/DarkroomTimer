#include "../ModeProcessor.h"

#include <stdint.h>

#include "../Time.h"

class FStopTestMode final : public ModeProcessor {
    enum class Step { initTime, fstopSet, run, last_ };
    enum class RunView { common, log, last_ };

public:
    FStopTestMode();
    void process() override;
    void reset() override;
    void switchMode() override;

    bool canSwitchView() const override;
    void switchView() override;

    void printLog(bool& logOverFlowed) const override;

private:
    void processRun();
    Time getPrintTime() const;

    Step m_step;
    RunView m_view;
    uint8_t m_currentRun;
    uint8_t m_FStopPartId;
    Time m_initTime;
};
