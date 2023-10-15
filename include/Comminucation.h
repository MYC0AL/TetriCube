#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "esp_log.h"

#define COMM_TX 20
#define IO19 19
#define IO18 18
#define COMM_RX 17

enum comm_error_t {COMM_SUCCESS, COMM_ERROR};

comm_error_t InitComm();

#endif
