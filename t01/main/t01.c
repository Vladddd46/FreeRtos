#include "header.h"


#define LF_ASCII_CODE 0xA
#define CR_ASCII_CODE 0xD // \r the same

#define COMMAND_LINE_MAX_LENGTH 1024
#define UART_PORT UART_NUM_1


#define GPIO_LED1 27
#define GPIO_LED2 26
#define GPIO_LED3 33

xQueueHandle global_queue_handle;

int led1_is_pulsing = 0;
int led2_is_pulsing = 0;
int led3_is_pulsing = 0;

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
    ledc_channel.gpio_num   = GPIO_LED1;
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
            break;

        // ascending.
        ledc_set_fade_with_time(ledc_channel.speed_mode, ledc_channel.channel, 255, 1000);
        ledc_fade_start(ledc_channel.speed_mode, ledc_channel.channel, LEDC_FADE_WAIT_DONE);
        // descending.
        ledc_set_fade_with_time(ledc_channel.speed_mode, ledc_channel.channel, 0, 1000);
        ledc_fade_start(ledc_channel.speed_mode, ledc_channel.channel, LEDC_FADE_WAIT_DONE);
        vTaskDelay(1);
    }
    vTaskDelete(NULL);
}

void led2_pulsing() {
    dac_output_enable(DAC_CHANNEL_2);

    while (1) {
        if (led2_is_pulsing == 0)
            vTaskDelete(NULL);
        
        int v;
        for(v = 0; v < 255; v++) {
            dac_output_voltage(DAC_CHANNEL_2, v);
            ets_delay_us(5000);
        } 
        for(v = 255; v > 0; v--) {
            dac_output_voltage(DAC_CHANNEL_2, v);
            ets_delay_us(5000);
        }
        vTaskDelay(1);
    }
}

void led3_pulsing() {
    // timer configuration.
    ledc_timer_config_t ledc_timer1;
    ledc_timer1.speed_mode      = LEDC_HIGH_SPEED_MODE;
    ledc_timer1.freq_hz         = 100;
    ledc_timer1.duty_resolution = LEDC_TIMER_8_BIT; // 256
    ledc_timer1.timer_num       = LEDC_TIMER_2;
    if(ledc_timer_config(&ledc_timer1) != ESP_OK) 
        ESP_LOGI("ledc_timer_config ", "%s", "some error occured");

    // chanel configuration.
    ledc_channel_config_t ledc_channel1;
    ledc_channel1.gpio_num   = GPIO_LED3;
    ledc_channel1.speed_mode = LEDC_HIGH_SPEED_MODE;
    ledc_channel1.channel    = LEDC_CHANNEL_2;
    ledc_channel1.intr_type  = LEDC_INTR_FADE_END;
    ledc_channel1.timer_sel  = LEDC_TIMER_2;
    ledc_channel1.duty       = 0;

    if (ledc_channel_config(&ledc_channel1) != ESP_OK) 
        ESP_LOGI("ledc_channel_config ", "%s", "some error occured");
    if (ledc_fade_func_install(0) != ESP_OK) 
        ESP_LOGI("ledc_fade_func_install ", "%s", "some error occured");

    while(1) {
        if (led3_is_pulsing == 0)
            vTaskDelete(NULL);

        // ascending.
        ledc_set_fade_with_time(ledc_channel1.speed_mode, ledc_channel1.channel, 255, 1000);
        ledc_fade_start(ledc_channel1.speed_mode, ledc_channel1.channel, LEDC_FADE_WAIT_DONE);
        // descending.
        ledc_set_fade_with_time(ledc_channel1.speed_mode, ledc_channel1.channel, 0, 1000);
        ledc_fade_start(ledc_channel1.speed_mode, ledc_channel1.channel, LEDC_FADE_WAIT_DONE);
        vTaskDelay(1);
    }
}


