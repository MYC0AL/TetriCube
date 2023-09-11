#include "Cube.h"
#include "Display_helper.h"
#include "State_driver.h"

//#define DISP_NUM 0

const char* HOME_SCREEN = "/home_screen_480.jpg";

void setup(void)
{
  

  //Initialize rubiks cube object
  //Cube rbx;
  
  Serial.begin(115200);

  touch_init();

  if (sd_init() == TC_SUCCESS)
  {
    drawImage(HOME_SCREEN);
  }

  //rbx.startGame();
}

void loop(void)
{
}