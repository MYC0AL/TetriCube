#ifndef TETROMINO_H
#define TETROMINO_H

#define TETROMINO_WIDTH 4
#define TETROMINO_HEIGHT 3

#define TETROMINO_COUNT 7

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
    uint x;
    uint y;
};

// All 7 Tetrominos Shapes
const tetromino_t MINO_I = {{
                            {' ',' ',' ',' '},
                            {' ',' ',' ',' '},
                            {'C','C','C','C'}},5,1};

const tetromino_t MINO_O = {{
                            {' ',' ',' ',' '},
                            {'Y','Y',' ',' '},
                            {'Y','Y',' ',' '}},5,1};

const tetromino_t MINO_T = {{
                            {' ',' ',' ',' '},
                            {'P','P','P',' '},
                            {' ','P',' ',' '}},5,1};

const tetromino_t MINO_J = {{
                            {' ','N',' ',' '},
                            {' ','N',' ',' '},
                            {'N','N',' ',' '}},5,1};

const tetromino_t MINO_L = {{
                            {'O',' ',' ',' '},
                            {'O',' ',' ',' '},
                            {'O','O',' ',' '}},5,1};

const tetromino_t MINO_S = {{
                            {' ',' ',' ',' '},
                            {' ','G','G',' '},
                            {'G','G',' ',' '}},5,1};

const tetromino_t MINO_Z = {{
                            {' ',' ',' ',' '},
                            {'R','R',' ',' '},
                            {' ','R','R',' '}},5,1};

const tetromino_t ALL_MINOS[TETROMINO_COUNT] = {MINO_I, MINO_O, MINO_T, MINO_J, MINO_L, MINO_S, MINO_Z};

#endif