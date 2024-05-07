#include "ExternalLink.h"

/******************************************************************
 * @brief Constructor for ExternalLink class
******************************************************************/
ExternalLink::ExternalLink() : m_state(EL_CMD_WAIT), m_setup_state(true), m_cmd_ready(false)
{
    Serial1.begin(UART_BAUD, SERIAL_8N1, -1, EL0_TX);
    log_printf("EL: External Link 0 Started\n");

    Serial2.begin(UART_BAUD, SERIAL_8N1, EL0_RX, -1);
    log_printf("EL: External Link 1 Started\n");

    delay(100);
}

/******************************************************************
 * @brief Request EL to transition to a different state
 * @param new_state Desired new state transition
 * @return EL_SUCCESS during valid transition otherwise EL_ERROR
******************************************************************/
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

        SetupState();
    }

    return ret_val;
}

/******************************************************************
 * @brief Send a command and trigger the EL FSM or wait
 * until CMD_TIMEOUT is reached
 * @param cmd A command to send to all ELs
 * @return Will return EL_SUCCESS if all sent out CMD
 * otherwise will return EL_ERROR
 ******************************************************************/
el_error_t ExternalLink::SendCMD(std::string cmd)
{
    el_error_t retCode = EL_ERROR;

    //DEBUG
    log_printf("EL: Attempting to send CMD: %s\r\n",cmd.c_str());
    log_printf("m_state: %d\n\r",m_state);
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

            //Request transition to SENT
            RequestState(EL_CMD_SENT);

            ready_to_tx = false;
        }
    }

    return retCode;
}

/******************************************************************
 * @brief Listen for a CMD that is transmitter over the recieving
 * EL
 * @return EL_SUCCESS if successfully recieved a CMD otherwise
 * EL_ERROR
******************************************************************/
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

    Serial2.flush();

    if (retCode == EL_SUCCESS)
    {
        m_last_read_str = tempBuff;

        log_printf("EL: CMD Received: %s\n",m_last_read_str.c_str());
    }

    return retCode;
}

/******************************************************************
 * @brief A queue-like behavior to pop the last read command into a
 * variable.
 * Safety-net to allow each EL to hold on to important CMDs
 * @return Last read CMD
******************************************************************/
std::string ExternalLink::PopLastReadCMD()
{
    std::string tempStr = m_last_read_str;
    m_last_read_str = EMPTY_STR;

    //DEBUG
    log_printf("EL: Popped last read CMD: %s\n\r",tempStr.c_str());

    return tempStr;
}

/******************************************************************
 * @brief Get the current stored CMD if the CMD is ready to be
 * recieved
 * @return Stored complete CMD
******************************************************************/
std::string ExternalLink::GetCMD()
{
    std::string ret_cmd;
    if (m_cmd_ready)
    {
        ret_cmd = m_loaded_cmd;
        m_cmd_ready = false;
        
        ready_to_tx = true;

        //DEBUG
        log_printf("EL: CMD Ready: %s\n\r",ret_cmd.c_str());
    }
    return ret_cmd;
}

/******************************************************************
 * @brief Check if a command is ready
 * @return CMD ready flag
******************************************************************/
bool ExternalLink::IsCmdReady()
{
    return m_cmd_ready;
}

/******************************************************************
 * @brief Check if EL is ready for transmission
 * @return EL ready flag
******************************************************************/
bool ExternalLink::IsReady()
{
    return m_is_ready;
}

