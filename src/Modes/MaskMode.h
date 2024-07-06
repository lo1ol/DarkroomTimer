#include "../ModeProcessor.h"

#include <stdint.h>

#include "../Time.h"

class MaskMode final : public ModeProcessor {
    enum class Step { setNum, setMasks, run, last_ };
    enum class View { common, log, last_ };

public:
    MaskMode();
    void process() override;
    void reset() override;
    void switchMode() override;

    bool canSwitchView() const override;
    void switchView() override;

    void printLog() const override;

private:
    void processRun();
    void processSetMasks();

    void printLog(bool& logOverFlowed) const;

    static constexpr uint8_t kMasksMaxNumber = 10;

    Step m_step;
    View m_view;
    uint8_t m_numberOfMasks;
    uint8_t m_currentMask;

    Time m_masks[kMasksMaxNumber];
};
