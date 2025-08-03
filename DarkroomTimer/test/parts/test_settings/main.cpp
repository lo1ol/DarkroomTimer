#include <Arduino.h>
#include <unity.h>

#include <EEPROM.h>

#include <Settings.h>

void setUp() {}

void tearDown() {}

void checkSettingsGeneric() {
    auto settings = kDefaultSettings;
    settings.lagTime = 1_s;
    settings.beepVolume = 7;
    settings.backlight = 2;
    settings.autoFinishViewMinutes = 7;
    settings.startWithSettings = 1;
    settings.melody = Melody::Name::alarm;
    settings.updateEEPROM();
    TEST_ASSERT(settings == Settings::load());

    EEPROM.put(0, 124);
    TEST_ASSERT(kDefaultSettings == Settings::load());

    settings.lagTime = kMaxLagTime + 1_ts;
    settings.updateEEPROM();
    TEST_ASSERT(kDefaultSettings == Settings::load());

    settings.lagTime = kMinLagTime - 1_ts;
    settings.updateEEPROM();
    TEST_ASSERT(kDefaultSettings == Settings::load());

    settings.beepVolume = kMinBeepVolume - 1;
    settings.updateEEPROM();
    TEST_ASSERT(kDefaultSettings == Settings::load());

    settings.beepVolume = kMaxBeepVolume + 1;
    settings.updateEEPROM();
    TEST_ASSERT(kDefaultSettings == Settings::load());

    settings.backlight = kMinBacklight - 1;
    settings.updateEEPROM();
    TEST_ASSERT(kDefaultSettings == Settings::load());

    settings.backlight = kMaxBacklight + 1;
    settings.updateEEPROM();
    TEST_ASSERT(kDefaultSettings == Settings::load());

    settings.autoFinishViewMinutes = kMaxAutoFinishViewMinutes + 1;
    settings.updateEEPROM();
    TEST_ASSERT(kDefaultSettings == Settings::load());

    settings.melody = static_cast<Melody::Name>(-1);
    settings.updateEEPROM();
    TEST_ASSERT(kDefaultSettings == Settings::load());

    settings.melody = Melody::Name::last_;
    settings.updateEEPROM();
    TEST_ASSERT(kDefaultSettings == Settings::load());
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(checkSettingsGeneric);
    UNITY_END();
}
