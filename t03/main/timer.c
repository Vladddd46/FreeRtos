#include "header.h"



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
 * Initialize selected timer of the timer group 0
 *
 * timer_idx - the timer number to initialize
 * auto_reload - should the timer auto reload on alarm?
 * timer_interval_sec - the interval of alarm to set
 */
void timer_initialization(int timer_idx, double timer_interval_sec) {
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



/*
 * The main task of this example program
 */
void timer_task(void *arg) {
    gpio_set_direction(32, GPIO_MODE_OUTPUT);
    gpio_set_level(32, 1);

    timer_queue = xQueueCreate(10, sizeof(timer_event_t));
    timer_initialization(TIMER_1, TIMER_INTERVAL1_SEC);

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


