#include "Driver.h"

/******************************************************************
 * @brief The default constructor for the StateDriver.
 * Responsible for setting the side number for each process.
 * The ctor will clear the screens. 
******************************************************************/
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

/******************************************************************
 * @brief Active controller that controlls when states transition
 * The controller contains the main loop of the program.
******************************************************************/ 
void StateDriver::state_controller()
{
    while(1)
    {   
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
                    else if (el.ready_to_tx)
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

                // Update game only on the main screen
                if (m_screen_num == 0)
                {
                    if (tetris.PlayGame() == TETRIS_END_GAME)
                    {
                        request_state_change(STATE_TETRIS_END);
                    }

                    if (tetris.UpdateReady())
                    {
                        // Update position of mino for other screens
                        std::string posCmd;
                        tetromino_t tempMino;
                        tetris.GetTetromino(tempMino);
                        posCmd += m_screen_num + '0';
                        posCmd += 'T';
                        posCmd += 'U';
                        log_printf("DEBUG: X CORD: %d\n\r",tempMino.x);
                        posCmd += std::to_string(tempMino.x);
                        posCmd += 'Y';
                        posCmd += std::to_string(tempMino.y);
                        log_printf("DEBUG: Y CORD: %d\n\r",tempMino.y);
                        el.SendCMD(posCmd);
                    }
                    
                }

                // Check for reset flag
                if (tetris_reset && m_screen_num == 0 && el.ready_to_tx)
                {
                    TetrisReset();
                }

                // Update the score
                if (m_screen_num == 3)
                {
                    unsigned long new_score = tetris.GetScore();

                    // Only update score if new
                    if (m_tetris_score != new_score)
                    {
                        // Reset background for tetris score
                        gfx->fillRect(230,150,30,30,BLACK);
                        m_tetris_score = new_score;
                        CenterAndPrintInt(m_tetris_score,230,180,/*text scale*/ 5, CYAN);
                    }
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
                if (dHelp.touch_touched() && m_screen_num == 4)
                {
                    if (dHelp.touch_decoder(UI_HOME) == TC_UI_TOUCH)
                    {
                        request_state_change(STATE_SELECT_GAME);
                    }
                }
                else if (dHelp.touch_touched() && m_screen_num == 0)
                {
                    if (dHelp.touch_decoder(UI_INITIAL_UP1) == TC_UI_TOUCH)
                    {
                        UpdateAlphaWheels(0,false);
                    }
                    else if (dHelp.touch_decoder(UI_INITIAL_UP2) == TC_UI_TOUCH)
                    {
                        UpdateAlphaWheels(1,false);
                    }
                    else if (dHelp.touch_decoder(UI_INITIAL_UP3) == TC_UI_TOUCH)
                    {
                        UpdateAlphaWheels(2,false);
                    }
                    else if (dHelp.touch_decoder(UI_INITIAL_DOWN1) == TC_UI_TOUCH)
                    {
                        UpdateAlphaWheels(0,true);
                    }
                    else if (dHelp.touch_decoder(UI_INITIAL_DOWN2) == TC_UI_TOUCH)
                    {
                        UpdateAlphaWheels(1,true);
                    }
                    else if (dHelp.touch_decoder(UI_INITIAL_DOWN3) == TC_UI_TOUCH)
                    {
                        UpdateAlphaWheels(2,true);
                    }
                    else if (dHelp.touch_decoder(UI_SUBMIT_INITIALS) == TC_UI_TOUCH)
                    {
                        std::string initials;
                        initials += m_alpha_wheel[0];
                        initials += m_alpha_wheel[1];
                        initials += m_alpha_wheel[2];

                        if (request_state_change(STATE_SELECT_GAME) == STATE_SUCCESS)
                        {
                            WriteHighScoreFile(initials,tetris.GetScore());
                        }
                    }
                }
                break;

            case STATE_RUBIKS_END:

                break;
        }
    }
}

/******************************************************************
 * @brief Center and print integers on the screen based on length
 * @param num An integer that will be printed to screen
 * @param x The X coord of where to print
 * @param y The Y coord of where to print
 * @param text_size Length of the num to be printed
 * @param color Color of the number to be printed
 * @return Returns STATE_SUCCESS
 ******************************************************************/
