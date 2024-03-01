// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: Copyright 2024 mzyy94

#ifndef LV_PORT_DISP_H
#define LV_PORT_DISP_H

#include "lvgl.h"
#include <TFT_eSPI.h>

typedef struct
{
    TFT_eSPI *tft;
} lv_tft_espi_t;

#ifdef __cplusplus
extern "C"
{
#endif

    /* Initialize low level display driver */
    void lv_port_disp_init(void);

    /* Enable updating the screen (the flushing process) when disp_flush() is called by LVGL
     */
    void disp_enable_update(void);

    /* Disable updating the screen (the flushing process) when disp_flush() is called by LVGL
     */
    void disp_disable_update(void);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // LV_PORT_DISP_H
