#include "External_link.h"

ExternalLink::ExternalLink()
{
    Serial1.begin(UART_BAUD, SERIAL_8N1, -1, EL0_TX);
    log_printf("External Link 0 Started\n");

    Serial2.begin(UART_BAUD, SERIAL_8N1, EL0_RX, -1);
    log_printf("External Link 1 Started\n");

    delay(100);

    el_error_t sendret = SendStr("L\n");
    log_printf("%d\n",sendret);
    while (ListenForStr() == EL_ERROR);
    PopLastReadStr();
}

el_error_t ExternalLink::SendStr(const char* str)
{
    el_error_t retCode = EL_ERROR;

    if (strlen(str) != 0)
    {
        if(Serial1.write(str) > 0)
        {
            retCode = EL_SUCCESS;
        }
    }
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
        newChar = Serial2.read();

        if (newChar != UART_EOL)
            tempBuff += newChar;
        
        else if (newChar == UART_EOL)
        {
            doneRecv = true;

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

std::string ExternalLink::PopLastReadStr()
{
    std::string tempStr = m_last_read_str;
    m_last_read_str = EMPTY_STR;
    return tempStr;
}

void ExternalLink::UpdateLog(const char *str, int status)
{
    log_printf("EL: %s returned %s\n",str, status);
}