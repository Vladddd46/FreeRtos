#include <stdio.h>
#include "driver/gpio.h"
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/ledc.h"
#include "driver/timer.h"
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include "esp_log.h"
#include "driver/dac.h"

#define LED1_GPIO 27
#define LED2_GPIO 26
#define LED3_GPIO 33

void gpio_set_direction_wrapper(int gpio, int mode);
void gpio_set_level_wrapper(int gpio, int level);
void gpio_set(int gpio, int mode, int level);
void dac_output_enable_wrapper(int dac_channel);
void led_mode(int gpio_led, int set);
void all_led_set(int mode);
void led_set_by_id(int led_id, int mode);
