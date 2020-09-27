#include "header.h"

#define LF_ASCII_CODE 0xA
#define CR_ASCII_CODE 0xD // \r the same
#define BACK_SPACE    127

/*
 * Reads user`s input from UART2 and stores it in comman_Line string.
 * When user press enter string command_line is sending to cmd_handler
 * task through queue.
 */
void user_input() {
    char *msg = "\n\rSorry, but command can`t be longer than 30 symbols.\n\r";
    uart_event_t event;
    const char *prompt = "Enter your command : ";
    uint8_t command_line[COMMAND_LINE_MAX_LENGTH];
    size_t buf_size    = 0;
    uint8_t *buf       = NULL;
    int index          = 0;

    while(1) {
        while (is_executing == 1)
            vTaskDelay(10);
        bzero(command_line, COMMAND_LINE_MAX_LENGTH);
        uart_write_bytes(UART_PORT, prompt, strlen(prompt));
        
        while (1) {
            if (xQueueReceive(uart0_queue, (void * )&event, (portTickType)portMAX_DELAY)) {
                if (event.type == UART_DATA) {
                    uart_get_buffered_data_len(UART_PORT, &buf_size);
                    if (buf_size > 30 || index > 30) {
                        uart_write_bytes(UART_PORT, msg, strlen(msg));
                        index = 0;
                        buf_size = 0;
                        break;
                    }
                    buf = malloc(sizeof(uint8_t) * (buf_size + 1));
                    if (buf == NULL) exit(1);
                    memset(buf, '\0', buf_size + 1);
                    uart_read_bytes(UART_PORT, buf, buf_size + 1, buf_size);
                    if (buf[0] == CR_ASCII_CODE && buf_size == 1) {
                        uart_write_bytes(UART_PORT, "\n\r", strlen("\n\r"));
                        if (!xQueueSend(global_queue_handle, command_line, (200 / portTICK_PERIOD_MS)))
                            printf("Failed to send data in queue\n");
                        index = 0;
                        break;
                    }
                    else if (buf[0] == BACK_SPACE && buf_size == 1) {
                        if (index > 0) {
                            char c = 8;
                            char *tmp = &c;
                            uart_write_bytes(UART_PORT, tmp, 1);
                            uart_write_bytes(UART_PORT, " ", 1);
                            uart_write_bytes(UART_PORT, tmp, 1);
                            command_line[index - 1] = '\0';
                            index -= 1;
                        }
                    }
                    uart_write_bytes(UART_PORT, (const char *)buf, strlen((const char *)buf));
                    for (int i = 0; buf[i]; ++i) {
                        if (buf[i] != BACK_SPACE) {
                            command_line[index] = buf[i];
                            index++;
                        }
                    }
                    free(buf);
                }
            }
        }
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
    char *result_lower;

    char **cmd = mx_strarr_new(100);
    while(1) {
        bzero(result, 1000);
        if (xQueueReceive(global_queue_handle, result, (200 / portTICK_PERIOD_MS))) {
            for (int i = 0; i < 100; ++i) cmd[i] = NULL;
            result_lower = mx_upper_to_lower(result);
            // splitting str into arr.
            int index = 0;
            char *p;
            p = strtok(result_lower, " ");
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
            free(result_lower);
        }
    }
}