#include "ModeProcessor.h"

#include "Timer.h"

#include <stdint.h>

class SettingsSetter final {
public:
    enum class Step {
        setLagTime,
        setBacklight,
        setBeepVolume,
        setConfirmAlarm,
        setAutoFinishView,
        setStartWithSettings,
        setViewInTests,
        setViewInMasks,
        setMelody,
        last_
    };

    SettingsSetter();
    ~SettingsSetter();

    void process();

    bool couldBeClosed() const;

private:
    void processSetLagTime();
    void processSetBacklight() const;
    void processSetBeepVolume() const;
    void processSetAutoFinishView() const;
    void processConfirmAlarm() const;
    void processStartWithSettings() const;
    void processSetViewInTests() const;
    void processSetViewInMasks() const;
    void processSetMelody() const;

    Step m_step = Step::setLagTime;
    Time m_lagTime;
    Timer m_timer;
};
