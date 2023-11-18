#include "thread.h"
#include "client_utils.h"
#include <dk_buttons_and_leds.h>

void on_thread_state_changed(otChangedFlags flags,
                             struct openthread_context *ot_context,
                             void *user_data)
{
	if (flags & OT_CHANGED_THREAD_ROLE) 
    {
		switch (otThreadGetDeviceRole(ot_context->instance)) 
        {
            case OT_DEVICE_ROLE_CHILD:
            case OT_DEVICE_ROLE_ROUTER:
            case OT_DEVICE_ROLE_LEADER:
            is_connected = true;
            dk_set_led_on(THREAD_CONNECTION_LED);
            break;

            case OT_DEVICE_ROLE_DISABLED:
            case OT_DEVICE_ROLE_DETACHED:
            default:
            is_connected = false;
            dk_set_led_off(THREAD_CONNECTION_LED);
            break;
		}
	}
}