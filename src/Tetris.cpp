#include "Tetris.h"

Tetris::Tetris() : m_round_num(1), m_gravity(1), m_mino_is_active(false)
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

void Tetris::StartGame()
{
    DisplayTetrisBoard();

    while(true)
    {
        if (!m_tetromino_queue.empty())
        {
            // Deploy active mino

            // Move active mino
            MoveTetromino('D');

            // Clear mino from queue
            m_tetromino_queue.pop();
        }
        else
        {
            EnqueueTetromino();
        }
    }
}

tetris_error_t Tetris::RequestMove(char direction)
{
    // Check if move request is valid (D, L, R)
    // and if moving will result in out of bounds

    tetris_error_t ret_code = TETRIS_ERR;


    
}

void Tetris::DisplayTetrisBoard()
{
    for (uint x = 0; x < TETRIS_HEIGHT; ++x)
    {
        for (uint y = 0; y < TETRIS_WIDTH; ++y)
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
    // Set active mino
    m_active_mino = m_tetromino_queue.front();

    // Add tetromino to board
    for(int x = 0; x < TETRIS_HEIGHT; ++x)
    {
        for(int y = 0; y < TETRIS_WIDTH; ++y)
        {
            if (m_active_mino.tetromino[x][y] != ' ') //PROBLEM - Incorrect indexes, need to add indexes for tetromino insteas
            {
                m_tetris_board[x][y] = m_active_mino.tetromino[x][y];
            }
        }
    }

    // Update board
    DisplayTetrisBoard();
    
 }

 tetris_error_t Tetris::MoveTetromino(char direction)
 {

    // Copy position of active tetromino
    tetromino_t temp_mino = m_active_mino;

    // Update the position using gravity or direction
    switch(direction)
    {
        case 'D':
            m_active_mino.y += m_gravity;
        break;

        case 'L':
            m_active_mino.x -= 1;
        break;

        case 'R':
            m_active_mino.x += 1;
        break;
    }

    // Remove old position

    return TETRIS_SUCCESS;
 }

 tetris_error_t Tetris::CollideTetromino()
 {
    // Add current position of active mino
    // into tetris board

 }
