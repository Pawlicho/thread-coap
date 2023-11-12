#include "regulator.h"
#include "client_utils.h"

#include <zephyr/kernel.h>
#include "dimmer.h"
#include <math.h>
#include <zephyr/logging/log.h>

LOG_MODULE_DECLARE(COAP_CLIENT);

static struct k_timer regulate_timer;

static struct k_work regulate_work;

static const float regulator_resolution = 0.2f;

static void regulate_work_cb(struct k_work *work)
{
    // LOG_INF("Current Illuminance: %.2f\n", dimmer.current_illuminance);
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