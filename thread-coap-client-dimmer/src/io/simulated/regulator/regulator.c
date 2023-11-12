#include "regulator.h"
#include "client_utils.h"

#include <zephyr/kernel.h>
#include "dimmer.h"
#include <math.h>
#include <zephyr/logging/log.h>

LOG_MODULE_DECLARE(COAP_CLIENT);

static struct k_timer regulate_timer;

static struct k_work regulate_work;

static const float regulator_resolution = 10.0f;

static void regulate_work_cb(struct k_work *work)
{
    /* Update temperature if correction needed */
    /* Maximum single correction is bounded by regulator_resolution */
    /* e.g. 0.2 degrees */

    /* If temperature can be corrected in single tick */

    if (fabs(dimmer.correction) >= regulator_resolution)
    {
        /* If temperature must be increased */
        if (dimmer.correction > 0)
        {
            /* Increase temperature */
            dimmer.current_illuminance += regulator_resolution;
            /* Update needed correction */
            dimmer.correction -= regulator_resolution;
        }
        /* If temperature must be decreased */
        else
        {
            /* Decrease temperature */
            dimmer.current_illuminance -= regulator_resolution;
            /* Update needed correction */
            dimmer.correction += regulator_resolution;
        }
    }
    LOG_INF("Current Illuminance: %.2f\n", dimmer.current_illuminance);
}

static void regulate_timer_cb(struct k_timer *timer)
{
    k_work_submit(&regulate_work);
}

void regulator_init()
{
    /* Initialize regulator timer */
    k_timer_init(&regulate_timer, regulate_timer_cb, NULL);

    /* Initialize regulator work */
    k_work_init(&regulate_work, regulate_work_cb);

    /* Start regulator timer */
    /* Every 100ms update temperature by 0.20 if needed */
    k_timer_start(&regulate_timer, K_MSEC(3000), K_MSEC(3000));
}