void user_input() {
    const char *prompt = "Enter your command : ";
    uint8_t command_line[COMMAND_LINE_MAX_LENGTH];

    while(1) {
        bzero(command_line, COMMAND_LINE_MAX_LENGTH);
        int  i = 0;
        bool quit = false;
        uart_write_bytes(UART_PORT, prompt, strlen(prompt));

        do {
            uart_flush_input(UART_PORT);
            int r = uart_read_bytes(UART_PORT, &command_line[i], 1, (200 / portTICK_PERIOD_MS));

            if (r == 1) {
                if (command_line[i] != 127) {
                    char *tmp = (char *)&command_line[i];
                    uart_write_bytes(UART_PORT, (const char *)tmp, 1);
                    i++;
                }
                else if (command_line[i] == 127 ) {
                    if (i == 0)
                        command_line[i] = '\0';
                    else {
                        char c = 8;
                        char *tmp = &c;
                        uart_write_bytes(UART_PORT, tmp, 1);
                        uart_write_bytes(UART_PORT, " ", 1);
                        uart_write_bytes(UART_PORT, tmp, 1);
                        command_line[i] = '\0';
                        i -= 1;
                        command_line[i] = '\0';
                    }
                }
            }
            
            if (r != -1) {
                char *p = strchr((const char *)command_line, CR_ASCII_CODE);
                if (p != NULL) {
                    uart_write_bytes(UART_PORT, "\n\r", 2);
                    uart_write_bytes(UART_PORT, (const char *)command_line, i);
                    uart_write_bytes(UART_PORT, "\n\r", 2);
                    int index = p - (char *)command_line;
                    command_line[index] = '\0';
                    quit = true;
                }
            }
        } while(!quit && i < COMMAND_LINE_MAX_LENGTH);

        if (!xQueueSend(global_queue_handle, command_line, 10000))
            printf("Failed to send data in queue\n");
        vTaskDelay(1);
    }
}


void led_commands(char **cmd, int len) {
    int err = 0;
    int led_num;

    if (cmd[1] && !strcmp(cmd[1], "on")) {
        if (len == 2)
            all_led_set(1);
        else if (len == 3) {
            led_num = atoi(cmd[2]);
            if (led_num <= 0 || led_num > 3)
                err = 1;
            else
                led_set_by_id(led_num, 1);
        }
    }
    else if  (cmd[1] && !strcmp(cmd[1], "off")) {
        if (len == 2)
            all_led_set(0);
        else if (len == 3) {
            led_num = atoi(cmd[2]);
            if (led_num == 0 || led_num > 3)
                err = 1;
            else
                led_set_by_id(led_num, 0);
        }
    }
    else if  (cmd[1] && !strcmp(cmd[1], "pulse")) {
        if (cmd[2]) {
            led_num = atoi(cmd[2]);
            printf("%d %d %d\n", led1_is_pulsing, led2_is_pulsing, led3_is_pulsing);
            if (led_num == 1) {
                if (led1_is_pulsing)
                    led1_is_pulsing = 0;
                else {
                    led1_is_pulsing = 1;
                    xTaskCreate(led1_pulsing, "led1_pulsing", 10040, NULL, 10, NULL);
                }
            }
            if (led_num == 2) {
                if (led2_is_pulsing)
                    led2_is_pulsing = 0;
                else {
                    led2_is_pulsing = 1;
                    xTaskCreate(led2_pulsing, "led2_pulsing", 4040, NULL, 10, NULL);
                }
            }
            if (led_num == 3) {
                 if (led3_is_pulsing)
                    led3_is_pulsing = 0;
                else {
                    led3_is_pulsing = 1;
                    xTaskCreate(led3_pulsing, "led3_pulsing", 4040, NULL, 10, NULL);
                }
            }
        }
        else {
            led1_is_pulsing = 1;
            led2_is_pulsing = 1;
            led3_is_pulsing = 1;      
            xTaskCreate(led1_pulsing, "led1_pulsing", 4040, NULL, 10, NULL);
            xTaskCreate(led2_pulsing, "led2_pulsing", 4040, NULL, 10, NULL);
            xTaskCreate(led3_pulsing, "led3_pulsing", 4040, NULL, 10, NULL);
        }
    }
}


void execute(char **cmd, int len) {

    if (cmd[0] && !strcmp(cmd[0], "led"))
        led_commands(cmd, len);
    // else if (other commands)
}



void cmd_handler() {
    char result[1000];
    bzero(result, 1000);

    char **cmd = (char **)malloc(100 * sizeof(char *));
    while(1) {
        if (xQueueReceive(global_queue_handle, result, 10000)) {
            for (int i = 0; i < 100; ++i) cmd[i] = NULL;

            // splitting str into arr.
            int index = 0;
            char *p;
            p = strtok(result, " ");
            cmd[index] = p;
            index++;
            while(p != NULL || index < 100) {
                p = strtok(NULL, " ");
                cmd[index] = p;
                index++;
            }
            // 
            int cmd_len = 0;
            while(cmd[cmd_len] && cmd_len < 100) cmd_len++;
            execute(cmd, cmd_len);
        }
        vTaskDelay(1);
    }
}



void app_main() {
    global_queue_handle = xQueueCreate(5, COMMAND_LINE_MAX_LENGTH);
    uart_init(9600);
    xTaskCreate(user_input,  "user_input",  4040, NULL, 10, NULL);
    xTaskCreate(cmd_handler, "cmd_handler", 4040, NULL, 10, NULL);
}

