#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include <zephyr/logging/log.h>
#include "coap_common.h"

typedef void (*mtd_mode_toggle_cb_t)(uint32_t val);

void coap_client_init(mtd_mode_toggle_cb_t on_toggle);

void testConnection(void);

void updateCurrentTemperature(void);

void updateOccupationState(void);

void toggle_minimal_sleepy_end_device(void);

enum OCCUPATION_STATE
{
    OCCUPATION_STATE_ROOM_NOT_OCCUPIED = 0,
    OCCUPATION_STATE_ROOM_OCCUPIED = 1,
};
