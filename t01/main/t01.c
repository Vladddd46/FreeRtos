#include "header.h"

/*
 * This is CLI for board ESP32, which supports commands for led manipulation.
 * Communication through UART2.
 * Support commands: 
 *  >led on [1-3] 
 *  >led off [1-3]
 *  >led pulse [1-3]
 * *if led num. is not specified, all leds are pulse/on/off.
 */



void global_variables_init() {
    led1_is_pulsing = 0;
    led2_is_pulsing = 0;
    led3_is_pulsing = 0;

    global_queue_handle = xQueueCreate(5, COMMAND_LINE_MAX_LENGTH);
}



void app_main() {
    global_variables_init();
    uart_init(9600);


    /*
     * Two tasks are running constantly.
     * user_input - gets user`s input from UART2 and sends it  to cmd_handler
     *  task through queue global_queue_handle.
     * cmd_handler - receives user`s input as string from user_input task
     *  through queue. Splits received string in array and determines which
     *  command to execute.
     * PS. user_input and cmd_handler are declared in input.c
     */
    xTaskCreate(user_input,  "user_input",  4040, NULL, 10, NULL);
    xTaskCreate(cmd_handler, "cmd_handler", 4040, NULL, 10, NULL);
}
