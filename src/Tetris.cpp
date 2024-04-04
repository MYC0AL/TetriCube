#include "Tetris.h"

/******************************************************************
 * @brief Constructor that initializes tetris board
******************************************************************/
Tetris::Tetris() : m_level(0), m_move_delay(1000), m_mino_is_active(false),
                   m_mino_time(0), m_score(0), m_total_rows_cleared(0), m_update_ready(NONE),
                   m_reset_mino_time_flag(true)
{
    // Initialize the tetris board to empty spaces
    for (uint row = 0; row < TETRIS_HEIGHT; ++row)
    {
        for (uint col = 0; col < TETRIS_WIDTH; ++col)
        {
            m_tetris_board[row][col] = AIR;
        }
    }
}

/******************************************************************
 * @brief Tetris dtor. Clears screen.
******************************************************************/
Tetris::~Tetris()
{
    gfx->fillScreen(BLACK);
}

/******************************************************************
 * @brief Set the side number of the cube for tetris
 * @param screen_num The desired side number
******************************************************************/
void Tetris::SetSideNum(int screen_num)
{
    m_screen_num = screen_num;
    log_printf("TETRIS: SIDE NUM SET TO %d\n\r",m_screen_num);

    // Map new subsection
    MapSubsection();
}

/******************************************************************
 * @brief Get the status of the update flag
 * @param ret_queue Queue to store all updated partitions
 * @return Status of the update flag
******************************************************************/
std::queue<int> Tetris::UpdatedPartitions()
{
    // DEBUG
    CLEAR_QUEUE(m_updated_partitions)
    m_updated_partitions.push(0);
    m_updated_partitions.push(1);
    m_updated_partitions.push(2);
    m_updated_partitions.push(3);

    return m_updated_partitions;
}

/******************************************************************
 * @brief Get the status of the update flag
 * @return Status of the update flag
******************************************************************/
update_t Tetris::UpdateStatus()
{
    update_t tempStatus = m_update_ready;
    m_update_ready = NONE;
    return tempStatus;
}

