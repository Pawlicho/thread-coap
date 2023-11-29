#pragma once

#include <stdint.h>

typedef struct DIMMER_CONTEXT
{
    float current_illuminance;
    float output_power;
}DIMMER_CONTEXT_T;

extern volatile DIMMER_CONTEXT_T dimmer;
