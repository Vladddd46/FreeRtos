#include "header.h"

/* @time command
 * This file implements time commands functionality.
 * Current time is stored in global var. timer_counter in seconds.
 * User should set current time, when he/she turns on the board.
 * time - prints current time, stored in timer_counter in console.
 * time reset - sets timer_counter to 0.
 * timer set hh mm ss - converts hh(hours) mm(minutes) ss(seconds) 
 *  in seconds and sets timer_counter.
 */



static void print_time_in_console(int hours, int minutes, int seconds) {
	char timeb[70];
	bzero(timeb, 70);
	sprintf(timeb, "Time: %d:%d:%d", hours, minutes, seconds);
	uart_write_bytes(UART_PORT, (const char*)timeb, strlen((const char*)timeb));
	uart_write_bytes(UART_PORT, NEWLINE, strlen(NEWLINE));
}



static void print_current_time() {
	int h = timer_counter / 3600;
    int m = (timer_counter - (3600 * h)) / 60;
    int s = timer_counter - (3600 * h) - (60 * m);
    print_time_in_console(h, m, s);
}



static void reset_time() {
    timer_counter = 0;
	int h = timer_counter / 3600;
    int m = (timer_counter - (3600 * h)) / 60;
    int s = timer_counter - (3600 * h) - (60 * m);
    print_time_in_console(h, m, s);
}



static void usage_error() {
    char *red_color     = "\e[31m";
    char *default_color = "\e[0m";
    char *msg           = "usage: time [reset; set hh mm ss]";
    uart_write_bytes(UART_PORT, (const char*)red_color, strlen((const char*)red_color));
    uart_write_bytes(UART_PORT, (const char*)msg, strlen((const char*)msg));
    uart_write_bytes(UART_PORT, (const char*)default_color, strlen((const char*)default_color));
	uart_write_bytes(UART_PORT, NEWLINE, strlen(NEWLINE));
}



static void value_error() {
	char *red_color     = "\e[31m";
    char *default_color = "\e[0m";
    char *msg = "value error: -1 < hh < 24; -1 < mm < 60; -1 < ss < 60";
    uart_write_bytes(UART_PORT, (const char*)red_color, strlen((const char*)red_color));
    uart_write_bytes(UART_PORT, (const char*)msg, strlen((const char*)msg));
    uart_write_bytes(UART_PORT, (const char*)default_color, strlen((const char*)default_color));
	uart_write_bytes(UART_PORT, NEWLINE, strlen(NEWLINE));
}



static void set_time(char **cmd) {
	int h = atoi(cmd[2]);
    int m = atoi(cmd[3]);
    int s = atoi(cmd[4]);
    if ((h == 0 && strcmp("0", cmd[2]) != 0) 
    	|| (m == 0 && strcmp("0", cmd[3]) != 0)
    	 || (s == 0 && strcmp("0", cmd[4]) != 0))
		value_error();
	else if (h > 23 || m > 60 || s > 60)
		value_error();
	else {
		timer_counter = (h * 3600) + (m * 60) + s;
		print_time_in_console(h, m, s);
	}
}



void time_cmd(char **cmd) {
	if (mx_strarr_len(cmd) == 1)
		print_current_time();
	else if (mx_strarr_len(cmd) == 2 && !strcmp(cmd[1], "reset"))
		reset_time();
	else if (mx_strarr_len(cmd) == 5 && !strcmp(cmd[1], "set"))
		set_time(cmd);
	else
		usage_error();
}


