#include "State_driver.h"

/**
 * State driver default constructor
*/
StateDriver::StateDriver()
{
    drv_state = STATE_INIT;

    //DEBUG
    dHelp.clear_screen();
    for (int i = 0; i < 6; i++)
    {
        gfx->fillRect(UI_SCREENS[i].x,UI_SCREENS[i].y,UI_SCREENS[i].w,UI_SCREENS[i].h,ORANGE);
    }

    m_screen_num = -1;
    while(m_screen_num < 0)
    {
        for(int i = 0; i < 6; i++)
        {
            if (dHelp.touch_touched() && dHelp.touch_decoder(UI_SCREENS[i]) == TC_UI_TOUCH)
            {
                m_screen_num = i;
            }
        }
    }
    //END DEBUG

    rbx.SetSideNum(m_screen_num);
}

/**
 * Active controller that controlls when states transition
*/ 
void StateDriver::state_controller()
{
    while(1)
    {   
        // DEBUG
        gfx->fillCircle(20,230,5,RED);
        //delayMicroseconds(15);
        gfx->fillCircle(20,230,5,BLACK);
        gfx->fillCircle(20,230,5,RED);
        //delayMicroseconds(15);

        // Check the EL if a CMD
        // is ready to be ran
        std::string cmd = el.GetCMD();
        if (!cmd.empty())
        {
            DecodeCMD(cmd);
        }

        // Update EL
        el.StateController();

        switch (drv_state)
        {
            case STATE_START:
                if (dHelp.touch_touched() && m_screen_num == 0)
                {
                    if (dHelp.touch_decoder(UI_START) == TC_UI_TOUCH)
                    {
                        request_state_change(STATE_SELECT_GAME);
                    }
                }
                break;

            case STATE_SELECT_GAME:
                if (dHelp.touch_touched() && m_screen_num == 0)
                {
                    if (dHelp.touch_decoder(UI_BACK) == TC_UI_TOUCH)
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
                    else if (dHelp.touch_decoder(UI_SETTINGS) == TC_UI_TOUCH)
                    {
                        request_state_change(STATE_SETTINGS);
                    }
                    else if (dHelp.touch_decoder(UI_HIGH_SCORES) == TC_UI_TOUCH)
                    {
                        request_state_change(STATE_HIGH_SCORES);
                    }
                }
                break;

            case STATE_SETTINGS:
                if (dHelp.touch_touched() && m_screen_num == 0)
                {
                    if (dHelp.touch_decoder(UI_BACK) == TC_UI_TOUCH)
                    {
                        request_state_change(STATE_SELECT_GAME);
                    }
                }
                break;

            case STATE_HIGH_SCORES:
                if (dHelp.touch_touched() && m_screen_num == 0)
                {
                    if (dHelp.touch_decoder(UI_BACK) == TC_UI_TOUCH)
                    {
                        request_state_change(STATE_SELECT_GAME);
                    }
                }
                break;

            case STATE_TETRIS:
                // POC to represent buttons on others displays being pushed
                if (dHelp.touch_touched())
                {
                    if (dHelp.current_touches[0].y > 380)
                        el.SendCMD("D");
                    else if (dHelp.current_touches[0].y < 100)
                        el.SendCMD("^");
                    else if (dHelp.current_touches[0].x < 100)
                        el.SendCMD("L");
                    else if (dHelp.current_touches[0].x > 380)
                       el.SendCMD("R");
                }

                // Listen for EL update, and translate to Tetris
                if (el.ListenForCMD() == EL_SUCCESS)
                {
                    std::string recvStr = el.PopLastReadCMD();
                    //std::string el_str = el.PopLastReadCMD();
                    //if (el_str.size() == 1) // Ensure just a char was sent
                    tetris.EnqueueMove(recvStr[0]);
                }

                // Update game
                if (tetris.PlayGame() == TETRIS_END_GAME)
                {
                    request_state_change(STATE_TETRIS_END);
                }
                break;

            case STATE_RUBIKS:
            {
                // Start rubiks cube
                if (dHelp.touch_touched())
                {
                    rbx.TouchUpdate(dHelp.current_touches[0].x, dHelp.current_touches[0].y);
                }
                std::string cmd = rbx.PlayGame();
                if (!cmd.empty())
                {
                    el.SendCMD(cmd);
                }
            }
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

    // Reset active ui
    dHelp.active_ui.clear();

    switch(new_state)
    {
        case STATE_START:
        {
            if (m_screen_num == 0)
            {
                dHelp.drawImage(SCENE_HOME.image);
                dHelp.active_ui = SCENE_HOME.ui_elements;
            }
            else {
                dHelp.clear_screen();
            }
            break;
        }

        case STATE_SELECT_GAME:
        {
            if (m_screen_num == 0) {
                dHelp.drawImage(SCENE_SELECT_GAME.image);
                dHelp.active_ui = SCENE_SELECT_GAME.ui_elements;
            }
            else {
                dHelp.clear_screen();
            }
            break;
        }

        case STATE_SETTINGS:
        {
            if (m_screen_num == 0)
            {
                dHelp.drawImage(SCENE_SETTINGS.image);
                dHelp.active_ui = SCENE_SETTINGS.ui_elements;
            }
            else {
                dHelp.clear_screen();           
            }
            break;
        }

        case STATE_HIGH_SCORES:
        {
            if (m_screen_num == 0)
            {
                dHelp.drawImage(SCENE_HIGH_SCORES.image);
                dHelp.active_ui = SCENE_HIGH_SCORES.ui_elements;
            }
            else {
                dHelp.clear_screen();
            }
            break;
        }

        case STATE_TETRIS:
        {
            dHelp.clear_screen();
            break;
        }

        case STATE_RUBIKS:
        {// <-- must use brackets here to stop 'switch' related errors
            // Clear screen to reset background
            dHelp.clear_screen();

            // Draw initial rubiks cube
            rbx.drawRubiksSide(rbx.GetSideNum());

            break;
        }

        case STATE_TETRIS_END:
        {
            //dHelp.clear_screen();
            //gfx->fillScreen(RED);
            break;
        }

        case STATE_RUBIKS_END:
        {
            break;
        }
    }

    // Update private state variable 
    drv_state = new_state;

    // -----DEBUG-----
    //dHelp.drawUI();

    // Pause between state transistions
    delay(400);

    // Reset touches
    dHelp.touch_reset();
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

    //TODO: Check if bus is available and lock it.

    if (m_screen_num == 0)
    {
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
                    new_state == STATE_RUBIKS || new_state == STATE_SETTINGS ||
                    new_state == STATE_HIGH_SCORES)
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

            case STATE_HIGH_SCORES:
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
    } else {
        retCode = STATE_SUCCESS;
    }

    // Valid state transistion
    if (retCode == STATE_SUCCESS)
    {

        //DEBUG
        log_printf("STATE: Transitioning from %d to %d\n\r",drv_state,new_state);

        // Broadcast update to other devices
        if (m_screen_num == 0 && new_state > 0) {
            broadcast_state_transition(new_state);
        }

        update_new_state(new_state);
    }

    return retCode;
}

/**
 * @brief Send state update to all displays
 * @param new_state desired state to transition to
 * @return STATE_SUCCESS on success otherwise STATE_ERROR
*/
state_code_t StateDriver::broadcast_state_transition(state_t new_state)
{
    state_code_t ret_code = STATE_ERROR;
    bool formedStr = false;

    std::string cmdToSend;
    cmdToSend += (m_screen_num + '0');
    cmdToSend += 'S';
    cmdToSend += StateToChar(new_state);

    //DEBUG
    log_printf("STATE: Broadcasting transition to %d\r\n",new_state);
    log_printf("STATE: Broadcasting CMD: %s\r\n",cmdToSend.c_str());

    ret_code = STATE_SUCCESS;

    // Send CMD to neighboring EL
    if (el.SendCMD(cmdToSend) == EL_SUCCESS)
    {
        ret_code = STATE_SUCCESS;
    }
        
   return ret_code;
}

state_code_t StateDriver::DecodeCMD(std::string CMD)
{
    //DEBUG
    log_printf("DRIVER: Executing CMD: %s\n\r",CMD.c_str());

    state_code_t ret_val = STATE_SUCCESS;
    if (!CMD.empty() && CMD != EMPTY_STR)
    {
        // Get which screen sent CMD
        int sender_screen = CMD[0] - '0';
        if (sender_screen < 0 || sender_screen > 5) {
            ret_val = STATE_ERROR;
        }

        // Get the unit from the CMD
        char sender_unit = CMD[1];

        switch(CMD[1])
        {
            case 'S':
            {
                request_state_change(CharToState(CMD[2]));
            }
            break;

            case 'T':
            {
            }
            break;

            case 'C':
            {
                int dirSwiped = CMD[2] - '0';
                if (CMD[2] == 'A') {
                    dirSwiped = 10;
                }
                else if (CMD[2] == 'B') {
                    dirSwiped = 11;
                }
                rbx.RotateCube(sender_screen,dirSwiped);
            }
            break;

            default:
            {
                ret_val = STATE_ERROR;
            }
        }
    }
    return ret_val;
}

char StateDriver::StateToChar(state_t state)
{
    char ret_val = STATE_INIT;

    switch (state)
    {
        case STATE_INIT:        ret_val ='I'; break;
        case STATE_START:       ret_val ='S'; break;
        case STATE_SELECT_GAME: ret_val ='G'; break;
        case STATE_SETTINGS:    ret_val ='E'; break;
        case STATE_HIGH_SCORES: ret_val ='H'; break;
        case STATE_TETRIS:      ret_val ='T'; break;
        case STATE_TETRIS_END:  ret_val ='Y'; break;
        case STATE_RUBIKS:      ret_val ='R'; break;
        case STATE_RUBIKS_END : ret_val ='F'; break;
    }
    return ret_val;
}

state_t StateDriver::CharToState(char ch)
{
    state_t ret_val = STATE_INIT;

    switch (ch)
    {
        case 'I': ret_val = STATE_INIT; break;
        case 'S': ret_val = STATE_START; break;
        case 'G': ret_val = STATE_SELECT_GAME; break;
        case 'E': ret_val = STATE_SETTINGS; break;
        case 'H': ret_val = STATE_HIGH_SCORES; break;
        case 'T': ret_val = STATE_TETRIS; break;
        case 'Y': ret_val = STATE_TETRIS_END; break;
        case 'R': ret_val = STATE_RUBIKS; break;
        case 'F': ret_val = STATE_RUBIKS_END; break;
    }
    return ret_val;
}
