#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include <string.h>
#include "libmx.h"
#include "wrappers.h"
#include "freertos/queue.h"

#define LF_ASCII_CODE 0xA
#define COMMAND_LINE_MAX_LENGTH 100

#define GPIO_LED1 27
#define GPIO_LED2 26
#define GPIO_LED3 33

xQueueHandle global_queue_handle;


/*
 * This task performs user`s input functional.
 * Stores data inputed by user in command_line variable.
 * Sends command_line variable to cmd_handler task through queue.
 */
void user_input() {
    char command_line[COMMAND_LINE_MAX_LENGTH];

    while(1) {
        int i = 0;
        bool q = false;
        printf("\nEnter your command: ");

        do {
            if (scanf("%c", &command_line[i]) != -1) {
                printf("%c", command_line[i]);
                fflush(stdout);
                if (command_line[i] == LF_ASCII_CODE) {
                    command_line[i] = '\0';
                    q = true;
                }
                else
                    i++;
            }
            vTaskDelay(100 / portTICK_PERIOD_MS);
        } while(!q && i < COMMAND_LINE_MAX_LENGTH);
        printf("\n\n");
        char *res = mx_string_copy(command_line);
        if (!xQueueSend(global_queue_handle, res, 10000))
            printf("Failed to send data in queue\n");
        free(res);
        vTaskDelay(1);
    }
    vTaskDelete(NULL);
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
        else
            continue;
        vTaskDelay(1);
    }
}


void app_main() {
    global_queue_handle = xQueueCreate(5, COMMAND_LINE_MAX_LENGTH);
    xTaskCreate(user_input,  "user_input",  4040, NULL, 10, NULL);
    xTaskCreate(cmd_handler, "cmd_handler", 40040, NULL, 10, NULL);
}



