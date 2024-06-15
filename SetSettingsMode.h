#include "ModeProcessor.h"

#include <stdint.h>

class SetSettingsMode final : public ModeProcessor {
public:
    enum class Step { setLagTime, setBeepVolume, last_ };

    SetSettingsMode();
    ~SetSettingsMode();

    void process() override;
    void reset() override {}

private:
    void processSetLagTime();
    void processSetBeepVolume();

    Step m_step;
    uint16_t m_lagTime;
};
