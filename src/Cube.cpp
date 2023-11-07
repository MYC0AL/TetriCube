#include "Cube.h"
#include <iomanip>
// Globals for Cube
SQ SQ_POS[9] = {{RBX_SQ0}, {RBX_SQ1}, {RBX_SQ2}, {RBX_SQ3}, {RBX_SQ4}, {RBX_SQ5}, {RBX_SQ6}, {RBX_SQ7}, {RBX_SQ8}};

// short DISP_NUM = 0;
short possibleSwipes[12][SWIPE_SIZE] = {{0,3,6}, // 0
                                        {1,4,7}, // 1
                                        {2,5,8}, // 2
                                        {6,7,8}, // 3
                                        {3,4,5}, // 4
                                        {0,1,2}, // 5
                                        {8,5,2}, // 6
                                        {7,4,1}, // 7
                                        {6,3,0}, // 8
                                        {2,1,0}, // 9
                                        {5,4,3}, // 10
                                        {8,7,6}};// 11 

// Color stuff
unsigned short color_index = 0;
int RBX_Colors[6] = {GREEN, WHITE, RED, YELLOW, ORANGE, BLUE};

Cube::Cube(int side_num) : m_side_num(side_num), m_prevSq(-1), m_prevTouchCount(0), m_prevDist(0), m_swipedFlag(false)
{
    for (int i = 0; i < PREV_ARR_SIZE; ++i)
        m_prevTouches[i] = -1;

    InitCube();
}

Cube::~Cube()
{

}

void Cube::PlayGame()
{    
    if (m_swipedFlag)
    {
        short dir = dirSwiped();
        if (dir != SWIPE_ERR)
        {
            RotateCube(m_side_num, dir);
            ResetVars(); //TODO Remove this and combine one below
            drawRubiksSide(m_side_num);
        }
        else
            ResetVars();

        m_swipedFlag = false;
    }
}

void Cube::drawRubiksSide(int sideNum)
{
    for (int i = 0; i < NUM_SQUARES; i++)
    {
        gfx->fillRect(m_cube[sideNum][i].x,
                         m_cube[sideNum][i].y,
                         m_cube[sideNum][i].w,
                         m_cube[sideNum][i].h,
                         m_cube[sideNum][i].c);
    }
}

void Cube::drawRubiksCube()
{
    for (int i = 0; i < NUM_SIDES; i++)
    {
        drawRubiksSide(i);
        sleep(4);
    }
    drawRubiksSide(m_side_num);
}

short Cube::sqTapped(short xPos, short yPos)
{
    short SQ_num = -1;
    bool found = false;
    for (int i = 0; i < NUM_SQUARES && !found; i++)
    {
        if ((xPos > m_cube[m_side_num][i].x && xPos < m_cube[m_side_num][i].x + m_cube[m_side_num][i].w) &&
            (yPos > m_cube[m_side_num][i].y && yPos < m_cube[m_side_num][i].y + m_cube[m_side_num][i].h))
        {
            SQ_num = i;
            found = true;
        }
    }
    return SQ_num;
}

short Cube::dirSwiped()
{
    short dir = -1;
    short matchCount = 0;
    bool found = false;
    for (int i = 0; i < 12 && !found; i++)
    {
        matchCount = 0;
        for (int j = 0; j < PREV_ARR_SIZE && !found; j++)
        {
            if (m_prevTouches[j] == possibleSwipes[i][j])
            {
                matchCount++;
            }
            if (matchCount == 3)
            {
                dir = i;
                found = true;
            }
        }
    }
    if (dir == 1 || dir == 7 || dir == 10 || dir == 4)
        dir = SWIPE_ERR;
    return dir;
}

void Cube::TouchUpdate(uint16_t touch_x, uint16_t touch_y)
{    
    short sqTouch = sqTapped(touch_x, touch_y);
    if (sqTouch != -1 && sqTouch != m_prevSq)
    {
        m_prevSq = sqTouch;

        if (m_prevTouches[0] == -1)
        {
            m_prevTouches[m_prevTouchCount] = m_prevSq;
            m_prevTouchCount = (m_prevTouchCount + 1) % SWIPE_SIZE;
        }
        else if (validTouch())
        {
            m_prevTouches[m_prevTouchCount] = m_prevSq;

            if (m_prevTouchCount + 1 == SWIPE_SIZE)
            {
                m_swipedFlag = true;
            }
            m_prevTouchCount = (m_prevTouchCount + 1) % SWIPE_SIZE;
        }
        else
        {
            ResetVars();
        }
    }
}

