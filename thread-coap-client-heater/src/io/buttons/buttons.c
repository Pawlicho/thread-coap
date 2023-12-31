#include "buttons.h"
#include "sensor.h"
#include "regulator.h"
#include "environment.h"

void on_button_changed(uint32_t button_state, uint32_t has_changed)
{
    uint32_t buttons = button_state & has_changed;

	if (buttons & DK_BTN1_MSK) 
    { 
        increase_current_temp();   
	}
    else if (buttons & DK_BTN2_MSK)
    {
        decrease_current_temp();
    }
    else if (buttons & DK_BTN3_MSK)
    {
        /* Initialize everything needed for application */
        client_init();
        environment_init();
        sensor_init();
        regulator_init();
    }
    else if (buttons & DK_BTN4_MSK)
    {
        toggle_minimal_sleepy_end_device();
    }
}