#ifndef STATE_DRIVER_H
#define STATE_DRIVER_H

#include "Display_helper.h"
#include "Tetris.h"
#include "External_link.h"
#include "Cube.h"
#include <ctime>

// Transmit data structure
// Num of sender unit symbol data
// Ex: 0TR\n
// Screen 0 sent a tetris update about a right rotation UART_EOL

// Sender range: 0-5
// Unit symbols:
//      S: state transition
//      T: tetris
//      C: cube

// Data decode:
//      S: I,S,G,E,H,T,Y,R,F
//      T: R,L,D,^,
//      C: 0,1,2,3,4,5,6,7,8,9,A,B  (A: 10,  B: 11)

// Defines
#define MSG_SIZE 3
#define SCREEN_IDX 0
#define SYMBOL_IDX 1
//#define DATA_IDX 2

// State definitions
enum state_t {STATE_INIT, STATE_START, STATE_SELECT_GAME, STATE_SETTINGS,
            STATE_HIGH_SCORES, STATE_TETRIS, STATE_TETRIS_PAUSE, STATE_TETRIS_END,
            STATE_RUBIKS, STATE_RUBIKS_PAUSE, STATE_RUBIKS_END};

// State return codes
enum state_code_t {STATE_SUCCESS, STATE_ERROR};

// Class declaration
class StateDriver
{
private:

    state_t drv_state;
    state_code_t broadcast_state_transition(state_t new_state);
    void update_new_state(state_t new_state);
    state_code_t DecodeCMD(std::string CMD);

    char StateToChar(state_t state);
    state_t CharToState(char ch);

    void SetSeed(int seed);

    state_code_t ScrambleCube();
    state_code_t SolveCube();

public:

    // Has-a relationship with display helper
    DisplayHelper dHelp;

    // Has-a relationship with EL
    ExternalLink el;

    StateDriver();
    state_code_t request_state_change(state_t new_state);
    void state_controller();

    // Has-a relationship with Cube and Tetris
    Cube rbx;
    Tetris tetris;

    // Move timer for tetris
    unsigned long m_tetris_move_timer;

    // Screen number
    int m_screen_num;

    // Cube scramble helper vars
    int rbx_scram_count = 0;
    bool scrambling = false;
};

#endif