state_code_t StateDriver::CenterAndPrintInt(int num, int x, int y, int text_size, int color)
{
    int text_offset = (log10(num)+1)/2;
    int one_char_width = 6 * text_size;
    gfx->setTextSize(text_size);
    gfx->setTextColor(color);
    gfx->setCursor(x-(one_char_width*text_offset),y);
    gfx->printf("%d",num);

    return STATE_SUCCESS;
}

/******************************************************************
 * @brief Initialize the Alphabet Wheels to default values
 * @return STATE_SUCCESS
******************************************************************/
state_code_t StateDriver::InitAlphaWheels()
{
    for(int i = 0; i < ALPHA_WHEEL_SIZE; i++)
    {
        m_alpha_wheel[i] = 'A';
    }

    return STATE_SUCCESS;
}

/******************************************************************
 * @brief Display the alphabet wheels to the screen
 * @return STATE_SUCCESS
******************************************************************/
state_code_t StateDriver::DisplayAlphaWheels()
{
    // Setup cursor
    gfx->setTextSize(7);
    gfx->setTextColor(WHITE);

    for(int i = 0; i < 3; i++)
    {
        // Clear old letter
        gfx->fillRect(95+(131*i),200,60,60,BLACK);

        // Set cursor position and print
        gfx->setCursor(95+(131*i),200);
        gfx->printf("%c",m_alpha_wheel[i]);
    }

    return STATE_SUCCESS;
}

/******************************************************************
 * @brief Update the alphabet wheels to the next or previous letter
 * in sequence
 * @param wheel Index of which wheel to change
 * @param next A bool that decides which direction to move down the
 * alphabet
 * @return STATE_SUCCESS is valid rotation, otherwise STATE_ERROR.
******************************************************************/
state_code_t StateDriver::UpdateAlphaWheels(int wheel, bool next)
{
    state_code_t ret = STATE_SUCCESS;

    if (wheel >= 0 && wheel <= 2)
    {
        char curr_letter = m_alpha_wheel[wheel];

        // Make sure the next or previous letter is in alphabet
        char next_letter = next ? ( (curr_letter + 1) > 'Z' ? 'A' : curr_letter + 1)
                                : ( (curr_letter - 1) < 'A' ? 'Z' : curr_letter - 1);

        // Update wheel
        m_alpha_wheel[wheel] = next_letter;

        DisplayAlphaWheels();
        delay(300);

    }
    else {
        ret = STATE_ERROR;
    }

    return ret;
}

