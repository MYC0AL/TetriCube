#ifndef COMMUNICATION_H
#define COMMUNICATION_H

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

#define RBX_SOLVE_SYM '%'
#define TETRIS_RESET_SYM 'X'
#define TTT_RESET_SYM 'X'
#define PONG_RESET_SYM 'X'
#define PONG_SET_BALL_SYM 'B'
#define TTT_HARD_MODE_SYM 'H'

#define RETRY_COUNT 5

enum el_error_t {EL_SUCCESS, EL_ERROR, EL_TRANSMIT_ERROR};

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
    bool IsCmdReady();

    bool IsReady();

    bool ready_to_tx = true;
private: 

    el_error_t SetupState();
    el_error_t EchoCMD(std::string cmd);
    el_error_t CheckTimeout();
    void UpdateLog(const char* str, int status);

    el_state_t m_state;
    std::string m_last_read_str;
    std::string m_loaded_cmd;
    bool m_cmd_ready;
    bool m_setup_state;
    bool m_lock;
    bool m_is_ready;

    unsigned long m_timeout;

};

#endif