#include "External_link.h"
// static void echo_task(void *arg)
// {
//     uart_config_t uart_config = {
//         .baud_rate = UART_BAUD,
//         .data_bits = UART_DATA_8_BITS,
//         .parity = UART_PARITY_DISABLE,
//         .stop_bits = UART_STOP_BITS_1,
//         .flow_ctrl = UART_HW_FLOWCTRL_DISABLE, //UART_HW_FLOWCTRL_CTS_RTS
//         .rx_flow_ctrl_thresh = 122,
//     };

//     // Install UART driver using an event queue here
//     ESP_ERROR_CHECK(uart_driver_install(0, UART_BUFF_SIZE*2, 0, 0, nullptr, 0));

//     // Configure UART parameters
//     ESP_ERROR_CHECK(uart_param_config(0, &uart_config));

//     // Set UART 
//     ESP_ERROR_CHECK(uart_set_pin(0, COMM_TX, COMM_RX, COMM_RTS, COMM_CTS));

//     uint8_t *data = (uint8_t *) malloc(UART_BUFF_SIZE);

//     while (1) {
//         // Read data from the UART
//         int len = uart_read_bytes(0, data, (UART_BUFF_SIZE - 1), 20 / portTICK_PERIOD_MS);
//         // Write data back to the UART
//         uart_write_bytes(0, (const char *) data, len);
//         if (len) {
//             data[len] = '\0';
//             ESP_LOGI("UART TEST", "Recv str: %s", (char *) data);
//         }
//     }
// }

// Comm::Comm(uart_port_t uart_num, int gpio_num) : m_uart_num(uart_num), m_gpio_num(gpio_num)
// {
//     //SetupComm();
//     xTaskCreate(echo_task, "uart_echo_task", 2048, NULL, 10, NULL);
// }

// comm_error_t Comm::SendStr(const char* str)
// {
//     comm_error_t ret_val = COMM_SUCCESS;

//     if (!uart_write_bytes(m_uart_num, str, strlen(str)))
//         ret_val = COMM_ERROR;

//     ESP_ERROR_CHECK(uart_wait_tx_done(m_uart_num, UART_WAIT_TICKS));

//     return ret_val;
// }

// comm_error_t Comm::RecvStr(char* buff)
// {
//     comm_error_t ret_val = COMM_SUCCESS;

//     // Read data from UART.
//     int buff_length = 0;
//     ESP_ERROR_CHECK(uart_get_buffered_data_len(m_uart_num, (size_t*)&buff_length));
//     buff_length = uart_read_bytes(m_uart_num, buff, buff_length, UART_WAIT_TICKS);
    
//     return ret_val;
// }

// comm_error_t Comm::SetupComm()
// {
//     comm_error_t ret_val = COMM_SUCCESS;

//     uart_config_t uart_config = {
//     .baud_rate = UART_BAUD,
//     .data_bits = UART_DATA_8_BITS,
//     .parity = UART_PARITY_DISABLE,
//     .stop_bits = UART_STOP_BITS_1,
//     .flow_ctrl = UART_HW_FLOWCTRL_DISABLE, //UART_HW_FLOWCTRL_CTS_RTS
//     .rx_flow_ctrl_thresh = 122,
//     };

//     // Install UART driver using an event queue here
//     ESP_ERROR_CHECK(uart_driver_install(m_uart_num, UART_BUFF_SIZE*2, 0, 0, nullptr, 0));

//     // Configure UART parameters
//     ESP_ERROR_CHECK(uart_param_config(m_uart_num, &uart_config));

//     // Set UART 
//     ESP_ERROR_CHECK(uart_set_pin(m_uart_num, COMM_TX, COMM_RX, COMM_RTS, COMM_CTS));

//     // // Setup UART mode
//     //ESP_ERROR_CHECK(uart_set_mode(m_uart_num, UART_MODE_UART));

//     // // Set a pattern to execute an event when 20 '\0' chars are processed
//     // uart_enable_pattern_det_baud_intr(m_uart_num, '\0', UART_PATTERN_NUM, 9, 0, 0);
//     // uart_pattern_queue_reset(m_uart_num, 20);
    
//     // Create task to handle UART event from ISR
//     //xTaskCreate(uart_event_task, "uart_event_task", 2048, NULL, 12, NULL);
    
//      // Configure a temporary buffer for the incoming data
//     uint8_t *data = (uint8_t *) malloc(UART_BUFF_SIZE);

//     while (1) {
//         // Read data from the UART
//         int len = uart_read_bytes(m_uart_num, data, (UART_BUFF_SIZE - 1), 20 / portTICK_PERIOD_MS);
//         // Write data back to the UART
//         uart_write_bytes(m_uart_num, (const char *) data, len);
//         if (len) {
//             data[len] = '\0';
//             ESP_LOGI("UART TEST", "Recv str: %s", (char *) data);
//         }
//     }

//     return ret_val;
// }

void el_rx_interrupt()
{

}

ExternalLink::ExternalLink()
{
    //Serial1.begin(UART_BAUD, EL0_TX, EL0_RX);
    Serial0.begin(UART_BAUD, EL1_TX, EL1_RX);
}

el_error_t ExternalLink::SendStr(const char* str)
{
    el_error_t retCode = EL_ERROR;

    if (strlen(str) != 0 && Serial2.write(str) > 0)
    {
        retCode = EL_SUCCESS;
    }

    return retCode;
}

el_error_t ExternalLink::ListenForStr()
{
    el_error_t retCode = EL_ERROR;

    char newChar;
    
    int buff_idx = 0;

    char tempBuff[UART_BUFF_SIZE];

    while (Serial1.available() > 0)
    {
        newChar = Serial1.read();

        if (newChar != UART_EOL && buff_idx < UART_BUFF_SIZE-1)
        {
            tempBuff[buff_idx++] = newChar;

            if (buff_idx >= UART_BUFF_SIZE)
            {
                // If idx overflow, wrap incoming bits to the front
                buff_idx = 0;
            }
        }
        else if (newChar == UART_EOL)
        {
            // Recieved the terminating EOL char
            retCode = EL_SUCCESS;

            tempBuff[buff_idx] = '\0';
        }
    }

    if (retCode == EL_SUCCESS)
    {
        m_last_read_str = tempBuff;
    }

    return retCode;
}

char* ExternalLink::GetStr()
{
    return m_last_read_str;
}