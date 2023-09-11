#include "Cube.h"

// Globals for Cube
LGFX display;
SQ SQ_POS[9] = {{RBX_SQ0}, {RBX_SQ1}, {RBX_SQ2}, {RBX_SQ3}, {RBX_SQ4}, {RBX_SQ5}, {RBX_SQ6}, {RBX_SQ7}, {RBX_SQ8}};

// short DISP_NUM = 0;
short possibleSwipes[12][SWIPE_SIZE] = {{6, 3, 0}, {7, 4, 1}, {8, 5, 2}, // 0 1 2
                                        {0, 1, 2},
                                        {3, 4, 5},
                                        {6, 7, 8}, // 3 4 5
                                        {2, 5, 8},
                                        {1, 4, 7},
                                        {0, 3, 6}, // 6 7 8
                                        {8, 7, 6},
                                        {5, 4, 3},
                                        {2, 1, 0}}; // 9 10 11

short prevSq = -1;
short prevTouches[PREV_ARR_SIZE] = {-1, -1, -1};
short prevTouchCount = 0;
short prevDist = 0;
bool validity;
short x, y;

// Color stuff
unsigned short color_index = 0;
int RBX_Colors[6] = {WHITE, BLUE, RED, GREEN, ORANGE, YELLOW};

Cube::Cube()
{
    InitCube();
    InitDisplay();
}

Cube::~Cube()
{

}

void Cube::InitDisplay()
{
    // Init display
    display.init();

    // LGFX stuff
    display.setRotation(4);
    display.setColorDepth(16);
}

void Cube::startGame()
{
    // Draw initial rubiks cube
    drawRubiksSide(SIDE_NUM);

    // Infinite game loop
    while (1)
    {
        // If a touch was detected
        if (display.getTouch(&x, &y))
        {
            short sqTouch = sqTapped(x, y);
            if (sqTouch != -1 && sqTouch != prevSq)
            {
                prevSq = sqTouch;

                if (prevTouches[0] == -1)
                {
                    prevTouches[prevTouchCount] = prevSq;
                    prevTouchCount = (prevTouchCount + 1) % SWIPE_SIZE;
                }
                else if (validTouch())
                {
                    prevTouches[prevTouchCount] = prevSq;

                    if (prevTouchCount + 1 == SWIPE_SIZE)
                    {
                        short dir = dirSwiped();
                        if (dir != SWIPE_ERR)
                        {
                            RotateCube(SIDE_NUM, dir);
                            resetGlobals();
                            DisplayCube();
                            // drawRubiksSide(SIDE_NUM);
                            drawRubiksCube();
                        }
                        else
                            resetGlobals();
                    }
                    prevTouchCount = (prevTouchCount + 1) % SWIPE_SIZE;
                }
                else
                {
                    resetGlobals();
                }
            }
        }
    }
}

