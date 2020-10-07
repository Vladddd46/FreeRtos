#include "header.h"

#define LF_ASCII_CODE 0xA
#define CR_ASCII_CODE 0xD // \r the same
#define BACK_SPACE    127
#define TIME_PROMPT   "Please, enter current time: hh mm ss:"
#define PROMPT        "Enter your command: "
#define INPUT_2_LONG  "\n\rSorry, but command can`t be longer than 30 symbols.\n\r"
#define RED_COLOR     "\e[31m"
#define DEFAULT_COLOR "\e[0m"



void write_red_time_promt() {
    uart_write_bytes(UART_PORT, RED_COLOR,    strlen(RED_COLOR));
    uart_write_bytes(UART_PORT, TIME_PROMPT,  strlen(TIME_PROMPT));
    uart_write_bytes(UART_PORT, DEFAULT_COLOR,strlen(DEFAULT_COLOR));
}


void force_time_set(char **cmd, int cmd_len) {
    int h,m,s;

    if (cmd_len == 3) {
        h = atoi(cmd[0]);
        m = atoi(cmd[1]);
        s = atoi(cmd[2]);

        if (h == 0 && (strcmp("0", cmd[0]) != 0 && strcmp("00", cmd[0]) != 0))
            write_red_time_promt();
        else if (m == 0 && (strcmp("0", cmd[1]) != 0 && strcmp("00", cmd[1]) != 0))
            write_red_time_promt();
        else if (s == 0 && (strcmp("0", cmd[2]) != 0 && strcmp("00", cmd[2]) != 0))
            write_red_time_promt();
        else if ((h > 23 || m > 60 || s > 60) || (m < 0 || h < 0 || s < 0))
            write_red_time_promt();
        else {
            current_time = (h * 3600) + (m * 60) + s;
            uart_write_bytes(UART_PORT, PROMPT, strlen(PROMPT));
        }
    }
    else {
        if (cmd_len == 0)
            uart_write_bytes(UART_PORT, TIME_PROMPT, strlen(TIME_PROMPT));
        else
            write_red_time_promt();
    }
}

/*
 * Reads user`s input from UART2 and stores it in comman_Line string.
 * When user press enter string command_line is sending to cmd_handler
 * task through queue.
 */
void user_input() {
    uart_event_t event;
    uint8_t command_line[COMMAND_LINE_MAX_LENGTH];
    size_t buf_size    = 0;
    uint8_t *buf       = NULL;
    int index          = 0;
    uart_write_bytes(UART_PORT, TIME_PROMPT, strlen(TIME_PROMPT));
    while(1) {
        bzero(command_line, COMMAND_LINE_MAX_LENGTH);
        
        while (1) {
            if (xQueueReceive(uart0_queue, (void * )&event, (portTickType)portMAX_DELAY)) {
                if (event.type == UART_DATA) {
                    uart_get_buffered_data_len(UART_PORT, &buf_size);
                    if (buf_size > 30 || index > 30) {
                        uart_write_bytes(UART_PORT, INPUT_2_LONG, strlen(INPUT_2_LONG));
                        uart_read_bytes(UART_PORT, NULL, buf_size + 1, buf_size);
                        if (current_time == -1)
                            uart_write_bytes(UART_PORT, PROMPT, strlen(PROMPT));
                        else 
                            uart_write_bytes(UART_PORT, TIME_PROMPT, strlen(TIME_PROMPT));
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
                        if (!xQueueSend(global_input_queue, command_line, (200 / portTICK_PERIOD_MS)))
                            printf("Failed to send data in queue\n");
                        index = 0;
                        free(buf);
                        break;
                    }
                    else if (buf[0] == 27 || buf[0] == '[') {
                        free(buf);
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


char **split_input(char *inp) {
    char **cmd = mx_strarr_new(100);
    int index = 0;
    char *p;
            
    p = strtok(inp, " ");
    cmd[index] = p;
    index++;
    while(p != NULL && index < 100) {
        p = strtok(NULL, " ");
        cmd[index] = p;
        index++;
    }
    return cmd;
}


/*
 * Receives user`s input from Queue.
 * Change it in lowercase.
 * Splits user`s input in arr.
 * Calls execute function, which is in charge 
 * of executing command.
 */
void cmd_handler() {
    char result[1000];
    char *result_lower;
    char **cmd = NULL;
    int  cmd_len;

    while(1) {
        bzero(result, 1000);
        if (xQueueReceive(global_input_queue, result, (200 / portTICK_PERIOD_MS))) {
            result_lower = mx_upper_to_lower(result);
            cmd          = split_input(result_lower);
            cmd_len      = mx_strarr_len(cmd);
            if (current_time == -1) 
                force_time_set(cmd, cmd_len);
            else 
                execute(cmd, cmd_len);
            free(result_lower);
        }
    }
}
