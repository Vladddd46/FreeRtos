#include "header.h"

#define TIMER_DIVIDER         80 //  Hardware timer clock divider
#define TIMER_SCALE           (TIMER_BASE_CLK / TIMER_DIVIDER)  // convert counter value to seconds
#define TIMER_INTERVAL1_SEC   1.0 // how often the interrupt is triggered (1 sec)
#define TEST_WITH_RELOAD      1        
#define DAY_IN_SECONDS        86400

/* @ Digital clock implementation.
 * time_task performs in infinite loop and suspends on QueueReceive until 
 *  interrupt sends notification in queue.
 * Each second interrupt occures and sends notification in queue.
 * After getting notification from interrupt time_task increments global var.
 *  current_time by 1. Current time represents time in seconds.
 * Also time_task converts current_time in hours:minutes:seconds and prints it
 *  on OLED display.
 *
 */



/*
 * Interrupt, that occurs each seconds and sends
 *  notification to time_task.
 */
void IRAM_ATTR timer_interrupt(void *para) {
    timer_spinlock_take(TIMER_GROUP_0);
    int timer_idx = (int) para;

    /* Retrieve the interrupt status and the counter value
       from the timer that reported the interrupt */
    uint32_t timer_intr = timer_group_get_intr_status_in_isr(TIMER_GROUP_0);
    uint64_t current_time_value = timer_group_get_counter_value_in_isr(TIMER_GROUP_0, timer_idx);

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
 * timer_idx - the timer number to initialize
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
    timer_isr_register(TIMER_GROUP_0, timer_idx, timer_interrupt,
                       (void *)timer_idx, ESP_INTR_FLAG_IRAM, NULL);
    timer_start(TIMER_GROUP_0, timer_idx);
}



void print_current_time_on_display(sh1106_t *display) {
    int hours   = current_time / 3600;
    int minutes = (current_time - (3600 * hours)) / 60;
    int seconds = current_time - (3600 * hours) - (60 * minutes);

    char time_buff[70];
    bzero(time_buff, 70);
    sprintf(time_buff, "Time: %d:%d:%d", hours, minutes,seconds);

    sh1106_clear(display);
    print_str_in_line(&display, (char *)time_buff, 3);
    sh1106_update(display);
}



void timer_task(void *arg) {
    timer_initialization(TIMER_1, TIMER_INTERVAL1_SEC);

    // turning on display.
    sh1106_t display;
    sh1106_init(&display);
    sh1106_clear(&display);

    while (1) {
        timer_event_t evt;
        xQueueReceive(timer_queue, &evt, portMAX_DELAY);
        if (evt.type == TEST_WITH_RELOAD) {
            if (current_time == DAY_IN_SECONDS)
                current_time = 0;
            print_current_time_on_display(&display);
            current_time += 1;
        }
    }
}


