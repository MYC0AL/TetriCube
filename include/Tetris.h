#ifndef TETRIS_H
#define TETRIS_H

#include "Display_helper.h"
#include "Tetromino.h"
#include <queue>
#include <ctime>

using std::queue;

// Tetris defines
#define TETRIS_WIDTH 10
#define TETRIS_HEIGHT 10
#define TETRIS_DISPLAYS 4
#define TETRIS_SQ_PXL 48
#define TETRIS_EMPTY_COLOR DARKGREY
#define TETRIS_MAX_QUEUE 3
#define TETRIS_GRAVITY 1

// Tetris error code
#define TETRIS_SUCCESS 0
#define TETRIS_ERR 1
#define TETRIS_MINO_COLLIDE 2
#define TETRIS_END_GAME 3

typedef short tetris_error_t;

class Tetris
{
public:
    Tetris();
    ~Tetris();

    tetris_error_t PlayGame();
    tetris_error_t EnqueueMove(char direction);
    tetris_error_t RequestRotate(char direction);

private:
    char m_tetris_board[TETRIS_WIDTH][TETRIS_HEIGHT];
    uint32_t m_move_delay; // In ms
    uint m_round_num;
    bool m_mino_is_active;
    uint m_mino_time;

    queue<char> m_moves;
    queue<tetromino_t> m_tetromino_queue;
    tetromino_t m_active_mino;

    void DisplayTetrisBoard();
    int CharToColor(char color);

    tetris_error_t EnqueueTetromino();
    tetris_error_t DeployTetromino();
    tetris_error_t RotateTetromino();
    tetris_error_t RequestMove(char direction);
    tetris_error_t MoveTetromino(char direction);
    tetris_error_t ClearTetromino();
    tetris_error_t UpdateBoard();
    tetris_error_t CollideTetromino();
    tetris_error_t CheckGame(tetromino_t new_mino);

    bool downCollision(int row, int col);

    // Debug tool
    void SquareCheck(int x, int y);

    void DummyHandler();

};

#endif