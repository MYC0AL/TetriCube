#include "Pong.h"

pong_error_t Pong::Play()
{
    pong_error_t ret_val = PONG_SUCCESS;

    static clock_t prev_tick = 0;

    if (m_ball_on_screen)
    {
         // Check ticks to see if ball should update
        if (clock() - prev_tick >= (BALL_MAX_SPEED - m_ball.S) + PONG_BASE_TICK_RATE)
        {
            prev_tick = clock();

            //Render the ball every frame, the ball is always moving
            RenderBall( (m_ball.x + m_ball.Vx), (m_ball.y + m_ball.Vy) );

            pong_collision_t coll_type = CheckWallCollision();
            if (coll_type != NO_COLLISION) // Check if moving the ball would cause a collision
            {
                if (coll_type == TOP_WALL)
                {
                    if (m_screen_num == 5) {
                        ret_val = ScoreGoal(true); // P1 scored
                    }
                    else {
                        m_ball_move_screen = true;
                        m_ball_on_screen = false;
                        gfx->fillRect(m_ball.x,m_ball.y,BALL_WIDTH,BALL_HEIGHT,ARENA_BG_COLOR);
                    }
                }
                else if (coll_type == BOTTOM_WALL) 
                {
                    if (m_screen_num == 4) {
                        ret_val = ScoreGoal(false); // P2 scored
                    }
                    else {
                        m_ball_move_screen = true;
                        m_ball_on_screen = false;
                        gfx->fillRect(m_ball.x,m_ball.y,BALL_WIDTH,BALL_HEIGHT,ARENA_BG_COLOR);
                    }
                }
                else if (coll_type == LEFT_WALL || coll_type == RIGHT_WALL) {
                    m_ball.Vx = abs(m_ball.Vx) < BALL_MAX_VEL ? -(m_ball.Vx) : -m_ball.Vx;
                    float new_speed = CalculateSpeed(m_ball.Vx,m_ball.Vy);
                    m_ball.S = new_speed < BALL_MAX_SPEED ? new_speed : BALL_MAX_SPEED;
                }
            }

            // Check paddles for collision
            pong_collision_t coll = CheckPaddleCollision(m_paddle);

            if ( coll != pong_collision_t::NO_COLLISION )
            {
                CollideWithPaddle(coll);
                RenderPaddle(m_paddle, m_paddle.x);
            }
        }
    }

    return ret_val;
}

pong_error_t Pong::MovePaddle(float x)
{
    pong_error_t ret_val = PONG_SUCCESS;

    if (x >= m_paddle.x + PADDLE_CENTER/2 && x <= m_paddle.x + PADDLE_WIDTH - PADDLE_CENTER/2)
    {
        m_paddle_move = NO_PADDLE_MOVE;
    }
    else if (x < m_paddle.x + PADDLE_CENTER)
    {
        int new_pos = x - PADDLE_CENTER < 0 ? 0 : x - PADDLE_CENTER;
        RenderPaddle(m_paddle, new_pos);
        m_paddle_move = NO_PADDLE_MOVE;
    }
    else if (x > m_paddle.x + PADDLE_CENTER)
    {
        int new_pos = x > ARENA_MAX_WIDTH - PADDLE_WIDTH ? ARENA_MAX_WIDTH - PADDLE_WIDTH : x - PADDLE_CENTER;
        RenderPaddle(m_paddle, new_pos);
        m_paddle_move = NO_PADDLE_MOVE;
    }

    return ret_val;
}

// Must call reset before playing
pong_error_t Pong::Reset()
{
    pong_error_t ret_val = PONG_SUCCESS;

    // Clear ball on screen flag
    m_ball_on_screen = false;

    // Reset screen
    gfx->fillScreen(ARENA_BG_COLOR);

    if (m_screen_num == 4)
    {
        m_paddle.x = ARENA_MAX_WIDTH/2-PADDLE_WIDTH/2;
        m_paddle.y = ARENA_MAX_HEIGHT - 60 - PADDLE_HEIGHT;
        m_paddle.Vx = 0;

        gfx->fillRect(m_ball.x,m_ball.y,BALL_WIDTH,BALL_HEIGHT,ARENA_BG_COLOR);

        m_ball.x = ARENA_MAX_WIDTH/2;
        m_ball.y = ARENA_MAX_HEIGHT/2;
        m_ball.Vx = BALL_X_START_VEL;
        m_ball.Vy = BALL_Y_START_VEL;

        // Set ball on screen flag
        m_ball_on_screen = true;

        RenderBall(m_ball.x, m_ball.y);
        RenderPaddle(m_paddle, m_paddle.x);
    }
    else if (m_screen_num == 5)
    {
        m_paddle.x = ARENA_MAX_WIDTH/2-PADDLE_WIDTH/2;
        m_paddle.y = 60;
        m_paddle.Vx = 0;
        RenderPaddle(m_paddle, m_paddle.x);
    }
    else
    {
        m_ball.x = ARENA_OUT;
        m_ball.y = ARENA_OUT;
        m_ball.Vx = 0;
        m_ball.Vy = 0;

        m_paddle.x = ARENA_OUT;
        m_paddle.y = ARENA_OUT;
        m_paddle.Vx = 0;
    }

    return ret_val;
}

pong_error_t Pong::setScreenNum(int screen_num)
{
    m_screen_num = screen_num;
    return PONG_SUCCESS;
}

