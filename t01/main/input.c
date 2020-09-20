#include "header.h"

#define LF_ASCII_CODE 0xA
#define CR_ASCII_CODE 0xD // \r the same

/*
 * Reads user`s input from UART2 and stores it in comman_Line string.
 * When user press enter string command_line is sending to cmd_handler
 * task through queue.
 */
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
                    int index = p - (char *)command_line;
                    command_line[index] = '\0';
                    quit = true;
                }
            }
        } while(!quit && i < COMMAND_LINE_MAX_LENGTH);

        if (!xQueueSend(global_queue_handle, command_line, (200 / portTICK_PERIOD_MS)))
            printf("Failed to send data in queue\n");
        vTaskDelay(1);
    }
}




/*
 * Receives user`s input from Queue.
 * Splits user`s input in arr.
 * Calls execute function, which is in charge 
 * of executing command.
 */
void cmd_handler() {
    char result[1000];
    bzero(result, 1000);

    char **cmd = (char **)malloc(100 * sizeof(char *));
    while(1) {
        if (xQueueReceive(global_queue_handle, result, (200 / portTICK_PERIOD_MS))) {
            for (int i = 0; i < 100; ++i) cmd[i] = NULL;

            // splitting str into arr.
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
            // 
            int cmd_len = 0;
            while(cmd[cmd_len] && cmd_len < 100) cmd_len++;
            execute(cmd, cmd_len);
        }
    }
}