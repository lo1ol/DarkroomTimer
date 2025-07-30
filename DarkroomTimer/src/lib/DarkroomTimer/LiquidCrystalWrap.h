#pragma once

#include <stdint.h>

class LiquidCrystal;

// Needed to create nock class in tests
#ifdef PIO_UNIT_TESTING
    #define DT_VIRTUAL virtual
#else
    #define DT_VIRTUAL
#endif

class LiquidCrystalWrap {
public:
    LiquidCrystalWrap(LiquidCrystal* lcd = nullptr);
    DT_VIRTUAL ~LiquidCrystalWrap() = default;

    DT_VIRTUAL void setCursor(uint8_t, uint8_t);
    DT_VIRTUAL void print(const char*);

private:
    LiquidCrystal* m_lcd;
};

#undef DT_VIRTUAL
