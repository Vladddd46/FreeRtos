#include "header.h"

// Determines type of led cmd.
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



/*
 * Determines the type of program
 * to be executed.
 */
void execute(char **cmd, int len) {
    if (cmd[0] && !strcmp(cmd[0], "led"))
        led_commands(cmd, len);
    else if (cmd[0] && !strcmp(cmd[0], "help"))
        help_command();
    else if (cmd[0] && !strcmp(cmd[0], "tehu")) {
        dht11_log();
    }
    else if (len == 0) {
        // pass
    }
    else 
        error_msg(NO_SUCH_COMMAND);
}

