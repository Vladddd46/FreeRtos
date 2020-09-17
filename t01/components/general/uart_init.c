#include "general.h"

#define CMD_MAX_LENGTH 1024
#define UART_TX_PIN 17
#define UART_RX_PIN 16
#define UART UART_NUM_1

void uart_init(int baud_rate) {
    uart_config_t uart_config = {
        .baud_rate = baud_rate,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    uart_param_config(UART, &uart_config);
    uart_set_pin(UART, UART_TX_PIN, UART_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(UART, CMD_MAX_LENGTH * 2, 0, 0, NULL, 0);
}
