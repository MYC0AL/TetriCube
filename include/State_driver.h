#ifndef STATE_DRIVER_H
#define STATE_DRIVER_H

#include "Display_helper.h"

// State definitions
enum state_t {STATE_START, STATE_SELECT_GAME, STATE_SETTINGS,
            STATE_TETRIS, STATE_TETRIS_END, STATE_RUBIKS, 
            STATE_RUBIKS_END};

// State return codes
enum state_code_t {STATE_SUCCESS, STATE_ERROR};


// Class declaration
class StateDriver
{
private:

    state_t drv_state;
    void update_new_state(state_t new_state);

public:

    // Has-a relationship with display helper
    DisplayHelper dHelp;

    int request_state_change(state_t new_state);
    void broadcast_state_transition(state_t new_state);

};

#endif