#include "header.h"

#define LED1 27
#define LED2 26
#define LED3 33


void led1_pulsing() {
    // timer configuration.
    ledc_timer_config_t ledc_timer;
    ledc_timer.speed_mode      = LEDC_HIGH_SPEED_MODE;
    ledc_timer.freq_hz         = 100;
    ledc_timer.duty_resolution = LEDC_TIMER_8_BIT; // 256
    ledc_timer.timer_num       = LEDC_TIMER_1;
    if(ledc_timer_config(&ledc_timer) != ESP_OK) 
        ESP_LOGI("ledc_timer_config ", "%s", "some error occured");

    // chanel configuration.
    ledc_channel_config_t ledc_channel;
    ledc_channel.gpio_num   = LED1;
    ledc_channel.speed_mode = LEDC_HIGH_SPEED_MODE;
    ledc_channel.channel    = LEDC_CHANNEL_1;
    ledc_channel.intr_type  = LEDC_INTR_FADE_END;
    ledc_channel.timer_sel  = LEDC_TIMER_1;
    ledc_channel.duty       = 0;

    if (ledc_channel_config(&ledc_channel) != ESP_OK) 
        ESP_LOGI("ledc_channel_config ", "%s", "some error occured");
    if (ledc_fade_func_install(0) != ESP_OK) 
        ESP_LOGI("ledc_fade_func_install ", "%s", "some error occured");

     while(1) {
        if (led1_is_pulsing == 0)
            vTaskDelete(NULL);

        // ascending.
        ledc_set_fade_with_time(ledc_channel.speed_mode, ledc_channel.channel, 255, 1000);
        ledc_fade_start(ledc_channel.speed_mode, ledc_channel.channel, LEDC_FADE_WAIT_DONE);
        // descending.
        ledc_set_fade_with_time(ledc_channel.speed_mode, ledc_channel.channel, 0, 1000);
        ledc_fade_start(ledc_channel.speed_mode, ledc_channel.channel, LEDC_FADE_WAIT_DONE);
        vTaskDelay(1);
    }
}

void led2_pulsing() {
    // timer configuration.
    ledc_timer_config_t ledc_timer;
    ledc_timer.speed_mode      = LEDC_HIGH_SPEED_MODE;
    ledc_timer.freq_hz         = 100;
    ledc_timer.duty_resolution = LEDC_TIMER_8_BIT; // 256
    ledc_timer.timer_num       = LEDC_TIMER_1;
    if(ledc_timer_config(&ledc_timer) != ESP_OK) 
        ESP_LOGI("ledc_timer_config ", "%s", "some error occured");

    // chanel configuration.
    ledc_channel_config_t ledc_channel;
    ledc_channel.gpio_num   = LED2;
    ledc_channel.speed_mode = LEDC_HIGH_SPEED_MODE;
    ledc_channel.channel    = LEDC_CHANNEL_1;
    ledc_channel.intr_type  = LEDC_INTR_FADE_END;
    ledc_channel.timer_sel  = LEDC_TIMER_1;
    ledc_channel.duty       = 0;

    if (ledc_channel_config(&ledc_channel) != ESP_OK) 
        ESP_LOGI("ledc_channel_config ", "%s", "some error occured");
    if (ledc_fade_func_install(0) != ESP_OK) 
        ESP_LOGI("ledc_fade_func_install ", "%s", "some error occured");

     while(1) {
        if (led2_is_pulsing == 0)
            vTaskDelete(NULL);

        // ascending.
        ledc_set_fade_with_time(ledc_channel.speed_mode, ledc_channel.channel, 255, 1000);
        ledc_fade_start(ledc_channel.speed_mode, ledc_channel.channel, LEDC_FADE_WAIT_DONE);
        // descending.
        ledc_set_fade_with_time(ledc_channel.speed_mode, ledc_channel.channel, 0, 1000);
        ledc_fade_start(ledc_channel.speed_mode, ledc_channel.channel, LEDC_FADE_WAIT_DONE);
        vTaskDelay(1);
    }
}

void led3_pulsing() {
    // timer configuration.
    ledc_timer_config_t ledc_timer;
    ledc_timer.speed_mode      = LEDC_HIGH_SPEED_MODE;
    ledc_timer.freq_hz         = 100;
    ledc_timer.duty_resolution = LEDC_TIMER_8_BIT; // 256
    ledc_timer.timer_num       = LEDC_TIMER_1;
    if(ledc_timer_config(&ledc_timer) != ESP_OK) 
        ESP_LOGI("ledc_timer_config ", "%s", "some error occured");

    // chanel configuration.
    ledc_channel_config_t ledc_channel;
    ledc_channel.gpio_num   = LED3;
    ledc_channel.speed_mode = LEDC_HIGH_SPEED_MODE;
    ledc_channel.channel    = LEDC_CHANNEL_1;
    ledc_channel.intr_type  = LEDC_INTR_FADE_END;
    ledc_channel.timer_sel  = LEDC_TIMER_1;
    ledc_channel.duty       = 0;

    if (ledc_channel_config(&ledc_channel) != ESP_OK) 
        ESP_LOGI("ledc_channel_config ", "%s", "some error occured");
    if (ledc_fade_func_install(0) != ESP_OK) 
        ESP_LOGI("ledc_fade_func_install ", "%s", "some error occured");

    while(1) {
        if (led3_is_pulsing == 0)
            vTaskDelete(NULL);

        // ascending.
        ledc_set_fade_with_time(ledc_channel.speed_mode, ledc_channel.channel, 255, 1000);
        ledc_fade_start(ledc_channel.speed_mode, ledc_channel.channel, LEDC_FADE_WAIT_DONE);
        // descending.
        ledc_set_fade_with_time(ledc_channel.speed_mode, ledc_channel.channel, 0, 1000);
        ledc_fade_start(ledc_channel.speed_mode, ledc_channel.channel, LEDC_FADE_WAIT_DONE);
        vTaskDelay(1);
    }
}

