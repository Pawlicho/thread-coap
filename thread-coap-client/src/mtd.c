#include "mtd.h"

#include <openthread/thread.h>
#include <zephyr/net/openthread.h>

LOG_MODULE_DECLARE(COAP_CLIENT);

static void mtd_mode_toggle(uint32_t med)
{
	const struct device *cons = DEVICE_DT_GET(DT_CHOSEN(zephyr_console));

	if (!device_is_ready(cons)) 
    {
		return;
	}

	if (med) 
    {
		pm_device_action_run(cons, PM_DEVICE_ACTION_RESUME);
	} 
    else 
    {
		pm_device_action_run(cons, PM_DEVICE_ACTION_SUSPEND);
	}
}

void toggle_minimal_sleepy_end_device_work_cb(struct k_work *item)
{
	otError error;
	otLinkModeConfig mode;
	struct openthread_context *context = openthread_get_default_context();

	__ASSERT_NO_MSG(context != NULL);

	openthread_api_mutex_lock(context);
	mode = otThreadGetLinkMode(context->instance);
	mode.mRxOnWhenIdle = !mode.mRxOnWhenIdle;
	error = otThreadSetLinkMode(context->instance, mode);
	openthread_api_mutex_unlock(context);

	if (error != OT_ERROR_NONE) 
    {
		LOG_ERR("Failed to set MLE link mode configuration");
	} 
    else 
    {
		mtd_mode_toggle(mode.mRxOnWhenIdle);
	}
}