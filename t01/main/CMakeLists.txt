idf_component_register(SRCS "t01.c" "led_wrappers.c" "led_pulsing_tasks.c" "input.c" "execute.c" "led_functions.c" "error_msg.c"
                    INCLUDE_DIRS "."
                    REQUIRES wrappers sh1106 libmx general)