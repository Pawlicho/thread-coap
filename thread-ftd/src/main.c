#include <zephyr/kernel.h>
#include <dk_buttons_and_leds.h>
#include <openthread/thread.h>
#include <zephyr/net/openthread.h>
#include <zephyr/logging/log.h>
#include <stdbool.h>

bool is_connected;

#define THREAD_CONNECTION_LED   (DK_LED2)

LOG_MODULE_REGISTER(FTD, CONFIG_LOG_DEFAULT_LEVEL);

static void on_thread_state_changed(otChangedFlags flags,
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

static struct openthread_state_changed_cb ot_state_chaged_cb = { .state_changed_cb =
									 on_thread_state_changed };

int main(void)
{
    int ret;

    if ( (ret = dk_leds_init()) != 0 )
    {
        LOG_ERR("Cannot init leds (error: %d)", ret);
		return ret;
    }

    if ( (ret = openthread_state_changed_cb_register(openthread_get_default_context(), &ot_state_chaged_cb)) != 0)
    {
        LOG_ERR("Unable to set openthread_state_changed_cb_register: %d", ret);
        return ret;
    }

    return 0;
}
