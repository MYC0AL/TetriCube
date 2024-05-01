#include "Pong.h"

pong_error_t Pong::Play()
{
    pong_error_t ret_val = PONG_SUCCESS;

    static clock_t prev_tick = 0;

    // Check ticks to see if ball should update
    if (clock() - prev_tick >= (BALL_MAX_SPEED - m_ball.S) + PONG_BASE_TICK_RATE)
    {
        prev_tick = clock();

        pong_collision_t coll_type = CheckWallCollision();
        if (coll_type != NO_COLLISION) // Check if moving the ball would cause a collision
        {
            if (coll_type == TOP_WALL || coll_type == BOTTOM_WALL) {
                m_ball.Vy = abs(m_ball.Vy) < BALL_MAX_VEL ? -(m_ball.Vy) : -m_ball.Vy;
                float new_speed = CalculateSpeed(m_ball.Vx,m_ball.Vy);
                m_ball.S = new_speed < BALL_MAX_SPEED ? new_speed : BALL_MAX_SPEED;
                //CheckGoal();
            }
            else if (coll_type == LEFT_WALL || coll_type == RIGHT_WALL) {
                m_ball.Vx = abs(m_ball.Vx) < BALL_MAX_VEL ? -(m_ball.Vx) : -m_ball.Vx;
                float new_speed = CalculateSpeed(m_ball.Vx,m_ball.Vy);
                m_ball.S = new_speed < BALL_MAX_SPEED ? new_speed : BALL_MAX_SPEED;
            }
        }

        //Render the ball every frame, the ball is always moving
        RenderBall( (m_ball.x + m_ball.Vx), (m_ball.y + m_ball.Vy) );

        // Check paddles for collision
        pong_collision_t coll1 = CheckPaddleCollision(m_paddle_1);
        pong_collision_t coll2 = CheckPaddleCollision(m_paddle_2);

        if ( coll1 != pong_collision_t::NO_COLLISION )
        {
            CollideWithPaddle(coll1);
            RenderPaddle(m_paddle_1, m_paddle_1.x);
        }
        else if (coll2 != pong_collision_t::NO_COLLISION )
        {
            CollideWithPaddle(coll2);
            RenderPaddle(m_paddle_2, m_paddle_2.x);
        }

        // Update paddle position at end of frame
        switch (m_paddle_move)
        {
            case LEFT_PADDLE_MOVE:
            {
                int new_pos = m_paddle_2.x - PADDLE_SPEED < 0 ? 0 : m_paddle_2.x - PADDLE_SPEED;
                RenderPaddle(m_paddle_2, new_pos);
                m_paddle_move = NO_PADDLE_MOVE;
            }
            break;

            case RIGHT_PADDLE_MOVE:
            {
                int new_pos = m_paddle_2.x + PADDLE_SPEED > ARENA_MAX_WIDTH - PADDLE_WIDTH ? ARENA_MAX_WIDTH - PADDLE_WIDTH : m_paddle_2.x + PADDLE_SPEED;
                RenderPaddle(m_paddle_2, new_pos);
                m_paddle_move = NO_PADDLE_MOVE;
            }
            break;
        }
    }    
    return ret_val;
}

pong_error_t Pong::MovePaddle(float x)
{
    pong_error_t ret_val = PONG_SUCCESS;

    if (x >= m_paddle_2.x + PADDLE_CENTER/2 && x <= m_paddle_2.x + PADDLE_CENTER/2 + PADDLE_WIDTH)
    {
        m_paddle_move = NO_PADDLE_MOVE;
    }
    else if (x < m_paddle_2.x + PADDLE_CENTER)
    {
        m_paddle_move = LEFT_PADDLE_MOVE;
    }
    else if (x > m_paddle_2.x + PADDLE_CENTER)
    {
        m_paddle_move = RIGHT_PADDLE_MOVE;
    }

    return ret_val;
}

// Must call reset before playing
pong_error_t Pong::Reset()
{
    pong_error_t ret_val = PONG_SUCCESS;

    m_paddle_1.x = ARENA_MAX_WIDTH/2-PADDLE_WIDTH/2;
    m_paddle_1.y = 60;
    m_paddle_1.Vx = 0;

    m_paddle_2.x = ARENA_MAX_WIDTH/2-PADDLE_WIDTH/2;
    m_paddle_2.y = ARENA_MAX_HEIGHT - 60 - PADDLE_HEIGHT;
    m_paddle_2.Vx = 0;

    gfx->fillRect(m_ball.x,m_ball.y,BALL_WIDTH,BALL_HEIGHT,ARENA_BG_COLOR);

    m_ball.x = ARENA_MAX_WIDTH/2;
    m_ball.y = ARENA_MAX_HEIGHT/2;
    m_ball.Vx = BALL_X_START_VEL;
    m_ball.Vy = BALL_Y_START_VEL;


    ret_val = RenderBall(m_ball.x, m_ball.y);

    ret_val = RenderPaddle(m_paddle_1, m_paddle_1.x);
    ret_val = RenderPaddle(m_paddle_2, m_paddle_2.x);

    return ret_val;
}

pong_error_t Pong::setScreenNum(int screen_num)
{
    m_screen_num = screen_num;
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
    m_ball.y = std::max(0, std::min(y, ARENA_MAX_HEIGHT-BALL_HEIGHT));

    // Set ball back to ball color in new position
    gfx->fillRect(m_ball.x,m_ball.y,BALL_WIDTH,BALL_HEIGHT,BALL_COLOR);

    return ret_val;
}

pong_collision_t Pong::CheckWallCollision()
{
    pong_collision_t coll_type = NO_COLLISION;

    if (m_ball.x + BALL_WIDTH >= ARENA_MAX_WIDTH) {
        coll_type = LEFT_WALL;
    }
    else if (m_ball.x <= 0) {
        coll_type = RIGHT_WALL;
    }
    else if (m_ball.y + BALL_HEIGHT >= ARENA_MAX_HEIGHT) {
        coll_type = TOP_WALL;
    }
    else if (m_ball.y <= 0) {
        coll_type = BOTTOM_WALL;
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

    if (coll == TOP_PADDLE)
    {
        m_ball.Vy = -m_ball.Vy;
        pen_y = -pen_base;
    }
    else if (coll == BOTTOM_PADDLE)
    {
        m_ball.Vy = -m_ball.Vy;
        pen_y = pen_base;
    }
    else if (coll == LEFT_PADDLE)
    {
        m_ball.Vx = -m_ball.Vx;
        pen_x = -pen_base;
    }
    else if (coll == RIGHT_PADDLE)
    {
        m_ball.Vx = -m_ball.Vx;
        pen_x = pen_base;
    }

    RenderBall(m_ball.x + pen_x, m_ball.y + pen_y);

    return ret_val;
}

pong_error_t Pong::CheckGoal()
{
    Reset();
    return PONG_SUCCESS;
}

float Pong::CalculateSpeed(float Vx, float Vy)
{
    return (sqrt( (Vx*Vx) + (Vy*Vy) ));
}
