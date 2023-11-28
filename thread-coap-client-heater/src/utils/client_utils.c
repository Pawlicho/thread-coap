#include "client_utils.h"
#include "coap_client.h"
#include "common.h"
#include "mtd.h"
#include "thread.h"
#include "heater.h"
#include "nat64_utils.h"

#include <zephyr/net/coap.h>
#include <zephyr/kernel.h>

#define BUTTON_TEMPERATURE_UPDATE_STEP  (5.0f)

LOG_MODULE_DECLARE(COAP_CLIENT);

/* Global var indicating Thread state */
bool is_connected;

volatile HEATER_CONTEXT_T heater = {.current_temp = 19.5f, .output_power = 10};

static struct openthread_state_changed_cb ot_state_chaged_cb = 
{
    .state_changed_cb = on_thread_state_changed
};

static void decrease_current_temp_work_cb(struct k_work *item)
{
    heater.current_temp -= BUTTON_TEMPERATURE_UPDATE_STEP;
    LOG_INF("Decreased environment temperature by %.2f. Current temperature: %.2f.",
            BUTTON_TEMPERATURE_UPDATE_STEP,
            heater.current_temp);

}

static void increase_current_temp_work_cb(struct k_work *item)
{
    heater.current_temp += BUTTON_TEMPERATURE_UPDATE_STEP;
    LOG_INF("Increased environment temperature by %.2f. Current temperature: %.2f.",
            BUTTON_TEMPERATURE_UPDATE_STEP,
            heater.current_temp);
}

/* Tasks declaration */
static struct k_work update_temperature_work;
static struct k_work update_heater_regulation_work;
static struct k_work toggle_MTD_SED_work;
static struct k_work decrease_current_temp_work;
static struct k_work increase_current_temp_work;

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
    uint8_t ipv6_address[INET6_ADDRSTRLEN + 1] = {0};

    k_work_init(&update_temperature_work, update_temperature_work_cb);
    k_work_init(&update_heater_regulation_work, update_heater_regulation_work_cb);
    k_work_init(&toggle_MTD_SED_work, toggle_minimal_sleepy_end_device_work_cb);
    k_work_init(&decrease_current_temp_work, decrease_current_temp_work_cb);
    k_work_init(&increase_current_temp_work, increase_current_temp_work_cb);

	if ( (err = openthread_state_changed_cb_register(openthread_get_default_context(), &ot_state_chaged_cb)) != 0)
    {
        LOG_ERR("Unable to set openthread_state_changed_cb_register: %d", err);
        return err;
    }

    /* Waiting for NAT64 Prefix acquiring */
    /* This while is kind of "blocking", but without the prefix, nothing runs correctly */
    while (!(synthesize_ipv4_to_ipv6(ipv6_address)));

    serv_addr_init(ipv6_address);
    coap_init(AF_INET6, NULL);

    return 0;
}

/* UI functions */

void update_tempreture(void)
{
    submit_work_if_connected(&update_temperature_work);
}

void update_heater_regulation(void)
{
    submit_work_if_connected(&update_heater_regulation_work);
}

void toggle_minimal_sleepy_end_device(void)
{
	k_work_submit(&toggle_MTD_SED_work);
}

void decrease_current_temp(void)
{
    k_work_submit(&decrease_current_temp_work);
}

void increase_current_temp(void)
{
    k_work_submit(&increase_current_temp_work);
}

