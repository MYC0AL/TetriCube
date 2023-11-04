#include "Tetris.h"

void notify(int color)
{
    const int d = 1000;
    gfx->fillRect(3*48,3*48,48,48,color);
    delay(d);
    gfx->fillRect(3*48,3*48,48,48,BLACK);
    gfx->drawRect(3*48,3*48,48,48,TETRIS_EMPTY_COLOR);

}

void Tetris::SquareCheck(int row, int col)
{
    const int d = 100;
    int tempColor = CharToColor(m_tetris_board[row][col]);
    for(int i = 0; i < 2; i++)
    {
        gfx->fillRect(col*48,row*48,48,48,MAROON);
        delay(d);
        gfx->fillRect(col*48,row*48,48,48,tempColor);
        delay(d);
    }

}

Tetris::Tetris() : m_level(0), m_move_delay(1000), m_mino_is_active(false), m_mino_time(0)
{
    // Randomize seed
    srand(static_cast<unsigned int>(std::time(0)*2));

    // Initialize the tetris board to empty spaces
    for (uint row = 0; row < TETRIS_HEIGHT; ++row)
    {
        for (uint col = 0; col < TETRIS_WIDTH; ++col)
        {
            m_tetris_board[row][col] = AIR;
            if (row > TETRIS_HEIGHT-5 && col < TETRIS_WIDTH - 1)
                m_tetris_board[row][col] = 'G';

        }
    }
}

Tetris::~Tetris()
{
    gfx->fillScreen(BLACK);
}

tetris_error_t Tetris::PlayGame()
{
    tetris_error_t ret_code = TETRIS_SUCCESS;

    if (m_tetromino_queue.empty())
    {
        // Enqueue new random tetromino
        EnqueueTetromino();
    }

    // If the mino is active on the board
    if(m_mino_is_active)
    {
        // Delay between downward movements
        if (m_reset_mino_time_flag)
        {
            m_mino_time = millis();
            m_reset_mino_time_flag = false;
        }

        // DEBUG log_printf("%d\n",m_mino_time);
        if (millis() < m_mino_time + m_move_delay)
        {
            if (!m_moves.empty())
            {
                // Get next queued move
                char new_move = m_moves.front();

                // Check if it is valid
                tetris_error_t valid_move = RequestMove(new_move);

                // If it is valid, move the tetromino
                if (valid_move == TETRIS_SUCCESS)
                {
                    MoveTetromino(new_move);
                }
                else if (valid_move == TETRIS_MINO_COLLIDE)
                {
                    CollideTetromino();
                }

                // Valid or invalid, pop the move
                m_moves.pop();
            }
        }
        else //Time up to apply gravity
        {
            // Apply gravity to tetromino
            ApplyGravity();
            m_reset_mino_time_flag = true;
        }
    }
    else
    {
        ret_code = DeployTetromino();
    }

    return ret_code;
}

tetris_error_t Tetris::EnqueueMove(char direction)
{
    m_moves.push(direction);
    return TETRIS_SUCCESS;
}

tetris_error_t Tetris::GetTetrominoSize(tetromino_t mino, int& width, int& height)
{
    height = mino.tetromino.size();

    width = 0;

    for (int row = 0; row < m_active_mino.tetromino.size(); ++row) {
        if (m_active_mino.tetromino[row].size() > width) {
            width = m_active_mino.tetromino[row].size();
        }
    }

    return TETRIS_SUCCESS;
}

