// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: Copyright 2024 mzyy94

#include "lv_port_disp.h"
#include <stdbool.h>

#define MY_DISP_HOR_RES 240
#define MY_DISP_VER_RES 240

static lv_color_t buf_2_1[MY_DISP_HOR_RES * 10];
// static lv_color_t buf_2_2[MY_DISP_HOR_RES * 10];

static void disp_init(void);
static void disp_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map);
TFT_eSPI tft = TFT_eSPI();
lv_tft_espi_t dsc = {&tft};
lv_display_t *disp = NULL;

void lv_port_disp_init(void)
{
    disp_init();
    disp = lv_display_create(MY_DISP_HOR_RES, MY_DISP_VER_RES);
    lv_display_set_driver_data(disp, (void *)&dsc);
    lv_display_set_default(disp);
    lv_display_set_flush_cb(disp, disp_flush);

    lv_display_set_buffers(disp, buf_2_1, NULL, sizeof(buf_2_1), LV_DISPLAY_RENDER_MODE_PARTIAL);
}

static void disp_init(void)
{
    tft.begin(); /* TFT init */
    vTaskDelay(200 / portTICK_PERIOD_MS);
    tft.setRotation(0); /* Landscape orientation, flipped */
    tft.invertDisplay(1);
    vTaskDelay(200 / portTICK_PERIOD_MS);
    tft.fillScreen(TFT_BLACK);
}

volatile bool disp_flush_enabled = true;

void disp_enable_update(void)
{
    disp_flush_enabled = true;
}

void disp_disable_update(void)
{
    disp_flush_enabled = false;
}

static void disp_flush(lv_display_t *disp_drv, const lv_area_t *area, uint8_t *px_map)
{
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    dsc.tft->startWrite();
    dsc.tft->setAddrWindow(area->x1, area->y1, w, h);
    dsc.tft->pushColors((uint16_t *)px_map, w * h, true);
    dsc.tft->endWrite();

    lv_display_flush_ready(disp);
}