void Cube::drawRubiksSide(int sideNum)
{
    for (int i = 0; i < NUM_SQUARES; i++)
    {
        display.fillRect(m_cube[sideNum][i].x,
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
    drawRubiksSide(SIDE_NUM);
}

short Cube::sqTapped(short xPos, short yPos)
{
    short SQ_num = -1;
    bool found = false;
    for (int i = 0; i < NUM_SQUARES && !found; i++)
    {
        if ((xPos > m_cube[SIDE_NUM][i].x && xPos < m_cube[SIDE_NUM][i].x + m_cube[SIDE_NUM][i].w) &&
            (yPos > m_cube[SIDE_NUM][i].y && yPos < m_cube[SIDE_NUM][i].y + m_cube[SIDE_NUM][i].h))
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
            if (prevTouches[j] == possibleSwipes[i][j])
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

bool Cube::validTouch()
{
    validity = false;

    if (prevTouchCount == 1)
    {
        if ((abs(prevSq - prevTouches[prevTouchCount - 1]) == 1) &&
            (m_cube[SIDE_NUM][prevSq].y == m_cube[SIDE_NUM][prevTouches[prevTouchCount - 1]].y))
        {
            validity = true;
            prevDist = 1;
        }
        else if (abs(prevSq - prevTouches[prevTouchCount - 1]) == 3)
        {
            validity = true;
            prevDist = 3;
        }
    }
    else if (prevTouchCount == 2)
    {
        if (abs(prevSq - prevTouches[prevTouchCount - 1]) == prevDist)
        {
            validity = true;
        }
    }
    return validity;
}

void Cube::displayAllData()
{
    char buff[200];
    sprintf(buff, "PrevTouches: %d, %d, %d\nTouchCount: %d\nValidity: %d \n", prevTouches[0], prevTouches[1], prevTouches[2], prevTouchCount, validity);
    Serial.println(buff);
}

void Cube::resetGlobals()
{
    for (int i = 0; i < SWIPE_SIZE; i++)
        prevTouches[i] = -1;
    prevTouchCount = 0;
    prevTouches[0] = prevSq;
    prevTouchCount++;
    prevDist = 0;
    prevSq = -1;
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

void Cube::Rotate(short sidesToMove[5], short sqToMove[3], bool prime) // Front rotation: side 0
{
    // Create temp of first side in sequence
    short tempIndex = prime ? 3 : 0;
    short tempOp = prime ? 0 : 3;
    SQ tempSq[SWIPE_SIZE] = {m_cube[sidesToMove[tempIndex]][sqToMove[0]],
                             m_cube[sidesToMove[tempIndex]][sqToMove[1]],
                             m_cube[sidesToMove[tempIndex]][sqToMove[2]]};

    short startIndex = !prime ? 0 : 3;
    short endIndex = !prime ? 3 : 0;
    short increment = !prime ? 1 : -1;

    // Rotate all sides
    for (int i = startIndex; i != endIndex; i += increment)
    {
        for (int j = 0; j < 3; j++)
        {
            m_cube[sidesToMove[i]][sqToMove[j]] = m_cube[sidesToMove[i + increment]][sqToMove[j]];
        }
    }
    // Replace temp
    for (int i = 0; i < 3; i++)
    {
        m_cube[sidesToMove[tempOp]][sqToMove[i]] = tempSq[i];
    }

    // Rotate host side
    rotateSide(sidesToMove[4], prime); // prime and cw are inverse, sidesToMove[4] is  the face of the roating side.
}

void Cube::rotateSide(short sideNum, bool prime)
{
    const short sqArrSize = 4;
    short cornerRotSeq[sqArrSize] = {0, 6, 8, 2};
    short edgeRotSeq[sqArrSize] = {1, 3, 7, 5};

    SQ tempCorner = m_cube[sideNum][cornerRotSeq[prime ? 0 : sqArrSize - 1]];
    SQ tempEdge = m_cube[sideNum][edgeRotSeq[prime ? 0 : sqArrSize - 1]];

    for (int i = prime ? 0 : sqArrSize - 1; prime ? i < sqArrSize - 1 : i > 0; prime ? i++ : i--)
    {
        m_cube[sideNum][cornerRotSeq[i]] = m_cube[sideNum][cornerRotSeq[i - (prime ? 0 : 1)]];
        m_cube[sideNum][edgeRotSeq[i]] = m_cube[sideNum][edgeRotSeq[i - (prime ? 0 : 1)]];
    }

    m_cube[sideNum][cornerRotSeq[prime ? sqArrSize - 1 : 0]] = tempCorner;
    m_cube[sideNum][edgeRotSeq[prime ? sqArrSize - 1 : 0]] = tempEdge;
}

void Cube::DisplayCube() // Print doesnt work
{
    for (int i = 0; i < NUM_SIDES; i++)
    {
        Serial.print("Side #");
        Serial.println(i);
        for (int j = 0; j < NUM_SQUARES; j++)
        {
            Serial.print(m_cube[i][j].c, HEX);
            Serial.print(" ");
        }
        Serial.println("\n");
    }
}

void Cube::RotateCube(short sideNum, short dirSwiped) // Green is up, white is front
{
    bool prime;
    if ((sideNum != 5 && sideNum != 0) && (dirSwiped == 0 || dirSwiped == 8)) // F
    {
        prime = dirSwiped != 0 ? true : false;
        FrontRotation(prime);
    }
    else if ((sideNum != 4 && sideNum != 2) && (dirSwiped == 2 || dirSwiped == 6)) // R
    {
        prime = dirSwiped != 2 ? true : false;
        RightRotation(prime);
    }
    else if ((sideNum != 3 && sideNum != 1) && (dirSwiped == 9 || dirSwiped == 5)) // U
    {
        prime = dirSwiped != 5 ? true : false;
        UpRotation(prime);
    }
    else if ((sideNum != 0 && sideNum != 5) && (dirSwiped == 2 || dirSwiped == 6)) // B
    {
        prime = dirSwiped != 2 ? true : false;
        BackRotation(prime);
    }
    else if ((sideNum != 2 && sideNum != 4) && (dirSwiped == 0 || dirSwiped == 8)) // L
    {
        prime = dirSwiped != 0 ? true : false;
        LeftRotation(prime);
    }
    else if ((sideNum != 2 && sideNum != 4) && (dirSwiped == 3 || dirSwiped == 11)) // D
    {
        prime = dirSwiped != 3 ? true : false;
        DownRotation(prime);
    }
}
void Cube::FrontRotation(bool prime)
{
    short sidesToMove[5] = {1, 2, 3, 4, 0};
    short sqToMove[3] = {6, 7, 8};
    Rotate(sidesToMove, sqToMove, prime);
}

void Cube::RightRotation(bool prime)
{
    Serial.println("Right Rotation\r\n");
    short sidesToMove[5] = {3, 5, 1, 0, 2}; // 0 1 5 3
    short sqToMove[3] = {2, 5, 8};
    Rotate(sidesToMove, sqToMove, prime);
}

void Cube::UpRotation(bool prime)
{
    Serial.println("Up Rotation\r\n");
    short sidesToMove[5] = {4, 0, 2, 5, 1};
    short sqToMove[3] = {6, 7, 8};
    Rotate(sidesToMove, sqToMove, prime);
}

void Cube::BackRotation(bool prime)
{
    Serial.println("Back Rotation\r\n");
    short sidesToMove[5] = {1, 2, 3, 4, 5};
    short sqToMove[3] = {0, 1, 2};
    Rotate(sidesToMove, sqToMove, prime);
}

void Cube::LeftRotation(bool prime)
{
    Serial.println("Left Rotation\r\n");
    short sidesToMove[5] = {3, 5, 1, 0, 4}; // 0 1 5 3
    short sqToMove[3] = {0, 3, 6};
    Rotate(sidesToMove, sqToMove, prime);
}

void Cube::DownRotation(bool prime)
{
    Serial.println("Down Rotation\r\n");
    short sidesToMove[5] = {4, 0, 2, 5, 3};
    short sqToMove[3] = {0, 1, 2};
    Rotate(sidesToMove, sqToMove, prime);
}