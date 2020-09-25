#include "header.h"



void dht11_monitor() {
	dht11_data_queue = xQueueCreate(60, 70);

	char *data = NULL;
	while(1) {
		char *data = get_dht11_data(2, 4, 1);
		if (data != NULL)
			xQueueSend(dht11_data_queue,(void *)data, (TickType_t)0);
		vTaskDelay(1);
	}
}
