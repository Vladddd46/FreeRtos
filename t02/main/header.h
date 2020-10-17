#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include <string.h>
#include "driver/uart.h"
#include "driver/gpio.h"
#include "wrappers.h"
#include "driver/dac.h"
#include "driver/ledc.h"
#include <regex.h> 
#include "get_dht11_data.h"
#include "libmx.h"
#include <strings.h>
#include <ctype.h>

/* Config */
#define COMMAND_LINE_MAX_LENGTH 1024
#define UART_PORT 				UART_NUM_1
#define NEWLINE  				"\r\n"
/* Errors */
#define WRONG_SYNTAX_LED_ON_OFF 10
#define INVALID_ARGUMENT        11
#define LED_BUSY                12
#define NO_SUCH_COMMAND         13
#define WRONG_SYNTAX_PULSE      14
#define WRONG_FREQUENCY_VALUE   15

/* LED PINS */
#define LED1 27
#define LED2 26
#define LED3 33

/* LED states */
#define LED_IS_OFF				20
#define LED_IS_ON 				21
#define LED_IS_PULSING 			22


struct led_settings_description {
	int   led_id;
	float freq;
};

int led1_state;
int led2_state;
int led3_state;

xQueueHandle  global_queue_handle;
QueueHandle_t uart0_queue;
QueueHandle_t dht11_data_queue;
TaskHandle_t xTaskWeather;

void user_input();
void cmd_handler();
void execute(char **cmd, int len);
void error_msg(int err);
void uart_init(int baud_rate);

/* dht11 fucntions */
void dht11_monitor();
void dht11_log(char **cmd);
void help_command();

/* led functions */
void led_on(char **cmd, int len);
void led_off(char **cmd, int len);
void led_pulse(char **cmd, int len);
void led_pulsing_task(void *settings);
