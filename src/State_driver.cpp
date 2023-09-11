#include "State_driver.h"
#include "Display_helper.h"

/* 
 * This function will update the display and any neccessary
 * reachable variables to prepare the application for a new
 * state transistion. This will update the global drv_state.
 */
void update_new_state(state_t new_state)
{
    switch(new_state)
    {
        case STATE_START:
        {

        }

        case STATE_SELECT_GAME:
        {

        }

        case STATE_SETTINGS:
        {

        }

        case STATE_TETRIS:
        {

        }

        case STATE_RUBIKS:
        {

        }

        case STATE_TETRIS_END:
        {

        }

        case STATE_RUBIKS_END:
        {

        }
    }
}

/*
 * Application will request a state change using this function.
 * If the desired state is achievable, the transistion will be made.
 * If the transistion was successful, the drv_state will update and
 * any new application updates will be performed. 
*/
int request_state_change(state_t new_state)
{
    state_code_t retCode = STATE_ERROR;

    switch (drv_state)
    {
        case STATE_START:
        {
            if (new_state == STATE_SELECT_GAME)
            {
                retCode = STATE_SUCCESS;
            }
            break;
        }

        case STATE_SELECT_GAME:
        {
            if (new_state == STATE_START || new_state == STATE_TETRIS ||
                new_state == STATE_RUBIKS || new_state == STATE_SETTINGS)
            {
                retCode = STATE_SUCCESS;
            }
            break;
        }

        case STATE_SETTINGS:
        {
            if (new_state == STATE_SELECT_GAME || new_state == STATE_TETRIS || 
                new_state == STATE_RUBIKS)
            {
                retCode = STATE_SUCCESS;
            }
            break;
        } 

        case STATE_TETRIS:
        {
            if (new_state == STATE_SETTINGS || new_state == STATE_TETRIS_END)
            {
                retCode = STATE_SUCCESS;
            }
            break;
        }

        case STATE_RUBIKS:
        {
            if (new_state == STATE_SETTINGS || new_state == STATE_RUBIKS_END)
            {
                retCode = STATE_SUCCESS;
            }
            break;
        }

        case STATE_TETRIS_END:
        {
            if (new_state == STATE_TETRIS || new_state == STATE_SELECT_GAME)
            {
                retCode = STATE_SUCCESS;
            }
            break;
        }

        case STATE_RUBIKS_END:
        {
            if (new_state == STATE_RUBIKS || new_state == STATE_SELECT_GAME)
            {
                retCode = STATE_SUCCESS;
            }
            break;
        }
    }

    // Valid state transistion
    if (retCode == STATE_SUCCESS)
        update_new_state(new_state);

    return retCode;
}

void clear_all()
{
    
}

void broadcast_update()
{

}