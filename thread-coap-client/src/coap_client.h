#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include <zephyr/logging/log.h>
#include "coap_common.h"

void coap_client_init();

void testConnection();

void updateCurrentTemperature();

void updateOccupationState();

enum OCCUPATION_STATE
{
    OCCUPATION_STATE_ROOM_NOT_OCCUPIED = 0,
    OCCUPATION_STATE_ROOM_OCCUPIED = 1,
};
