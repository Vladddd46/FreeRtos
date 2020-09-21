#include "header.h"

#define GPIO_LED1 27
#define GPIO_LED2 26
#define GPIO_LED3 33


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
        err = WRONG_SYNTAX_LED_ON_OFF;

    error_msg(err);
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
        err = WRONG_SYNTAX_LED_ON_OFF;

    error_msg(err);
}



/*
 * Retrieves 
 *
 */
float freq_determine(char *subcmd) {
    if (subcmd == NULL)
        return -1;
    char *freq_str = (char *)malloc(5 * sizeof(char));
    bzero(freq_str, 5);

    int index = 0;
    for (int i = 2; subcmd[i]; ++i) {
        freq_str[index] = subcmd[i];
        index++;
    }
    float freq = atof(freq_str);
    free(freq_str);
    return freq;
}



/*
 * Checks, whether str matches 
 * frequency subcommand pattern.
 * ^f=x.y$ , where 0 <= x <= 2, 0 <= y <= 9
 */
int freq_match(char *substr) {
    if (substr == NULL)
        return 0;
    regex_t regex;
    int reti = regcomp(&regex, "^f=[0-2].[0-9]$", 0);

    if (reti) 
        return 0;
    reti = regexec(&regex, substr, 0, NULL, 0);
    if (!reti) 
        return 1;
    return  0;
}


void led_pulse(char **cmd, int len) {
    int err = 0;
    int led_num;
    float freq = 1;

    if (freq_match(cmd[2]))
        freq = freq_determine(cmd[2]);
    else if (freq_match(cmd[3]))
        freq = freq_determine(cmd[3]);

    if (freq < 0.0 || freq > 2.0)
        err = WRONG_FREQUENCY_VALUE;

    if (len > 4)
        err = WRONG_SYNTAX_PULSE;
    else if (cmd[2] == NULL || freq_match(cmd[2])) {
        led1_is_pulsing = 1;
        led2_is_pulsing = 1;
        led3_is_pulsing = 1;      
        xTaskCreate(led1_pulsing, "led1_pulsing", 4040, NULL, 10, NULL);
        xTaskCreate(led2_pulsing, "led2_pulsing", 4040, NULL, 10, NULL);
        xTaskCreate(led3_pulsing, "led3_pulsing", 4040, NULL, 10, NULL);
    }
    else {
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
    error_msg(err);
}