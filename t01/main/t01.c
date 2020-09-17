#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include <string.h>
#include "driver/uart.h"
#include "driver/gpio.h"
#include "wrappers.h"

#define LF_ASCII_CODE 0xA
#define CR_ASCII_CODE 0xD // \r the same

#define COMMAND_LINE_MAX_LENGTH 1024
#define UART_TX_PIN 17
#define UART_RX_PIN 16
#define UART_PORT UART_NUM_1


#define GPIO_LED1 27
#define GPIO_LED2 26
#define GPIO_LED3 33

xQueueHandle global_queue_handle;


static void  init_uart() {
    uart_config_t uart_config = {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    uart_param_config(UART_PORT, &uart_config);
    uart_set_pin(UART_PORT, UART_TX_PIN, UART_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(UART_PORT, COMMAND_LINE_MAX_LENGTH * 2, 0, 0, NULL, 0);
}

void user_input() {
    const char *prompt = "Enter your command : ";
    uint8_t command_line[COMMAND_LINE_MAX_LENGTH];
    while(1) {
        bzero(command_line, COMMAND_LINE_MAX_LENGTH);
        int  i = 0;
        bool q = false;
        uart_write_bytes(UART_PORT, prompt, strlen(prompt));

        do {
            uart_flush_input(UART_PORT);
            int r = uart_read_bytes(UART_PORT, &command_line[i], 1, (200 / portTICK_PERIOD_MS));

            if (r == 1) {
                if (command_line[i] != 127) {
                    char *tmp = (char *)&command_line[i];
                    int x = uart_write_bytes(UART_PORT, (const char *)tmp, 1);
                    i++;
                }
                else if (command_line[i] == 127 && i == 0)
                    command_line[i] = '\0';
                else if (command_line[i] == 127) {
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
            
            if (r != -1) {
                char *p = strchr((const char *)command_line, CR_ASCII_CODE);
                if (p != NULL) {
                    uart_write_bytes(UART_PORT, "\n\r", 2);
                    uart_write_bytes(UART_PORT, (const char *)command_line, i);
                    uart_write_bytes(UART_PORT, "\n\r", 2);
                    int index = p - (char *)command_line;
                    command_line[index] = '\0';
                    q = true;
                }
            }
        } while(!q && i < COMMAND_LINE_MAX_LENGTH);

        if (!xQueueSend(global_queue_handle, command_line, 10000))
            printf("Failed to send data in queue\n");
        vTaskDelay(1);
    }
}

void led_mode(int gpio_led, int set) {
    gpio_set(gpio_led, GPIO_MODE_OUTPUT, set);
}

void cmd_handler() {
    char result[1000];
    bzero(result, 1000);

    char **cmd = (char **)malloc(100 * sizeof(char *));

    while(1) {
        if (xQueueReceive(global_queue_handle, result, 10000)) {
            for (int i = 0; i < 100; ++i) cmd[i] = NULL;

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


            if (cmd[0] && strcmp(cmd[0], "led") != 0)
                printf("\n\n 1 error: wrong command...\n");
            else if (cmd[1] && (strcmp(cmd[1], "on") != 0 && strcmp(cmd[1], "off") != 0))
                printf("\n\n 2 error: wrong command... %s %d %d\n", cmd[1],strcmp(cmd[1], "on") , strcmp(cmd[1], "off"));
            else if (cmd[1]) {

                if (!strcmp(cmd[1], "on")) {
                    led_mode(GPIO_LED1, 1);
                    led_mode(GPIO_LED2, 1);
                    led_mode(GPIO_LED3, 1);
                }
                else if (!strcmp(cmd[1], "off")) {
                    led_mode(GPIO_LED1, 0);
                    led_mode(GPIO_LED2, 0);
                    led_mode(GPIO_LED3, 0);
                }
            }
        }
        vTaskDelay(1);
    }
}



void app_main() {
    global_queue_handle = xQueueCreate(5, COMMAND_LINE_MAX_LENGTH);
    init_uart();
    xTaskCreate(user_input,  "user_input",  4040, NULL, 10, NULL);
    xTaskCreate(cmd_handler, "cmd_handler", 40040, NULL, 10, NULL);
}


