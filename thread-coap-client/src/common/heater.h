#pragma once

#include <stdint.h>

typedef struct HEATER_CONTEXT
{
    float current_temp;
    float correction;
}HEATER_CONTEXT_T;

extern volatile HEATER_CONTEXT_T heater;
