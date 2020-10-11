#include "header.h"



static void print_current_time_on_display(sh1106_t *display) {
    int hours   = current_time / 3600;
    int minutes = (current_time - (3600 * hours)) / 60;
    int seconds = current_time - (3600 * hours) - (60 * minutes);

    char hours_str[30];
    bzero(hours_str, 30);
    if (hours < 10) sprintf(hours_str, "0%d", hours);
    else            sprintf(hours_str, "%d", hours);
    char minutes_str[30];
    bzero(minutes_str, 30);
    if (minutes < 10) sprintf(minutes_str, "0%d", minutes);
    else              sprintf(minutes_str, "%d", minutes);
    char seconds_str[30];
    bzero(seconds_str, 30);
    if (seconds < 10) sprintf(seconds_str, "0%d", seconds);
    else              sprintf(seconds_str, "%d", seconds);
    char time_buff[100];
    bzero(time_buff, 100);
    sprintf(time_buff, "%s:%s:%s", hours_str, minutes_str, seconds_str);

    screen_print(&display, (char *)time_buff, 1, 15, 2);
}

char **str_split(char *str) {
	char **cmd = mx_strarr_new(100);
	// splitting str into arr.
    int index = 0;
    char *p;
    p = strtok(str, " ");
    cmd[index] = p;
	index++;
	while(p != NULL && index < 100) {
		p = strtok(NULL, " ");
		cmd[index] = p;
		index++;
	}
	return cmd;
}

void oled_view_task() {
    sh1106_t display;
    sh1106_t *display1 = &display;
    sh1106_init(&display);
    sh1106_clear(&display);

    char buff[100];
   	char temperature_buff[100];
   	char humidity_buff[100];
  	char *temperature = "wait";
  	char *humidity = "wait";
   	char **res;

    while(1) {
    	bzero(buff, 100);
    	sh1106_clear(&display);
    	xQueueReceive(dht11_current_queue, buff, (TickType_t)0);
    	if (strlen(buff) != 0) {
    		res = str_split((char *)buff);
    		if (strcmp(temperature, "wait") != 0 && strcmp(humidity, "wait") != 0) {
    			free(temperature);
    			free(humidity);
    		}
    		temperature = mx_string_copy(res[1]);
    		humidity    = mx_string_copy(res[4]);
            free(res);
    	}
    	sprintf(temperature_buff, "temperature = %s C", temperature);
    	sprintf(humidity_buff, "humidity    = %s", humidity);
    	screen_print(&display1, temperature_buff, 4, 15, 1);
    	screen_print(&display1, humidity_buff, 5, 15, 1);
        print_current_time_on_display(&display);
        sh1106_update(&display);
    } 
}
