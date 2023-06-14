//Make sure to use version 1.3.1 or hell breaks loose
#include <Arduino_GFX_Library.h>

#define I2C_SDA_PIN 17
#define I2C_SCL_PIN 18

#define JPEG_FILENAME_LOGO "/logo.jpg"
#define JPEG_FILENAME_COVER "/cover.jpg"
#define JPEG_FILENAME_COVER_01 "/cover01.jpg"

#define JPEG_FILENAME_01 "/image01.jpg"
#define JPEG_FILENAME_02 "/image02.jpg"
#define JPEG_FILENAME_03 "/image03.jpg"
#define JPEG_FILENAME_04 "/image04.jpg"
#define JPEG_FILENAME_05 "/image05.jpg"
#define JPEG_FILENAME_06 "/image06.jpg"
#define JPEG_FILENAME_07 "/image07.jpg"

//microSD card
#define SD_SCK  12
#define SD_MISO 13
#define SD_MOSI 11
#define SD_CS   10

#define AUDIO_FILENAME_01   "/ChildhoodMemory.mp3"

#define AUDIO_FILENAME_02   "/SoundofSilence.mp3"

#define AUDIO_FILENAME_03   "/MoonlightBay.mp3"

//I2S
#define I2S_DOUT      19
#define I2S_BCLK      20
#define I2S_LRCK      46

#define TOUCH_INT -1
#define TOUCH_RST 38

#define TOUCH_ROTATION ROTATION_NORMAL
#define TOUCH_MAP_X1 480
#define TOUCH_MAP_X2 0
#define TOUCH_MAP_Y1 480
#define TOUCH_MAP_Y2 0

#define GFX_BL -1
#define TFT_BL GFX_BL

Arduino_ESP32RGBPanel *bus = new Arduino_ESP32RGBPanel(
  1 /* CS */, 12 /* SCK */, 11 /* SDA */,
  45 /* DE */, 4/* VSYNC */, 5 /* HSYNC */, 21 /* PCLK */,
  39 /* R0 */, 40 /* R1 */, 41 /* R2 */, 42 /* R3 */, 2 /* R4 */,
  0 /* G0/P22 */, 9 /* G1/P23 */, 14 /* G2/P24 */, 47 /* G3/P25 */, 48 /* G4/P26 */, 3 /* G5 */,
  6 /* B0 */, 7 /* B1 */, 15 /* B2 */, 16 /* B3 */, 8 /* B4 */
);

Arduino_ST7701_RGBPanel *gfx = new Arduino_ST7701_RGBPanel(
  bus, GFX_NOT_DEFINED /* RST */, 0 /* rotation */,
  true /* IPS */, 480 /* width */, 480 /* height */,
  st7701_type1_init_operations, sizeof(st7701_type1_init_operations),
  true /* BGR */);

