#pragma once

#include "common/Communication/ComData.h"
#include "common/Communication/ComMaster.h"

class LightMeterMenu {
    enum class Menu : uint8_t {
        ShowLightInfo,
        SetBaseTime,
    };

public:
    LightMeterMenu();

    bool askStart();

    void process();
    bool needExit();
    void exit();

private:
    void handleShowLightInfo();
    void handleSetBaseTime();

    void repaint();
    void repaintShowLightInfo();
    void repaintSetBaseTime();

    void syncLightmeter();
    void error();

    bool m_bwOnly = true;
    bool m_needRepaint = false;
    Communication::LightMeasure m_lightMeasure;
    uint32_t m_lastAskTime = 0;

    Menu m_menu = Menu::ShowLightInfo;

    bool m_settingBaseLogD = true;
    Communication::BaseTime m_baseTime = { 0, 0_ts };

    uint32_t m_lastUserCommunicationTime = 0;
};
