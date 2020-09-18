#include "header.h"

#define LED1 27
#define LED2 26
#define LED3 33

void led_mode(int gpio_led, int set) {
    gpio_set(gpio_led, GPIO_MODE_OUTPUT, set);
}

void all_led_set(int mode) {
    led_mode(LED1, mode);
    led_mode(LED2, mode);
    led_mode(LED3, mode);
}

void led_set_by_id(int led_id, int mode) {
    if (led_id == 1)
        led_mode(LED1, mode);
    if (led_id == 2)
        led_mode(LED2, mode);
    if (led_id == 3)
        led_mode(LED3, mode);
}
