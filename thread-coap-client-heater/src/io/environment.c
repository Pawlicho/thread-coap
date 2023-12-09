#include "environment.h"

#include "heater.h"
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_DECLARE(COAP_CLIENT);

static struct k_timer environment_timer;

static void environment_timer_cb(struct k_timer *timer)
{
    /* Approximation with linear function */
    /* Goal is to achive difference of 1 celcius degrees celsius in 5s, supposing 50% output power */
    /* So every second, temperature needs to increase by 0.2 degrees celsius*/
    /* delta_T = a * output_power + b */

    /* Let's firstly suppose there is no temperature loss and environment is isolated
       From any other heat source */

    float a = 0.00025f;
    float b = a * (-5.0f);
    heater.current_temp += a * heater.output_power + b;
}

void environment_init()
{
    k_timer_init(&environment_timer, environment_timer_cb, NULL);

    k_timer_start(&environment_timer, K_MSEC(100), K_MSEC(100));
}