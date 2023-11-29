#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include <zephyr/logging/log.h>
#include "coap_common.h"

int client_init(void);

void decrease_current_illuminance(void);

void increase_current_illuminance(void);

void update_illuminance(void);

void update_dimmer_regulation(void);

void toggle_minimal_sleepy_end_device(void);