bool Cube::validTouch()
{
    bool validity = false;

    if (m_prevTouchCount == 1)
    {
        if ((abs(m_prevSq - m_prevTouches[m_prevTouchCount - 1]) == 1) &&
            (m_cube[m_side_num][m_prevSq].y == m_cube[m_side_num][m_prevTouches[m_prevTouchCount - 1]].y))
        {
            validity = true;
            m_prevDist = 1;
        }
        else if (abs(m_prevSq - m_prevTouches[m_prevTouchCount - 1]) == 3)
        {
            validity = true;
            m_prevDist = 3;
        }
    }
    else if (m_prevTouchCount == 2)
    {
        if (abs(m_prevSq - m_prevTouches[m_prevTouchCount - 1]) == m_prevDist)
        {
            validity = true;
        }
    }
    return validity;
}

void Cube::ResetVars()
{
    for (int i = 0; i < SWIPE_SIZE; i++)
        m_prevTouches[i] = -1;
    m_prevTouchCount = 0;
    m_prevTouches[0] = m_prevSq;
    m_prevTouchCount++;
    m_prevDist = 0;
    m_prevSq = -1;
}

void Cube::InitCube()
{
    for (int i = 0; i < NUM_SIDES; i++)
    {
        for (int j = 0; j < NUM_SQUARES; j++)
        {
            m_cube[i][j].x = SQ_POS[j].x;
            m_cube[i][j].y = SQ_POS[j].y;
            m_cube[i][j].w = SQ_POS[j].w;
            m_cube[i][j].h = SQ_POS[j].h;
            m_cube[i][j].c = RBX_Colors[i];
        }
    }
}

void Cube::Rotate(int sidesToMove[MOVE_SIZE], int sqToMove[MOVE_SIZE][SWIPE_SIZE], int spinSide, bool prime)
{
    if (prime)
    {
        std::swap(sidesToMove[0],sidesToMove[3]);
        std::swap(sidesToMove[1],sidesToMove[2]);
        std::swap(sqToMove[0],sqToMove[3]);
        std::swap(sqToMove[1],sqToMove[2]);
    }

    int tempCube[NUM_SIDES][NUM_SQUARES];
    for(int si = 0; si < NUM_SIDES; ++si)
        for(int sq = 0; sq < NUM_SQUARES; ++sq)
            tempCube[si][sq] = m_cube[si][sq].c;

    for(int i = 0; i < MOVE_SIZE; ++i)
    {
        int nextSide = (i + 1) % MOVE_SIZE;
        for(int j = 0; j < SWIPE_SIZE; ++j)
        {
            m_cube[sidesToMove[i]][sqToMove[i][j]].c = tempCube[sidesToMove[nextSide]][sqToMove[nextSide][j]];
        }
    }

    //DisplayCube();

    // Rotate host side
    rotateSide(spinSide, prime);
}

void Cube::rotateSide(short sideNum, bool prime)
{
    int tempSide[NUM_SQUARES];
    for(int i = 0; i < NUM_SQUARES; ++i)
    {
        tempSide[i] = m_cube[sideNum][i].c;
    }

    m_cube[sideNum][0].c = tempSide[2];
    m_cube[sideNum][6].c = tempSide[0];
    m_cube[sideNum][8].c = tempSide[6];
    m_cube[sideNum][2].c = tempSide[8];

    m_cube[sideNum][1].c = tempSide[5];
    m_cube[sideNum][3].c = tempSide[1];
    m_cube[sideNum][7].c = tempSide[3];
    m_cube[sideNum][5].c = tempSide[7];

    // for(int s = 0; s < NUM_SIDES; ++s)
    // {
    //     log_printf("Side #%d: \n",s);
    //     for(int i = 0; i < NUM_SQUARES; ++i)
    //     {
    //         if (i == 0 || i == 3 || i == 6)
    //         {
    //             log_printf("%s %s %s\t\t",ColorToStr(m_cube[s][i].c).c_str(),ColorToStr(m_cube[s][i+1].c).c_str(),ColorToStr(m_cube[s][i+2].c).c_str());
    //             if (sideNum == s)
    //             {
    //                 log_printf("%s %s %s",
    //                 ColorToStr(tempSide[i]).c_str(),ColorToStr(tempSide[i+1]).c_str(),ColorToStr(tempSide[i+2]).c_str());
    //             }
    //         log_printf("\n");
    //         }
    //     }
    // }
}

