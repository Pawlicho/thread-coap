#include <zephyr/kernel.h>
#include <dk_buttons_and_leds.h>
#include <zephyr/logging/log.h>

#include "buttons.h"

LOG_MODULE_REGISTER(COAP_CLIENT, LOG_LEVEL_DBG);

int main(void)
{
    int ret;

	if ( (ret = dk_buttons_init(on_button_changed)) != 0) 
    {
		LOG_ERR("Cannot init buttons (error: %d)", ret);
		return ret;
	}

    if ( (ret = client_init()) != 0)
    {
        LOG_ERR("Cannot initialize client (error: %d)", ret);
        return ret;
    }

    return 0;
}
