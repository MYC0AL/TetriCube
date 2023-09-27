#ifndef TETROMINO_H
#define TETROMINO_H

#define TETROMINO_WIDTH 4
#define TETROMINO_HEIGHT 4

#define TETROMINO_COUNT 7
#define TETROMINO_START_POS_X 3
#define TETROMINO_START_POS_Y 6

typedef unsigned int uint;

struct tetris_sq_t
{
    uint x;
    uint y;
    uint width;
    uint height;
    uint color;
};

struct tetromino_t 
{
    char tetromino[TETROMINO_HEIGHT][TETROMINO_WIDTH];
    uint x; // X coord on board
    uint y; // Y coord on board
    uint width;
    uint height;
};

// All 7 Tetrominos Shapes
const tetromino_t MINO_I = {{
                            {'C','C','C','C'},
                            {' ',' ',' ',' '},
                            {' ',' ',' ',' '},
                            {' ',' ',' ',' '}},
                            TETROMINO_START_POS_X,
                            TETROMINO_START_POS_Y};

const tetromino_t MINO_O = {{
                            {'Y','Y',' ',' '},
                            {'Y','Y',' ',' '},
                            {' ',' ',' ',' '},
                            {' ',' ',' ',' '}},
                            TETROMINO_START_POS_X,
                            TETROMINO_START_POS_Y};

const tetromino_t MINO_T = {{
                            {'P','P','P',' '},
                            {' ','P',' ',' '},
                            {' ',' ',' ',' '},
                            {' ',' ',' ',' '}},
                            TETROMINO_START_POS_X,
                            TETROMINO_START_POS_Y};

const tetromino_t MINO_J = {{
                            {' ','N',' ',' '},
                            {' ','N',' ',' '},
                            {'N','N',' ',' '},
                            {' ',' ',' ',' '}},
                            TETROMINO_START_POS_X,
                            TETROMINO_START_POS_Y};

const tetromino_t MINO_L = {{
                            {'O',' ',' ',' '},
                            {'O',' ',' ',' '},
                            {'O','O',' ',' '},
                            {' ',' ',' ',' '}},
                            TETROMINO_START_POS_X,
                            TETROMINO_START_POS_Y};

const tetromino_t MINO_S = {{
                            {' ','G','G',' '},
                            {'G','G',' ',' '},
                            {' ',' ',' ',' '},
                            {' ',' ',' ',' '}},
                            TETROMINO_START_POS_X,
                            TETROMINO_START_POS_Y};

const tetromino_t MINO_Z = {{
                            {'R','R',' ',' '},
                            {' ','R','R',' '},
                            {' ',' ',' ',' '},
                            {' ',' ',' ',' '}},
                            TETROMINO_START_POS_X,
                            TETROMINO_START_POS_Y};

const tetromino_t ALL_MINOS[TETROMINO_COUNT] = {MINO_I, MINO_O, MINO_T, MINO_J, MINO_L, MINO_S, MINO_Z};

#endif