#ifndef STATE_DRIVER_H
#define STATE_DRIVER_H

#include "DisplayHelper.h"
#include "Tetris.h"
#include "ExternalLink.h"
#include "Cube.h"
#include "TicTacToe.h"
#include "Pong.h"
#include <ctime>
#include <cmath>
#include <sstream>
#include <iomanip>

// Transmit data structure
// Num of sender unit symbol data
// Ex: 0TR\n
// Screen 0 sent a tetris update about a right rotation UART_EOL

// Sender range: 0-5
// Unit symbols:
//      S: state transition
//      T: tetris
//      C: cube
//      O: TicTacToe
//      P: Pong

// Data decode:
//      S: I,S,G,E,H,T,Y,R,F,O
//      T: R,L,D,^,U,I,Y
//      C: 0,1,2,3,4,5,6,7,8,9,A,B  (A: 10,  B: 11)
//      O: R
//      P: B,S,R

// Defines
#define MSG_SIZE 3
#define SCREEN_IDX 0
#define SYMBOL_IDX 1

// Most important defines
#define SCREEN_NUM 2
#define DEBUG_MODE 0
// D    #
// 10 - 3
// 9 - 0
// 5 - 5
// 3 - 1

// High Score File
#define PODIUM_SIZE 100
#define HS_FILE_NAME "/highscores.txt"
typedef std::pair<std::string, std::string> entry_t;
typedef std::vector<entry_t> podium_t;

// Alphabet wheel size
#define ALPHA_WHEEL_SIZE 3

// State definitions
enum state_t {
            STATE_INIT, STATE_START, STATE_SELECT_GAME,
            STATE_SETTINGS,STATE_HIGH_SCORES, STATE_TETRIS,
            STATE_TETRIS_PAUSE, STATE_TETRIS_END, STATE_RUBIKS,
            STATE_RUBIKS_PAUSE, STATE_RUBIKS_END, STATE_TTT,
            STATE_PONG, STATE_PONG_PAUSE};

// State return codes
enum state_code_t {STATE_SUCCESS, STATE_ERROR};

// Class declaration
class StateDriver
{
private:
    
    state_code_t broadcast_state_transition(state_t new_state);
    void update_new_state(state_t new_state);
    state_code_t DecodeCMD(std::string CMD);

    char StateToChar(state_t state);
    state_t CharToState(char ch);

    void SetSeed(int seed);

    state_code_t ScrambleCube();
    state_code_t SolveCube();

    state_code_t EncodeBoardCMD(int tetris_partition);
    state_code_t EncodeTetrominoCMD();
    state_code_t DecodeBoardCMD(std::string RxCMD);
    state_code_t DecodeTetrominoCMD(std::string RxCMD);
    state_code_t TetrisReset();

    state_code_t WriteHighScoreFile(std::string user_name, long score);
    state_code_t ReadHighScoreFile(podium_t& podium);
    state_code_t SortHighScores(podium_t& podium);

    state_code_t CenterAndPrintInt(int num, int x, int y, int text_size, int color);

    state_code_t InitAlphaWheels();
    state_code_t DisplayAlphaWheels();
    state_code_t UpdateAlphaWheels(int wheel, bool next);

    state_code_t DisplayTicTacToe();
    state_code_t ClearTicTacToeTiles();
    int DecodeTicTacToeTouch();

    state_code_t SendSetBallCMD(const Ball& active_ball, int new_y_pos);
    std::string ftoa(float num);
    int findIndex(const int arr[], int size, int num_to_find);
    state_code_t PrintPongScores();

public:

    // Current Driver state
    state_t drv_state;

    // Has-a relationship with display helper
    DisplayHelper dHelp;

    // Has-a relationship with EL
    ExternalLink el;

    StateDriver();
    state_code_t request_state_change(state_t new_state);
    void state_controller();

    // Has-a relationship with Games
    Cube rbx;
    Tetris tetris;
    TicTacToe ttt;
    Pong pong;

    // Move timer for tetris
    unsigned long m_tetris_move_timer;

    // Screen number
    int m_screen_num;

    // Cube scramble helper vars
    int rbx_scram_count = 0;
    bool scrambling = false;
    bool solve = false;

    // Tetris reset helper vars
    bool tetris_reset = false;

    // Tetris Partition Update
    int m_tetris_partition;
    bool m_tetris_updating_all;
    std::queue<int> m_updated_partitions;

    // Controller control
    bool m_controller_active = true;
    int m_partition_count = 0;

    // Tetris score
    unsigned long m_tetris_score;

    // Initials Entering
    char m_alpha_wheel[ALPHA_WHEEL_SIZE];

    // Tic-Tac-Toe Settings
    bool m_ttt_hard_mode = false;

    // Pong scores
    int m_pong_p1_score;
    int m_pong_p2_score;

};

#endif