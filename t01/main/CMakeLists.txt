idf_component_register(SRCS "t01.c" "led_wrappers.c" "led_pulsing_task.c" "input.c" "execute.c" "led_functions.c" "error_msg.c" "uart_init.c"
                    INCLUDE_DIRS "."
                    REQUIRES wrappers sh1106 libmx)