#ifndef COMMUNICATION_H
#define COMMUNICATION_H

// #include <stdio.h>
// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"
// #include "driver/uart.h"
// #include "driver/gpio.h"
// #include "sdkconfig.h"
// #include "esp_log.h"
#include <cstring>
#include "Arduino.h"

#define EL0_TX 20
#define EL0_RX 19

#define EL1_TX 18
#define EL1_RX 17

#define UART_BUFF_SIZE 32
#define UART_QUEUE_SIZE 10

#define UART_BAUD 115200
#define UART_WAIT_TICKS 100
#define UART_PATTERN_NUM 3

#define UART_EOL '\n'
#define EMPTY_STR "\0"

enum el_error_t {EL_SUCCESS, EL_ERROR};

class ExternalLink
{
public:
    ExternalLink();

    el_error_t SendStr(std::string str);
    el_error_t ListenForStr();
    std::string PopLastReadStr();

private: 

    void UpdateLog(const char* str, int status);

    std::string m_last_read_str;
};

#endif
