#include "driver/periph_ctrl.h"
#include <math.h>
#include "audio_example_file.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include <unistd.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include <string.h>
#include <stdio.h>
#include "driver/ledc.h"
#include "freertos/semphr.h"
#include "driver/uart.h"
#include "esp_system.h"
#include "driver/i2s.h"
#include "driver/dac.h"


#define UART_NUMBER             UART_NUM_1
#define UART_TX_PIN             17
#define UART_RX_PIN             16
#define EN_AMP                  23
#define PI                      (3.14159265)
#define SAMPLE_RATE             (44100)
#define WAVE_FREQ_HZ            (1000)
#define SAMPLE_PER_CYCLE        (SAMPLE_RATE/WAVE_FREQ_HZ)

void make_noise(int bits) {
    unsigned int i, sample_val;
    size_t i2s_bytes_write = 0;

    i2s_write(0, audio_table, ((bits+8)/16)*SAMPLE_PER_CYCLE*4, &i2s_bytes_write, 100);

}


void app_main(void) {
    esp_err_t err;
    gpio_set_direction(EN_AMP, GPIO_MODE_OUTPUT);
    gpio_set_level(EN_AMP, 1);
    dac_output_enable(DAC_CHANNEL_1);

    static const int i2s_num = 0;
    static const i2s_config_t i2s_config = {
        .mode = I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_DAC_BUILT_IN,
        .sample_rate = 44100,
        .bits_per_sample = 16,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
        .intr_alloc_flags = 0,
        .dma_buf_count = 8,
        .dma_buf_len = 64,
        .use_apll = false
    };

    i2s_driver_install(i2s_num, &i2s_config, 0, NULL); 
    i2s_set_pin(i2s_num, NULL);

    
    int test_bits = 16;
    while (true) {
        make_noise(test_bits);
        vTaskDelay(1000/portTICK_RATE_MS);
        test_bits += 8;
        if(test_bits > 32)
            test_bits = 16;
    }
}