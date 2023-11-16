#ifndef CUBE_H
#define CUBE_H

#include "Display_helper.h"

/* Direction Code for cube
 *     8 7 6
 *     _ _ _
 * 9  |0 1 2| 5
 * 10 |3 4 5| 4
 * 11 |6 7 8| 3
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
#define MOVE_SIZE 4
#define NUM_SQUARES 9
#define NUM_SIDES 6

#define SWIPE_ERR -1

enum cube_error_t {CUBE_SUCCESS, CUBE_ERR};

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
  
  short m_prevSq;
  short m_prevTouches[PREV_ARR_SIZE];
  short m_prevTouchCount;
  short m_prevDist;
  bool m_validity;  
  bool m_swipedFlag;

  int m_side_num;

public:
  Cube();
  ~Cube();
  void PlayGame();
  void drawRubiksSide(int sideNum);
  void drawRubiksCube();
  short sqTapped(short xPos, short yPos);
  short dirSwiped();
  void TouchUpdate(uint16_t touch_x, uint16_t touch_y);
  bool validTouch();
  void ResetVars();
  void InitCube();
  void Rotate(int sidesToMove[MOVE_SIZE], int sqToMove[MOVE_SIZE][SWIPE_SIZE], int spinSide, bool prime);
  void rotateSide(short sideNum, bool prime);
  void PrintCube();
  void RotateCube(short sideNum, short dirSwiped);
  void SetSideNum(int side_num);

  int GetSideNum();

  String ColorToStr(int color);
  void ReverseArray(int arr[], int length);

  void FrontRotation(bool prime);
  void RightRotation(bool prime);
  void UpRotation(bool prime);
  void BackRotation(bool prime);
  void LeftRotation(bool prime);
  void DownRotation(bool prime);
};
#endif