/******************************************************************
 * @brief Function responsible for setting up each state
 * @param new_state The new state to transition to
******************************************************************/
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

                // Read from highscore file
                podium_t podium;
                ReadHighScoreFile(podium);

                // Sort the podium
                SortHighScores(podium);

                for(int i = 0; i < podium.size(); i++)
                {
                    log_printf("Entry #%d: Name:%s, Score:%s\n\r",i,podium[i].first.c_str(),podium[i].second.c_str());
                }

                // Print scores to the screen
                gfx->setTextSize(3);
                gfx->setTextColor(WHITE);

                int baseY = 250;
                gfx->setCursor(135,170);
                gfx->printf("Name  Tetris\n\r");
                gfx->setCursor(245,200);
                gfx->printf("Scores\n\r");
                for (int i = 0; i < 3 && i < podium.size(); i++)
                {
                    gfx->setCursor(135,baseY+(i*60));
                    gfx->printf("%s   %s\n\r",podium[i].first.c_str(),podium[i].second.c_str());
                }
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
            else if (m_screen_num == 3)
            {
                dHelp.clear_screen();
                gfx->setCursor(155,100);
                gfx->setTextColor(WHITE);
                gfx->setTextSize(6);
                gfx->printf("SCORE");
                CenterAndPrintInt(tetris.GetScore(),230,180,/*text scale*/ 5, CYAN);
            }
            else {
                dHelp.clear_screen();
                tetris.UpdateBoard();
                tetris.DisplayTetrisBoard();
            }
            break;
        }

        case STATE_RUBIKS:
        {// <-- must use brackets here to stop 'switch' related errors
            // Clear screen to reset background
            dHelp.clear_screen();

            // Draw initial rubiks cube
            rbx.DrawRubiksSide(rbx.GetSideNum());

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
                int text_size = 4;
                CenterAndPrintInt(tetris_score,230,410,text_size, WHITE);
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
            if (m_screen_num == 4)
            {
                dHelp.drawImage(SCENE_TETRIS_GAME_OVER.image);
                dHelp.active_ui = SCENE_TETRIS_GAME_OVER.ui_elements;

                // Update displayed score and level
                int text_size = 4;
                unsigned long curr_score = tetris.GetScore();
                CenterAndPrintInt(curr_score, 300, 270, text_size, WHITE);
                CenterAndPrintInt(tetris.GetLevel(), 300, 350, text_size, WHITE);               

            }
            else if (m_screen_num == 0)
            {
                //dHelp.clear_screen();
                dHelp.drawImage(SCENE_ENTER_INITIALS.image);
                dHelp.active_ui = SCENE_ENTER_INITIALS.ui_elements;

                // Initialize alpha wheel
                InitAlphaWheels();

                // Print the initial alpha wheel
                DisplayAlphaWheels();

            }
            else {
                dHelp.clear_screen();
                
            }

            // Reset tetris when it starts again
            tetris_reset = true;

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

/******************************************************************
 * @brief Application can request a state change or driver can
 * request internal state change
 * @param new_state desired state to transisiton to
 * @return During a valid state transistion STATE_SUCCESS will be
 * returned, otherwise STATE_ERROR.
******************************************************************/
state_code_t StateDriver::request_state_change(state_t new_state)
{
    //DEBUG
    log_printf("STATE: Requesting from %d to %d\n\r",drv_state,new_state);

    state_code_t retCode = STATE_ERROR;

    if (m_screen_num == 0 || 
    (drv_state == STATE_TETRIS_END && m_screen_num == 4) || 
    (drv_state == STATE_TETRIS && m_screen_num == 1) ||
    (drv_state == STATE_RUBIKS && m_screen_num == 1))
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

/******************************************************************
 * @brief Send state update to all displays
 * @param new_state desired state to transition to
 * @return STATE_SUCCESS on success otherwise STATE_ERROR
******************************************************************/
state_code_t StateDriver::broadcast_state_transition(state_t new_state)
{
    state_code_t ret_code = STATE_ERROR;

    if (m_screen_num == 0 || drv_state == STATE_TETRIS || drv_state == STATE_RUBIKS || (m_screen_num == 4 && drv_state == STATE_TETRIS_END))
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

/******************************************************************
 * @brief Decode the incomming command from the EL
 * @param CMD A string containing the command recieved
 * @return If a valid CMD was recieved STATE_SUCCESS will be
 * returned otherwise STATE_ERROR
******************************************************************/
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
                // if (CMD[2] == 'L' || CMD[2] == 'R' || CMD[2] == 'D' || CMD[2] == '^') {
                //     tetris.SetNextMove(CMD[2]);
                // }
                if (CMD[2] == 'U')
                {
                    // Update position of tetromino
                    std::string minoPosX;
                    std::string minoPosY;
                    bool onXPos = true;
                    for (int i = 3; i < CMD.length() && CMD[i] != UART_EOL; i++)
                    {
                        if (CMD[i] == 'Y') {
                            onXPos = false;
                        }
                        if (onXPos) {
                            minoPosX += CMD[i];
                        }
                        else {
                            minoPosY += CMD[i];
                        }
                    }
                    tetromino_t newMino;
                    newMino.x = atoi(minoPosX.c_str());
                    newMino.y = atoi(minoPosY.c_str());
                    log_printf("newmino X: %d  newmino Y: %d\n\r",newMino.x,newMino.y);
                    tetris.SetTetromino(newMino);
                    tetris.UpdateBoard();
                    tetris.DisplayTetrisBoard();

                }
                else if (CMD[2] == '^')
                {
                    tetris.UpdateBoard();
                    tetris.DisplayTetrisBoard();
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
                        rbx.DrawRubiksSide(m_screen_num,false);
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

/******************************************************************
 * @brief Convert a provided state to a encoded character
 * @param state The desired state to convert
 * @return Encoded char
******************************************************************/
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

/******************************************************************
 * @brief Convert a provided character to a encoded state
 * @param ch The desired character to convert
 * @return Encoded char
******************************************************************/
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

/******************************************************************
 * @brief Set the current random agreed upon seed
 * @param seed Desired seed
******************************************************************/
void StateDriver::SetSeed(int seed)
{
    srand(seed);
    log_printf("DRIVER: Random seed set to %d\n\r",seed);

}

/******************************************************************
 * @brief Send random rotation commands via the EL to scramble the cube
 * @return STATE_SUCCESS
******************************************************************/
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

/******************************************************************
 * @brief Send a CMD via the EL to solve the cube
 * @return STATE_SUCCESS
******************************************************************/
state_code_t StateDriver::SolveCube()
{
    std::string solve_cmd;
    solve_cmd += m_screen_num + '0';
    solve_cmd += 'C';
    solve_cmd += RBX_SOLVE_SYM;
    el.SendCMD(solve_cmd);

    return STATE_SUCCESS;
}

/******************************************************************
 * @brief Send a CMD via EL to reset tetris
 * @return STATE_SUCCESS
******************************************************************/
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

/******************************************************************
 * @brief Write the proveded name and score to the highscore file
 * @param user_name A string containing the user name
 * @param score A long containing the score from Tetris
 * @return STATE_SUCCESS
******************************************************************/
state_code_t StateDriver::WriteHighScoreFile(std::string user_name, long score)
{
    state_code_t ret = STATE_SUCCESS;

    // Create string for file name 
    String fname(HS_FILE_NAME);

    // Open file
    File hs_file = SD.open(fname,FILE_APPEND,true);

    if (hs_file)
    {
        log_printf("DRV: Writing to %s\n\r",HS_FILE_NAME);
        hs_file.printf("%s,%d\r\n",user_name.c_str(),score);
        hs_file.close();
    }
    else
    {
        ret = STATE_ERROR;
    }

    return ret;
}

/******************************************************************
 * @brief Read in the values stored in the highscore file
 * @param podium The structure to store the contents from the file
 * into
 * @return STATE_SUCCESS
******************************************************************/
state_code_t StateDriver::ReadHighScoreFile(podium_t& podium)
{
    state_code_t ret = STATE_SUCCESS;
 
    // Create string for file name 
    String fname(HS_FILE_NAME);

    // Open file
    File hs_file = SD.open(fname);

    int podium_idx = 0;

    if (hs_file)
    {
        log_printf("DRV: Reading from %s\n\r",HS_FILE_NAME);

        bool onFirst = true;
        podium.push_back(std::pair<std::string, std::string>());

        while (hs_file.available()) { // Reads a char at a time

            int new_char = hs_file.read();

            if (new_char == '\n') {
                podium_idx++;
                podium.push_back(std::pair<std::string, std::string>());
                onFirst = true;
            }
            else if (new_char == ',') {
                onFirst = !onFirst;
            }
            else if (onFirst) {
                podium[podium_idx].first += new_char;
            }
            else {
                podium[podium_idx].second += new_char;
            }
        }
        
        hs_file.close();
    }
    else
    {
        ret = STATE_ERROR;
    }

    return ret;
}

/******************************************************************
 * @brief Sort the highscores stored in the podium
 * @param podium A refernece to the podium struct containing the
 * highscores
 * @return STATE_SUCCESS if podium is not empty, otherwise
 * STATE_ERROR
******************************************************************/
state_code_t StateDriver::SortHighScores(podium_t &podium)
{
    state_code_t ret = STATE_SUCCESS;

    if (podium.empty())
    {
        ret = STATE_ERROR;
    }
    else
    {
        // Perform flagged bubble sort
        entry_t temp;
        int idx = podium.size() - 1;
        int sorted = 0;

		while ( !sorted )
		{
			sorted = 1;
			for  (int i = 0; i < idx; i++)
			{
				if  (atoi(podium[i].second.c_str()) < atoi(podium[i + 1].second.c_str()))
				{
					temp = podium[i];
					podium[i] = podium[i + 1];
					podium[i + 1] = temp;
					sorted = 0;
				}
			}
			idx--;
		}
    }

    return ret;
}
