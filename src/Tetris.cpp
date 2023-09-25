#include "Tetris.h"

Tetris::Tetris() : m_round_num(1), m_move_delay(1000), m_mino_is_active(false)
{
    //TODO: Randomize the seed

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

void Tetris::PlayGame()
{
    if (m_tetromino_queue.empty())
    {
        // Enqueue new random tetromino
        EnqueueTetromino();

        // Deploy new mino from queue
        DeployTetromino();

        // While the mino is active on the board
        while(m_mino_is_active)
        {
            // Delay between downward movements
            ulong curr_time = millis();

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
            RequestMove('D');
        }

        // Clear mino from queue
        m_tetromino_queue.pop();
    }
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
            // Check if moving down would collide with a block
            for(int row = 0; row < TETROMINO_WIDTH && !ret_code; ++row)
            {
                for(int col = 0; col < TETROMINO_HEIGHT && !ret_code; ++row)
                {
                    // Check if hitting lower bounds
                    if (m_active_mino.y + 1 > TETRIS_HEIGHT - 1)
                        ret_code = TETRIS_MINO_COLLIDE;

                    // Iterate over each row first
                    else if (m_active_mino.tetromino[row][col] != ' ' &&
                            m_tetris_board[row+m_active_mino.x - 1][col+m_active_mino.y] != ' ')
                    {
                        
                    }
                }
            }
        break;

        case 'L':
            // Check if moving to the left would hit a block
            for (int x = 0; x < TETROMINO_WIDTH && !ret_code; ++x)
            {
                for (int y = 0; y < TETROMINO_HEIGHT && !ret_code; ++y)
                {
                    // Check if move would be in bounds
                    if (m_active_mino.x - 1 < 0)
                        ret_code = TETRIS_ERR;

                    // Iterate over each col first 
                    else if (m_active_mino.tetromino[y][x] != ' ' &&
                            m_tetris_board[y+m_active_mino.y][x-1+m_active_mino.x] != ' ')
                    {
                        // The tetromino square pos and the board pos -1 both have a square,
                        // check if that square is apart of the tetromino. If it is not, than
                        // a barrier is reached.
                        if (m_active_mino.tetromino[y][x-1] != ' ')
                            ret_code = TETRIS_ERR;
                    }
                }
            }
        break;

        case 'R':
            // Check if moving out of bounds
            if (m_active_mino.x + 1 > TETRIS_HEIGHT-1)
            {
                ret_code = TETRIS_ERR;
            }
        break;
    }

    return ret_code;
}

void Tetris::DisplayTetrisBoard()
{
    for (uint x = 0; x < TETRIS_WIDTH; ++x)
    {
        for (uint y = 0; y < TETRIS_HEIGHT; ++y)
        {
            int decode_color = CharToColor(m_tetris_board[x][y]);
            int temp_color = (decode_color != TETRIS_ERR) ? decode_color : LIGHTGREY;

            if (temp_color != TETRIS_EMPTY_COLOR)
            {
                gfx->fillRect(x*TETRIS_SQ_PXL,
                              y*TETRIS_SQ_PXL,
                              TETRIS_SQ_PXL,
                              TETRIS_SQ_PXL,
                              temp_color);
            }
            else
            {
                // Clear old squares to black
                gfx->fillRect(x*TETRIS_SQ_PXL,
                              y*TETRIS_SQ_PXL,
                              TETRIS_SQ_PXL,
                              TETRIS_SQ_PXL,
                              BLACK);

                // Draw new wire frame square
                gfx->drawRect(x*TETRIS_SQ_PXL,
                              y*TETRIS_SQ_PXL,
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
        m_tetromino_queue.push(ALL_MINOS[rand() % TETROMINO_COUNT+1]);
        ret_code = TETRIS_SUCCESS;
    }

    return ret_code;
 }

 tetris_error_t Tetris::DeployTetromino()
 {
    // Set active mino from queue
    m_active_mino = m_tetromino_queue.front();

    UpdateBoard(TETROMINO_START_POS_X,TETROMINO_START_POS_Y);

    // Set active mino flag
    m_mino_is_active = true;

    // Update board
    DisplayTetrisBoard();
    
    return TETRIS_SUCCESS;
 }

 tetris_error_t Tetris::MoveTetromino(char direction)
 {

    // Copy position of active tetromino
    tetromino_t temp_mino = m_active_mino;

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
    UpdateBoard(m_active_mino.x,m_active_mino.y);

    // Display board
    DisplayTetrisBoard();

    return TETRIS_SUCCESS;
 }

 tetris_error_t Tetris::UpdateBoard(int start_pos_x, int start_pos_y)
 {
    // Use active tetrimino and update pos on board
    for(int x = 0; x < TETROMINO_WIDTH; ++x)
    {
        for(int y = 0; y < TETROMINO_HEIGHT; ++y)
        {
            if (m_active_mino.tetromino[x][y] != ' ')
            {
                m_tetris_board[x+start_pos_x][y+start_pos_y] = m_active_mino.tetromino[x][y];
            }
        }
    }
    return TETRIS_SUCCESS;
 }

 tetris_error_t Tetris::CollideTetromino()
 {
    // Add current position of active mino
    // into tetris board

    // Reset active mino flag
    m_mino_is_active = false;

    return TETRIS_SUCCESS;
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