/**
 * @brief Verify that a given movement is valid
 * @param direction direction or rotation desired by user
 * @return Will return TETRIS_MINO_COLLIDE if collision happened
*/
tetris_error_t Tetris::RequestMove(char direction)
{
    tetris_error_t ret_code = TETRIS_SUCCESS;

    switch(direction)
    {
        case 'D':
            // Check if hitting lower bounds
            if (m_active_mino.y + TETRIS_GRAVITY + m_active_mino.tetromino.size() > TETRIS_HEIGHT)
                ret_code = TETRIS_MINO_COLLIDE;

            // Check if moving down would collide with a block
            for (int row = 0; row < m_active_mino.tetromino.size() && !ret_code; ++row) {
                for (int col = 0; col < m_active_mino.tetromino[row].size() && !ret_code; ++col) {
                    if (m_active_mino.tetromino[row][col] != AIR)
                    {
                        // Check the square below to see if it is air
                        if (m_tetris_board[row + m_active_mino.y + 1][col + m_active_mino.x] != AIR)
                        {
                            // If last row, and still collide, was not in the tetromino
                            if (row == m_active_mino.tetromino.size() - 1)
                            {
                                //DEBUG SquareCheck(row + m_active_mino.y + 1, col + m_active_mino.x);
                                ret_code = TETRIS_MINO_COLLIDE;
                            }
                            // Check if the block that isnt air, is apart of the tetromino
                            else if (m_active_mino.tetromino[row + 1][col] == AIR)
                            {
                                //DEBUG SquareCheck(row + m_active_mino.y + 1, col + m_active_mino.x);
                                ret_code = TETRIS_MINO_COLLIDE;
                            }
                        }
                    }
                }
            }
        break;

        case 'L':
            // Check if hitting left bounds
            if (m_active_mino.x <= 0)
                ret_code = TETRIS_ERR;

            for (int row = 0; row < m_active_mino.tetromino.size() && !ret_code; ++row) 
            {
                if (m_active_mino.tetromino[row].front() != AIR)
                {
                    if (m_tetris_board[row + m_active_mino.y][m_active_mino.x - 1] != AIR)
                    {
                        //DEBUG SquareCheck(m_active_mino.tetromino[row].size() + m_active_mino.x, row + m_active_mino.y);
                        ret_code = TETRIS_ERR;
                    }
                }
            }
        break;

        case 'R':
            // Check each row and verify that each one is not hitting wall
            for (int row = 0; row < m_active_mino.tetromino.size() && !ret_code; ++row) 
            {
                if (m_active_mino.tetromino[row].size() + m_active_mino.x >= TETRIS_WIDTH)
                {
                    ret_code = TETRIS_ERR;
                }
            }

            // Check if moving right would collide with a block
            for (int row = 0; row < m_active_mino.tetromino.size() && !ret_code; ++row) 
            {
                if (m_active_mino.tetromino[row].back() != AIR)
                {
                    if (m_tetris_board[row + m_active_mino.y][m_active_mino.tetromino[row].size() + m_active_mino.x] != AIR)
                    {
                        //DEBUG SquareCheck(m_active_mino.tetromino[row].size() + m_active_mino.x, row + m_active_mino.y);
                        ret_code = TETRIS_ERR;
                    }
                }
            }
        break;

        case '^':
        {
            // Check if the height and width would collide with the barrier, on all sides
            int height, width;
            GetTetrominoSize(m_active_mino, width, height);
            if (m_active_mino.x + height > TETRIS_HEIGHT || m_active_mino.y + width > TETRIS_WIDTH) {
                ret_code = TETRIS_ERR;
            }

            // Capture active mino color
            char active_color = ' ';

            // Replace active mino with ghost air
            for(int row = 0; row < m_active_mino.tetromino.size() && !ret_code; ++row)
            {
                for(int col = 0; col < m_active_mino.tetromino[row].size(); ++col)
                {
                    if (m_active_mino.tetromino[row][col] != AIR) {
                        // Update active color
                        active_color = m_tetris_board[row + m_active_mino.y][col + m_active_mino.x];

                        // Fill with ghost air
                        m_tetris_board[row + m_active_mino.y][col + m_active_mino.x] = GHOST_AIR;
                    }
                }
            }

            // Create ghost tetromino to rotate
            tetromino_t ghost = m_active_mino;

            // Rotate ghost
            if (!ret_code)
                RotateTetromino(ghost);

            // Check if rotated ghost overlaps with existing tetrominos
            for(int row = 0; row < ghost.tetromino.size() && !ret_code; ++row)
            {
                for(int col = 0; col < ghost.tetromino[row].size(); ++col)
                {
                    if (ghost.tetromino[row][col] != AIR) {
                        char check_sq = m_tetris_board[row + ghost.y][col + ghost.x];
                        if (check_sq != AIR && check_sq != GHOST_AIR) {
                            ret_code = TETRIS_ERR;
                        }
                    }
                }
            }

            // Fill ghost air with old mino color if rotation failed
            // Remove ghost air if rotation was successful
            for(int row = 0; row < TETRIS_HEIGHT; ++row)
            {
                for(int col = 0; col < TETRIS_WIDTH; ++col)
                {
                    if (m_tetris_board[row][col] == GHOST_AIR) 
                    {
                        if (ret_code)
                            m_tetris_board[row][col] = active_color;
                        else
                            m_tetris_board[row][col] = AIR;
                    }
                }
            }

            break;
        }
    }

    return ret_code;
}

