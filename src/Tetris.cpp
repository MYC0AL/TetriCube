#include "Tetris.h"

void notify(int color)
{
    const int d = 1000;
    gfx->fillRect(3*48,3*48,48,48,color);
    delay(d);
    gfx->fillRect(3*48,3*48,48,48,BLACK);
    gfx->drawRect(3*48,3*48,48,48,TETRIS_EMPTY_COLOR);

}

Tetris::Tetris() : m_round_num(1), m_move_delay(1000), m_mino_is_active(false)
{
    // Randomize seed
    srand(static_cast<unsigned int>(std::time(0)));

    // Initialize the tetris board to empty spaces
    for (uint x = 0; x < TETRIS_WIDTH; ++x)
    {
        for (uint y = 0; y < TETRIS_HEIGHT; ++y)
        {
            m_tetris_board[x][y] = ' ';
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

        // Clear old queued moves
        while(!m_moves.empty()) m_moves.pop();

        // Check if game should end
        if (CheckGame(m_tetromino_queue.front()) == TETRIS_END_GAME)
        {
            return TETRIS_END_GAME;
        };

        // Deploy new mino from queue
        DeployTetromino();

        // While the mino is active on the board
        while(m_mino_is_active)
        {
            // Delay between downward movements
            long curr_time = millis();

            // Empty out the move queue or wait for delay
            while(millis() < curr_time + m_move_delay)
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

            // Apply gravity to tetromino
            EnqueueMove('D');
            EnqueueMove('R');
        }

        // Clear mino from queue
        m_tetromino_queue.pop();
    }

    return ret_code;
}

tetris_error_t Tetris::EnqueueMove(char direction)
{
    m_moves.push(direction);
    return TETRIS_SUCCESS;
}

tetris_error_t Tetris::RequestMove(char direction)
{
    // Check if move request is valid (D, L, R)
    // and if moving will result in out of bounds
    // Will return TETRIS_MINO_COLLIDE if collision happened
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
                    if (downCollision(row, col)) {
                        ret_code = TETRIS_MINO_COLLIDE;
                    }
                }
            }
        break;

        case 'L':
            // Check if hitting left bounds
            if (m_active_mino.x <= 0)
                ret_code = TETRIS_ERR;

            // Check if moving left would collide with a block
            for (int col = 0; col < m_active_mino.tetromino[col].size() && !ret_code; ++col) {
                for (int row = 0; row < m_active_mino.tetromino.size() && !ret_code; ++row) {
                    if (m_active_mino.tetromino[row][col] != ' ')
                    {
                        if (col == m_active_mino.tetromino[col].size())
                        {
                            if (m_tetris_board[row + m_active_mino.y][col + m_active_mino.x - 1 ] != ' ')
                                ret_code = TETRIS_MINO_COLLIDE;
                        }
                        else if (m_active_mino.tetromino[row][col-1] == ' ')
                        {
                            if (m_tetris_board[row + m_active_mino.y + 1][col + m_active_mino.x] != ' ')
                                ret_code = TETRIS_MINO_COLLIDE;
                        }
                    }
                    
                }
            }
        break;

        case 'R':
            // Check each row and verify that each one is not hitting wall
            for(int row = 0; row < m_active_mino.tetromino.size(); ++row)
            {
                for (int col = 0; col < m_active_mino.tetromino[row].size(); ++col)
                {
                    if (m_active_mino.x + m_active_mino.tetromino[row].size() >= TETRIS_WIDTH)
                    {
                        ret_code = TETRIS_ERR;
                    }
                }
            }

            // Check if moving right would collide with a block
            for (int col = 0; col < m_active_mino.tetromino[col].size() && !ret_code; ++col) {
                for (int row = 0; row < m_active_mino.tetromino.size() && !ret_code; ++row) {
                    if (m_active_mino.tetromino[row][col] != ' ')
                    {
                        if (col == m_active_mino.tetromino[col].size())
                        {
                            if (m_tetris_board[row + m_active_mino.y][col + m_active_mino.x + 1 ] != ' ')
                                ret_code = TETRIS_MINO_COLLIDE;
                        }
                        else if (m_active_mino.tetromino[row][col+1] == ' ')
                        {
                            if (m_tetris_board[row + m_active_mino.y + 1][col + m_active_mino.x] != ' ')
                                ret_code = TETRIS_MINO_COLLIDE;
                        }
                    }
                    
                }
            }
        break;
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
        {' ', TETRIS_EMPTY_COLOR}
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

 tetris_error_t Tetris::DeployTetromino()
 {
    tetris_error_t ret_code = TETRIS_SUCCESS;

    // Clear old active mino
    m_active_mino = {};

    // Set active mino from queue
    m_active_mino = m_tetromino_queue.front();

    // Update virtual board
    UpdateBoard();

    // Set active mino flag
    m_mino_is_active = true;

    // Display board
    DisplayTetrisBoard();

    return ret_code;
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
            if (m_active_mino.tetromino[row][col] != ' ')
            {
                m_tetris_board[row+m_active_mino.y][col+m_active_mino.x] = ' ';
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
            if (m_active_mino.tetromino[row][col] != ' ')
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
            if (m_tetris_board[row + new_mino.y + 1][col + new_mino.x] != ' ') {
                ret_code = TETRIS_END_GAME;
            }
        }
    }

    return ret_code;
 }

bool Tetris::downCollision(int row, int col) {

    bool collided = false;

    if (row == m_active_mino.tetromino.size() - 1)
    {
        collided = m_tetris_board[row + m_active_mino.y + 1][col + m_active_mino.x] != ' ';
    }
    else
    {
        collided = m_active_mino.tetromino[row][col] != ' ' && m_active_mino.tetromino[row + 1][col] == ' ' &&
           m_tetris_board[row + m_active_mino.y + 1][col + m_active_mino.x] != ' ';
    }
    return collided;
}

bool Tetris::leftCollision(int row, int col)
{
    bool collided = false;

    if (row == m_active_mino.tetromino.size() - 1)
    {
        collided = m_tetris_board[row + m_active_mino.y + 1][col + m_active_mino.x] != ' ';
    }
    else
    {
        collided = m_active_mino.tetromino[row][col] != ' ' && m_active_mino.tetromino[row + 1][col] == ' ' &&
           m_tetris_board[row + m_active_mino.y + 1][col + m_active_mino.x] != ' ';
    }
    return collided;
}

/**
 * Handle the dev being a dummy.
 * Prevent constant reset. 
*/
 void Tetris::DummyHandler()
 {
    gfx->fillScreen(WHITE);
    gfx->setCursor(gfx->width() / 2, gfx->height() / 2);
    gfx->print("Dummy handler");
    while(1);
 }
