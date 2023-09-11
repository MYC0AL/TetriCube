#ifndef STATE_DRIVER_H
#define STATE_DRIVER_H

// State definitions
enum state_t {STATE_START, STATE_SELECT_GAME, STATE_SETTINGS,
            STATE_TETRIS, STATE_TETRIS_END, STATE_RUBIKS, 
            STATE_RUBIKS_END};

// State return codes
enum state_code_t {STATE_SUCCESS, STATE_ERROR};

// Global state variable
state_t drv_state;

void update_new_state(state_t* tc_state);
int request_state_change(state_t* tc_state);
void clear_all();
void broadcast_update();

#endif