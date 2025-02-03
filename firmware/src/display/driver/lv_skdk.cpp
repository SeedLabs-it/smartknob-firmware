/**
 * @file lv_tft_espi.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "display/driver/lv_skdk.h"
#include "display_task.h"

// #include "LGFX_SKDK.hpp"

// #include <LovyanGFX.hpp>
// #include <Arduino_GFX.h>

/*********************
 *      DEFINES
 *********************/

#define LV_TICK_PERIOD_MS 1

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_tick_task(void *arg);
static void flush_cb(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p);

/**********************
 *  STATIC VARIABLES
 **********************/

Arduino_DataBus *bus = new Arduino_ESP32QSPI(
    LCD_CS /* CS */, LCD_SCLK /* SCK */, LCD_SDIO0 /* SDIO0 */, LCD_SDIO1 /* SDIO1 */,
    LCD_SDIO2 /* SDIO2 */, LCD_SDIO3 /* SDIO3 */);
Arduino_GFX *gfx = new Arduino_CO5300(bus, LCD_RST /* RST */,
                                      0 /* rotation */, false /* IPS */, TFT_WIDTH, TFT_HEIGHT,
                                      6 /* col offset 1 */, 0 /* row offset 1 */, 0 /* col_offset2 */, 0 /* row_offset2 */);

static lv_disp_draw_buf_t draw_buf;
static lv_disp_drv_t disp_drv;

static const uint32_t DISP_BUF_SIZE = ((TFT_WIDTH * TFT_HEIGHT) * sizeof(uint16_t));

static lv_color_t *buf1 = NULL;
static lv_color_t *buf2 = NULL;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_skdk_create()
{
    // lcd.init();
    // lcd.initDMA();
    // lcd.setRotation(SK_DISPLAY_ROTATION);
    // lcd.setBrightness(255);
    // lcd.setSwapBytes(true);
    // lcd.setColorDepth(16);

    pinMode(LCD_EN, OUTPUT);
    digitalWrite(LCD_EN, HIGH);

    gfx->begin(80000000);
    gfx->fillScreen(BLACK);

    buf1 = (lv_color_t *)heap_caps_aligned_alloc(4, DISP_BUF_SIZE, MALLOC_CAP_SPIRAM);
    assert(buf1 != NULL);

    buf2 = (lv_color_t *)heap_caps_aligned_alloc(4, DISP_BUF_SIZE, MALLOC_CAP_SPIRAM);
    assert(buf2 != NULL);

    lv_disp_draw_buf_init(&draw_buf, buf1, buf2, TFT_WIDTH * TFT_HEIGHT);

    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    /*Change the following line to your display resolution*/
    disp_drv.hor_res = TFT_WIDTH;
    disp_drv.ver_res = TFT_HEIGHT;
    disp_drv.flush_cb = flush_cb;
    disp_drv.draw_buf = &draw_buf;
    disp_drv.full_refresh = 0;
    disp_drv.direct_mode = 0;

    lv_disp_drv_register(&disp_drv);

    // for (int i = 0; i <= 255; i++)
    // {
    //     gfx->Display_Brightness(i);
    //     delay(3);
    // }
}

lv_disp_drv_t *lv_skdk_get_disp_drv()
{
    return &disp_drv;
}

// LGFX *lv_skdk_get_lcd()
// {
//     return &lcd;
// }

/**********************
 *   STATIC FUNCTIONS
 **********************/
static bool buf1_in_use = true; // Flag to track which buffer is in use

static void flush_cb(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

#if (LV_COLOR_16_SWAP != 0)
    gfx->draw16bitBeRGBBitmap(area->x1, area->y1, (uint16_t *)&color_p->full, w, h);
#else
    gfx->draw16bitRGBBitmap(area->x1, area->y1, (uint16_t *)&color_p->full, w, h);
#endif

    lv_disp_flush_ready(disp);
}
