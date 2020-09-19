#include "header.h"

#define GPIO_LED1 27
#define GPIO_LED2 26
#define GPIO_LED3 33

#define WRONG_SYNTAX     10
#define INVALID_ARGUMENT 11
#define LED_BUSY         12


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

void led_pulse(char **cmd, int len) {
    int err = 0;
    int led_num;

    if (cmd[2]) {
        led_num = atoi(cmd[2]);

        if (led_num == 1) {
            if (gpio_get_level(GPIO_LED1))
                err = LED_BUSY;
            else {
                led1_is_pulsing = 1;
                xTaskCreate(led1_pulsing, "led1_pulsing", 10040, NULL, 10, NULL);
            }
        }
        if (led_num == 2) {
            if (gpio_get_level(GPIO_LED2))
                err = LED_BUSY;
            else {
                led2_is_pulsing = 1;
                xTaskCreate(led2_pulsing, "led2_pulsing", 4040, NULL, 10, NULL);
            }
        }
        if (led_num == 3) {
            if (gpio_get_level(GPIO_LED3))
                err = LED_BUSY;
            else {
                led3_is_pulsing = 1;
                xTaskCreate(led3_pulsing, "led3_pulsing", 4040, NULL, 10, NULL);
            }
        }
    }
    else {
        if (gpio_get_level(GPIO_LED1) || gpio_get_level(GPIO_LED2) || gpio_get_level(GPIO_LED3))
            err = LED_BUSY;
        else {
            led1_is_pulsing = 1;
            led2_is_pulsing = 1;
            led3_is_pulsing = 1;      
            xTaskCreate(led1_pulsing, "led1_pulsing", 4040, NULL, 10, NULL);
            xTaskCreate(led2_pulsing, "led2_pulsing", 4040, NULL, 10, NULL);
            xTaskCreate(led3_pulsing, "led3_pulsing", 4040, NULL, 10, NULL);
        }
    }

    char *msg;
    if (err == LED_BUSY) {
        msg = "\e[31m led is busy| led[s] you are trying to make pulse is[are] busy. Turn off your led in order\
        to do manipulations with it. syntax: led off led_number(1-3)\e[0m\n\r";
        uart_write_bytes(UART_PORT, msg, strlen(msg));
    }
}

void led_commands(char **cmd, int len) {
    if (cmd[1] && !strcmp(cmd[1], "on"))
        led_on(cmd, len);
    else if  (cmd[1] && !strcmp(cmd[1], "off"))
        led_off(cmd, len);
    else if  (cmd[1] && !strcmp(cmd[1], "pulse"))
        led_pulse(cmd, len);
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



void global_variables_init() {
    led1_is_pulsing = 0;
    led2_is_pulsing = 0;
    led3_is_pulsing = 0;

    global_queue_handle = xQueueCreate(5, COMMAND_LINE_MAX_LENGTH);
}

void app_main() {
    global_variables_init();
    uart_init(9600);

    xTaskCreate(user_input,  "user_input",  4040, NULL, 10, NULL);
    xTaskCreate(cmd_handler, "cmd_handler", 4040, NULL, 10, NULL);
}

