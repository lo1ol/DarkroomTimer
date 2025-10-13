#pragma once

#include "common/Communication/ComData.h"
#include "common/Communication/ComMaster.h"

class LightMeterMenu {
    enum class State : int {
        NotStarted,
    };

public:
    bool askStart();

    void process();
    bool needExit();
    void exit();

private:
    bool m_bwOnly = true;
    bool m_needRepaint = false;
    Communication::ColorDesc m_lightDesc;
    Communication::ComMaster m_con;
    uint32_t m_lastSlaveResponseTime = 0;
};
