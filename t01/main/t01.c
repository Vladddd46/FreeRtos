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

        char *res = mx_string_copy(command_line);
        if (!xQueueSend(global_queue_handle, res, 10000))
            printf("Failed to send data in queue\n");
        free(res);
        vTaskDelay(1);
    }
    vTaskDelete(NULL);
}

void cmd_handler() {
    char result[1000];
    bzero(result, 1000);

    while(1) {

        if (xQueueReceive(global_queue_handle, result, 10000)) {
            printf("\n\nresult ==> %s %d\n",  result, strlen(mx_string_copy(result)));
        }
        else
            continue;
        vTaskDelay(1);
    }
}


void app_main() {
    global_queue_handle = xQueueCreate(5, COMMAND_LINE_MAX_LENGTH);
    xTaskCreate(user_input,  "user_input",  4040, NULL, 10, NULL);
    xTaskCreate(cmd_handler, "cmd_handler", 4040, NULL, 10, NULL);
}