/******************************************************************
 * @brief Main controller of the EL FSM
 * @return EL_SUCCESS
******************************************************************/
el_error_t ExternalLink::StateController()
{

    switch(m_state)
    {
        case EL_CMD_WAIT:
            // Listen for received CMD
            if (ListenForCMD() == EL_SUCCESS)
            {
                // Delay or will break; Gives enough time for Rx
                delay(5);

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
            do 
            {
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
                        m_lock = false;
                    }
                    else if (cmd == EXE_CMD)
                    {
                        //Echo EXE_CMD
                        EchoCMD(EXE_CMD);
                        
                        // Set execution flag
                        m_cmd_ready = true;

                        RequestState(EL_CMD_WAIT);
                        m_lock = false;
                    }
                }
            }
            while (m_lock);
        break;

        case EL_CMD_SENT:
        {
            do
            {
                if (CheckTimeout() != EL_SUCCESS)
                {
                    RequestState(EL_CMD_ABORT);
                    m_lock = false;
                }
                else if (ListenForCMD() == EL_SUCCESS)
                {
                    std::string cmd = PopLastReadCMD();

                    //DEBUG
                    log_printf("EL: In EL_CMD_SENT comparing: %s to %s\n\r",cmd.c_str(),m_loaded_cmd.c_str());

                    if (cmd == m_loaded_cmd)
                    {
                        // Correct CMD received
                        RequestState(EL_CMD_SUCCESS);
                        m_lock = false;
                    }
                    else
                    {
                        // Incorrect CMD received
                        RequestState(EL_CMD_ABORT);
                        m_lock = false;
                    }
                }
            }
            while (m_lock);
        }
        break;

        case EL_CMD_SUCCESS:
        {
            unsigned long sucess_time = millis();
            bool success_timeout = false;
            do
            {
                if (ListenForCMD() == EL_SUCCESS)
                {
                    if (PopLastReadCMD() == EXE_CMD)
                    {
                        m_cmd_ready = true;
                        RequestState(EL_CMD_WAIT);
                        m_lock = false;
                    }
                }
                if (millis() >= sucess_time + CMD_TIMEOUT) {
                    success_timeout = true;
                }
            }
            while (m_lock && !success_timeout);
        }
        break;

        case EL_CMD_ABORT:
        {
            unsigned long abort_time = millis();
            bool abort_timeout = false;
            do
            {
                if (ListenForCMD() == EL_SUCCESS)
                {
                    if (PopLastReadCMD() == ABORT_CMD)
                    {
                        RequestState(EL_CMD_WAIT);
                    }
                }
                if (millis() >= abort_timeout + CMD_TIMEOUT) {
                    abort_timeout = true;
                }
            }
            while (m_lock && !abort_timeout);
        }
        break;
    }

    return EL_SUCCESS;
}

/******************************************************************
 * @brief Setup the new state before activation
 * @return EL_SUCCESS
******************************************************************/
el_error_t ExternalLink::SetupState()
{
    switch(m_state)
    {
        case EL_CMD_WAIT:
            m_timeout = 0;

            // Set ready flag
            m_is_ready = true;

            //DEBUG
            log_printf("EL: WAIT\n\r");
            break;

        case EL_CMD_RECEIVED:
            //DEBUG
            log_printf("EL: RECEIVED\n\r");
            
            m_is_ready = false;
            m_cmd_ready = false;
            m_timeout = millis();
            m_lock = true;
            break;

        case EL_CMD_SENT:
            //DEBUG
            log_printf("EL: SENT\n\r");

            m_is_ready = false;
            m_cmd_ready = false;
            m_timeout = millis();
            m_lock = true;
        break;

        case EL_CMD_SUCCESS:
            //DEBUG
            log_printf("EL: SUCCESS\n\r");

            m_lock = true;

            // Send EXE_CMD to all ELs
            EchoCMD(EXE_CMD);
        break;

        case EL_CMD_ABORT:
            //DEBUG
            log_printf("EL: ABORT\n\r");

            m_lock = true;

            // Send ABORT_CMD to all ELs
            EchoCMD(ABORT_CMD);
        break;
    }
    return EL_SUCCESS;
}

/******************************************************************
 * @brief Echo the recieved CMD to the next EL
 * @param cmd The CMD to be echoed
 * @return EL_SUCCESS if successfull tx otherwise EL_ERROR
******************************************************************/
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
    Serial1.flush();

    return retCode;
}

/******************************************************************
 * @brief Check if the timeout for the EL was reached.
 * If it was, ABORT all current transactions
 * @return EL_SUCCESS if timeout has not be reached, otherwise
 * EL_ERROR
******************************************************************/
el_error_t ExternalLink::CheckTimeout()
{
    el_error_t ret = EL_SUCCESS;
    if (millis() - m_timeout >= CMD_TIMEOUT)
    {
        ret = EL_ERROR;
        //DEBUG
        log_printf("EL: Timeout expired\n\r");

        RequestState(EL_CMD_ABORT);
    }
    return ret;
}

/******************************************************************
 * @brief Debugging tool to update the log outputs
 * @param str Constant cstr with log message
 * @param status Status returned
******************************************************************/
void ExternalLink::UpdateLog(const char *str, int status)
{
    log_printf("EL: %s returned %s\n\r",str, status);
}