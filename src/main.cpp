#include "Cube.h"
#include "State_driver.h"

// JPG files
const char* HOME_SCREEN_FILE = "/home_screen_opt.jpg";
const char* SELECT_GAME_FILE = "/select_game_opt.jpg";

void setup(void)
{
  Serial.begin(115200);

  // Initialize state driver
  StateDriver drv;

  drv.dHelp.touch_init();
  drv.dHelp.gfx_init();  

  if (drv.dHelp.sd_init() == TC_SUCCESS)
  {
    while(1)
    {
      drv.dHelp.drawImage(HOME_SCREEN_FILE);
      sleep(2);
      drv.dHelp.drawImage(SELECT_GAME_FILE);
      sleep(2);
    }
  }

  //Initialize rubiks cube object
  //Cube rbx;

  //rbx.startGame();
}

void loop(void)
{
}