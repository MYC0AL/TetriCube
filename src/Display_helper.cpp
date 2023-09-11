#include "Display_helper.h"

TAMC_GT911 ts = TAMC_GT911(I2C_SDA_PIN, I2C_SCL_PIN, TOUCH_INT, TOUCH_RST, max(TOUCH_MAP_X1, TOUCH_MAP_X2), max(TOUCH_MAP_Y1, TOUCH_MAP_Y2));

int touch_last_x;
int touch_last_y;

const char* home_screen = "/home_screen.jpg";

// int ColorArray[] = {BLACK, BLUE, GREEN, WHITE, RED, ORANGE, NAVY, DARKGREEN, DARKCYAN, MAROON, PURPLE, OLIVE, LIGHTGREY, DARKCYAN, DARKGREY, MAGENTA, YELLOW, GREENYELLOW, PINK};
// // int ColorArray[]={BLACK,BLUE,GREEN,WHITE,RED,ORANGE};

// Bus and diaply setup
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

void touch_init(void)
{
    // Preemptive pin control
    pinMode(TOUCH_RST, OUTPUT);
    delay(100);
    digitalWrite(TOUCH_RST, LOW);
    delay(100);
    digitalWrite(TOUCH_RST, HIGH);
    delay(100);

    digitalWrite(TOUCH_RST, LOW);
    delay(1000);
    digitalWrite(TOUCH_RST, HIGH);
    delay(1000);

    // Initialize the touch wires
    Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
    ts.begin();
    ts.setRotation(TOUCH_ROTATION);
}

bool touch_touched(void)
{
    ts.read();
    if (ts.isTouched)
    {
        for (int i = 0; i < ts.touches; i++)
        {
            touch_last_x = map(ts.points[0].x, TOUCH_MAP_X1, TOUCH_MAP_X2, 0, 480 - 1);
            touch_last_y = map(ts.points[0].y, TOUCH_MAP_Y1, TOUCH_MAP_Y2, 0, 480 - 1);

            Serial.print("Touch ");
            Serial.print(i + 1);
            Serial.print(": ");
            ;
            Serial.print("  x: ");
            Serial.print(ts.points[i].x);
            Serial.print("  y: ");
            Serial.print(ts.points[i].y);
            Serial.print("  size: ");
            Serial.println(ts.points[i].size);
            Serial.println(' ');
            break;
        }
        ts.isTouched = false;
        return true;
    }
    else
    {
        return false;
    }
}

int sd_init(void)
{
    int retCode = TC_SUCCESS;
    gfx->begin();

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

static int jpegDrawCallback(JPEGDRAW *pDraw)
{
  // Serial.printf("Draw pos = %d,%d. size = %d x %d\n", pDraw->x, pDraw->y, pDraw->iWidth, pDraw->iHeight);
  gfx->draw16bitBeRGBBitmap(pDraw->x, pDraw->y, pDraw->pPixels, pDraw->iWidth, pDraw->iHeight);
  return 1;
}

void drawImage(const char *file_name)
{
  jpegDraw(file_name, jpegDrawCallback, true /* useBigEndian */,
           0 /* x */, 0 /* y */, gfx->width() /* widthLimit */, gfx->height() /* heightLimit */);
}