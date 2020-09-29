#include "header.h"

/*
 * Determines the type of command
 * to be executed.
 */



static void no_such_command_error() {
    char *msg = "\e[31mNo such command.\e[36mWrite \e[32mhelp \e[36mto list all supported commands.\e[0m\n\r";
    uart_write_bytes(UART_PORT, msg, strlen(msg));
}



// Determines type of led cmd.
static void led_commands(char **cmd, int len) {
    if (cmd[1] && !strcmp(cmd[1], "on"))
        led_on(cmd, len);
    else if  (cmd[1] && !strcmp(cmd[1], "off"))
        led_off(cmd, len);
    else if  (cmd[1] && !strcmp(cmd[1], "pulse"))
        led_pulse(cmd, len);
    else
        no_such_command_error();
}



void execute(char **cmd, int len) {
    if (cmd[0] && !strcmp(cmd[0], "led"))
        led_commands(cmd, len);
    else if (cmd[0] && !strcmp(cmd[0], "help"))
        help_command();
    else if (cmd[0] && !strcmp(cmd[0], "tehu"))
        tehu(cmd);
    else if (cmd[0] && !strcmp(cmd[0], "time"))
        time_command(cmd);
    else if (len == 0) {
        // pass
    }
    else 
        no_such_command_error();
    const char *prompt = "Enter your command : ";
    uart_write_bytes(UART_PORT, prompt, strlen(prompt));
}

