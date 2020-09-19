#include "header.h"


#define LF_ASCII_CODE 0xA
#define CR_ASCII_CODE 0xD // \r the same

#define COMMAND_LINE_MAX_LENGTH 1024
#define UART_PORT UART_NUM_1


#define GPIO_LED1 27
#define GPIO_LED2 26
#define GPIO_LED3 33

#define WRONG_SYNTAX     10
#define INVALID_ARGUMENT 11
#define LED_BUSY         12

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
    // timer configuration.
    ledc_timer_config_t ledc_timer;
    ledc_timer.speed_mode      = LEDC_HIGH_SPEED_MODE;
    ledc_timer.freq_hz         = 100;
    ledc_timer.duty_resolution = LEDC_TIMER_8_BIT; // 256
    ledc_timer.timer_num       = LEDC_TIMER_2;
    if(ledc_timer_config(&ledc_timer) != ESP_OK) 
        ESP_LOGI("ledc_timer_config ", "%s", "some error occured");

    // chanel configuration.
    ledc_channel_config_t ledc_channel;
    ledc_channel.gpio_num   = GPIO_LED2;
    ledc_channel.speed_mode = LEDC_HIGH_SPEED_MODE;
    ledc_channel.channel    = LEDC_CHANNEL_2;
    ledc_channel.intr_type  = LEDC_INTR_FADE_END;
    ledc_channel.timer_sel  = LEDC_TIMER_2;
    ledc_channel.duty       = 0;

    if (ledc_channel_config(&ledc_channel) != ESP_OK) 
        ESP_LOGI("ledc_channel_config ", "%s", "some error occured");
    if (ledc_fade_func_install(0) != ESP_OK) 
        ESP_LOGI("ledc_fade_func_install ", "%s", "some error occured");

     while(1) {
        if (led2_is_pulsing == 0)
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

void led3_pulsing() {
    // timer configuration.
    ledc_timer_config_t ledc_timer1;
    ledc_timer1.speed_mode      = LEDC_HIGH_SPEED_MODE;
    ledc_timer1.freq_hz         = 100;
    ledc_timer1.duty_resolution = LEDC_TIMER_8_BIT; // 256
    ledc_timer1.timer_num       = LEDC_TIMER_3;
    if(ledc_timer_config(&ledc_timer1) != ESP_OK) 
        ESP_LOGI("ledc_timer_config ", "%s", "some error occured");

    // chanel configuration.
    ledc_channel_config_t ledc_channel1;
    ledc_channel1.gpio_num   = GPIO_LED3;
    ledc_channel1.speed_mode = LEDC_HIGH_SPEED_MODE;
    ledc_channel1.channel    = LEDC_CHANNEL_3;
    ledc_channel1.intr_type  = LEDC_INTR_FADE_END;
    ledc_channel1.timer_sel  = LEDC_TIMER_3;
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


void led_on(char **cmd, int len) {
    int err = 0;
    int led_num;

    if (len == 2) {
        if (led1_is_pulsing || led2_is_pulsing || led3_is_pulsing)
            err = LED_BUSY;
        else
            all_led_set(1);
    }
    else if (len == 3) {
        led_num = atoi(cmd[2]);

        if ((led_num == 1 && led1_is_pulsing) 
            || (led_num == 2 && led2_is_pulsing) 
            || (led_num == 3 && led3_is_pulsing))
            err = LED_BUSY;
        else if (led_num <= 0 || led_num > 3)
            err = INVALID_ARGUMENT;
        else
            led_set_by_id(led_num, 1);
    }
    else
        err = WRONG_SYNTAX;

    char *msg;
    if (err == INVALID_ARGUMENT) {
        msg = "\e[31minvalid led number| led on [1-3]\e[0m\n\r";
        uart_write_bytes(UART_PORT, msg, strlen(msg));
    }
    else if (err == WRONG_SYNTAX) {
        msg = "\e[31mwrong syntax| led on [1-3]\e[0m\n\r";
        uart_write_bytes(UART_PORT, msg, strlen(msg));
    }
    else if (err == LED_BUSY) {
        msg = "\e[31m led is busy| led[s] you are trying to turn on is[are] busy. Turn off your led in order\
        to do manipulations with it. syntax: led off led_number(1-3)\e[0m\n\r";
        uart_write_bytes(UART_PORT, msg, strlen(msg));
    }
}


/*
 * Turns ledX off if it`s on or if it`s pulsing.
 * if len == 2 => "led off" => turns all leds off.
 * if len == 3 => "led on 1-3" => turns 1-3 led off.
 * if len <=0 or >3 => error.
 */
void led_off(char **cmd, int len) {
    int err = 0;
    int led_num;

    if (len == 2) {
        all_led_set(0);
        led1_is_pulsing = 0;
        led2_is_pulsing = 0;
        led3_is_pulsing = 0;
    }
    else if (len == 3) {
        led_num = atoi(cmd[2]);
        if (led_num == 0 || led_num > 3)
            err = INVALID_ARGUMENT;
        else {
            led_set_by_id(led_num, 0);
            if (led_num == 1) led1_is_pulsing = 0;
            if (led_num == 2) led2_is_pulsing = 0;
            if (led_num == 3) led3_is_pulsing = 0;
        }
    }
    else
        err = WRONG_SYNTAX;

    char *msg;
    if (err == INVALID_ARGUMENT) {
        msg = "\e[31minvalid led number| led off [1-3]\e[0m\n\r";
        uart_write_bytes(UART_PORT, msg, strlen(msg));
    }
    else if (err == WRONG_SYNTAX) {
        msg = "\e[31mwrong syntax| led off [1-3]\e[0m\n\r";
        uart_write_bytes(UART_PORT, msg, strlen(msg));
    }
}


void led_commands(char **cmd, int len) {
    int err = 0;
    int led_num;

    if (cmd[1] && !strcmp(cmd[1], "on"))
        led_on(cmd, len);
    else if  (cmd[1] && !strcmp(cmd[1], "off"))
        led_off(cmd, len);
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



/*
 * Receives user`s input from Queue.
 * Splits user`s input in arr.
 * Calls execute function, which is in charge 
 * of executing command.
 */
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
        vTaskDelay(10);
    }
}



void app_main() {
    global_queue_handle = xQueueCreate(5, COMMAND_LINE_MAX_LENGTH);
    uart_init(9600);
    xTaskCreate(user_input,  "user_input",  4040, NULL, 10, NULL);
    xTaskCreate(cmd_handler, "cmd_handler", 4040, NULL, 10, NULL);
}

