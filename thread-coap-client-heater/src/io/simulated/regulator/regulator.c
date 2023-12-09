#include "regulator.h"
#include "client_utils.h"

#include <zephyr/kernel.h>
#include "heater.h"
#include <math.h>
#include <zephyr/logging/log.h>

LOG_MODULE_DECLARE(COAP_CLIENT);

static struct k_timer regulate_timer;

static void regulate_timer_cb(struct k_timer *timer)
{
    update_heater_regulation();
}

void regulator_init()
{
    /* Initialize regulator timer */
    k_timer_init(&regulate_timer, regulate_timer_cb, NULL);

    /* Start regulator timer */
    k_timer_start(&regulate_timer, K_MSEC(1000), K_MSEC(1000));
}