void Cube::DisplayCube()
{
    log_printf("=== Display Cube ===\n");
    for(int s = 0; s < NUM_SIDES; ++s)
    {
        log_printf("Side #%d: \n",s);
        for(int i = 0; i < NUM_SQUARES; ++i)
        {
            if (i == 0 || i == 3 || i == 6)
            {
                log_printf("%s %s %s\n",ColorToStr(m_cube[s][i].c).c_str(),ColorToStr(m_cube[s][i+1].c).c_str(),ColorToStr(m_cube[s][i+2].c).c_str());
            }
        }
    }
}

void Cube::RotateCube(short sideNum, short dirSwiped) // Green is up, white is front
{
    bool prime = false;
    // if ((sideNum != 5 && sideNum != 0) && (dirSwiped == 0 || dirSwiped == 8)) // F
    // {
    //     prime = dirSwiped != 0 ? true : false;
    //     FrontRotation(prime);
    // }
    // else if ((sideNum != 4 && sideNum != 2) && (dirSwiped == 2 || dirSwiped == 6)) // R
    // {
    //     prime = dirSwiped != 2 ? true : false;
    //     RightRotation(prime);
    // }
    // else if ((sideNum != 3 && sideNum != 1) && (dirSwiped == 9 || dirSwiped == 5)) // U
    // {
    //     prime = dirSwiped != 5 ? true : false;
    //     UpRotation(prime);
    // }
    // else if ((sideNum != 0 && sideNum != 5) && (dirSwiped == 2 || dirSwiped == 6)) // B
    // {
    //     prime = dirSwiped != 2 ? true : false;
    //     BackRotation(prime);
    // }
    // else if ((sideNum != 2 && sideNum != 4) && (dirSwiped == 0 || dirSwiped == 8)) // L
    // {
    //     prime = dirSwiped != 0 ? true : false;
    //     LeftRotation(prime);
    // }
    // else if ((sideNum != 2 && sideNum != 4) && (dirSwiped == 3 || dirSwiped == 11)) // D
    // {
    //     prime = dirSwiped != 3 ? true : false;
    //     DownRotation(prime);
    // }

    log_printf("DirSwiped: %d\n",dirSwiped);
    switch(sideNum)
    {
        case 0:
            switch(dirSwiped)
            {
                case 5: 
                    prime = true;
                case 9: 
                    UpRotation(prime); 
                break;

                case 3: 
                    prime = true;
                case 11:
                    DownRotation(prime);
                break;

                case 8:
                    prime = true;
                case 0:
                    LeftRotation(prime);
                break;

                case 2:
                    prime = true;
                case 6:
                    RightRotation(prime);
                break;
            }
            break;
        case 1:
            switch(dirSwiped)
            {
                case 5: prime = true; case 9: BackRotation(prime); break;
                case 11: prime = true; case 3: FrontRotation(prime); break;
                case 8: prime = true; case 0: LeftRotation(prime); break;
                case 2: prime = true; case 6: RightRotation(prime); break;
            }
            break;
        case 2:
            switch(dirSwiped)
            {
                case 5: prime = true; case 9: UpRotation(prime); break;
                case 11: prime = true; case 3: DownRotation(prime); break;
                case 8: prime = true; case 0: FrontRotation(prime); break;
                case 2: prime = true; case 6: BackRotation(prime); break;
            }
            break;
        case 3:
            switch(dirSwiped)
            {
                case 5: prime = true; case 9: FrontRotation(prime); break;
                case 11: prime = true; case 3: BackRotation(prime); break;
                case 8: prime = true; case 0: LeftRotation(prime); break;
                case 2: prime = true; case 6: RightRotation(prime); break;
            }
            break;
        case 4:
            switch(dirSwiped)
            {
                case 5: prime = true; case 9: UpRotation(prime); break;
                case 11: prime = true; case 3: DownRotation(prime); break;
                case 8: prime = true; case 0: BackRotation(prime); break;
                case 2: prime = true; case 6: FrontRotation(prime); break;
            }
            break;
        case 5:
            switch(dirSwiped)
            {
                case 5: prime = true; case 9: UpRotation(prime); break;
                case 11: prime = true; case 3: DownRotation(prime); break;
                case 8: prime = true; case 0: RightRotation(prime); break;
                case 2: prime = true; case 6: LeftRotation(prime); break;
            }
            break;
    }
}
int Cube::GetSideNum()
{
    return m_side_num;
}
void Cube::FrontRotation(bool prime)
{
    if (prime)
            log_printf("Prime Front Rotation\n");
        else
            log_printf("Front Rotation\n");

        int sidesToMove[MOVE_SIZE] = {4,3,2,1}; //Normal


        int sqToMove[MOVE_SIZE][SWIPE_SIZE] = {
                                            {6,7,8},
                                            {2,5,8},
                                            {0,1,2},
                                            {0,3,6}
                                            };

        Rotate(sidesToMove, sqToMove, 0, prime);
}

