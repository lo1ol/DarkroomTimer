#include "../ModeProcessor.h"

#include <stdint.h>

#include "../Time.h"

class LinearTestMode final : public ModeProcessor {
    enum class Step { baseTime, initTime, stepTime, run, last_ };
    enum class RunView { common, log, last_ };

public:
    LinearTestMode(bool splitGrade);
    void process() override;
    void reset() override;
    void switchMode() override;

    bool canSwitchView() const override;
    void switchView() override;

    void printLog() const override;

    const char* preview() const override;

private:
    void printLog(bool& logOverFlowed) const;
    Time getPrintTime() const;

    bool kSplit;
    Step m_step;
    RunView m_view;
    uint8_t m_currentRun;
    Time m_baseTime;
    Time m_initTime;
    Time m_stepTime;
};
