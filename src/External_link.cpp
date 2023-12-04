#include "External_link.h"

ExternalLink::ExternalLink() : m_state(EL_CMD_WAIT), m_setup_state(true), m_cmd_ready(false)
{
    Serial1.begin(UART_BAUD, SERIAL_8N1, -1, EL0_TX);
    log_printf("External Link 0 Started\n");

    Serial2.begin(UART_BAUD, SERIAL_8N1, EL0_RX, -1);
    log_printf("External Link 1 Started\n");

    delay(100);

    // POC DEBUG
    // el_error_t sendret = SendCMD("L\n");
    // log_printf("%d\n",sendret);
    // while (ListenForCMD() == EL_ERROR);
    // PopLastReadCMD();
}

el_error_t ExternalLink::RequestState(el_state_t new_state)
{
    el_error_t ret_val = EL_ERROR;

    switch(m_state)
    {
        case EL_CMD_WAIT:
            if (new_state == EL_CMD_SENT || new_state == EL_CMD_RECEIVED)
                ret_val = EL_SUCCESS;
            break;

        case EL_CMD_RECEIVED:
            if (new_state == EL_CMD_WAIT)
                ret_val = EL_SUCCESS;
            break;

        case EL_CMD_SENT:
            if (new_state == EL_CMD_ABORT || new_state == EL_CMD_SUCCESS)
                ret_val = EL_SUCCESS;
            break;

        case EL_CMD_SUCCESS:
            if (new_state == EL_CMD_WAIT)
                ret_val = EL_SUCCESS;
        break;

        case EL_CMD_ABORT:
            if (new_state == EL_CMD_WAIT)
                ret_val = EL_SUCCESS;
            break;
    }

    if (ret_val == EL_SUCCESS)
    {
        m_state = new_state;
        m_setup_state = true;

        //DEBUG
        log_printf("EL: State transition to: %d\n\r",m_state);
    }

    return ret_val;
}

el_error_t ExternalLink::SendCMD(std::string cmd)
{
    el_error_t retCode = EL_ERROR;

    //DEBUG
    log_printf("EL: Attempting to send CMD: %s\r\n",cmd.c_str());

    // If the Tx string is not empty and EL in WAIT
    if (!cmd.empty() && m_state == EL_CMD_WAIT)
    {
        std::string tempStr = cmd;
        tempStr += UART_EOL; // Add the TC-specific EL termination char

        if(Serial1.write(tempStr.c_str()) > 0)
        {
            retCode = EL_SUCCESS;
            
            // Save sent CMD
            m_loaded_cmd = cmd;

            //DEBUG
            log_printf("EL: Sent CMD: %s\n\r",m_loaded_cmd.c_str());
            log_printf("EL: WAIT to SENT\n\r");

            //Request transition to SENT
            RequestState(EL_CMD_SENT);
        }
    }

    return retCode;
}

el_error_t ExternalLink::ListenForCMD()
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

        log_printf("EL: CMD Received: %s\n",m_last_read_str.c_str());
    }

    return retCode;
}

std::string ExternalLink::PopLastReadCMD()
{
    std::string tempStr = m_last_read_str;
    m_last_read_str = EMPTY_STR;

    //DEBUG
    log_printf("EL: Popped last read CMD: %s\n\r",tempStr.c_str());

    return tempStr;
}

std::string ExternalLink::GetCMD()
{
    std::string ret_cmd;
    if (m_cmd_ready)
    {
        ret_cmd = m_loaded_cmd;
        
        //DEBUG
        log_printf("EL: CMD Ready: %s\n\r",ret_cmd.c_str());
    }
    return ret_cmd;
}

