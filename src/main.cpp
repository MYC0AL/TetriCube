#include "Cube.h"
#include "JpegFunc.h"
#include "GFX_JPG.h"

#define LGFX_AUTODETECT
#define ESP32_8048S043

//#define DISP_NUM 0

// pixel drawing callback
static int jpegDrawCallback(JPEGDRAW *pDraw)
{
  gfx->draw16bitBeRGBBitmap(pDraw->x, pDraw->y, pDraw->pPixels, pDraw->iWidth, pDraw->iHeight);
  return 1;
}

void setup(void)
{
  Cube rbx;
  jpegDraw(JPEG_FILENAME_02,
          jpegDrawCallback,
          true /* useBigEndian */,
          0 /* x */,
          0 /* y */,
          gfx->width() /* widthLimit */,
          gfx->height() /* heightLimit */);
  delay(10000);
  rbx.startGame();
}

void loop(void)
{
}