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

    // Set side num for each screen
    rbx.SetSideNum(m_screen_num);
    tetris.SetSideNum(m_screen_num);
}

/**
 * Active controller that controlls when states transition
*/ 
void StateDriver::state_controller()
{
    while(1)
    {   
        // DEBUG
        //gfx->fillCircle(20,230,5,RED);
        //delayMicroseconds(15);
        //gfx->fillCircle(20,230,5,BLACK);
        //gfx->fillCircle(20,230,5,RED);
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
            {
                if (dHelp.touch_touched() && m_screen_num == 1
                    && (millis() - m_tetris_move_timer >= TETRIS_MOVE_DELAY))
                {
                    if (dHelp.touch_decoder(UI_TETRIS_PAUSE) == TC_UI_TOUCH)
                    {
                        request_state_change(STATE_TETRIS_PAUSE);
                    }
                    else
                    {
                        std::string tetris_cmd;
                        tetris_cmd += m_screen_num + '0';
                        tetris_cmd += 'T';

                        char dir = '0';
                        if (dHelp.touch_decoder(UI_TETRIS_LEFT) == TC_UI_TOUCH) {
                            dir = 'L';
                        }
                        else if (dHelp.touch_decoder(UI_TETRIS_RIGHT) == TC_UI_TOUCH) {
                            dir = 'R';
                        }
                        else if (dHelp.touch_decoder(UI_TETRIS_DOWN) == TC_UI_TOUCH) {
                            dir = 'D';
                        }
                        else if (dHelp.touch_decoder(UI_TETRIS_ROTATE) == TC_UI_TOUCH) {
                            dir = '^';
                        }

                        tetris_cmd += dir;
                        if (dir != '0') {
                            //DEBUG
                            log_printf("TETRIS: Broadcasting '%c'\n\r",dir);
                            el.SendCMD(tetris_cmd);
                        }

                        // Reset move timer
                        m_tetris_move_timer = millis();
                    }
                }

                // Update game
                if (tetris.PlayGame() == TETRIS_END_GAME)
                {
                    request_state_change(STATE_TETRIS_END);
                }

                // Check for reset flag
                if (tetris_reset && m_screen_num == 0 && el.ready_to_tx)
                {
                    TetrisReset();
                }
            }
            break;

            case STATE_RUBIKS:
            {
                // Start rubiks cube
                if (dHelp.touch_touched() && !scrambling)
                {
                    if (m_screen_num == 1 && dHelp.touch_decoder(UI_RUBIKS_PAUSE) == TC_UI_TOUCH)
                    {
                        request_state_change(STATE_RUBIKS_PAUSE);
                    }
                    else
                    {
                        rbx.TouchUpdate(dHelp.current_touches[0].x, dHelp.current_touches[0].y);
                    }
                }
                std::string cmd = rbx.PlayGame();
                if (!cmd.empty())
                {
                    el.SendCMD(cmd);
                }
                else if (scrambling && m_screen_num == 0 && el.ready_to_tx)
                {
                    delay(10);
                    ScrambleCube();
                }
                else if (solve && m_screen_num == 0 && el.ready_to_tx)
                {
                    SolveCube();
                    solve = false;
                }
            }
            break;

            case STATE_TETRIS_PAUSE:
                if (dHelp.touch_touched() && m_screen_num == 0)
                {
                    if (dHelp.touch_decoder(UI_TETRIS_BACK_TO_GAME) == TC_UI_TOUCH)
                    {
                        request_state_change(STATE_TETRIS);
                    }
                    else if (dHelp.touch_decoder(UI_HOME) == TC_UI_TOUCH)
                    {
                        request_state_change(STATE_SELECT_GAME);
                    }
                    else if (dHelp.touch_decoder(UI_TETRIS_RESET) == TC_UI_TOUCH)
                    {
                        request_state_change(STATE_TETRIS);
                        tetris_reset = true;
                    }
                }
                break;

            case STATE_RUBIKS_PAUSE:
                if (dHelp.touch_touched() && m_screen_num == 0)
                {
                    if (dHelp.touch_decoder(UI_RUBIKS_BACK_TO_GAME) == TC_UI_TOUCH)
                    {
                        request_state_change(STATE_RUBIKS);
                    }
                    else if (dHelp.touch_decoder(UI_HOME) == TC_UI_TOUCH)
                    {
                        request_state_change(STATE_SELECT_GAME);
                    }
                    else if (dHelp.touch_decoder(UI_RUBIKS_SCRAMBLE) == TC_UI_TOUCH)
                    {
                        request_state_change(STATE_RUBIKS);
                        scrambling = true;
                    }
                    else if (dHelp.touch_decoder(UI_RUBIKS_SOLVE) == TC_UI_TOUCH)
                    {
                        request_state_change(STATE_RUBIKS);
                        solve = true;
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

                // Set universal seed
                std::srand(static_cast<unsigned int>(std::time(nullptr)));
                char seed = (33 + rand() % (126 - 33 + 1));
                char rand_char = 'R';
                std::string cmd;
                cmd += m_screen_num + '0';
                cmd += rand_char;
                cmd += seed;
                log_printf("DRIVER: Random seed set to %s\n\r",cmd.c_str());
                el.SendCMD(cmd);
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
            if (m_screen_num == 1)
            {
                dHelp.drawImage(SCENE_TETRIS_CONTROLS.image);
                dHelp.active_ui = SCENE_TETRIS_CONTROLS.ui_elements;

                // Overlay pause icon
                gfx->fillRect(30,410,20,50,WHITE);
                gfx->fillRect(60,410,20,50,WHITE);
            }
            else {
                dHelp.clear_screen();
            }
            break;
        }

        case STATE_RUBIKS:
        {// <-- must use brackets here to stop 'switch' related errors
            // Clear screen to reset background
            dHelp.clear_screen();

            // Draw initial rubiks cube
            rbx.drawRubiksSide(rbx.GetSideNum());

            // Draw ui elements
            if (m_screen_num == 1)
            {
                dHelp.active_ui = SCENE_RUBIKS_CONTROLS.ui_elements;
            }

            // Update Overlayed Pause ICON
            if (m_screen_num == 1)
            {
                gfx->fillRect(215,215,20,50,BLACK);
                gfx->fillRect(245,215,20,50,BLACK);
            }

            break;
        }

        case STATE_TETRIS_PAUSE:
        {
            if (m_screen_num == 0)
            {
                dHelp.drawImage(SCENE_TETRIS_PAUSE.image);
                dHelp.active_ui = SCENE_TETRIS_PAUSE.ui_elements;

                // Update displayed score
                unsigned long tetris_score = tetris.GetScore();
                int text_offset = (log10(tetris_score)+1)/2;
                int text_size = 4;
                int one_char_width = 6 * text_size;
                gfx->setTextSize(text_size);
                gfx->setCursor(230-(one_char_width*text_offset),410);
                gfx->printf("%d",tetris_score);
                log_printf("tetris_score: %d\ntext_offset: %d\n\r",tetris_score,text_offset);
            }
            else {
                dHelp.clear_screen();           
            }
            break;
        }

        case STATE_RUBIKS_PAUSE:
        {
            if (m_screen_num == 0)
            {
                dHelp.drawImage(SCENE_RUBIKS_PAUSE.image);
                dHelp.active_ui = SCENE_RUBIKS_PAUSE.ui_elements;
            }
            else {
                dHelp.clear_screen();           
            }
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
    //DEBUG
    log_printf("STATE: Requesting from %d to %d\n\r",drv_state,new_state);

    state_code_t retCode = STATE_ERROR;

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
                if (new_state == STATE_TETRIS_PAUSE || new_state == STATE_TETRIS_END)
                {
                    retCode = STATE_SUCCESS;
                }
                break;

            case STATE_RUBIKS:
                if (new_state == STATE_RUBIKS_PAUSE || new_state == STATE_RUBIKS_END)
                {
                    retCode = STATE_SUCCESS;
                }
                break;

            case STATE_TETRIS_PAUSE:
                if (new_state == STATE_TETRIS || new_state == STATE_SELECT_GAME)
                {
                    retCode = STATE_SUCCESS;
                }
                break;

            case STATE_RUBIKS_PAUSE:
                if (new_state == STATE_RUBIKS || new_state == STATE_SELECT_GAME)
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
    } 
    else 
    {
        retCode = STATE_SUCCESS;
    }

    //DEBUG
    if (!retCode)
    {
        log_printf("STATE: Transition valid\n\r");
    }
    else
    {
        log_printf("STATE: Transition invalid\n\r");
    }

    if (retCode == STATE_SUCCESS)
    {
        broadcast_state_transition(new_state);
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

    if (m_screen_num == 0 || drv_state == STATE_TETRIS || drv_state == STATE_RUBIKS)
    {
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

        switch(sender_unit)
        {
            case 'S':
            {
                update_new_state(CharToState(CMD[2]));
            }
            break;

            case 'T':
            {
                if (CMD[2] == 'L' || CMD[2] == 'R' || CMD[2] == 'D' || CMD[2] == '^') {
                    tetris.EnqueueMove(CMD[2]);
                }
                else if (CMD[2] == TETRIS_RESET_SYM) {
                    tetris.Reset();
                    tetris_reset = false;
                }
            }
            break;

            case 'C':
            {
                if (drv_state == STATE_RUBIKS)
                {
                    if (CMD[2] == RBX_SOLVE_SYM)
                    {
                        rbx.SolveCube();
                    }
                    else
                    {
                        // Rotation detection

                        int dirSwiped = CMD[2] - '0';
                        if (CMD[2] == 'A') {
                            dirSwiped = 10;
                        }
                        else if (CMD[2] == 'B') {
                            dirSwiped = 11;
                        }
                        rbx.RotateCube(sender_screen,dirSwiped);
                        rbx.drawRubiksSide(m_screen_num,false);
                    }
                }
            }
            break;

            case 'R':
            {
                SetSeed(CMD[2]);
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
        case STATE_TETRIS_PAUSE:ret_val ='P'; break;
        case STATE_TETRIS_END:  ret_val ='Y'; break;
        case STATE_RUBIKS:      ret_val ='R'; break;
        case STATE_RUBIKS_PAUSE:ret_val ='K'; break;
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
        case 'P': ret_val = STATE_TETRIS_PAUSE; break;
        case 'Y': ret_val = STATE_TETRIS_END; break;
        case 'R': ret_val = STATE_RUBIKS; break;
        case 'K': ret_val = STATE_RUBIKS_PAUSE; break;
        case 'F': ret_val = STATE_RUBIKS_END; break;
    }
    return ret_val;
}

void StateDriver::SetSeed(int seed)
{
    srand(seed);
}

state_code_t StateDriver::ScrambleCube()
{
    state_code_t ret = STATE_SUCCESS;

    if (rbx_scram_count < SCRAM_COUNT && scrambling)
    {
        // Form command to send on ELs
        std::string scram_cmd;
        scram_cmd += m_screen_num + '0';
        scram_cmd += 'C';
        int randDir = rand() % NUM_DIR;
        if (randDir == 1 || randDir == 4 || randDir == 7 || randDir == 10)
        {
            randDir--;
        }
        scram_cmd += (randDir + '0');
        el.SendCMD(scram_cmd);

        // Increment scrambling counter
        rbx_scram_count++;
    }
    else
    {
        rbx_scram_count = 0;
        scrambling = false;
    }

    return ret;
}

state_code_t StateDriver::SolveCube()
{
    std::string solve_cmd;
    solve_cmd += m_screen_num + '0';
    solve_cmd += 'C';
    solve_cmd += RBX_SOLVE_SYM;
    el.SendCMD(solve_cmd);

    return STATE_SUCCESS;
}

state_code_t StateDriver::TetrisReset()
{
    std::string reset_cmd;
    reset_cmd += m_screen_num + '0';
    reset_cmd += 'T';
    reset_cmd += TETRIS_RESET_SYM;
    el.SendCMD(reset_cmd);
    tetris_reset = false;
    return STATE_SUCCESS;
}
