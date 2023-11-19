#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <openthread/border_router.h>
#include <openthread/netdata.h>
#include "common.h"
#include <zephyr/net/socket.h>
#include <openthread/thread.h>
#include <zephyr/net/openthread.h>

bool synthesize_ipv4_to_ipv6(char* ipv6_buff);