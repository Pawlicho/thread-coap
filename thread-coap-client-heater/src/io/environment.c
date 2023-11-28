#include "environment.h"

#include "heater.h"
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_DECLARE(COAP_CLIENT);

static struct k_timer environment_timer;

static void environment_timer_cb(struct k_timer *timer)
{
    /* Approximation with linear function */
    /* Goal is to achive difference of 10 celcius degrees celsius in 10s, supposing 100% output power */
    /* So every second, temperature needs to increase by 1 degrees celsius*/
    /* delta_T = a * output_power + b */
    /* b = -0.1 => if no output_power, temperatore slowly decreases */
    /* a = 0,011 */
    /* delta_T_per_1s(output_power) = 0,011 * output_power - 0.1 */

    float a = 0.011f;
    float b = -0.1f;
    heater.current_temp += a * heater.output_power + b;
}

void environment_init()
{
    k_timer_init(&environment_timer, environment_timer_cb, NULL);

    k_timer_start(&environment_timer, K_MSEC(1000), K_MSEC(1000));
}