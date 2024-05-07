#include "Driver.h"

/***********************
* Tags applied to code *
************************/
// TODO: Things to do
// DEBUG: Code implemented to debug
// BUG: Code known to be broken
// POC: Temporary proof of concept code

void setup(void)
{

  delay(10);

  // Initialize state driver
  StateDriver drv;

  #if (SCREEN_NUM == 0 && DEBUG_MODE != 1)
  {
    delay(1000);
    drv.request_state_change(STATE_START);
  }
  #endif

  // Infinite state controller
  drv.state_controller();
}

void loop(void)
{
}