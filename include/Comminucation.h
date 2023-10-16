#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "esp_log.h"
#include <cstring>

#define COMM_TX 20
#define COMM_RTS 19
#define COMM_CTS 18
#define COMM_RX 17

#define UART_BUFF_SIZE (1024 * 2)
#define UART_QUEUE_SIZE 10

#define UART_BAUD 115200
#define UART_WAIT_TICKS 100
#define UART_PATTERN_NUM 3

enum comm_error_t {COMM_SUCCESS, COMM_ERROR};

class Comm
{
public:
    Comm(uart_port_t uart_num);

    comm_error_t SendStr(const char* str);
    comm_error_t RecvStr(char* buff);

private: 
    comm_error_t SetupComm();

    uart_port_t m_uart_num;
    QueueHandle_t m_uart_queue;

};

#endif
