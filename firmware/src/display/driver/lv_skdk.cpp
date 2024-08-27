/**
 * @file lv_tft_espi.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "display/driver/lv_skdk.h"
#include "display_task.h"

#include "LGFX_SKDK.hpp"

#include <LovyanGFX.hpp>

/*********************
 *      DEFINES
 *********************/

#define LV_TICK_PERIOD_MS 1

/**********************
 *      TYPEDEFS
 **********************/
typedef struct
{
    TFT_eSPI *tft;
} lv_tft_espi_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_tick_task(void *arg);
static void flush_cb(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p);

/**********************
 *  STATIC VARIABLES
 **********************/
static LGFX lcd;                 // Instance of LGFX
static LGFX_Sprite sprite(&lcd); // Instance of LGFX_Sprite when using sprites

static lv_disp_draw_buf_t draw_buf;
static lv_disp_drv_t disp_drv;

#define TFT_HOR_RES 240
#define TFT_VER_RES 240

static const uint32_t DISP_BUF_SIZE = ((TFT_HOR_RES * TFT_HOR_RES) * sizeof(uint16_t));

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
    lcd.init();
    lcd.initDMA();
    lcd.setRotation(SK_DISPLAY_ROTATION);
    lcd.setBrightness(255);
    lcd.setSwapBytes(true);
    lcd.setColorDepth(16);

    buf1 = (lv_color_t *)heap_caps_aligned_alloc(4, DISP_BUF_SIZE, MALLOC_CAP_SPIRAM);
    assert(buf1 != NULL);

    buf2 = (lv_color_t *)heap_caps_aligned_alloc(4, DISP_BUF_SIZE, MALLOC_CAP_SPIRAM);
    assert(buf2 != NULL);

    lv_disp_draw_buf_init(&draw_buf, buf1, buf2, TFT_HOR_RES * TFT_VER_RES);

    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    /*Change the following line to your display resolution*/
    disp_drv.hor_res = TFT_HOR_RES;
    disp_drv.ver_res = TFT_VER_RES;
    disp_drv.flush_cb = flush_cb;
    disp_drv.draw_buf = &draw_buf;
    disp_drv.full_refresh = 0;
    disp_drv.direct_mode = 0;

    lv_disp_drv_register(&disp_drv);
}

lv_disp_drv_t *lv_skdk_get_disp_drv()
{
    return &disp_drv;
}

LGFX *lv_skdk_get_lcd()
{
    return &lcd;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
static bool buf1_in_use = true; // Flag to track which buffer is in use

static void flush_cb(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
    uint32_t w = lv_area_get_width(area);
    uint32_t h = lv_area_get_height(area);

    // Ensure only necessary operations within the flush callback
    if (lcd.getStartCount() == 0)
    {
        lcd.startWrite();
    }
    lcd.setAddrWindow(area->x1, area->y1, w, h);
    lcd.pushPixelsDMA((uint16_t *)color_p, w * h);
    lcd.endWrite();

    // Indicate you are ready with the flushing
    lv_disp_flush_ready(disp);
}
