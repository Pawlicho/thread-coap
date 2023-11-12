#include "client_utils.h"
#include "coap_client.h"
#include "common.h"
#include "mtd.h"
#include "thread.h"
#include "dimmer.h"

#include <zephyr/net/coap.h>
#include <zephyr/kernel.h>

#define BUTTON_ILLUMINANCE_UPDATE_STEP  (50)

LOG_MODULE_DECLARE(COAP_CLIENT);

/* Global var indicating Thread state */
bool is_connected;

volatile DIMMER_CONTEXT_T dimmer = {.current_illuminance = 300, .correction = 0};

static struct openthread_state_changed_cb ot_state_chaged_cb = 
{
    .state_changed_cb = on_thread_state_changed
};

static void decrease_current_illuminance_work_cb(struct k_work *item)
{
    dimmer.current_illuminance -= BUTTON_ILLUMINANCE_UPDATE_STEP;
    LOG_INF("Decreased environment illuminance by %.2f. Current illuminance: %.2f.",
            BUTTON_ILLUMINANCE_UPDATE_STEP,
            dimmer.current_illuminance);

}

static void increase_current_illuminance_work_cb(struct k_work *item)
{
    dimmer.current_illuminance += BUTTON_ILLUMINANCE_UPDATE_STEP;
    LOG_INF("Increased environment illuminance by %.2f. Current illuminance: %.2f.",
            BUTTON_ILLUMINANCE_UPDATE_STEP,
            dimmer.current_illuminance);


}

/* Tasks declaration */
static struct k_work update_dimmer_work;
static struct k_work toggle_MTD_SED_work;
static struct k_work decrease_current_illuminance_work;
static struct k_work increase_current_illuminance_work;

static void submit_work_if_connected(struct k_work *work)
{
	if (is_connected) 
    {
		k_work_submit(work);
	} 
    else 
    {
		LOG_WRN("Thread connection is broken");
	}
}

int client_init()
{
    int err;

    serv_addr_init();
    coap_init(AF_INET6, NULL);

    k_work_init(&update_dimmer_work, update_dimmer_work_cb);
    k_work_init(&toggle_MTD_SED_work, toggle_minimal_sleepy_end_device_work_cb);
    k_work_init(&decrease_current_illuminance_work, decrease_current_illuminance_work_cb);
    k_work_init(&increase_current_illuminance_work, increase_current_illuminance_work_cb);

	if ( (err = openthread_state_changed_cb_register(openthread_get_default_context(), &ot_state_chaged_cb)) != 0)
    {
        LOG_ERR("Unable to set openthread_state_changed_cb_register: %d", err);
        return err;
    }
    else
    if ( (err = openthread_start(openthread_get_default_context())) != 0)
    {
        LOG_ERR("Unable to start Openthread: %d", err);
        return err;
    }

    return 0;
}

/* UI functions */

void updateDimmer(void)
{
    submit_work_if_connected(&update_dimmer_work);
}

void toggle_minimal_sleepy_end_device(void)
{
	k_work_submit(&toggle_MTD_SED_work);
}

void decrease_current_illuminance(void)
{
    k_work_submit(&decrease_current_illuminance_work);
}

void increase_current_illuminance(void)
{
    k_work_submit(&increase_current_illuminance_work);
}