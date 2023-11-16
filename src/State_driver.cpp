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
        gfx->fillCircle(20,230,5,RED);
        delayMicroseconds(15);
        gfx->fillCircle(20,230,5,BLACK);
        delayMicroseconds(15);
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
                if (dHelp.touch_touched())
                {
                    if (dHelp.touch_decoder(UI_BACK) == TC_UI_TOUCH)
                    {
                        request_state_change(STATE_SELECT_GAME);
                    }
                }
                break;

            case STATE_HIGH_SCORES:
                if (dHelp.touch_touched())
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
                        el.SendStr("D");
                    else if (dHelp.current_touches[0].y < 100)
                        el.SendStr("^");
                    else if (dHelp.current_touches[0].x < 100)
                        el.SendStr("L");
                    else if (dHelp.current_touches[0].x > 380)
                       el.SendStr("R");
                }

                // Listen for EL update, and translate to Tetris
                if (el.ListenForStr() == EL_SUCCESS)
                {
                    std::string recvStr = el.PopLastReadStr();
                    //std::string el_str = el.PopLastReadStr();
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
                // Start rubiks cube
                if (dHelp.touch_touched())
                {
                    rbx.TouchUpdate(dHelp.current_touches[0].x, dHelp.current_touches[0].y);
                }
                rbx.PlayGame();
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
            #if (DISP_NUM == 0)
                dHelp.drawImage(SCENE_HOME.image);
                dHelp.active_ui = SCENE_HOME.ui_elements;
            #else
                dHelp.clear_screen();
            #endif
            break;
        }

        case STATE_SELECT_GAME:
        {
            #if (DISP_NUM == 0)
                dHelp.drawImage(SCENE_SELECT_GAME.image);
                dHelp.active_ui = SCENE_SELECT_GAME.ui_elements;
            #else
                dHelp.clear_screen();
            #endif
            break;
        }

        case STATE_SETTINGS:
        {
            #if (DISP_NUM == 0)
                dHelp.drawImage(SCENE_SETTINGS.image);
                dHelp.active_ui = SCENE_SETTINGS.ui_elements;
            #else
                dHelp.clear_screen();
            #endif             
            break;
        }

        case STATE_HIGH_SCORES:
        {
            #if (DISP_NUM == 0)
                dHelp.drawImage(SCENE_HIGH_SCORES.image);
                dHelp.active_ui = SCENE_HIGH_SCORES.ui_elements;
            #else
                dHelp.clear_screen();
            #endif
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
 * @brief Send state update to all displays
 * @param new_state desired state to transition to
 * @return STATE_SUCCESS on success otherwise STATE_ERROR
*/
state_code_t StateDriver::broadcast_state_transition(state_t new_state)
{
    state_code_t ret_code = STATE_ERROR;
    bool formedStr = false;

    std::string strToSend;
    strToSend += m_screen_num;
    strToSend += 'S';
    
    static const std::unordered_map<char, state_t> stateMap = {
    {'I', STATE_INIT},
    {'S', STATE_START},
    {'G', STATE_SELECT_GAME},
    {'E', STATE_SETTINGS},
    {'H', STATE_HIGH_SCORES},
    {'T', STATE_TETRIS},
    {'Y', STATE_TETRIS_END},
    {'R', STATE_RUBIKS},
    {'F', STATE_RUBIKS_END}
    };

    auto it = stateMap.find(new_state);
    if (it != stateMap.end()) {
        strToSend += it->second;
    }

    if (strToSend.length() == MSG_SIZE)
    {
        // Tx string to neighboring EL
        el.SendStr(strToSend);

        // Listen for feedback signal
        listen_and_echo();
    }
        
   return ret_code;
}

state_code_t StateDriver::listen_and_echo()
{
    state_code_t ret_code = STATE_ERROR;

    if (el.ListenForStr() == EL_SUCCESS)
    {
        // Receive string
        std::string RxStr = el.PopLastReadStr();
        
        // Check if msg received is msg sent
        if (RxStr[SCREEN_IDX] - '0' == m_screen_num) {
            ret_code = STATE_SUCCESS;
        }
        else {
            el.SendStr(RxStr);
        }
        // If not, echo msg
    }
    return STATE_ERROR;
}
