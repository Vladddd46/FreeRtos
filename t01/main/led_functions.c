#include "header.h"

#define GPIO_LED1 27
#define GPIO_LED2 26
#define GPIO_LED3 33

#define WRONG_SYNTAX     10
#define INVALID_ARGUMENT 11
#define LED_BUSY         12

void eror_msg(int err) {
    char *msg;
    if (err == INVALID_ARGUMENT) {
        msg = "\e[31minvalid led number| led on/off [1-3]\e[0m\n\r";
        uart_write_bytes(UART_PORT, msg, strlen(msg));
    }
    else if (err == WRONG_SYNTAX) {
        msg = "\e[31mwrong syntax| led on/off [1-3]\e[0m\n\r";
        uart_write_bytes(UART_PORT, msg, strlen(msg));
    }
    else if (err == LED_BUSY) {
        msg = "\e[31m led is busy| led[s] you are trying to turn on is[are] busy. Turn off your led in order\
        to do manipulations with it. syntax: led off led_number(1-3)\e[0m\n\r";
        uart_write_bytes(UART_PORT, msg, strlen(msg));
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

    eror_msg(err);
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

    eror_msg(err);
}

void led_pulse(char **cmd, int len) {
    int err = 0;
    int led_num;

    if (cmd[2]) {
        if (cmd[3]) {
            regex_t regex;
            int reti = regcomp(&regex, "^f=[0-2].[0-9]$", 0);
            if (reti) {
                fprintf(stderr, "Could not compile regex\n");
                exit(1);
            }
            /* Execute regular expression */
            reti = regexec(&regex, cmd[3], 0, NULL, 0);
            if (!reti)
                puts("Match");
            else if (reti == REG_NOMATCH)
                puts("No match");
        }

        led_num = atoi(cmd[2]);

        if (led_num == 1) {
            led1_is_pulsing = 1;
            xTaskCreate(led1_pulsing, "led1_pulsing", 4040, NULL, 10, NULL);
        }
        if (led_num == 2) {
            led2_is_pulsing = 1;
            xTaskCreate(led2_pulsing, "led2_pulsing", 4040, NULL, 10, NULL);
        }
        if (led_num == 3) {
            led3_is_pulsing = 1;
            xTaskCreate(led3_pulsing, "led3_pulsing", 4040, NULL, 10, NULL);
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
    eror_msg(err);
}