el_error_t ExternalLink::StateController()
{
    switch(m_state)
    {
        case EL_CMD_WAIT:
            if (m_setup_state) {
                m_loaded_cmd = std::string();
                m_cmd_ready = false;
                m_setup_state = false;

                //DEBUG
                log_printf("EL: WAIT\n\r");
            }

            // Listen for received CMD
            if (ListenForCMD() == EL_SUCCESS)
            {
                // With 2 displays, delay or else will break
                delay(10);

                //DEBUG
                log_printf("EL: In WAIT Received CMD: %s\n\r",m_last_read_str.c_str());

                //Echo CMD to next EL
                m_loaded_cmd = PopLastReadCMD();
                if (!m_loaded_cmd.empty()){
                    EchoCMD(m_loaded_cmd);
                }

                //DEBUG
                log_printf("EL: WAIT to RECEIVED\n\r");

                RequestState(EL_CMD_RECEIVED);
            }
            break;

        case EL_CMD_RECEIVED:
            if (m_setup_state)
            {
                //DEBUG
                log_printf("EL: State: RECEIVED\n\r");

                m_setup_state = false;

            }
            if (ListenForCMD() == EL_SUCCESS)
            {
                std::string cmd = PopLastReadCMD();

                //DEBUG
                log_printf("EL: EL_CMD_RECEIVED: cmd: %s\n\r",cmd.c_str());

                if (cmd == ABORT_CMD)
                {
                    // Echo ABORT_CMD and go to EL_CMD_WAIT
                    EchoCMD(ABORT_CMD);
                    RequestState(EL_CMD_WAIT);
                }
                else if (cmd == EXE_CMD)
                {
                    //Echo EXE_CMD
                    EchoCMD(EXE_CMD);
                    
                    // Set execution flag
                    m_cmd_ready = true;

                    RequestState(EL_CMD_WAIT);
                }
            }
            break;

        case EL_CMD_SENT:
        {
            if (ListenForCMD() == EL_SUCCESS)
            {
                std::string cmd = PopLastReadCMD();

                //DEBUG
                log_printf("EL: In EL_CMD_SENT comparing: %s to %s\n\r",cmd.c_str(),m_loaded_cmd.c_str());

                if (cmd == m_loaded_cmd)
                {
                    // Correct CMD received
                    RequestState(EL_CMD_SUCCESS);
                }
                else
                {
                    // Incorrect CMD received
                    RequestState(EL_CMD_ABORT);
                }
            }
        }
        break;

        case EL_CMD_SUCCESS:
        {
            //DEBUG
            log_printf("EL: State: EL_CMD_SUCCESS\n\r");

            // Send EXE_CMD to all ELs
            EchoCMD(EXE_CMD);

            // Block until EXE_CMD is echoed back
            bool waitForCMD = true;
            while(waitForCMD)
            {
                if (ListenForCMD() == EL_SUCCESS)
                {
                    if (PopLastReadCMD() == EXE_CMD)
                    {
                        m_cmd_ready = true;
                        waitForCMD = false;
                        RequestState(EL_CMD_WAIT);
                    }
                }
            }
        }
        break;

        case EL_CMD_ABORT:
        {
            //DEBUG
            log_printf("EL: State: EL_CMD_ABORT\n\r");

            // Send ABORT_CMD to all ELs
            EchoCMD(ABORT_CMD);

            // Block until ABORT_CMD is echoed back
            bool waitForCMD = true;
            while(waitForCMD)
            {
                if (ListenForCMD() == EL_SUCCESS)
                {
                    if (PopLastReadCMD() == ABORT_CMD)
                    {
                        waitForCMD = false;
                        RequestState(EL_CMD_WAIT);
                    }
                }
            }
        }
        break;
    }

    return EL_SUCCESS;
}

el_error_t ExternalLink::EchoCMD(std::string cmd)
{
    el_error_t retCode = EL_ERROR;

    //DEBUG
    log_printf("EL: Echo CMD: %s\n\r",cmd.c_str());

    // If the Tx string is not empty and EL in WAIT
    if (!cmd.empty())
    {
        std::string tempStr = cmd;
        tempStr += UART_EOL; // Add the TC-specific EL termination char

        if(Serial1.write(tempStr.c_str()) > 0)
        {
            retCode = EL_SUCCESS;
        }
    }

    return retCode;
}

void ExternalLink::UpdateLog(const char *str, int status)
{
    log_printf("EL: %s returned %s\n",str, status);
}