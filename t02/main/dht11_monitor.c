#include "header.h"

#define QUEUE_SIZE 60


void dht11_monitor() {
	dht11_data_queue = xQueueCreate(QUEUE_SIZE, 70);
	char black_hole[100];
	char *data = NULL;

	while(1) {
		data = get_dht11_data(2, 4, 1);	

		if (uxQueueMessagesWaiting(dht11_data_queue) == QUEUE_SIZE)
			xQueueReceive(dht11_data_queue, (void *)black_hole, (TickType_t)0);

		if (data != NULL)
			xQueueSend(dht11_data_queue,(void *)data, (TickType_t)0);
		vTaskDelay(350);
	}
}
