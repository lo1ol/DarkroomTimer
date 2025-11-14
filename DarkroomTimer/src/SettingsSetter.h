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
        setStartWith,
        setMelody,
        checkVersion,
        last_
    };

    SettingsSetter();
    ~SettingsSetter();

    void process();
    void repaint() const;

    [[nodiscard]] bool couldBeClosed() const;

private:
    void processSetLagTime();
    void processSetBacklight() const;
    void processSetBeepVolume() const;
    void processSetAutoFinishView() const;
    void processStartWith() const;
    void processSetMelody() const;

    Step m_step = Step::setLagTime;
    Time m_lagTime;
    Timer m_timer{};
};
