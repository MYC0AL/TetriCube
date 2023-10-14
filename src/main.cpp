#include "State_driver.h"
//#include "Cube.h"

#define IO20 20
#define IO19 19
#define IO18 18
#define IO17 17

void setup(void)
{
  Serial.begin(115200);

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