void Tetris::DisplayTetrisBoard()
{
    for (uint row = 0; row < TETRIS_WIDTH; ++row)
    {
        for (uint col = 0; col < TETRIS_HEIGHT; ++col)
        {
            int decode_color = CharToColor(m_tetris_board[row][col]);
            int temp_color = (decode_color != TETRIS_ERR) ? decode_color : LIGHTGREY;

            if (temp_color != TETRIS_EMPTY_COLOR)
            {
                gfx->fillRect(col*TETRIS_SQ_PXL,
                              row*TETRIS_SQ_PXL,
                              TETRIS_SQ_PXL,
                              TETRIS_SQ_PXL,
                              temp_color);
            }
            else
            {
                // Clear old squares to black
                gfx->fillRect(col*TETRIS_SQ_PXL,
                              row*TETRIS_SQ_PXL,
                              TETRIS_SQ_PXL,
                              TETRIS_SQ_PXL,
                              BLACK);

                // Draw new wire frame square
                gfx->drawRect(col*TETRIS_SQ_PXL,
                              row*TETRIS_SQ_PXL,
                              TETRIS_SQ_PXL,
                              TETRIS_SQ_PXL,
                              temp_color);
            }
        }
    }
}

int Tetris::CharToColor(char color)
{
    static const std::unordered_map<char, int> colorMap = {
        {'C', CYAN},
        {'Y', YELLOW},
        {'P', PURPLE},
        {'N', NAVY},
        {'O', ORANGE},
        {'G', GREEN},
        {'R', RED},
        {AIR, TETRIS_EMPTY_COLOR}
    };

    auto it = colorMap.find(color);
    if (it != colorMap.end()) {
        return it->second;
    }

    return TETRIS_ERR;
}

tetris_error_t Tetris::EnqueueTetromino()
{
    tetris_error_t ret_code = TETRIS_ERR;

    // Add random tetromino to queue
    if (m_tetromino_queue.size() < TETRIS_MAX_QUEUE)
    {
        m_tetromino_queue.push(ALL_MINOS[rand() % TETROMINO_COUNT]);
        ret_code = TETRIS_SUCCESS;
    }

    return ret_code;
}

tetris_error_t Tetris::ApplyGravity()
{
    if (m_moves.empty())
    {
        EnqueueMove('D');
    }
    else
    {
        // Change first element to be a 'D'
        m_moves.front() = 'D';
    }

    return TETRIS_SUCCESS;
}

tetris_error_t Tetris::DeployTetromino()
{
    tetris_error_t ret_code = TETRIS_SUCCESS;

    // Clear old queued moves
    while(!m_moves.empty()) m_moves.pop();

    // Check if line is full
    vector<int> filled_lines;
    if (CheckFullLines(filled_lines) == TETRIS_SUCCESS)
    {
        // Clear Full lines
        ClearFullLines(filled_lines);
    }

    // Check if game should end
    if (CheckGame(m_tetromino_queue.front()) == TETRIS_END_GAME)
    {
        ret_code = TETRIS_END_GAME;
    };

    // Clear old active mino
    m_active_mino = {};

    // Set active mino from queue
    m_active_mino = m_tetromino_queue.front();

    // Pop the front mino
    m_tetromino_queue.pop();

    // Update virtual board
    UpdateBoard();

    // Set active mino flag
    m_mino_is_active = true;

    // Display board
    DisplayTetrisBoard();

    return ret_code;
}

/*********************************************************
 * @brief Rotate 'mino' 90 degrees in 'dir' direction
 * @param dir Direction to rotate in
 * @param mino Mino to preform rotation on
 ********************************************************/
tetris_error_t Tetris::RotateTetromino(tetromino_t& mino)
{
    tetris_error_t ret = TETRIS_SUCCESS;

    int rows = mino.tetromino.size();
    int cols = mino.tetromino[0].size();

    // create new vector with swapped dimensions
    std::vector<std::vector<char>> temp(cols, std::vector<char>(rows));

    // populate the new vector
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            temp[c][rows-r-1] = mino.tetromino[r][c];
        }
    }
    
    mino.tetromino = temp;

    return ret;
}

tetris_error_t Tetris::MoveTetromino(char direction)
{
    // Clear old tetromino
    ClearTetromino();

    // Update the position using gravity or direction
    switch(direction)
    {
        case 'D':
            m_active_mino.y += TETRIS_GRAVITY;
        break;

        case 'L':
            m_active_mino.x -= 1;
        break;

        case 'R':
            m_active_mino.x += 1;
        break;

        // If a rotation, fall through and call rotate on active mino
        case '^':
            RotateTetromino(m_active_mino);
        break;

    }

    // Update board
    UpdateBoard();

    // Display board
    DisplayTetrisBoard();

    return TETRIS_SUCCESS;
}

