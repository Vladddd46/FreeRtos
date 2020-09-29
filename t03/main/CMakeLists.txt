idf_component_register(SRCS "t03.c" "led_wrappers.c" "led_pulsing_task.c" "input.c" "execute.c" "led_functions.c" "error_msg.c" "uart_init.c" "help_command.c" "tehu.c" "time_cmd.c" "digital_clock.c"
                    INCLUDE_DIRS "."
                    REQUIRES wrappers sh1106 libmx dht11)