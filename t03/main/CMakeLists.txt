idf_component_register(SRCS "t03.c" "led_pulsing_task.c" "input.c" "execute.c" "led_pulse_command.c" "uart_init.c" "help_command.c" "tehu.c" "time_command.c" "digital_clock.c" "led_on.c" "led_off.c"
                    INCLUDE_DIRS "."
                    REQUIRES wrappers sh1106 libmx dht11)