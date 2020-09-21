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
#include <regex.h> 


#define COMMAND_LINE_MAX_LENGTH 1024
#define UART_PORT UART_NUM_1


#define WRONG_SYNTAX_LED_ON_OFF 10
#define INVALID_ARGUMENT        11
#define LED_BUSY                12
#define NO_SUCH_COMMAND         13
#define WRONG_SYNTAX_PULSE      14
#define WRONG_FREQUENCY_VALUE   15

struct led_settings_description {
	int   led_id;
	float freq;
};

int led1_is_pulsing;
int led2_is_pulsing;
int led3_is_pulsing;
xQueueHandle global_queue_handle;



void led_mode(int gpio_led, int set);
void all_led_set(int mode);
void led_set_by_id(int led_id, int mode);

void led1_pulsing(void *settings);

void user_input();
void cmd_handler();

void execute(char **cmd, int len);
void error_msg(int err);

/* led functions */
void led_on(char **cmd, int len);
void led_off(char **cmd, int len);
void led_pulse(char **cmd, int len);