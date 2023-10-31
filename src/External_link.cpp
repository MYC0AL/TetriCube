#include "External_link.h"

ExternalLink::ExternalLink()
{
    Serial1.begin(UART_BAUD, SERIAL_8N1, -1, EL0_TX);
    log_printf("External Link 0 Started\n");

    Serial2.begin(UART_BAUD, SERIAL_8N1, EL0_RX, -1);
    log_printf("External Link 1 Started\n");

    delay(100);

    // POC DEBUG
    // el_error_t sendret = SendStr("L\n");
    // log_printf("%d\n",sendret);
    // while (ListenForStr() == EL_ERROR);
    // PopLastReadStr();
}

el_error_t ExternalLink::SendStr(std::string str)
{
    el_error_t retCode = EL_ERROR;

    if (!str.empty())
    {
        std::string tempStr = str;
        tempStr += UART_EOL; // Add the TC-specific EL termination char

        if(Serial1.write(tempStr.c_str()) > 0)
        {
            retCode = EL_SUCCESS;
        }
    }

    // Delay to allow for transmission
    delay(20);

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