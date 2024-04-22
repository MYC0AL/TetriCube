#include "TicTacToe.h"

using namespace std;

ttt_error_t TicTacToe::getBoard(int& xPos, int& oPos)
{
    xPos = m_x_pos;
    oPos = m_o_pos;

    return TTT_SUCCESS;
}

ttt_error_t TicTacToe::ResetBoard()
{
    ttt_error_t ret_val = TTT_SUCCESS;

    if ((m_x_pos == 0) && (m_o_pos == 0))
    {
        ret_val = TTT_ERROR;
    }

    m_status = X_TURN;
    m_x_pos = 0;
    m_o_pos = 0;

    for(int i = 0; i < MAX_MOVE_COUNT; i++)
    {
        m_x_prev_moves[i] = 0;
        m_o_prev_moves[i] = 0;
    }

    m_x_move_count = 0;
    m_o_move_count = 0;

	return ret_val;
}

ttt_error_t TicTacToe::PlacePiece(int pos)
{
    ttt_error_t ret_val = TTT_SUCCESS;

    if (!VALID_POS(pos))
    {
        ret_val = TTT_ERROR;
    }

    if (m_status == GAME_OVER)
    {
        ret_val = TTT_ERROR;
    }

    if (getBit(pos))
    {
        ret_val = TTT_ERROR;
    }

    if (ret_val == TTT_SUCCESS) // Check if in valid playing field
    {
        if (m_status == X_TURN)
        {
            setBit(m_x_pos, pos);

            if (m_hard_mode)
            {
                UpdateHardMode(m_x_pos, m_x_prev_moves, m_x_move_count, pos);
            }

            m_status = O_TURN;
        }
        else if (m_status == O_TURN)
        {
            setBit(m_o_pos, pos);

            if (m_hard_mode)
            {
                UpdateHardMode(m_o_pos, m_o_prev_moves, m_o_move_count, pos);
            }

            m_status = X_TURN;
        }

        if (isWin() == TTT_SUCCESS) // Check if win
        {
            m_status = GAME_OVER;
        }
    }

    return ret_val;
}

ttt_status_t TicTacToe::getStatus()
{
    return m_status;
}

ttt_error_t TicTacToe::SetHardMode(bool on_off)
{
    m_hard_mode = on_off;
    return TTT_SUCCESS;
}

ttt_error_t TicTacToe::isWin()
{
    ttt_error_t ret_val = TTT_ERROR;

    for (int i = 0; i < WIN_PATTERN_COUNT; i++)
    {
        if ((m_x_pos & WIN_PATTERNS[i]) == WIN_PATTERNS[i]) {
            ret_val = TTT_SUCCESS;
        }
        else if ((m_o_pos & WIN_PATTERNS[i]) == WIN_PATTERNS[i]) {
            ret_val = TTT_SUCCESS;
        }
    }

    return ret_val;
}

ttt_error_t TicTacToe::setBit(unsigned int& num, int pos, int val)
{
    ttt_error_t ret_val = TTT_SUCCESS;

    if (!VALID_POS(pos))
    {
        ret_val = TTT_ERROR;
    }
    if (val != 0 && val != 1)
    {
        ret_val = TTT_ERROR;
    }

    if (ret_val == TTT_SUCCESS)
    {
        unsigned int mask = 1 << pos;
        num = ((num & ~mask) | (val << pos));
    }

    return ret_val;
}

int TicTacToe::getBit(int pos)
{
    int ret_val = -1;

    if (VALID_POS(pos))
    {
        unsigned int mask = 1 << pos;
        ret_val = ((m_x_pos | m_o_pos) & mask);
    }

    return ret_val;
}

ttt_error_t TicTacToe::UpdateHardMode(unsigned int& num, unsigned int prev_moves[MAX_MOVE_COUNT], int& count, int pos)
{
    int offset = count % MAX_MOVE_COUNT;

    if (count >= 3)
    {
        // Mask out the oldest previous move
        num ^= prev_moves[offset];

    }

    prev_moves[offset] &= 0b000000000;
    prev_moves[offset] = (1 << pos);

    count++;

    return TTT_SUCCESS;
}
