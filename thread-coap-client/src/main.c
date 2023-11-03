#include <zephyr/kernel.h>
#include <dk_buttons_and_leds.h>
#include <zephyr/logging/log.h>

#include "coap_client.h"

LOG_MODULE_REGISTER(main, LOG_LEVEL_DBG);

static void on_button_changed(uint32_t button_state, uint32_t has_changed)
{
    uint32_t buttons = button_state & has_changed;

	if (buttons & DK_BTN1_MSK) 
    {
        testConnection();
	}
}

int main(void)
{
    int ret = dk_buttons_init(on_button_changed);

	if (ret) 
    {
		LOG_ERR("Cannot init buttons (error: %d)", ret);
		return 0;
	}

    coap_client_init();

    return 0;
}
