#include "ModeProcessor.h"

#include "Timer.h"

#include <stdint.h>

class SettingsSetter final {
public:
    enum class Step {
        setLagTime,
        setBacklight,
        setBeepVolume,
        setAutoFinishView,
        setStartWithSettings,
        setViewInTests,
        setViewInMasks,
        last_
    };

    SettingsSetter();
    ~SettingsSetter();

    void process();

    bool couldBeClosed() const;

private:
    void processSetLagTime();
    void processSetBacklight();
    void processSetBeepVolume();
    void processSetAutoFinishView();
    void processStartWithSettings();
    void processSetViewInTests();
    void processSetViewInMasks();

    Step m_step = Step::setLagTime;
    Time m_lagTime;
    uint32_t m_demoStartBeep;
    Timer m_timer;
};
