#include "Cube.h"
#define LGFX_AUTODETECT
#define ESP32_8048S043

//#define DISP_NUM 0

void setup(void)
{
  Serial.begin(115200);
  Cube rbx;
  rbx.startGame();
}

void loop(void)
{
}