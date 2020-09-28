#include "header.h"

/* @ This is CLI for board ESP32, which supports commands for led manipulation 
 * and getting data from dht11 sensor.
 * Communication through UART2.
 *
 * Support commands: 
 *  >led on [1-3] 
 *  >led off [1-3]
 *  >led pulse [1-3]
 *  *if led num. is not specified, all leds are pulse/on/off.
 *  >tehu - prints log with temperature and humidity.
 */



void global_variables_init() {
    led1_state = LED_IS_OFF;
    led2_state = LED_IS_OFF;
    led3_state = LED_IS_OFF;

    is_executing = 0;
    global_queue_handle = xQueueCreate(5, COMMAND_LINE_MAX_LENGTH);
}

/*
 * Timer group0 ISR handler
 *
 * Note:
 * We don't call the timer API here because they are not declared with IRAM_ATTR.
 * If we're okay with the timer irq not being serviced while SPI flash cache is disabled,
 * we can allocate this interrupt without the ESP_INTR_FLAG_IRAM flag and use the normal API.
 */
void IRAM_ATTR timer_group0_isr(void *para) {
    timer_spinlock_take(TIMER_GROUP_0);
    int timer_idx = (int) para;

    /* Retrieve the interrupt status and the counter value
       from the timer that reported the interrupt */
    uint32_t timer_intr = timer_group_get_intr_status_in_isr(TIMER_GROUP_0);
    uint64_t timer_counter_value = timer_group_get_counter_value_in_isr(TIMER_GROUP_0, timer_idx);

    /* Prepare basic event data
       that will be then sent back to the main program task */
    timer_event_t evt;


    if (timer_intr & TIMER_INTR_T1) {
        evt.type = TEST_WITH_RELOAD;
        timer_group_clr_intr_status_in_isr(TIMER_GROUP_0, TIMER_1);
    } else
        evt.type = -1; // not supported even type

    /* After the alarm has been triggered
      we need enable it again, so it is triggered the next time */
    timer_group_enable_alarm_in_isr(TIMER_GROUP_0, timer_idx);

    /* Now just send the event data back to the main program task */
    xQueueSendFromISR(timer_queue, &evt, NULL);
    timer_spinlock_give(TIMER_GROUP_0);
}

/*
 * The main task of this example program
 */
static void timer_example_evt_task(void *arg) {
    timer_counter = 0;
    sh1106_t display;
    sh1106_init(&display);
    sh1106_clear(&display);

    sh1106_t *display1 = &display;
    
    char timeb[70];

    while (1) {
        timer_event_t evt;
        xQueueReceive(timer_queue, &evt, portMAX_DELAY);

        if (evt.type == TEST_WITH_RELOAD) {
            bzero(timeb, 70);
            char *x = timeb;
            if (timer_counter == 86400)
                timer_counter = 0;
            int h = timer_counter / 3600;
            int m = (timer_counter - (3600 * h)) / 60;
            int s = timer_counter - (3600 * h) - (60 * m);
            sh1106_clear(&display);
            sprintf(timeb, "Time: %d:%d:%d", h,m,s);
            print_str_in_line(&display1, x, 3);
            sh1106_update(&display);
            printf("\n %d\n", timer_counter);
            timer_counter += 1;
            if (timer_counter % 60 == 0)
                printf("%d\n", timer_counter/60);

        } 
    }
}

/*
 * Initialize selected timer of the timer group 0
 *
 * timer_idx - the timer number to initialize
 * auto_reload - should the timer auto reload on alarm?
 * timer_interval_sec - the interval of alarm to set
 */
static void timer_initialization(int timer_idx, double timer_interval_sec) {
    timer_config_t config = {
        .divider     = TIMER_DIVIDER,
        .counter_dir = TIMER_COUNT_UP,
        .counter_en  = TIMER_PAUSE,
        .alarm_en    = TIMER_ALARM_EN,
        .auto_reload = true,
    };
    timer_init(TIMER_GROUP_0, timer_idx, &config);

    /* Timer's counter will initially start from value below.
       Also, if auto_reload is set, this value will be automatically reload on alarm */
    timer_set_counter_value(TIMER_GROUP_0, timer_idx, 0x00000000ULL);

    /* Configure the alarm value and the interrupt on alarm. */
    timer_set_alarm_value(TIMER_GROUP_0, timer_idx, timer_interval_sec * TIMER_SCALE);
    timer_enable_intr(TIMER_GROUP_0, timer_idx);
    timer_isr_register(TIMER_GROUP_0, timer_idx, timer_group0_isr,
                       (void *) timer_idx, ESP_INTR_FLAG_IRAM, NULL);

    timer_start(TIMER_GROUP_0, timer_idx);
}



void app_main() {
    gpio_set_direction(32, GPIO_MODE_OUTPUT);
    gpio_set_level(32, 1);

    global_variables_init();
    uart_init(9600);
    timer_queue = xQueueCreate(10, sizeof(timer_event_t));
    timer_initialization(TIMER_1, TIMER_INTERVAL1_SEC);


    init_i2c_driver();
    

    /*
     * Three tasks are running constantly.
     * user_input - gets user`s input from UART2 and sends it  to cmd_handler
     *  task through queue global_queue_handle.
     * cmd_handler - receives user`s input as string from user_input task
     *  through queue. Splits received string in array and determines which
     *  command to execute.
     * dht11_monitor - monitors dht11 temperature/humidity sensor every 5 seconds and
     *  sends got data in Queue(dht11_data_queue)
     * PS. user_input and cmd_handler are declared in input.c
     */
    xTaskCreate(user_input,    "user_input",    12040, NULL, 10, NULL);
    xTaskCreate(cmd_handler,   "cmd_handler",   12040, NULL, 10, NULL);
    xTaskCreate(dht11_monitor, "dht11_monitor", 12040, NULL, 10, &xTaskWeather);
    xTaskCreate(timer_example_evt_task, "timer_evt_task", 12040, NULL, 5, NULL);
}
