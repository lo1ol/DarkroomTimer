#pragma once

#include "Config.h"

#ifdef PIO_UNIT_TESTING
    #include "LcdImpl/Test.h"
#else
    #if LCD_VERSION == LCD_VERSION_DIRECT
        #include "LcdImpl/Direct.h"
    #elif LCD_VERSION == LCD_VERSION_I2C
        #include "LcdImpl/I2C.h"
    #else
        #error "Unknown LCD Version. Supported versions: LCD_VERSION_DIRECT and LCD_VERSION_I2C"
    #endif
#endif
