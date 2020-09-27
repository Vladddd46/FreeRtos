#include "header.h"

/* @ Prints dht11 log in CLI.
 * 1. Suspend task dht11_monitor in order it can`t write in queue.
 * 2. Receive all data from queue and store it in buffer data.
 * 3. Print data in CLI and send it back in Queue.
 * 4. Resume task dht11_monitor.
 */
void dht11_log() {
    char buff[70];
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
        for (int i = 0; data[i]; ++i) {
            xQueueSend(dht11_data_queue,(void *)data[i], (TickType_t)0);
            uart_write_bytes(UART_PORT, "\n\r", strlen("\n\r"));
            uart_write_bytes(UART_PORT, (const char*)data[i], strlen((const char*)data[i]));
            free(data[i]);
        }
        uart_write_bytes(UART_PORT, "\n\r", strlen("\n\r"));
    }
    vTaskResume(xTaskWeather);
}

