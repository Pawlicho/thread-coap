#include <zephyr/kernel.h>
#include <dk_buttons_and_leds.h>
#include <zephyr/logging/log.h>
#include <ram_pwrdn.h>
#include <zephyr/device.h>
#include <zephyr/pm/device.h>

#include "client_utils.h"

LOG_MODULE_REGISTER(COAP_CLIENT, LOG_LEVEL_DBG);

static void mtd_mode_toggle_cb(uint32_t med)
{
	const struct device *cons = DEVICE_DT_GET(DT_CHOSEN(zephyr_console));

	if (!device_is_ready(cons)) 
    {
		return;
	}

	if (med) 
    {
		pm_device_action_run(cons, PM_DEVICE_ACTION_RESUME);
	} 
    else 
    {
		pm_device_action_run(cons, PM_DEVICE_ACTION_SUSPEND);
	}
}

static void on_button_changed(uint32_t button_state, uint32_t has_changed)
{
    uint32_t buttons = button_state & has_changed;

	if (buttons & DK_BTN1_MSK) 
    {
        /* This button will be responsible for increasing curr_temp */     
	}
    else if (buttons & DK_BTN2_MSK)
    {
        /* This button will be responsible for increasing curr_temp */
        updateCurrentTemperature();
    }
    else if (buttons & DK_BTN3_MSK)
    {
        testConnection();
    }
    else if (buttons * DK_BTN4_MSK)
    {
        toggle_minimal_sleepy_end_device();
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

    coap_client_init(mtd_mode_toggle_cb);

    return 0;
}
