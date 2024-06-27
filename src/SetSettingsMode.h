#include "ModeProcessor.h"

#include <stdint.h>

class SetSettingsMode final : public ModeProcessor {
public:
    enum class Step { setLagTime, setBacklight, setBeepVolume, setAutoFinishView, last_ };

    SetSettingsMode();
    ~SetSettingsMode();

    void process() override;
    void reset() override {}

private:
    void processSetLagTime();
    void processSetBacklight();
    void processSetBeepVolume();
    void processSetAutoFinishView();

    Step m_step;
    uint16_t m_lagTime;
    uint32_t m_demoStartBeep;
};
