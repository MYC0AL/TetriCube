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

#define UART_EOL '*'
#define EMPTY_STR "\0"

#define EXE_CMD "$"
#define ABORT_CMD "!"
#define CMD_TIMEOUT 1000

#define RETRY_COUNT 5

enum el_error_t {EL_SUCCESS, EL_ERROR};

enum el_state_t {EL_CMD_WAIT, EL_CMD_RECEIVED, EL_CMD_SENT, EL_CMD_SUCCESS, EL_CMD_ABORT};

class ExternalLink
{
public:
    ExternalLink();
    el_error_t StateController();

    el_error_t RequestState(el_state_t new_state);

    el_error_t SendCMD(std::string cmd);
    el_error_t ListenForCMD();
    std::string PopLastReadCMD();

    std::string GetCMD();
private: 

    el_state_t m_state;
    el_error_t EchoCMD(std::string cmd);

    void UpdateLog(const char* str, int status);

    std::string m_last_read_str;
    std::string m_loaded_cmd;
    bool m_cmd_ready;
    bool m_setup_state;

};

#endif