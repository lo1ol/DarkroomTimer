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
        setMelody,
        checkVersion,
        last_
    };

    SettingsSetter();
    ~SettingsSetter();

    void process();

    [[nodiscard]] bool couldBeClosed() const;

private:
    void processSetLagTime();
    void processSetBacklight() const;
    void processSetBeepVolume() const;
    void processSetAutoFinishView() const;
    void processStartWithSettings() const;
    void processSetMelody() const;
    void processCheckVersion() const;

    Step m_step = Step::setLagTime;
    Time m_lagTime;
    Timer m_timer{};
};
