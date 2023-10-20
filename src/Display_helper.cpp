#include "Display_helper.h"

using std::make_pair;

// GFX Bus and display setup
Arduino_ESP32RGBPanel *bus = new Arduino_ESP32RGBPanel(
    1 /* CS */, 12 /* SCK */, 11 /* SDA */,
    45 /* DE */, 4 /* VSYNC */, 5 /* HSYNC */, 21 /* PCLK */,
    39 /* R0 */, 40 /* R1 */, 41 /* R2 */, 42 /* R3 */, 2 /* R4 */,
    0 /* G0/P22 */, 9 /* G1/P23 */, 14 /* G2/P24 */, 47 /* G3/P25 */, 48 /* G4/P26 */, 3 /* G5 */,
    6 /* B0 */, 7 /* B1 */, 15 /* B2 */, 16 /* B3 */, 8 /* B4 */
);

Arduino_ST7701_RGBPanel *gfx = new Arduino_ST7701_RGBPanel(
    bus, GFX_NOT_DEFINED /* RST */, 0 /* rotation */,
    true /* IPS */, 480 /* width */, 480 /* height */,
    st7701_type1_init_operations, sizeof(st7701_type1_init_operations),
    true /* BGR */);

// Touchscreen driver initialization
TAMC_GT911 ts = TAMC_GT911(I2C_SDA_PIN, I2C_SCL_PIN, TOUCH_INT, TOUCH_RST, max(TOUCH_MAP_X1, TOUCH_MAP_X2), max(TOUCH_MAP_Y1, TOUCH_MAP_Y2));

// Out of class call back function
static int jpegDrawCallback(JPEGDRAW *pDraw)
{
  gfx->draw16bitBeRGBBitmap(pDraw->x, pDraw->y, pDraw->pPixels, pDraw->iWidth, pDraw->iHeight);
  return 1;
}

void DisplayHelper::touch_init(void)
{
    // Initialize the touch library
    ts.begin();
    ts.setRotation(ROTATION_INVERTED);
}

bool DisplayHelper::touch_touched(void)
{
    bool retValue = false;

    // Reset old touches
    touch_reset();

    // Read current touches
    ts.read();

    // Set touch count
    touch_count = ts.touches;

    if (ts.isTouched)
    {
        // Save all current touches
        for (uint8_t i = 0; i < touch_count; ++i)
        {
            current_touches[i].x = map(ts.points[i].x, TOUCH_MAP_X1, TOUCH_MAP_X2, 0, 480 - 1);
            current_touches[i].y = map(ts.points[i].y, TOUCH_MAP_Y1, TOUCH_MAP_Y2, 0, 480 - 1);
        }

        // Reset the 'isTouched' flag
        ts.isTouched = false;

        // Set return value
        retValue = true;
    }

    return retValue;
}

int DisplayHelper::sd_init(void)
{
    int retCode = TC_SUCCESS;

    SPI.begin(SD_SCK, SD_MISO, SD_MOSI);
    if (!SD.begin(SD_CS))
    {
        Serial.println(F("ERROR: SD Mount Failed!"));
        {
            gfx->fillScreen(WHITE);
            gfx->setTextSize(3);
            gfx->setTextColor(RED);
            gfx->setCursor(50, 180);
            gfx->println(F("ERROR: SD Mount Failed!"));
            retCode = TC_SD_ERR;
        }
    }
    retCode = TC_SUCCESS;

    return retCode;
}

void DisplayHelper::drawImage(const char *file_name)
{
  jpegDraw(file_name, jpegDrawCallback, true /* useBigEndian */,
           0 /* x */, 0 /* y */, gfx->width() /* widthLimit */, gfx->height() /* heightLimit */);
}

void DisplayHelper::clear_screen()
{
    gfx->fillScreen(BLACK);
}

void DisplayHelper::gfx_init()
{
    //TODO: update rotations, (changes where 0,0 is)
    gfx->begin();
    gfx->setRotation(ROTATION_RIGHT);
}

DisplayHelper::DisplayHelper()
{
    // Initialize touch driver
    touch_init();

    // Initialize display
    gfx_init();

    // Initialize SD driver
    sd_init();

    // Variable initialization
    touch_count = 0;
}

void DisplayHelper::gfx_uninit()
{
    
}

tc_ret_code DisplayHelper::touch_decoder(UIButton button)
{
    tc_ret_code returnCode = TC_NO_UI_TOUCH;

    for (uint8_t i = 0; i < touch_count && returnCode != TC_UI_TOUCH; ++i)
    {
        if (current_touches[i].x > button.x && 
            current_touches[i].x < button.x + button.w &&
            current_touches[i].y > button.y &&
            current_touches[i].y < button.y + button.h)
        {
            // Set return code
            returnCode = TC_UI_TOUCH;

            // Reset touch variables
            touch_reset();
        }
    }

    return returnCode;
}

void DisplayHelper::drawUI()
{
    for (int i = 0; i < active_ui.size(); i++)
    {
        gfx->drawRect(active_ui[i].x, active_ui[i].y, active_ui[i].w, active_ui[i].h, PINK);
    }
}

void DisplayHelper::touch_reset()
{
    touch_count = 0;
    ts.isTouched = false;
    ts.touches = 0;

    // Clear ts var by reading
    ts.read();

    for(int i = 0; i < TOUCH_MAX; ++i)
    {
        current_touches[i] = TP_Point();
        ts.points[i] = TP_Point();
    }
}