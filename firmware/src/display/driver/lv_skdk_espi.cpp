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

/*********************
 *      DEFINES
 *********************/

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
static void flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map);

/**********************
 *  STATIC VARIABLES
 **********************/
#define TFT_HOR_RES 240
#define TFT_VER_RES 240

static const uint32_t DISP_BUF_SIZE = ((TFT_HOR_RES * TFT_HOR_RES) * sizeof(lv_color_t));

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
    buf1 = (lv_color_t *)heap_caps_aligned_alloc(16, DISP_BUF_SIZE, MALLOC_CAP_SPIRAM);
    assert(buf1 != NULL);

    buf2 = (lv_color_t *)heap_caps_aligned_alloc(16, DISP_BUF_SIZE, MALLOC_CAP_SPIRAM);
    assert(buf2 != NULL);

    lv_tft_espi_t *dsc = (lv_tft_espi_t *)lv_malloc_zeroed(sizeof(lv_tft_espi_t));
    LV_ASSERT_MALLOC(dsc);
    if (dsc == NULL)
        return NULL;

    lv_display_t *disp = lv_display_create(TFT_HOR_RES, TFT_VER_RES);
    if (disp == NULL)
    {
        lv_free(dsc);
        return NULL;
    }

    dsc->tft = new TFT_eSPI(TFT_HOR_RES, TFT_VER_RES);
    dsc->tft->begin();                          /* TFT init */
    dsc->tft->initDMA(true);                    /* Enable DMA */
    dsc->tft->setRotation(SK_DISPLAY_ROTATION); /* Landscape orientation, flipped */
    dsc->tft->invertDisplay(1);                 /* Invert display */
    dsc->tft->setSwapBytes(true);               /* TFT byte order is swapped */

    lv_display_set_driver_data(disp, (void *)dsc);
    lv_display_set_flush_cb(disp, flush_cb);
    lv_display_set_buffers(disp, (void *)buf1, (void *)buf2, DISP_BUF_SIZE, LV_DISPLAY_RENDER_MODE_FULL);
    return disp;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{
    lv_tft_espi_t *dsc = (lv_tft_espi_t *)lv_display_get_driver_data(disp);

    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);
    uint32_t size = w * h;

    // lv_draw_buf_t *off_screen = lv_display_get_buf_active(disp);
    // lv_draw_buf_t *on_screen = (lv_color_t *)off_screen == buf1 ? (lv_draw_buf_t *)buf1 : (lv_draw_buf_t *)buf2;

    dsc->tft->startWrite();
    dsc->tft->setAddrWindow(area->x1, area->y1, w, h);
    // dsc->tft->pushPixelsDMA((uint16_t *)px_map, size);
    dsc->tft->pushImageDMA(area->x1, area->y1, w, h, (uint16_t *)px_map);
    dsc->tft->endWrite();

    lv_display_flush_ready(disp);
}

#endif /*LV_USE_TFT_ESPI*/
