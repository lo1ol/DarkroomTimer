#pragma once

#include <stdint.h>

#define ADD_TO_ENUM(enumName, current, num) \
    (enumName)(((uint8_t)(current) + (uint8_t)enumName::last_ + num) % (uint8_t)enumName::last_)

void alignStr(char* buf, uint8_t align, char ch = ' ');
