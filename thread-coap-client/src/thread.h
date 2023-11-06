#pragma once 

#include <openthread/thread.h>
#include <zephyr/net/openthread.h>
#include "common.h"

void on_thread_state_changed(otChangedFlags flags,
                             struct openthread_context *ot_context,
                             void *user_data);