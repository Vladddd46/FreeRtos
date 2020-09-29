#include "header.h"



static void error_type_print(int err) {
    char *msg       = NULL;
    char *red_color = "\e[31m";

    if (err == INVALID_ARGUMENT)
        msg = "invalid led number |";
    else if (err == WRONG_SYNTAX_LED_ON_OFF || err == WRONG_SYNTAX_PULSE)
        msg = "wrong syntax |";
    else if (err == LED_BUSY)
        msg = "led is busy |";
    else if (err == NO_SUCH_COMMAND)
        msg = "No such command |";
    else if (err == WRONG_FREQUENCY_VALUE)
        msg = "wrong frequency value |";

    if (msg) {
        uart_write_bytes(UART_PORT, red_color, strlen(red_color));
        uart_write_bytes(UART_PORT, msg, strlen(msg));
    }
}



void error_msg(int err) {
    error_type_print(err);
    char *msg = NULL;

    if (err == INVALID_ARGUMENT)
        msg = "\e[33m led on/off [1-3]\e[0m\n\r";
    else if (err == WRONG_SYNTAX_LED_ON_OFF)
        msg = "\e[33m led on/off [1-3]\e[0m\n\r";
    else if (err == LED_BUSY) {
        msg = "\e[33m led[s] you are trying to turn on is[are] busy.\n\rTurn off your led in order\
to do manipulations with it.\e[33msyntax: led off led_number(1-3)\e[0m\n\r";
    }
    else if (err == NO_SUCH_COMMAND)
        msg = "\e[36m Write \e[32mhelp \e[36mto list all supported commands.\e[0m\n\r";
    else if (err == WRONG_SYNTAX_PULSE)
        msg = "\e[33m led pulse [1-3] [f=x.y](frequency; 0 <= x <= 2, 0 <= y <= 9; optional; by default f=1)\e[0m\n\r";
    else if (err == WRONG_FREQUENCY_VALUE)
        msg = "\e[33m f must be >= 0.0 and <= 2.0\e[0m\n\r";

    if (msg) uart_write_bytes(UART_PORT, msg, strlen(msg));
}