/******************************************************************
 * @brief Main control function to progress the game
 * @return TETRIS_END_GAME if Tetris is failed, otherwise
 * TETRIS_SUCCESS
******************************************************************/
tetris_error_t Tetris::PlayGame()
{
    tetris_error_t ret_code = TETRIS_SUCCESS;

    if (m_next_tetromino.tetromino.empty())
    {
        // Set next new random tetromino
        SetNextTetromino();
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
        if (millis() < m_mino_time + m_move_delay)
        {
            // Check if next move is valid
            tetris_error_t valid_move = RequestMove(m_next_move);

            // If it is valid, move the tetromino
            if (valid_move == TETRIS_SUCCESS)
            {
                MoveTetromino(m_next_move);
            }
            else if (valid_move == TETRIS_MINO_COLLIDE)
            {
                CollideTetromino();
            }

            // Clear m_next_move
            m_next_move = 0;
        }
        else // Time up to apply gravity
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

/******************************************************************
 * @brief Enqueue the next desired Tetris move
 * @param direction A char representing the direction of movement
 * @return TETRIS_SUCCESS
******************************************************************/
tetris_error_t Tetris::SetNextMove(char direction)
{
    m_next_move = direction;
    return TETRIS_SUCCESS;
}

/******************************************************************
 * @brief Determine if provided tetromino is rotated
 * @param mino Tetromino to determine the rotation of
 * @return True if tetromino is rotated, false if in
 * default position
******************************************************************/
bool Tetris::IsTetrominoRotated(tetromino_t mino)
{

    // Get dimensions of mino
    int new_height, new_width;
    GetTetrominoSize(mino, new_width, new_height);

    // Get dimensions of default mino;
    int height, width;
    GetTetrominoSize(ALL_MINOS[mino.idx],width,height);

    return !(height == new_height && width == new_width);
}

/******************************************************************
 * @brief Reset Tetris to default values
 * @return TETRIS_SUCCESS
******************************************************************/
tetris_error_t Tetris::Reset()
{
    m_level = 0;
    m_move_delay = 1000;
    m_mino_is_active = false;
    m_mino_time = 0;
    m_score = 0;
    m_active_mino = {};
    m_next_tetromino = {};
    m_total_rows_cleared = 0;
    m_update_ready = NONE;
    m_rotation_count = 0;

    // Reset the tetris board to empty spaces
    for (uint row = 0; row < TETRIS_HEIGHT; ++row)
    {
        for (uint col = 0; col < TETRIS_WIDTH; ++col)
        {
            m_tetris_board[row][col] = AIR;
        }
    }

    CLEAR_QUEUE(m_updated_partitions)

    log_printf("TETRIS: Resetting...\n\r");
    return TETRIS_SUCCESS;
}

/******************************************************************
 * @brief Get the current rotation count of the current tetromino
 * @return Current rotation of the active tetromino
******************************************************************/
int Tetris::GetRotationCount()
{
    return m_rotation_count;
}

/******************************************************************
 * @brief Get the current score of Tetris
 * @return Current Tetris score
******************************************************************/
unsigned long Tetris::GetScore()
{
    return m_score;
}

/******************************************************************
 * @brief Get the current level of Tetris
 * @return Current Tetris level
******************************************************************/
unsigned int Tetris::GetLevel()
{
    return m_level;
}

/*****************************************************************
 * @brief Get the current active tetromino position
 * @param mino Tetromino to store the active tetromino data
 * @return TETRIS_SUCCESS
******************************************************************/
tetris_error_t Tetris::GetTetromino(tetromino_t &mino)
{
    mino = m_active_mino;
    return TETRIS_SUCCESS;
}

/*****************************************************************
 * @brief Set the current active tetromino
 * @param mino Tetromino containing new tetromino data
 * @return TETRIS_SUCCESS
******************************************************************/
tetris_error_t Tetris::SetTetromino(tetromino_t mino)
{
    m_active_mino = mino;
    
    return TETRIS_SUCCESS;
}

/*****************************************************************
 * @brief Get the current tetris board
 * @param tetris_board Board where the active board will be stored
 * @param partition Section of board to get
 * @return TETRIS_SUCCESS
******************************************************************/
tetris_error_t Tetris::GetBoard(char tetris_board[TETRIS_HEIGHT/4][TETRIS_WIDTH], int partition)
{
    int partition_size = (TETRIS_HEIGHT/4 * partition);
    for(int row = 0; row < TETRIS_HEIGHT/4; row++)
    {
        for(int col = 0; col < TETRIS_WIDTH; col++)
        {
            tetris_board[row][col] = m_tetris_board[row+partition_size][col];
        }
    }
    return TETRIS_SUCCESS;
}

/*****************************************************************
 * @brief Set the current tetris board
 * @param tetris_board Board containing the info for the master
 * board
 * @param partition Section of board to set
 * @return TETRIS_SUCCESS
******************************************************************/
tetris_error_t Tetris::SetBoard(char tetris_board[TETRIS_HEIGHT/4][TETRIS_WIDTH], int partition)
{
    int partition_size = (TETRIS_HEIGHT/4 * partition);
    for(int row = 0; row < TETRIS_HEIGHT/4; row++)
    {
        for(int col = 0; col < TETRIS_WIDTH; col++)
        {
            m_tetris_board[row+partition_size][col] = tetris_board[row][col];
        }
    }
    return TETRIS_SUCCESS;
}

/******************************************************************
 * @brief Get the active tetromino's size
 * @param mino The tetromino_t object that's size is desired
 * @param width The place where the width will be stored
 * @param height The place where the height will be stored
 * @return TETRIS_SUCCESS
******************************************************************/
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

/******************************************************************
 * @brief Verify that a given movement is valid
 * @param direction direction or rotation desired by user
 * @return Will return TETRIS_MINO_COLLIDE if collision happened
******************************************************************/
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
                                ret_code = TETRIS_MINO_COLLIDE;
                            }
                            // Check if the block that isnt air, is apart of the tetromino
                            else if (m_active_mino.tetromino[row + 1][col] == AIR)
                            {
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
                if (m_tetris_board[row + m_active_mino.y][m_active_mino.x - 1] != AIR)
                {
                    if (m_active_mino.tetromino[row].front() != AIR)
                    {
                        ret_code = TETRIS_ERR;
                    }
                    else if (m_tetris_board[row + m_active_mino.y][m_active_mino.x] != AIR)
                    {
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
                if (m_tetris_board[row + m_active_mino.y][m_active_mino.x + m_active_mino.tetromino[row].size()] != AIR)
                {
                    if (m_active_mino.tetromino[row].back() != AIR)
                    {
                        ret_code = TETRIS_ERR;
                    }
                    else if (m_tetris_board[row + m_active_mino.y][m_active_mino.x + m_active_mino.tetromino[row].size() - 1] != AIR)
                    {
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
            if (m_active_mino.x + height > TETRIS_WIDTH || m_active_mino.y + width > TETRIS_HEIGHT) {
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
        case 0:
        {
            ret_code = TETRIS_ERR;
        }
        default:
            ret_code = TETRIS_ERR;
    }

    return ret_code;
}

/******************************************************************
 * @brief Display the current Tetris board to the screen
******************************************************************/
void Tetris::DisplayTetrisBoard()
{
    if (m_screen_num != 1 && m_screen_num != 3)
    {
        for (uint row = 0; row < TETRIS_DISP_HEIGHT; ++row)
        {
            for (uint col = 0; col < TETRIS_WIDTH; ++col)
            {
                int decode_color = CharToColor(m_tetris_board[row + m_subsection][col]);
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
}

/******************************************************************
 * @brief Convert a character to a color
 * @param color The color char to convert
 * @return The integer value of the converted color
******************************************************************/
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

/******************************************************************
 * @brief Enqueue a tetromino into the queue which
 * deploys upcoming tetrominos
 * @return TETRIS_SUCCESS if there is room in the queue otherwise
 * TETRIS_ERR
******************************************************************/
tetris_error_t Tetris::SetNextTetromino()
{
    // Add random tetromino to queue
    m_next_tetromino = ALL_MINOS[rand() % TETROMINO_COUNT];
    return TETRIS_SUCCESS;
}

/******************************************************************
 * @brief Apply gravity by enqueueing a downward movement call.
 * Change the first element in the move queue to be a downward
 * movement.
 * @return TETRIS_SUCCESS
******************************************************************/
tetris_error_t Tetris::ApplyGravity()
{
    SetNextMove('D');

    return TETRIS_SUCCESS;
}

/******************************************************************
 * @brief Deploy the next queued tetromino onto the screen
 * @return TETRIS_END_GAME if deployed tetromino collides with
 * existing tetromino, otherwise TETRIS_SUCCESS
******************************************************************/
tetris_error_t Tetris::DeployTetromino()
{
    tetris_error_t ret_code = TETRIS_SUCCESS;

    // Clear next move
    m_next_move = 0;

    // Check if line is full
    vector<int> filled_lines;
    if (CheckFullLines(filled_lines) == TETRIS_SUCCESS)
    {
        // Clear Full lines
        ClearFullLines(filled_lines);
    }

    // Check if game should end
    if (CheckGame(m_next_tetromino) == TETRIS_END_GAME)
    {
        ret_code = TETRIS_END_GAME;
    };

    // Clear old active mino
    m_active_mino = {};

    // Set active mino from queue
    m_active_mino = m_next_tetromino;

    // Find new m_next_tetromino
    SetNextTetromino();

    // Update virtual board
    UpdateBoard();

    // Set active mino flag
    m_mino_is_active = true;

    // Set update flag to update whole board
    log_printf(" --------------- In deploy, setting flag to ALL\r\n");
    m_update_ready = ALL;

    // Display board
    DisplayTetrisBoard();

    // Reset external positional update counter 
    pos_update_counter = 0;

    // Reset rotation counter
    m_rotation_count = 0;

    // Delay to allow all previous commands to complete
    delay(100);

    return ret_code;
}

/*********************************************************
 * @brief Rotate 'mino' 90 degrees clockwise
 * @param mino Mino to preform rotation on
 * @return TETRIS_SUCCESS
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

/******************************************************************
 * @brief Move a tetromino to the desired direction
 * @param direction The desired direction to move the
 * tetromino
 * @return TETRIS_SUCCESS
******************************************************************/
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

        case '^':
            RotateTetromino(m_active_mino);
            m_rotation_count = (m_rotation_count + 1) % 4;

        break;
    }

    // Check if new position is between partitions
    int mino_width, mino_height;
    const int partition_size = (TETRIS_HEIGHT/TETRIS_DISPLAYS);
    GetTetrominoSize(m_active_mino, mino_width, mino_height);

    int yPartition = (m_active_mino.y - 1) / (partition_size);
    int hPartition = (m_active_mino.y + mino_height - 1) / (partition_size);

    if (yPartition != hPartition)
    {
        m_updated_partitions.push(hPartition);
    }
        m_updated_partitions.push(yPartition);

    // Update board
    UpdateBoard();

    // Display board
    DisplayTetrisBoard();

    if(m_screen_num == 0)
    {
        if (pos_update_counter + 1 == POS_UPDATE_REFRESH_COUNT)
        {
            m_update_ready = NEW_POS;
            log_printf(" --------------- In move, setting flag to NEW_POS\r\n");

        }
        pos_update_counter = (pos_update_counter + 1) % POS_UPDATE_REFRESH_COUNT;
    }

    return TETRIS_SUCCESS;
}

/******************************************************************
 * @brief Clear the active tetromino from the board
 * @return TETRIS_SUCCESS
******************************************************************/
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

/******************************************************************
 * @brief Update where the tetromino is on the board
 * @return TETRIS_SUCCESS
******************************************************************/
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

/******************************************************************
 * @brief Reset the active flag for a tetromino
 * @return TETRIS_SUCCESS
******************************************************************/
tetris_error_t Tetris::CollideTetromino()
{
    // Reset active mino flag which 
    // adds the mino to the tetris board
    m_mino_is_active = false;

    return TETRIS_SUCCESS;
}

/******************************************************************
 * @brief Check the game to make sure a new tetromino
 * didnt collide with an existing tetromino
 * @return TETRIS_END_GAME if collision detected,
 * otherwise TETRIS_SUCCESS
******************************************************************/
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

/******************************************************************
 * @brief Check the current tetris board for full lines
 * @param filled_lines A vector of ints that will populate 
 * with the index of the lines that are filled
 * @return TETRIS_SUCCESS if 1 or more lines full,
 * TETRIS_ERR if 0 lines full 
******************************************************************/
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

    // Update modified partition queue
    if (ret_code == TETRIS_SUCCESS)
    {
        for(int i = 0; i < TETRIS_DISPLAYS; i++) {
            m_updated_partitions.push(i);
        }
        log_printf(" --------------- In check lines, setting flag to ALL\r\n");
        m_update_ready = ALL;
    }
    return ret_code;
}

/******************************************************************
 * @brief Clear all full lines and shift down minos
 * @param filled_lines A vector<int> containing filled lines index
 * @return Always return TETRIS_SUCCESS
******************************************************************/
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

    // Update Row Clear Counter
    m_total_rows_cleared += filled_lines.size();
    
    // Check for level up
    CheckLevelUp();

    return TETRIS_SUCCESS;
}

/******************************************************************
 * @brief Check if Tetris needs to level up
 * @return TETRIS_SUCCESS
******************************************************************/
tetris_error_t Tetris::CheckLevelUp()
{
    // Level changes update level and
    unsigned int new_level = m_total_rows_cleared / 10;
    if (new_level != m_level)
    {
        m_level = new_level;
        
        // Update speed
        if (m_level < 10) {
            m_move_delay -= 100;
        }
        
        log_printf("TETRIS: Level up: %d  Speed: %d\n\r",m_level,m_move_delay);
    }

    return TETRIS_SUCCESS;
}

/******************************************************************
 * @brief Update score based on the combo of lines cleared.
 * The higher the level, the more points earned
 * @param rowsCleared Number of rows cleared
 * @return Always return TETRIS_SUCCESS
******************************************************************/
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
    log_printf("Tetris: SCORE: %d\n\r", m_score);
    return TETRIS_SUCCESS;
}

/******************************************************************
 * @brief Decide which Tetris subsection a given 
 * screen contains
 * @return TETRIS_SUCCESS
******************************************************************/
tetris_error_t Tetris::MapSubsection()
{
    m_subsection = 0;

    if (m_screen_num != 1 && m_screen_num != 3)
    {
        switch(m_screen_num)
        {
            case 4: m_subsection += TETRIS_DISP_HEIGHT; 
            case 0: m_subsection += TETRIS_DISP_HEIGHT; 
            case 2: m_subsection += TETRIS_DISP_HEIGHT;
            case 5: break;
        }

        // DEBUG
        log_printf("TETRIS: Subsection: %d\n\r",m_subsection);
    }
    
    return TETRIS_SUCCESS;
}
