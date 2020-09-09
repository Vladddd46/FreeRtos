#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include <driver/adc.h>
#include "driver/i2c.h"
#include "driver/gpio.h"
#include <string.h>
#include "sh1106.h"
#include "wrappers.h"

#define OLED_ENABLE 32

static int photoresistor_value   = 4095 / 17;
static int screen_contrast_value = 255;



/*
 * Checks value on photoresistors and
 * stores it in photoresistor_value variable.
 */
void photoresistor_checker() {
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC1_CHANNEL_0,ADC_ATTEN_DB_0);

    while(1) {
        photoresistor_value = adc1_get_raw(ADC1_CHANNEL_0);
        vTaskDelay(1);
    }
}



/*
 * Changes contrast on oled display
 * depending on photoresistor_value variable.
 */
void display_contrast_changer() {
    gpio_set(OLED_ENABLE, GPIO_MODE_OUTPUT, 1);
    
    sh1106_t display;
    init_i2c_driver();
    sh1106_init(&display);
    sh1106_clear(&display);

    sh1106_t *display1 = &display;
    print_str_in_line(&display1, "     Hello World!", 3);
    sh1106_update(&display);

    uint8_t res;
    while(1) {
        res = photoresistor_value / 17;

        // smooth stabilizator.
        if (screen_contrast_value < res - 5 && screen_contrast_value < res + 5 )
            screen_contrast_value += 5;
        else if (screen_contrast_value > res - 5 && screen_contrast_value > res + 5)
            screen_contrast_value -= 5;

        sh1106_contrast(&display, (uint8_t)screen_contrast_value);
        // printf("%d %d %d\n", (int)res, photoresistor_value, screen_contrast_value); // DEBUG

        vTaskDelay(5);
    }
}



void app_main() {
    xTaskCreate(photoresistor_checker, "photoresistor_checker", 4040, NULL, 10, NULL);
    xTaskCreate(display_contrast_changer, "display_contrast_changer", 4040, NULL, 10, NULL);
}


