#include "State_driver.h"
//#include "Cube.h"

// TODO: Things to do
// DEBUG: Code implemented to debug
// BUG: Code known to be broken
// POC: Temporary proof of concept code

void setup(void)
{
  //Serial.begin(9600);

  // Initialize state driver
  StateDriver drv;

  // Update start state
  drv.request_state_change(STATE_START);

  // Infinite state controller
  drv.state_controller();

  //Initialize rubiks cube object
  //Cube rbx;

  //rbx.startGame();
}

void loop(void)
{
}