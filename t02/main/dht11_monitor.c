#include "header.h"

#define DHT11_QUEUE_SIZE 60
#define ELEMENT_SIZE     70
#define DHT11_POWER      2
#define DHT11_DATA       4

/*
 * Gets data from  dht11 temperature/humidity sensor each 5 seconds.
 * Sends got data into dht11_data_queue with size of 60 elements.
 * If dht11_data_queue is already full with 60 elements, 
 * the last element recieved in black_hole.
 */
void dht11_monitor() {
    dht11_data_queue = xQueueCreate(DHT11_QUEUE_SIZE, ELEMENT_SIZE);
    char black_hole[100];
    char *data = NULL;

    while(1) {
        data = get_dht11_data(DHT11_POWER, DHT11_DATA); 
        if (uxQueueMessagesWaiting(dht11_data_queue) == DHT11_QUEUE_SIZE)
            xQueueReceive(dht11_data_queue, (void *)black_hole, (TickType_t)0);
        if (data != NULL)
            xQueueSend(dht11_data_queue,(void *)data, (TickType_t)0);
        vTaskDelay(350);
    }
}
