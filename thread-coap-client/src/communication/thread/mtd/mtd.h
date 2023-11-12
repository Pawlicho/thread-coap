#pragma once

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <ram_pwrdn.h>
#include <zephyr/device.h>
#include <zephyr/pm/device.h>

void toggle_minimal_sleepy_end_device_work_cb(struct k_work *item);
