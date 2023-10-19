#include "External_link.h"

ExternalLink::ExternalLink()
{
    Serial1.begin(UART_BAUD, SERIAL_8N1, -1, EL0_TX);
    log_printf("External Link 0 Started\n");

    Serial2.begin(UART_BAUD, SERIAL_8N1, EL0_RX, -1);
    log_printf("External Link 1 Started\n");

    el_error_t sendret = SendStr("this is a test\n");
    log_printf("%d\n",sendret);
    while (ListenForStr() == EL_ERROR);
    GetStr();
}

el_error_t ExternalLink::SendStr(const char* str)
{
    el_error_t retCode = EL_ERROR;

    if (strlen(str) != 0)
    {
        log_printf("strlen\n");
        if(Serial1.write(str) > 0)
        {
            log_printf("serial.write\n");
            retCode = EL_SUCCESS;
        }
    }
    log_printf("return from sendstr\n");
    return retCode;
}

el_error_t ExternalLink::ListenForStr()
{
    el_error_t retCode = EL_ERROR;
    
    int buff_idx = 0;
    bool doneRecv = false;

    char newChar;
    std::string tempBuff;

    while (Serial2.available() > 0 && !doneRecv)
    {
        log_printf("new char\n");
        newChar = Serial2.read();

        if (newChar != UART_EOL)
            tempBuff += newChar;
        
        else if (newChar == UART_EOL)
        {
            doneRecv = true;
            log_printf("is eol\n");

            // Recieved the terminating EOL char
            retCode = EL_SUCCESS;
        }
    }

    if (retCode == EL_SUCCESS)
    {
        m_last_read_str = tempBuff;



        log_printf("%s\n",m_last_read_str.c_str());

    }

    return retCode;
}

std::string ExternalLink::GetStr()
{
    return m_last_read_str;
}

void ExternalLink::UpdateLog(const char *str, int status)
{
    log_printf("EL: %s returned %s\n",str, status);
}