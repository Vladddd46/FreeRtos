#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include <string.h>
#include "driver/uart.h"
#include "driver/gpio.h"
#include "wrappers.h"
#include "general.h"
#include "driver/dac.h"
#include "driver/ledc.h"


void led_mode(int gpio_led, int set);
void all_led_set(int mode);
void led_set_by_id(int led_id, int mode);