pong_error_t Pong::getDetails(Ball& active_ball, bool& ball_status, bool& ball_move_screen)
{
    active_ball = m_ball;
    ball_status = m_ball_on_screen;

    bool temp_move_screen = m_ball_move_screen;
    ball_move_screen = temp_move_screen;

    // Reset move screen flag
    m_ball_move_screen = false;

    return PONG_SUCCESS;
}

pong_error_t Pong::setBall(const Ball &new_ball)
{
    m_ball = new_ball;
    m_ball_on_screen = true;
    m_ball.S = CalculateSpeed(m_ball.Vx,m_ball.Vy);
    return PONG_SUCCESS;
}

pong_error_t Pong::RenderPaddle(Paddle& paddle, float x)
{
    pong_error_t ret_val = PONG_SUCCESS;

    // Set old paddle to bg color
    gfx->fillRect(paddle.x,paddle.y,PADDLE_WIDTH,PADDLE_HEIGHT,ARENA_BG_COLOR);

    // Update position of paddle
    paddle.x = x;

    // Set new paddle to paddle color
    gfx->fillRect(paddle.x,paddle.y,PADDLE_WIDTH,PADDLE_HEIGHT,PADDLE_COLOR);

    return ret_val;
}

pong_error_t Pong::RenderBall(int x, int y)
{
    pong_error_t ret_val = PONG_SUCCESS;

    // Set current ball to background color
    gfx->fillRect(m_ball.x,m_ball.y,BALL_WIDTH,BALL_HEIGHT,ARENA_BG_COLOR);

    // Update position of ball, and ensure it is in the arena
    m_ball.x = std::max(0, std::min(x, ARENA_MAX_WIDTH-BALL_WIDTH));
    m_ball.y = y;

    // Set ball back to ball color in new position
    gfx->fillRect(m_ball.x,m_ball.y,BALL_WIDTH,BALL_HEIGHT,BALL_COLOR);

    return ret_val;
}

pong_collision_t Pong::CheckWallCollision()
{
    pong_collision_t coll_type = NO_COLLISION;

    if (m_ball.x + BALL_WIDTH >= ARENA_MAX_WIDTH) {
        coll_type = RIGHT_WALL;
    }
    else if (m_ball.x <= 0) {
        coll_type = LEFT_WALL;
    }
    else if (m_ball.y + BALL_HEIGHT >= ARENA_MAX_HEIGHT) {
        coll_type = BOTTOM_WALL;
    }
    else if (m_ball.y <= 0) {
        coll_type = TOP_WALL;
    }

    return coll_type;
}

pong_collision_t Pong::CheckPaddleCollision(const Paddle& paddle)
{
    pong_collision_t ret_val = NO_COLLISION;

    float ballLeft = m_ball.x;
	float ballRight = m_ball.x + BALL_WIDTH;
	float ballTop =  m_ball.y;
	float ballBottom = m_ball.y + BALL_HEIGHT;

	float paddleLeft = paddle.x;
	float paddleRight = paddle.x + PADDLE_WIDTH;
	float paddleTop = paddle.y;
	float paddleBottom = paddle.y + PADDLE_HEIGHT;

    // Is intersecting anywhere with paddle
	if (ballLeft <= paddleRight && ballRight >= paddleLeft && ballTop <= paddleBottom && ballBottom >= paddleTop)
	{
        // Check if ball right is right of left paddle
        if (m_ball.Vx > 0 && ballRight >= paddleLeft && ballLeft <= paddleLeft)
            ret_val = LEFT_PADDLE;

        // Check if ball left is left of right paddle
        else if (m_ball.Vx < 0 && ballLeft <= paddleRight && ballRight >= paddleRight)
            ret_val = RIGHT_PADDLE;

		// Check if ball bottom is below top of paddle
        else if (m_ball.Vy > 0)
            ret_val = TOP_PADDLE;

        // Check if ball top is above bottom of paddle
        else if (m_ball.Vy < 0)
            ret_val = BOTTOM_PADDLE;
	}

    return ret_val;
}

pong_error_t Pong::CollideWithPaddle(pong_collision_t coll)
{
    pong_error_t ret_val = PONG_SUCCESS;

    int pen_x = 0;
    int pen_y = 0;
    int pen_base = BALL_HEIGHT / 2;

    float ball_acc = 1.1F;
    float new_ball_vx = abs(m_ball.Vx*ball_acc) > BALL_MAX_VEL ? BALL_MAX_VEL : -(m_ball.Vx*ball_acc);
    float new_ball_vy = abs(m_ball.Vy*ball_acc) > BALL_MAX_VEL ? BALL_MAX_VEL : -(m_ball.Vy*ball_acc);

    if (coll == TOP_PADDLE)
    {
        m_ball.Vy = new_ball_vy;
        pen_y = -pen_base;
    }
    else if (coll == BOTTOM_PADDLE)
    {
        m_ball.Vy = new_ball_vy;
        pen_y = pen_base;
    }
    else if (coll == LEFT_PADDLE)
    {
        m_ball.Vx = new_ball_vx;
        pen_x = -pen_base;
    }
    else if (coll == RIGHT_PADDLE)
    {
        m_ball.Vx = new_ball_vx;
        pen_x = pen_base;
    }

    RenderBall(m_ball.x + pen_x, m_ball.y + pen_y);

    return ret_val;
}

pong_error_t Pong::ScoreGoal(bool p1Scored)
{
    Reset();
    return p1Scored ? PONG_P1_SCORED : PONG_P2_SCORED;
}

float Pong::CalculateSpeed(float Vx, float Vy)
{
    return (sqrt( (Vx*Vx) + (Vy*Vy) ));
}
