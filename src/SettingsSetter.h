#include "ModeProcessor.h"

#include <stdint.h>

class SettingsSetter final {
public:
    enum class Step { setLagTime, setBacklight, setBeepVolume, setAutoFinishView, last_ };

    SettingsSetter();
    ~SettingsSetter();

    void process();

private:
    void processSetLagTime();
    void processSetBacklight();
    void processSetBeepVolume();
    void processSetAutoFinishView();

    Step m_step;
    uint16_t m_lagTime;
    uint32_t m_demoStartBeep;
};
