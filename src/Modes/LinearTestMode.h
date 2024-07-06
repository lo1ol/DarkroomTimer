#include "../ModeProcessor.h"

#include <stdint.h>

#include "../Time.h"

class LinearTestMode final : public ModeProcessor {
    enum class Step { initTime, stepTime, run, last_ };
    enum class RunView { common, log, last_ };

public:
    LinearTestMode();
    void process() override;
    void reset() override;
    void switchMode() override;

    bool canSwitchView() const override;
    void switchView() override;

    void printLog() const override;

private:
    void printLog(bool& logOverFlowed) const;
    Time getPrintTime() const;

    Step m_step;
    RunView m_view;
    uint8_t m_currentRun;
    Time m_initTime;
    Time m_stepTime;
};
