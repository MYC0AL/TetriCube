#include "State_driver.h"
//#include "Cube.h"

/**
 * State driver default constructor
*/
StateDriver::StateDriver()
{
    drv_state = STATE_INIT;
}

/**
 * Active controller that controlls when states transition
*/ 
void StateDriver::state_controller()
{
    while(1)
    {
        switch (drv_state)
        {
            case STATE_START:
                if (dHelp.touch_touched())
                {
                    if (dHelp.touch_decoder(UI_START) == TC_UI_TOUCH)
                    {
                        request_state_change(STATE_SELECT_GAME);
                    }
                }
                break;

            case STATE_SELECT_GAME:
                if (dHelp.touch_touched())
                {
                    if (dHelp.touch_decoder(UI_BACK_TO_START) == TC_UI_TOUCH)
                    {
                        request_state_change(STATE_START);
                    }
                    else if (dHelp.touch_decoder(UI_RUBIKS) == TC_UI_TOUCH)
                    {
                        request_state_change(STATE_RUBIKS);
                    }
                    else if (dHelp.touch_decoder(UI_TETRIS) == TC_UI_TOUCH)
                    {
                        request_state_change(STATE_TETRIS);
                    }
                }
                break;

            case STATE_SETTINGS:

                break;

            case STATE_TETRIS:

                break;

            case STATE_RUBIKS:

                break;

            case STATE_TETRIS_END:

                break;

            case STATE_RUBIKS_END:

                break;
        }
    }
}

/* 
 * This function will update the display and any neccessary
 * reachable variables to prepare the application for a new
 * state transistion. This will update the global drv_state.
 */
void StateDriver::update_new_state(state_t new_state)
{
    // Clear screen to prepare for new state transistion
    dHelp.clear_screen();

    switch(new_state)
    {
        case STATE_START:
            #if (DISP_NUM == 0)
                dHelp.drawImage(SCENE_HOME.image);
                dHelp.active_ui = SCENE_HOME.ui_elements;
            #endif
            break;

        case STATE_SELECT_GAME:
            #if (DISP_NUM == 0)
                dHelp.drawImage(SCENE_SELECT_GAME.image);
                dHelp.active_ui = SCENE_SELECT_GAME.ui_elements;
            #endif
            break;

        case STATE_SETTINGS:

            break;

        case STATE_TETRIS:

            break;

        case STATE_RUBIKS:

            break;

        case STATE_TETRIS_END:

            break;

        case STATE_RUBIKS_END:

            break;
    }

    // Update private state variable 
    drv_state = new_state;

    // -----DEBUG-----
    dHelp.drawUI();

    // Pause between state transistions
    sleep(1);
}

/*
 * Application will request a state change using this function.
 * If the desired state is achievable, the transistion will be made.
 * If the transistion was successful, the drv_state will update and
 * any new application updates will be performed. 
*/
state_code_t StateDriver::request_state_change(state_t new_state)
{
    state_code_t retCode = STATE_ERROR;

    switch (drv_state)
    {
        case STATE_INIT:
            if (new_state == STATE_START)
            {
                retCode = STATE_SUCCESS;
            }
            break;

        case STATE_START:
            if (new_state == STATE_SELECT_GAME)
            {
                retCode = STATE_SUCCESS;
            }
            break;

        case STATE_SELECT_GAME:
            if (new_state == STATE_START || new_state == STATE_TETRIS ||
                new_state == STATE_RUBIKS || new_state == STATE_SETTINGS)
            {
                retCode = STATE_SUCCESS;
            }
            break;

        case STATE_SETTINGS:
            if (new_state == STATE_SELECT_GAME || new_state == STATE_TETRIS || 
                new_state == STATE_RUBIKS)
            {
                retCode = STATE_SUCCESS;
            }
            break;

        case STATE_TETRIS:
            if (new_state == STATE_SETTINGS || new_state == STATE_TETRIS_END)
            {
                retCode = STATE_SUCCESS;
            }
            break;

        case STATE_RUBIKS:
            if (new_state == STATE_SETTINGS || new_state == STATE_RUBIKS_END)
            {
                retCode = STATE_SUCCESS;
            }
            break;

        case STATE_TETRIS_END:
            if (new_state == STATE_TETRIS || new_state == STATE_SELECT_GAME)
            {
                retCode = STATE_SUCCESS;
            }
            break;

        case STATE_RUBIKS_END:
            if (new_state == STATE_RUBIKS || new_state == STATE_SELECT_GAME)
            {
                retCode = STATE_SUCCESS;
            }
            break;

    }

    // Valid state transistion
    if (retCode == STATE_SUCCESS)
    {
        // Broadcast update to other devices
        broadcast_state_transition(new_state);

        update_new_state(new_state);
    }

    return retCode;
}

/**
 * Send state update to all displays.
*/
void StateDriver::broadcast_state_transition(state_t new_state)
{

}