#include <zephyr/kernel.h>
#include <dk_buttons_and_leds.h>
#include <zephyr/logging/log.h>

#include "buttons.h"
#include "thread.h"

LOG_MODULE_REGISTER(COAP_CLIENT, CONFIG_LOG_DEFAULT_LEVEL);

static struct openthread_state_changed_cb ot_state_chaged_cb = 
{
    .state_changed_cb = on_thread_state_changed
};

int main(void)
{
    int ret;

	if ( (ret = dk_buttons_init(on_button_changed)) != 0) 
    {
		LOG_ERR("Cannot init buttons (error: %d)", ret);
		return ret;
	}

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
