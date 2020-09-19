#include "header.h"

#define LF_ASCII_CODE 0xA
#define CR_ASCII_CODE 0xD // \r the same


void user_input() {
    const char *prompt = "Enter your command : ";
    uint8_t command_line[COMMAND_LINE_MAX_LENGTH];

    while(1) {
        bzero(command_line, COMMAND_LINE_MAX_LENGTH);
        int  i = 0;
        bool quit = false;
        uart_write_bytes(UART_PORT, prompt, strlen(prompt));

        do {
            uart_flush_input(UART_PORT);
            int r = uart_read_bytes(UART_PORT, &command_line[i], 1, (200 / portTICK_PERIOD_MS));

            if (r == 1) {
                if (command_line[i] != 127) {
                    char *tmp = (char *)&command_line[i];
                    uart_write_bytes(UART_PORT, (const char *)tmp, 1);
                    i++;
                }
                else if (command_line[i] == 127 ) {
                    if (i == 0)
                        command_line[i] = '\0';
                    else {
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
            }
            
            if (r != -1) {
                char *p = strchr((const char *)command_line, CR_ASCII_CODE);
                if (p != NULL) {
                    uart_write_bytes(UART_PORT, "\n\r", 2);
                    uart_write_bytes(UART_PORT, (const char *)command_line, i);
                    uart_write_bytes(UART_PORT, "\n\r", 2);
                    int index = p - (char *)command_line;
                    command_line[index] = '\0';
                    quit = true;
                }
            }
        } while(!quit && i < COMMAND_LINE_MAX_LENGTH);

        if (!xQueueSend(global_queue_handle, command_line, 10000))
            printf("Failed to send data in queue\n");
        vTaskDelay(1);
    }
}