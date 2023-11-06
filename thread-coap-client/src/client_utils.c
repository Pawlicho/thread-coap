#include "client_utils.h"
#include "coap_client.h"
#include "common.h"
#include "mtd.h"
#include "thread.h"

#include <zephyr/kernel.h>

LOG_MODULE_DECLARE(COAP_CLIENT);

/* Global var indicating Thread state */
bool is_connected;

volatile float curr_temp = -11.23f;
volatile float correction;

static struct openthread_state_changed_cb ot_state_chaged_cb = 
{
    .state_changed_cb = on_thread_state_changed
};

/* Tasks declaration */
static struct k_work hello_world_work;
static struct k_work update_curr_temp_work;
static struct k_work toggle_MTD_SED_work;

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

    k_work_init(&hello_world_work, hello_world_work_cb);
    k_work_init(&update_curr_temp_work, update_curr_temp_work_cb);
    k_work_init(&toggle_MTD_SED_work, toggle_minimal_sleepy_end_device_work_cb);

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

void testConnection(void)
{
    submit_work_if_connected(&hello_world_work);
}

void updateCurrentTemperature(void)
{
    submit_work_if_connected(&update_curr_temp_work);
}

void toggle_minimal_sleepy_end_device(void)
{
	k_work_submit(&toggle_MTD_SED_work);
}