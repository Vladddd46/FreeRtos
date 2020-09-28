#include "header.h"


void time_cmd(char **cmd) {
	char timeb[70];
	bzero(timeb, 70);
	int h = 0;
    int m = 0;
    int s = 0;

	if (mx_strarr_len(cmd) == 1) {
		h = timer_counter / 3600;
    	m = (timer_counter - (3600 * h)) / 60;
    	s = timer_counter - (3600 * h) - (60 * m);
	}
	else if (mx_strarr_len(cmd) == 2 && !strcmp(cmd[1], "reset")) {
		timer_counter = 0;
		h = timer_counter / 3600;
    	m = (timer_counter - (3600 * h)) / 60;
    	s = timer_counter - (3600 * h) - (60 * m);
	}
	else if (mx_strarr_len(cmd) == 5 && !strcmp(cmd[1], "set")) {
		int h = atoi(cmd[2]);
		int m = atoi(cmd[3]);
		int s = atoi(cmd[4]);
		if ((h == 0 && strcmp("0", cmd[2]) != 0) || (m == 0 && strcmp("0", cmd[3]) != 0) || (s == 0 && strcmp("0", cmd[4]) != 0)) {
			// value error;
			return;
		}
		else if (h > 23 || m > 60 || s > 60) {
			// value error;
			return;
		}
		timer_counter = (h * 3600) + (m * 60) + (s * 60);
	}
	sprintf(timeb, "Time: %d:%d:%d", h,m,s);
	uart_write_bytes(UART_PORT, (const char*)timeb, strlen((const char*)timeb));
	uart_write_bytes(UART_PORT, NEWLINE, strlen(NEWLINE));
}