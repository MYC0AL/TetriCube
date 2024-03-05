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

/*******************************************************************
 * @brief Default ctor for Rubik's Cube
*******************************************************************/
Cube::Cube() : m_side_num(0), m_prevSq(-1), m_prevTouchCount(0), m_prevDist(0), m_swipedFlag(false)
{
    for (int i = 0; i < PREV_ARR_SIZE; ++i)
        m_prevTouches[i] = -1;

    InitCube();
}

/******************************************************************
 * @brief Continue playing the rubik's cube by polling the inputs
 * @return A CMD that describes the current movement of the cube
*******************************************************************/
std::string Cube::PlayGame()
{   
    std::string cmd;
    if (m_swipedFlag)
    {
        short dir = DirSwiped();
        if (dir != SWIPE_ERR)
        {
            //RotateCube(m_side_num, dir);
            ResetVars(); //TODO Remove this and combine one below
            //drawRubiksSide(m_side_num);

            // Form CMD for ELs
            cmd += m_side_num + '0';
            cmd += 'C';
            cmd += dir + '0';
        }
        else
            ResetVars();

        m_swipedFlag = false;
    }
    return cmd;
}

/******************************************************************
 * @brief Draw one side of the Rubik's Cube
 * @param sideNum The side of the cube to draw
 * @param drawCenter Update the center square as well
******************************************************************/
void Cube::DrawRubiksSide(int sideNum, bool drawCenter)
{
    for (int i = 0; i < NUM_SQUARES; i++)
    {
        if (!(drawCenter == false && i == 4))
        {
            gfx->fillRect(m_cube[sideNum][i].x,
                            m_cube[sideNum][i].y,
                            m_cube[sideNum][i].w,
                            m_cube[sideNum][i].h,
                            m_cube[sideNum][i].c);
        }
    }
}

/******************************************************************
 * @brief A debugging tool to draw the entire cube on one screen
******************************************************************/
void Cube::DrawRubiksCube()
{
    gfx->setCursor(200,200);
    gfx->setTextSize(4);
    gfx->setTextColor(PURPLE);
    for (int i = 0; i < NUM_SIDES; i++)
    {
        DrawRubiksSide(i);
        gfx->print(i);
        gfx->setCursor(200,200);
        sleep(2);
    }
    DrawRubiksSide(m_side_num);
}

/******************************************************************
 * @brief Find the square that was touched by the user
 * @param xPos The x position of where the user touched
 * @param yPos The y position of where the user touched
 * @return Square number that was touched, -1 for invalid region
******************************************************************/
short Cube::SqTapped(short xPos, short yPos)
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

/******************************************************************
 * @brief Find the direction that the user swiped in
 * @return The number that corrolates with the direction rotated in
******************************************************************/
short Cube::DirSwiped()
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

