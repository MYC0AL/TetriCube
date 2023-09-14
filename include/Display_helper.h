#ifndef DISPLAY_HELPER_H
#define DISPLAY_HELPER_H

//Make sure to use version 1.3.1 or hell breaks loose
#include <Arduino_GFX_Library.h>

#include <SD.h>
#include <TAMC_GT911.h>
#include <Wire.h>
//#include "Audio.h"
#include "SPI.h"
#include "FS.h"
#include "JpegFunc.h"

#define LGFX_AUTODETECT
#define ESP32_8048S043

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

//Audio audio;

#define TOUCH_INT -1
#define TOUCH_RST 38

#define TOUCH_ROTATION ROTATION_NORMAL
#define TOUCH_MAP_X1 480
#define TOUCH_MAP_X2 0
#define TOUCH_MAP_Y1 480
#define TOUCH_MAP_Y2 0

#define GFX_BL -1
#define TFT_BL GFX_BL

// Display number
#define DISP_NUM 0

// Error Codes
#define TC_SD_ERR 1

// Success Codes
#define TC_SUCCESS 0

// Struct declarations
struct UIButton {
    ushort x;
    ushort y;
    ushort w;
    ushort h;
};

// Declared extern to allow cross-file manipulation
extern Arduino_ESP32RGBPanel *bus;
extern Arduino_ST7701_RGBPanel *gfx;
extern TAMC_GT911 ts;

// Class definition
class DisplayHelper
{
protected:

    // Touchscreen helper variables
    int touch_last_x;
    int touch_last_y;

public:

    void gfx_init();
    void gfx_uninit();
    void touch_init(void);
    bool touch_touched(void);
    int sd_init(void);
    void drawImage(const char * file_name);
    void clear_screen();

};

#endif
