#pragma once

#include <iostream>


// Board layout
//    0 1 2
//    3 4 5
//    6 7 8

#define for_each_bit(num, bit) \
    for (int bit = 0; bit < sizeof(num) * 8; ++bit) \
        if (int bit = num & (1 << bit); bit)

#define VALID_POS(num) ((num >= 0) && (num <= 8))

#define WIN_PATTERN_COUNT 8
#define BOARD_SIZE 9
#define MAX_MOVE_COUNT 3

enum ttt_error_t {TTT_SUCCESS,TTT_ERROR};
enum ttt_status_t {X_TURN, O_TURN, GAME_OVER};

static const int WIN_PATTERNS[WIN_PATTERN_COUNT] = {
	0b111000000, 0b000111000, 0b000000111, // Horizontal wins
	0b100100100, 0b010010010, 0b001001001, // Vertical wins
	0b100010001, 0b001010100			   // Diagonal wins
};

class TicTacToe
{
public:
	
	ttt_error_t getBoard(int& xPos, int& oPos);
	ttt_error_t ResetBoard();

	ttt_error_t PlacePiece(int pos);
	ttt_status_t getStatus();

	ttt_error_t SetHardMode(bool on_off);

private:

	ttt_error_t isWin();
	ttt_error_t setBit(unsigned int& num, int pos, int val = 1);
	int getBit(int pos);
	
	ttt_error_t UpdateHardMode(unsigned int& num, unsigned int prev_moves[MAX_MOVE_COUNT], int& count, int pos);


	unsigned int m_x_pos = 0b000000000;
	unsigned int m_o_pos = 0b000000000;

	unsigned int m_x_prev_moves[MAX_MOVE_COUNT] = {0b000000000};
	unsigned int m_o_prev_moves[MAX_MOVE_COUNT] = {0b000000000};

	int m_x_move_count = 0;
	int m_o_move_count = 0;

	bool m_hard_mode = false;

	ttt_status_t m_status = X_TURN;
};