/******************************************************************
 * @brief Function to allow driver to input user touch points
******************************************************************/
void Cube::TouchUpdate(uint16_t touch_x, uint16_t touch_y)
{    
    short sqTouch = SqTapped(touch_x, touch_y);
    if (sqTouch != -1 && sqTouch != m_prevSq)
    {
        m_prevSq = sqTouch;

        if (m_prevTouches[0] == -1)
        {
            m_prevTouches[m_prevTouchCount] = m_prevSq;
            m_prevTouchCount = (m_prevTouchCount + 1) % SWIPE_SIZE;
        }
        else if (ValidTouch())
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

/******************************************************************
 * @brief Function to help form the 3 touch sequence needed in
 * rotating the cube
 * @return True if square touched was valid, false if not
******************************************************************/
bool Cube::ValidTouch()
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

/******************************************************************
 * @brief Reset the commonly used variables to default values
******************************************************************/
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

/******************************************************************
 * @brief Initialize the Rubik's Cube
******************************************************************/
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

/******************************************************************
 * @brief Rotate the cube
 * @param sidesToMove List of sides in which to rotate
 * @param sqToMove List of every side of the cube that will move,
 * contaning a list of cells that would be during a rotation
 * @param spinSide The side number of the side that will rotate 90
 * degrees when a rotation happens
 * @param prime Indicator if the rotation is prime or not
******************************************************************/
void Cube::Rotate(int sidesToMove[MOVE_SIZE], int sqToMove[MOVE_SIZE][SWIPE_SIZE], int spinSide, bool prime)
{
    if (prime)
    {
        ReverseArray(sidesToMove, MOVE_SIZE);
        std::swap(sqToMove[0],sqToMove[3]);
        std::swap(sqToMove[1],sqToMove[2]);
    }

    // Make a copy of current cube
    int tempCube[NUM_SIDES][NUM_SQUARES];
    for(int si = 0; si < NUM_SIDES; ++si)
        for(int sq = 0; sq < NUM_SQUARES; ++sq)
            tempCube[si][sq] = m_cube[si][sq].c;

    for(int i = 0; i < MOVE_SIZE; ++i)
    {
        int prevSide = i - 1 < 0 ? MOVE_SIZE - 1 : i - 1;
        for(int j = 0; j < SWIPE_SIZE; ++j)
        {
            m_cube[sidesToMove[i]][sqToMove[i][j]].c = tempCube[sidesToMove[prevSide]][sqToMove[prevSide][j]];
        }
    }

    // Rotate host side
    RotateSide(spinSide, prime);
}

/******************************************************************
 * @brief Rotate one side of the cube
 * @param sideNum Side of the cube to be rotated
 * @param prime Indicator whether the rotation is prime or not
******************************************************************/
void Cube::RotateSide(short sideNum, bool prime)
{
    // Create a temp of the rotating side
    int tempSide[NUM_SQUARES];
    for(int i = 0; i < NUM_SQUARES; ++i)
    {
        tempSide[i] = m_cube[sideNum][i].c;
    }

    const int ROT_COUNT = 4;

    // Indexs for rotation
    int edgeIdx[ROT_COUNT] = {2,0,6,8};
    int sideIdx[ROT_COUNT] = {1,3,7,5};

    // Reverse arrays if prime
    if (prime)
    {
        ReverseArray(edgeIdx,ROT_COUNT);
        ReverseArray(sideIdx,ROT_COUNT);
    }

    // Rotate the side
    for(int i = 0; i < ROT_COUNT; ++i)
    {
        int nextIdx = (i + 1) % ROT_COUNT;
        m_cube[sideNum][edgeIdx[i]].c = tempSide[edgeIdx[nextIdx]];
        m_cube[sideNum][sideIdx[i]].c = tempSide[sideIdx[nextIdx]];
    }
}

/******************************************************************
 * @brief A debugging tool that prints the cube to the console
******************************************************************/
void Cube::PrintCube()
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
    log_printf("\n");
}

/******************************************************************
 * @brief Decides which rotation occured based on the side that was
 * touched and the direction of the swipe
 * @param sideNum The side number that was touched by the user
 * @param dirSwiped The direction the user swiped
******************************************************************/
void Cube::RotateCube(short sideNum, short dirSwiped)
{
    bool prime = false;

    switch(sideNum)
    {
        case 0:
            switch(dirSwiped)
            {
                case 5: prime = true; case 9: UpRotation(prime); break;
                case 11: prime = true; case 3: DownRotation(prime); break;
                case 8:prime = true; case 0: LeftRotation(prime); break;
                case 2:prime = true; case 6: RightRotation(prime); break;
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

/******************************************************************
 * @brief Set the current side number of the Cube
 * @param side_num The desired side number
******************************************************************/
void Cube::SetSideNum(int side_num)
{
    m_side_num = side_num;
    log_printf("CUBE: SIDE NUM SET TO %d\n\r",m_side_num);
}

/******************************************************************
 * @brief Get the current side number of the cube
 * @return Current side number
******************************************************************/
int Cube::GetSideNum()
{
    return m_side_num;
}

/******************************************************************
 * @brief Setup variables for a front rotation
 * @param prime Whether or not the rotation is prime
******************************************************************/
void Cube::FrontRotation(bool prime)
{
    int sidesToMove[MOVE_SIZE] = {1,2,3,4};

    int sqToMove[MOVE_SIZE][SWIPE_SIZE] = {
                                        {6,7,8}, // 1
                                        {0,3,6}, // 2
                                        {2,1,0}, // 3
                                        {8,5,2}  // 4
                                        };

    Rotate(sidesToMove, sqToMove, 0, prime);
}

/******************************************************************
 * @brief Setup variables for a right rotation
 * @param prime Whether or not the rotation is prime
******************************************************************/
void Cube::RightRotation(bool prime)
{
    int sidesToMove[MOVE_SIZE] = {0,1,5,3}; //Normal

    int sqToMove[MOVE_SIZE][SWIPE_SIZE] = {
                                          {2,5,8},
                                          {2,5,8},
                                          {6,3,0},
                                          {2,5,8}
                                          };

    Rotate(sidesToMove, sqToMove, 2, prime);

}

/******************************************************************
 * @brief Setup variables for an up rotation
 * @param prime Whether or not the rotation is prime
******************************************************************/
void Cube::UpRotation(bool prime)
{
    int sidesToMove[MOVE_SIZE] = {0,4,5,2}; //Normal

    int sqToMove[MOVE_SIZE][SWIPE_SIZE] = {
                                        {0,1,2},
                                        {0,1,2},
                                        {0,1,2},
                                        {0,1,2}
                                        };

    Rotate(sidesToMove, sqToMove, 1, prime);
}

/******************************************************************
 * @brief Setup variables for a back rotation
 * @param prime Whether or not the rotation is prime
******************************************************************/
void Cube::BackRotation(bool prime)
{
    int sidesToMove[MOVE_SIZE] = {4,3,2,1};

    int sqToMove[MOVE_SIZE][SWIPE_SIZE] = {
                                        {6,3,0},
                                        {8,7,6},
                                        {2,5,8},
                                        {0,1,2}
                                        };

    Rotate(sidesToMove, sqToMove, 5, prime);
}

/******************************************************************
 * @brief Setup variables for a left rotation
 * @param prime Whether or not the rotation is prime
******************************************************************/
void Cube::LeftRotation(bool prime)
{
    int sidesToMove[MOVE_SIZE] = {0,3,5,1}; //Normal

    int sqToMove[MOVE_SIZE][SWIPE_SIZE] = {
                                          {0,3,6},
                                          {0,3,6},
                                          {8,5,2},
                                          {0,3,6}
                                          };

    Rotate(sidesToMove, sqToMove, 4, prime);
}

/******************************************************************
 * @brief Setup variables for a down rotation
 * @param prime Whether or not the rotation is prime
******************************************************************/
void Cube::DownRotation(bool prime)
{
    int sidesToMove[MOVE_SIZE] = {0,2,5,4}; //Normal

    int sqToMove[MOVE_SIZE][SWIPE_SIZE] = {
                                        {6,7,8},
                                        {6,7,8},
                                        {6,7,8},
                                        {6,7,8}
                                        };

    Rotate(sidesToMove, sqToMove, 3, prime);
}

/******************************************************************
 * @brief Reset the cube to initial values by initializing it again
******************************************************************/
void Cube::SolveCube()
{
    InitCube();
    DrawRubiksSide(m_side_num,false);
}

/******************************************************************
 * @brief Convert a color to a string representation
 * @param color Integer representing the color to be converted
 * @return String representing the color provided
******************************************************************/
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

/******************************************************************
 * @brief Helper function that reversed a provided array
 * @param arr Array to be reversed
 * @param length The length of the array
******************************************************************/
void Cube::ReverseArray(int arr[], int length)
{
    int start = 0;
    int end = length - 1;

    while (start < end) {
        // Swap elements at start and end indices
        int temp = arr[start];
        arr[start] = arr[end];
        arr[end] = temp;

        // Move indices towards the center
        start++;
        end--;
    }
}
