/**
 * @file lv_tft_espi.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#pragma once
#include "display/driver/lv_skdk.h"
#include "display_task.h"
#if LV_USE_TFT_ESPI

#include <TFT_eSPI.h>

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
static void flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *color_p);

/**********************
 *  STATIC VARIABLES
 **********************/
static LGFX lcd;                 // Instance of LGFX
static LGFX_Sprite sprite(&lcd); // Instance of LGFX_Sprite when using sprites

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

lv_display_t *lv_skdk_create()
{
    lcd.init();
    lcd.initDMA();
    lcd.setRotation(SK_DISPLAY_ROTATION);
    lcd.setBrightness(255);
    lcd.setSwapBytes(true);
    lcd.setColorDepth(16);

    buf1 = (lv_color_t *)heap_caps_aligned_alloc(4, DISP_BUF_SIZE, MALLOC_CAP_SPIRAM);
    // buf1 = (lv_color_t *)heap_caps_malloc(DISP_BUF_SIZE, MALLOC_CAP_SPIRAM);
    assert(buf1 != NULL);

    buf2 = (lv_color_t *)heap_caps_aligned_alloc(4, DISP_BUF_SIZE, MALLOC_CAP_SPIRAM);
    // buf2 = (lv_color_t *)heap_caps_malloc(DISP_BUF_SIZE, MALLOC_CAP_SPIRAM);
    assert(buf2 != NULL);

    // lv_tft_espi_t *dsc = (lv_tft_espi_t *)lv_malloc_zeroed(sizeof(lv_tft_espi_t));
    // LV_ASSERT_MALLOC(dsc);
    // if (dsc == NULL)
    //     return NULL;

    lv_display_t *disp = lv_display_create(TFT_HOR_RES, TFT_VER_RES);
    if (disp == NULL)
    {
        // lv_free(dsc);
        return NULL;
    }

    // dsc->tft = new TFT_eSPI(TFT_HOR_RES, TFT_VER_RES);
    // dsc->tft->begin();                          /* TFT init */
    // dsc->tft->initDMA(true);                    /* Enable DMA */
    // dsc->tft->setRotation(SK_DISPLAY_ROTATION); /* Landscape orientation, flipped */
    // dsc->tft->invertDisplay(1);                 /* Invert display */
    // dsc->tft->setSwapBytes(true);               /* TFT byte order is swapped */

    // lv_display_set_driver_data(disp, (void *)dsc);

    // lv_display_set_driver_data(disp, (void *)dsc);

    // static lv_disp_drv_t disp_drv;
    // lv_disp_drv_init(&disp_drv);

    const esp_timer_create_args_t periodic_timer_args = {
        .callback = &lv_tick_task,
        .name = "periodic_gui_timer"};
    esp_timer_handle_t periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, LV_TICK_PERIOD_MS * 1000));

    lv_display_set_flush_cb(disp, flush_cb);
    lv_display_set_driver_data(disp, (void *)buf1);
    lv_display_set_buffers(disp, (void *)buf1, (void *)buf2, DISP_BUF_SIZE, LV_DISPLAY_RENDER_MODE_FULL);

    return disp;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
static bool buf1_in_use = true; // Flag to track which buffer is in use

static void lv_tick_task(void *arg)
{
    (void)arg;
    lv_tick_inc(LV_TICK_PERIOD_MS);
}

static void flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *color_p)
{
    uint32_t w = lv_area_get_width(area);
    uint32_t h = lv_area_get_height(area);

    // Ensure only necessary operations within the flush callback
    if (lcd.getStartCount() == 0)
    {
        lcd.startWrite();
    }
    lcd.setAddrWindow(area->x1, area->y1, w, h);
    // lcd.pushImageDMA(area->x1, area->y1, w, h, (uint16_t *)color_p);
    lcd.pushPixelsDMA((uint16_t *)color_p, w * h);
    lcd.endWrite();

    // Indicate you are ready with the flushing
    lv_display_flush_ready(disp);
}

#endif /*LV_USE_TFT_ESPI*/

// uint32_t w = lv_area_get_width(area);
// uint32_t h = lv_area_get_height(area);

// LOGE("Flushing area, w: %d, h: %d", w, h);

// // lv_draw_sw_rgb565_swap(color_p, w * h);
// // lv_draw_sw_rgb565_swap((uint16_t *)color_p, w * h);

// lv_draw_buf_t *off_screen = lv_display_get_buf_active(disp);
// lv_draw_buf_t *on_screen = (lv_color_t *)off_screen == buf1 ? (lv_draw_buf_t *)buf1 : (lv_draw_buf_t *)buf2;

// if (lcd.getStartCount() == 0)
// { // Processing if not yet started
//     lcd.startWrite();
// }
// lcd.setAddrWindow(area->x1, area->y1, area->x2 - area->x1 + 1, area->y2 - area->y1 + 1);
// lcd.pushImageDMA(area->x1, area->y1, area->x2 - area->x1 + 1, area->y2 - area->y1 + 1, (uint16_t *)color_p);
// lcd.endWrite();
// lv_display_flush_ready(disp);

// lcd.drawPixel(120, 120, 0xFFFF); // White dot at coordinates 0, 0
// lv_tft_espi_t *dsc = (lv_tft_espi_t *)lv_display_get_driver_data(disp);

// uint32_t w = (area->x2 - area->x1 + 1);
// uint32_t h = (area->y2 - area->y1 + 1);
// uint32_t size = w * h;

// dsc->tft->startWrite();
// dsc->tft->setAddrWindow(area->x1, area->y1, w, h);
// // dsc->tft->pushPixelsDMA((uint16_t *)px_map, size);
// dsc->tft->pushImageDMA(area->x1, area->y1, w, h, (uint16_t *)px_map);
// dsc->tft->endWrite();

// delay(10);