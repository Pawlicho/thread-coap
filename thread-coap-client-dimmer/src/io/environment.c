#include "environment.h"

#include "dimmer.h"
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_DECLARE(COAP_CLIENT);

static struct k_timer environment_timer;

static void environment_timer_cb(struct k_timer *timer)
{
    /* Approximation with linear function */
    /* Goal is to achive difference 1000 lux in 2s, supposing 100% output power */
    /* So every 100ms, temperature needs to increase by 50 lux */
    /* delta_T = a * output_power + b */
    /* b = 0 => Room does not have any external light source */
    /* a = 0,5 */
    /* delta_T_per_1s(output_power) = 0,5 * output_power */

    float a = 0.5f;
    float b = 0;
    dimmer.current_illuminance += a * dimmer.output_power + b;
}

void environment_init()
{
    k_timer_init(&environment_timer, environment_timer_cb, NULL);

    k_timer_start(&environment_timer, K_MSEC(100), K_MSEC(100));
}