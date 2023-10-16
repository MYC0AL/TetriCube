#include "Comminucation.h"

Comm::Comm(uart_port_t uart_num) : m_uart_num(uart_num)
{
    SetupComm();
}

comm_error_t Comm::SendStr(const char* str)
{
    comm_error_t ret_val = COMM_SUCCESS;

    if (!uart_write_bytes(m_uart_num, str, strlen(str)))
        ret_val = COMM_ERROR;

    ESP_ERROR_CHECK(uart_wait_tx_done(m_uart_num, UART_WAIT_TICKS));

    return ret_val;
}

comm_error_t Comm::RecvStr(char* buff)
{
    comm_error_t ret_val = COMM_SUCCESS;

    // Read data from UART.
    int buff_length = 0;
    ESP_ERROR_CHECK(uart_get_buffered_data_len(m_uart_num, (size_t*)&buff_length));
    buff_length = uart_read_bytes(m_uart_num, buff, buff_length, UART_WAIT_TICKS);
    
    return ret_val;
}

comm_error_t Comm::SetupComm()
{
    comm_error_t ret_val = COMM_SUCCESS;

    uart_config_t uart_config = {
    .baud_rate = UART_BAUD,
    .data_bits = UART_DATA_8_BITS,
    .parity = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_CTS_RTS,
    .rx_flow_ctrl_thresh = 122,
    };

    // Install UART driver using an event queue here
    ESP_ERROR_CHECK(uart_driver_install(m_uart_num, UART_BUFF_SIZE, UART_BUFF_SIZE, UART_QUEUE_SIZE, &m_uart_queue, 0));

    // Configure UART parameters
    ESP_ERROR_CHECK(uart_param_config(m_uart_num, &uart_config));

    // Set UART 
    ESP_ERROR_CHECK(uart_set_pin(m_uart_num, COMM_TX, COMM_RX, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    // Setup UART mode
    ESP_ERROR_CHECK(uart_set_mode(m_uart_num, UART_MODE_UART));

    // Set a pattern to execute an event when 20 '\0' chars are processed
    uart_enable_pattern_det_baud_intr(m_uart_num, '\0', UART_PATTERN_NUM, 9, 0, 0);
    uart_pattern_queue_reset(m_uart_num, 20);
    
    // Create task to handle UART event from ISR
    //xTaskCreate(uart_event_task, "uart_event_task", 2048, NULL, 12, NULL);
    
    return ret_val;
}
