idf_component_register(SRCS "t01.c" "led_wrappers.c"
                    INCLUDE_DIRS "."
                    REQUIRES wrappers sh1106 libmx general)