void Cube::RightRotation(bool prime)
{
    if (prime)
        log_printf("Prime Right Rotation\n");
    else
        log_printf("Right Rotation\n");

    int sidesToMove[MOVE_SIZE] = {3,5,1,0}; //Normal


    int sqToMove[MOVE_SIZE][SWIPE_SIZE] = {
                                          {2,5,8},
                                          {2,5,8},
                                          {0,3,6},
                                          {2,5,8}
                                          };

    Rotate(sidesToMove, sqToMove, 2, prime);

}

void Cube::UpRotation(bool prime)
{
    if (prime)
        log_printf("Prime Up Rotation\n");
    else
        log_printf("Up Rotation\n");

    int sidesToMove[MOVE_SIZE] = {5,2,0,4}; //Normal


    int sqToMove[MOVE_SIZE][SWIPE_SIZE] = {
                                        {0,1,2},
                                        {0,1,2},
                                        {0,1,2},
                                        {0,1,2}
                                        };

    Rotate(sidesToMove, sqToMove, 1, prime);
}

void Cube::BackRotation(bool prime)
{
    if (prime)
        log_printf("Prime Back Rotation\n");
    else
        log_printf("Back Rotation\n");

    int sidesToMove[MOVE_SIZE] = {4,3,2,1}; //Normal


    int sqToMove[MOVE_SIZE][SWIPE_SIZE] = {
                                        {0,1,2},
                                        {0,3,6},
                                        {6,7,8},
                                        {2,5,8}
                                        };

    Rotate(sidesToMove, sqToMove, 5, prime);
}

void Cube::LeftRotation(bool prime)
{
    if (prime)
        log_printf("Prime Left Rotation\n");
    else
        log_printf("Left Rotation\n");

    int sidesToMove[MOVE_SIZE] = {3,5,1,0}; //Normal


    int sqToMove[MOVE_SIZE][SWIPE_SIZE] = {
                                          {0,3,6},
                                          {0,3,6},
                                          {2,5,8},
                                          {0,3,6}
                                          };

    Rotate(sidesToMove, sqToMove, 4, prime);
}

void Cube::DownRotation(bool prime)
{
    if (prime)
        log_printf("Prime Down Rotation\n");
    else
        log_printf("Down Rotation\n");

    int sidesToMove[MOVE_SIZE] = {5,2,0,4}; //Normal


    int sqToMove[MOVE_SIZE][SWIPE_SIZE] = {
                                        {6,7,8},
                                        {6,7,8},
                                        {6,7,8},
                                        {6,7,8}
                                        };

    Rotate(sidesToMove, sqToMove, 3, prime);
}

String Cube::ColorToStr(int color)
{
    String retColor;
    
    switch(color)
    {
        case WHITE:
            retColor = "W";
            break;
        case BLUE:
            retColor = "B";
            break;
        case RED:
            retColor = "R";
            break;
        case GREEN:
            retColor = "G";
            break;
        case ORANGE:
            retColor = "O";
            break;
        case YELLOW:
            retColor = "Y";
            break;
    }
    return retColor;
}
