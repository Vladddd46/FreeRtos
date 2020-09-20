#include "header.h"

// determines type of led cmd.
void led_commands(char **cmd, int len) {
    if (cmd[1] && !strcmp(cmd[1], "on"))
        led_on(cmd, len);
    else if  (cmd[1] && !strcmp(cmd[1], "off"))
        led_off(cmd, len);
    else if  (cmd[1] && !strcmp(cmd[1], "pulse"))
        led_pulse(cmd, len);
}

/*
 * Determines the type of program
 * to be executed.
 */
void execute(char **cmd, int len) {
    if (cmd[0] && !strcmp(cmd[0], "led"))
        led_commands(cmd, len);
    else {
        char *msg = "\e[31mERROR: No such command.\e[36m Write \e[32mhelp \e[36mto list all supported commands.\e[0m\n\r";
        uart_write_bytes(UART_PORT, msg, strlen(msg));
    }
}
