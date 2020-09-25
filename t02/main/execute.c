#include "header.h"

// determines type of led cmd.
void led_commands(char **cmd, int len) {
    if (cmd[1] && !strcmp(cmd[1], "on"))
        led_on(cmd, len);
    else if  (cmd[1] && !strcmp(cmd[1], "off"))
        led_off(cmd, len);
    else if  (cmd[1] && !strcmp(cmd[1], "pulse"))
        led_pulse(cmd, len);
    else
        error_msg(NO_SUCH_COMMAND);
}

void help() {
    char *msg = "List of supported commands:\
    \n\r\e[32mled on [1-3]\e[0m - enables led x. If no led specified, all leds are enabled.\
    \n\r\e[32mled off [1-3]\e[0m - unables led x. If no led specified, all leds are unabled.\
    \n\r\e[32mled pulse [1-3] [f=x.y(optional; 0 <= x <= 2, 0 <= y <= 9); f=1.0 by default]\e[0m - makes led pulse with given frequency. If no led specified, all leds are pulsing\
    \n\r\e[32mhelp\e[0m - lists all supported commands.\n\r";
    uart_write_bytes(UART_PORT, msg, strlen(msg));
}

void dht11_log() {
    char buff[70];

    if (dht11_data_queue != 0) {

        while(1) {
            bzero(buff, 70);
            xQueueReceive(dht11_data_queue, (void *)buff, (TickType_t)0);
            if (strlen((char *)buff) == 0)
                break;
            uart_write_bytes(UART_PORT, "\n\r", strlen("\n\r"));
            uart_write_bytes(UART_PORT, (const char*)buff, strlen((const char*)buff));
        }
        uart_write_bytes(UART_PORT, "\n\r", strlen("\n\r"));
    }
}

/*
 * Determines the type of program
 * to be executed.
 */
void execute(char **cmd, int len) {
    if (cmd[0] && !strcmp(cmd[0], "led"))
        led_commands(cmd, len);
    else if (cmd[0] && !strcmp(cmd[0], "help"))
        help();
    else if (cmd[0] && !strcmp(cmd[0], "log")) {
        dht11_log();
    }
    else if (len == 0) {
        // pass
    }
    else 
        error_msg(NO_SUCH_COMMAND);
}
