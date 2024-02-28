#ifndef DISPLAY_HELPER_H
#define DISPLAY_HELPER_H

//Make sure to use version 1.3.1 or hell breaks loose
#include <Arduino_GFX_Library.h>

#include <SD.h>
#include <TAMC_GT911.h>
#include <Wire.h>
#include "SPI.h"
#include "FS.h"
#include "JpegFunc.h"
#include "UI_Elements.h"

#include <unordered_map>

#define LGFX_AUTODETECT
#define ESP32_8048S043

#define I2C_SDA_PIN 17
#define I2C_SCL_PIN 18

//microSD card
#define SD_SCK  12
#define SD_MISO 13
#define SD_MOSI 11
#define SD_CS   10

// Jpg File Names
//const char* HOME_SCREEN_FILE = "/home_screen_opt.jpg";
//const char* SELECT_GAME_FILE = "/select_game_opt.jpg";

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
#define TOUCH_MAX 5

#define GFX_BL -1
#define TFT_BL GFX_BL

// Display number
#define DISP_NUM 0

// Error Codes
#define TC_SD_ERR 1
#define TC_NO_UI_TOUCH 2

// Success Codes
#define TC_SUCCESS 0
#define TC_UI_TOUCH 0

// Typedef for return codes
typedef short tc_ret_code;

// Declared extern to allow cross-file manipulation
extern Arduino_ESP32RGBPanel *bus;
extern Arduino_ST7701_RGBPanel *gfx;
extern TAMC_GT911 ts;

// Class definition
class DisplayHelper
{

private:
    void gfx_init();
    void touch_init(void);
    int sd_init(void);

public:

    DisplayHelper();
    bool touch_touched(void);
    void drawImage(const char *file_name);
    void drawImage(const char *file_name, int x, int y);
    void clear_screen();
    tc_ret_code touch_decoder(UIButton button);
    void touch_reset();
    void drawUI();

    // Touchscreen helper variables
    TP_Point current_touches[TOUCH_MAX];
    uint8_t touch_count;

    std::vector<UIButton> active_ui;

};

#endif
