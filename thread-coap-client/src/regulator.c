#include "regulator.h"
#include "client_utils.h"

#include <zephyr/kernel.h>
#include "heater.h"
#include <math.h>

static struct k_timer regulate_timer;

static struct k_work regulate_work;

static void regulate_work_cb(struct k_work *work)
{
    /* Update temperature if correction needed */
    /* Maximum single correction is bounded by REGULATOR_RESOLUTION */
    /* e.g. 0.2 degrees */
    if (fabs(heater.correction) > REGULATOR_RESOLUTION)
    {
        heater.current_temp += heater.correction > 0 ? REGULATOR_RESOLUTION : REGULATOR_RESOLUTION * -1.0f;
    }
    else
    {
        heater.current_temp += heater.correction > 0 ? heater.correction : heater.correction * -1.0f;
    }
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

    /* Start regulator timer */

    /* Every 100ms update temperature by 0.20 if needed */
    k_timer_start(&regulate_timer, K_MSEC(1333), K_MSEC(1333));

    return 0;
}