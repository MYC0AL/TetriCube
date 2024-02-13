#include "State_driver.h"

/***********************
 * Tags applied to code
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

  // Update start state
  drv.request_state_change(STATE_START);

  // Infinite state controller
  drv.state_controller();
}

void loop(void)
{
}