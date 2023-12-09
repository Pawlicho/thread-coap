#include "sensor.h"
#include "client_utils.h"

#include <zephyr/kernel.h>

static struct k_timer measure_timer;

static void measure_timer_cb(struct k_timer *dummy)
{
    update_tempreture();
}

void sensor_init()
{
    /* Initialize Timers */
    k_timer_init(&measure_timer,  measure_timer_cb,  NULL);

    /* Start timers */

    /* Every one second send measurement to the Server */
    k_timer_start(&measure_timer, K_MSEC(1000), K_MSEC(1000));
}