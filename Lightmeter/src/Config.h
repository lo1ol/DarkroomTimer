#pragma once

// WARNING: Don't change values here
// Create ../UserConfig.h file to redefine values from here

// Use user config values if exists
#if __has_include("../UserConfig.h")
    #include "../UserConfig.h"
#endif

// TODO
#define COMMUNICATION_SLAVE_CLK_PIN 2
#define COMMUNICATION_SLAVE_DATA_PIN 3

// clang-format on
