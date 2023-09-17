#include "State_driver.h"

void setup(void)
{
  Serial.begin(115200);

  // Initialize state driver
  StateDriver drv;

  //Update start state
  drv.request_state_change(STATE_START);

  // Infinite state controller
  drv.state_controller();

  //drv.dHelp.drawImage(HOME_SCREEN_FILE);
  //sleep(1);

  //Initialize rubiks cube object
  //Cube rbx;

  //rbx.startGame();
}

void loop(void)
{
}