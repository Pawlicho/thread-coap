#include <zephyr/kernel.h>
#include <dk_buttons_and_leds.h>


int main(void)
{
    dk_leds_init();
    dk_set_led_on(DK_LED1);
    return 0;
}
