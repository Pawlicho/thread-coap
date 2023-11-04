#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include <zephyr/logging/log.h>
#include "coap_common.h"

void coap_client_init();

void testConnection();
