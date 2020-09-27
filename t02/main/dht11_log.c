#include "header.h"

int arr_len(char **arr) {
    if (arr == NULL) return -1;
    int i = 0;
    while(arr[i]) i++;
    return i;
}

static void print_log(char **data) {
    char *previous_value = NULL;
    char *tmp = NULL;

    char time_buff[100];
    bzero(time_buff, 20);

    int seconds_passed;
    for (int i = 0; data[i]; ++i) {
        seconds_passed =  (arr_len(data) * 5) - (i * 5);
        xQueueSend(dht11_data_queue,(void *)data[i], (TickType_t)0);
        sprintf(time_buff, " |\e[35mvalues changed \e[31m%d\e[35mmin \e[31m%d\e[35msec ago\e[0m", seconds_passed / 60, seconds_passed % 60);
        uart_write_bytes(UART_PORT, "\n\r", strlen("\n\r"));
        uart_write_bytes(UART_PORT, (const char*)data[i], strlen((const char*)data[i]));

        if (previous_value && strcmp(previous_value, data[i]) != 0)
            uart_write_bytes(UART_PORT, (const char*)time_buff, strlen((const char*)time_buff));

        if (previous_value) tmp = previous_value;
        previous_value = mx_string_copy(data[i]);
        if (tmp) free(tmp);
        free(data[i]);
    }

}



/* @ Prints dht11 log in CLI.
 * 1. Suspend task dht11_monitor in order it can`t write in queue.
 * 2. Receive all data from queue and store it in buffer data.
 * 3. Print data in CLI and send it back in Queue.
 * 4. Resume task dht11_monitor.
 */
void dht11_log() {
    char buff[100];
    char **data = mx_strarr_new(60);

    int index = 0;
    vTaskSuspend(xTaskWeather);
    if (dht11_data_queue != 0) {
        while(1) {
            bzero(buff, 70);
            xQueueReceive(dht11_data_queue, (void *)buff, (TickType_t)0);
            if (strlen((char *)buff) == 0) break;
            data[index] = mx_string_copy(buff);
            index++;
        }
        print_log(data);
        uart_write_bytes(UART_PORT, "\n\r", strlen("\n\r"));
    }
    vTaskResume(xTaskWeather);
}