tetris_error_t Tetris::ClearTetromino()
{
    // Clear active tetromino on board
    for(int row = 0; row < m_active_mino.tetromino.size(); ++row)
    {
        for(int col = 0; col < m_active_mino.tetromino[row].size(); ++col)
        {
            if (m_active_mino.tetromino[row][col] != AIR)
            {
                m_tetris_board[row+m_active_mino.y][col+m_active_mino.x] = AIR;
            }
        }
    }
    return TETRIS_SUCCESS;
}

tetris_error_t Tetris::UpdateBoard()
{
    // Use active tetrimino and update pos on board
    for(int row = 0; row < m_active_mino.tetromino.size(); ++row)
    {
        for(int col = 0; col < m_active_mino.tetromino[row].size(); ++col)
        {
            if (m_active_mino.tetromino[row][col] != AIR)
            {
                m_tetris_board[row+m_active_mino.y][col+m_active_mino.x] = m_active_mino.tetromino[row][col];
            }
        }
    }
    return TETRIS_SUCCESS;
}

tetris_error_t Tetris::CollideTetromino()
{
    // Reset active mino flag which 
    // adds the mino to the tetris board
    m_mino_is_active = false;

    return TETRIS_SUCCESS;
}


// Check if piece is on board when a new piece spawns in
tetris_error_t Tetris::CheckGame(tetromino_t new_mino)
{
    tetris_error_t ret_code = TETRIS_SUCCESS;

    for (int row = 0; row < new_mino.tetromino.size(); row++) {
        for (int col = 0; col < new_mino.tetromino[row].size(); col++) {
            if (m_tetris_board[row + new_mino.y][col + new_mino.x] != AIR) {
                ret_code = TETRIS_END_GAME;
            }
        }
    }

    return ret_code;
}

/**
 * @brief Check the current tetris board for full lines
 * @param filled_lines A vector of ints that will populate 
 * with the index of the lines that are filled
 * @return TETRIS_SUCCESS if 1 or more lines full,
 * TETRIS_ERR if 0 lines full 
*/
tetris_error_t Tetris::CheckFullLines(vector<int>& filled_lines)
{
    tetris_error_t ret_code = TETRIS_ERR;

    filled_lines.clear();

    for(int row = 0; row < TETRIS_HEIGHT; ++row)
    {
        bool lineFull = true;
        for(int col = 0; col < TETRIS_WIDTH && lineFull; ++col)
        {
            if (m_tetris_board[row][col] == AIR)
                lineFull = false;
        }
        if (lineFull)
        {
            filled_lines.push_back(row);
            ret_code = TETRIS_SUCCESS;
        }
    }
    return ret_code;
}

/**
 * @brief Clear all full lines and shift down minos
 * @param filled_lines A vector<int> containing filled lines index
 * @return Always return TETRIS_SUCCESS
*/
tetris_error_t Tetris::ClearFullLines(vector<int> filled_lines)
{
    // For each filled line
    for(int i = 0; i < filled_lines.size(); ++i)
    {
        for(int row = filled_lines[i]; row > 0 ; --row)
        {
            for(int col = 0; col < TETRIS_WIDTH; ++col)
                m_tetris_board[row][col] = m_tetris_board[row-1][col];

        }
        
        // Fill in row[0] with AIR
        for(int j = 0; j < TETRIS_WIDTH; ++j)
        {
            m_tetris_board[0][j] = AIR;
        }
    }

    // Update score
    UpdateScore(filled_lines.size());
    
    return TETRIS_SUCCESS;
}

/**
 * @brief Update score based on the combo of lines cleared.
 * The higher the level, the more points earned
 * @param rowsCleared Number of rows cleared
 * @return Always return TETRIS_SUCCESS
*/
tetris_error_t Tetris::UpdateScore(int rowsCleared)
{
    int score_mult = 0;
    switch(rowsCleared)
    {
        case 1:
            score_mult = 40;
            break;
        case 2:
            score_mult = 100;
            break;
        case 3:
            score_mult + 300;
            break;
        case 4:
            score_mult + 1200;
            break;
    }
    m_score += score_mult * (m_level + 1);
    log_printf("SCORE: %d", m_score);
    return TETRIS_SUCCESS;
}
