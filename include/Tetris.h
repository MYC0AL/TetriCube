#ifndef TETRIS_H
#define TETRIS_H

#include "Display_helper.h"
#include "Tetromino.h"
#include <queue>
#include <random>

using std::queue;

// Tetris defines
#define TETRIS_WIDTH 10
#define TETRIS_HEIGHT 40
#define TETRIS_DISP_HEIGHT 10
#define TETRIS_DISPLAYS 4
#define TETRIS_SQ_PXL 48
#define TETRIS_EMPTY_COLOR DARKGREY
#define TETRIS_MAX_QUEUE 3
#define TETRIS_GRAVITY 1
#define TETRIS_MOVE_DELAY 150

// Tetris error code
#define TETRIS_SUCCESS 0
#define TETRIS_ERR 1
#define TETRIS_MINO_COLLIDE 2
#define TETRIS_END_GAME 3

// Macros
#define ITERATE_MINO(mino) \
    for(int r = 0; r < (mino).tetromino.size(); r++) \
    { \
        for(int c = 0; c < (mino).tetromino[r].size(); c++) \
        { \
            int& row = r; \
            int& col = c; \
            \
        } \
    }

typedef short tetris_error_t;

class Tetris
{
public:
    Tetris();
    ~Tetris();

    void SetSideNum(int screen_num);
    tetris_error_t PlayGame();
    tetris_error_t EnqueueMove(char direction);
    tetris_error_t Reset();

private:
    int m_screen_num;

    char m_tetris_board[TETRIS_HEIGHT][TETRIS_WIDTH];
    int m_subsection;
    uint32_t m_move_delay; // In ms
    uint m_level;
    bool m_mino_is_active;
    unsigned long m_mino_time;
    unsigned long m_score;

    bool m_reset_mino_time_flag = true;

    queue<char> m_moves;
    queue<tetromino_t> m_tetromino_queue;
    tetromino_t m_active_mino;

    void DisplayTetrisBoard();
    int CharToColor(char color);

    tetris_error_t EnqueueTetromino();
    tetris_error_t ApplyGravity();
    tetris_error_t DeployTetromino();
    tetris_error_t RotateTetromino(tetromino_t& mino);
    tetris_error_t GetTetrominoSize(tetromino_t mino, int& width, int& height);
    tetris_error_t RequestMove(char direction);
    tetris_error_t MoveTetromino(char direction);
    tetris_error_t ClearTetromino();
    tetris_error_t UpdateBoard();
    tetris_error_t CollideTetromino();
    tetris_error_t CheckGame(tetromino_t new_mino);
    tetris_error_t CheckFullLines(vector<int>& filled_lines);
    tetris_error_t ClearFullLines(vector<int> filled_lines);
    tetris_error_t UpdateScore(int rowsCleared);
    tetris_error_t MapSubsection();

    // Debug tool
    void SquareCheck(int x, int y);
};

#endif