#ifndef CUBE_H
#define CUBE_H

#include <Arduino.h>
#include <Arduino_GFX_Library.h>
#include "LGFX_ESP32S3_RGB_MakerfabsParallelTFTwithTouch40.h"

/* Direction Code for cube
 *     8 7 6
 *     _ _ _
 * 9  |     | 5
 * 10 |     | 4
 * 11 |_ _ _| 3
 *     0 1 2
 */

#define RBX_SQ0 15, 15, 140, 140
#define RBX_SQ1 170, 15, 140, 140
#define RBX_SQ2 325, 15, 140, 140

#define RBX_SQ3 15, 170, 140, 140
#define RBX_SQ4 170, 170, 140, 140
#define RBX_SQ5 325, 170, 140, 140

#define RBX_SQ6 15, 325, 140, 140
#define RBX_SQ7 170, 325, 140, 140
#define RBX_SQ8 325, 325, 140, 140

#define PREV_ARR_SIZE 3
#define SWIPE_SIZE 3
#define NUM_SQUARES 9
#define NUM_SIDES 6

#define SWIPE_ERR -1

#define SIDE_NUM 0

// Struct for Square info
struct SQ
{
  short x;
  short y;
  short w;
  short h;
  int c;
};

class Cube
{
private:
  SQ m_cube[NUM_SIDES][NUM_SQUARES];

public:
  Cube();
  ~Cube();
  void InitDisplay();
  void startGame();
  void drawRubiksSide(int sideNum);
  void drawRubiksCube();
  short sqTapped(short xPos, short yPos);
  short dirSwiped();
  bool validTouch();
  void displayAllData();
  void resetGlobals();
  void InitCube();
  void Rotate(short sidesToMove[5], short sqToMove[3], bool prime);
  void rotateSide(short sideNum, bool prime);
  void DisplayCube();
  void RotateCube(short sideNum, short dirSwiped);

  void FrontRotation(bool prime);
  void RightRotation(bool prime);
  void UpRotation(bool prime);
  void BackRotation(bool prime);
  void LeftRotation(bool prime);
  void DownRotation(bool prime);
};
#endif