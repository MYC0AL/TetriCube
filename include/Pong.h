
#include "DisplayHelper.h"

// Defines for Paddle
#define PADDLE_WIDTH 150
#define PADDLE_HEIGHT 25
#define PADDLE_COLOR WHITE
#define PADDLE_CENTER (PADDLE_WIDTH/2)
#define PADDLE_SPEED 20

// Defines for Ball
#define BALL_WIDTH 20
#define BALL_HEIGHT 20
#define BALL_COLOR MAGENTA
#define BALL_MAX_SPEED 10
#define BALL_MAX_VEL 10
#define BALL_X_START_VEL 2
#define BALL_Y_START_VEL 3

// Arena Defines
#define ARENA_MAX_HEIGHT 479
#define ARENA_MAX_WIDTH 479
#define ARENA_BG_COLOR BLACK

// General Defines
#define PONG_BASE_TICK_RATE 5

enum pong_error_t {PONG_SUCCESS, PONG_ERROR};

enum pong_collision_t { NO_COLLISION,
                        TOP_WALL, TOP_PADDLE,
                        BOTTOM_WALL, BOTTOM_PADDLE, 
                        LEFT_WALL, LEFT_PADDLE,
                        RIGHT_WALL, RIGHT_PADDLE
                      };

enum pong_paddle_move_t { NO_PADDLE_MOVE, LEFT_PADDLE_MOVE, RIGHT_PADDLE_MOVE};


struct Ball
{
    int x = ARENA_MAX_WIDTH/2;
    int y = ARENA_MAX_HEIGHT/2;
    float Vx = BALL_X_START_VEL;
    float Vy = BALL_Y_START_VEL;
    float S = BALL_X_START_VEL;
};

struct Paddle
{
    int x;
    int y;
    float Vx;
    float S;
};

class Pong
{
public:
    pong_error_t Play();
    pong_error_t MovePaddle(float x);
    pong_error_t Reset();
    pong_error_t setScreenNum(int screen_num);

private:

    pong_error_t RenderPaddle(Paddle& paddle, float x);
    pong_error_t RenderBall(int x, int y);
    pong_collision_t CheckWallCollision();
    pong_collision_t CheckPaddleCollision(const Paddle& paddle);
    pong_error_t CollideWithPaddle(pong_collision_t coll);
    pong_error_t CheckGoal();
    float CalculateSpeed(float Vx, float Vy);

    // Objects
    Paddle m_paddle_1;
    Paddle m_paddle_2;
    Ball m_ball;

    // Screen Num
    int m_screen_num;
    
    // Flags
    bool m_has_collided = false;
    pong_paddle_move_t m_paddle_move = NO_PADDLE_MOVE;

};