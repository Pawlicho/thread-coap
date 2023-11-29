#include "environment.h"

#include "dimmer.h"
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_DECLARE(COAP_CLIENT);

static struct k_timer environment_timer;

static int tick_n;

static void environment_timer_cb(struct k_timer *timer)
{
    int period = 1200;
    tick_n += 1;
    tick_n %= period;
    /* Environment illuminance is a sum of illuminance of dimmer and every other source, 
        e.g. daylight from every other window, door, additional light sources of lamp type
    */

   /*
      External source shifts over time and is described like this:
      - it changes in parabolic way
      - maximum illuminance is 500
      - minimum illuminance is 0
      - it reaches maximum illuminance from minimum illuminance in 600 ticks
      - function is periodic with period 1200 ticks
      - quadratic equation: external_illuminance(tick mod 1200) = f(x) = ( (-5/3600)*x^2 + (5/3)*x ) mod 600*x
   */
    
    float external_illuminance = (-5.0f / 3600.0f) * tick_n * tick_n + (5.0f / 3.0f) * tick_n;
    dimmer.current_illuminance = dimmer.output_power / 100.f * dimmer.max_illuminance + external_illuminance;
}

void environment_init()
{
    k_timer_init(&environment_timer, environment_timer_cb, NULL);

    k_timer_start(&environment_timer, K_MSEC(100), K_MSEC(100));
}