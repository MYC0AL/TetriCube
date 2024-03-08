#ifndef TETRIS_H
#define TETRIS_H

#include "DisplayHelper.h"
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

#define CLEAR_QUEUE(queue) \
    while (!(queue).empty()) { \
        (queue).pop(); \
    }

enum update_t{NONE,NEW_MINO,NEW_POS};

typedef short tetris_error_t;

class Tetris
{
public:
    Tetris();
    ~Tetris();

    update_t UpdateReady(std::queue<int> &ret_queue);
    void SetSideNum(int screen_num);
    tetris_error_t PlayGame();
    tetris_error_t SetNextMove(char direction);
    tetris_error_t Reset();
    unsigned long GetScore();
    unsigned int GetLevel();
    tetris_error_t GetTetromino(tetromino_t &mino);
    tetris_error_t SetTetromino(tetromino_t mino);
    tetris_error_t GetBoard(char tetris_board[TETRIS_HEIGHT/4][TETRIS_WIDTH], int partition);
    tetris_error_t SetBoard(char tetris_board[TETRIS_HEIGHT/4][TETRIS_WIDTH], int partition);
    tetris_error_t ClearTetromino();
    tetris_error_t UpdateBoard();
    void DisplayTetrisBoard();

private:
    int m_screen_num;

    char m_tetris_board[TETRIS_HEIGHT][TETRIS_WIDTH];
    int m_subsection;
    uint32_t m_move_delay; // In ms
    unsigned int m_level;
    unsigned int m_total_rows_cleared;
    bool m_mino_is_active;
    unsigned long m_mino_time;
    unsigned long m_score;
    unsigned short m_active_mino_idx;

    bool m_reset_mino_time_flag = true;

    update_t m_update_ready;
    std::queue<int> m_updated_partitions;

    char m_next_move;
    tetromino_t m_next_tetromino;
    tetromino_t m_active_mino;

    int CharToColor(char color);

    tetris_error_t SetNextTetromino();
    tetris_error_t ApplyGravity();
    tetris_error_t DeployTetromino();
    tetris_error_t RotateTetromino(tetromino_t& mino);
    tetris_error_t GetTetrominoSize(tetromino_t mino, int& width, int& height);
    tetris_error_t RequestMove(char direction);
    tetris_error_t MoveTetromino(char direction);
    tetris_error_t CollideTetromino();
    tetris_error_t CheckGame(tetromino_t new_mino);
    tetris_error_t CheckFullLines(vector<int>& filled_lines);
    tetris_error_t ClearFullLines(vector<int> filled_lines);
    tetris_error_t CheckLevelUp();
    tetris_error_t UpdateScore(int rowsCleared);
    tetris_error_t MapSubsection();
};

#endif