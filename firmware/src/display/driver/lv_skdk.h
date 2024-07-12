/**
 * @file lv_linux_fb_dev_h
 *
 */

// #ifndef LV_TFT_ESPI_H
// #define LV_TFT_ESPI_H
#include "LGFX_SKDK.hpp"

#include <LovyanGFX.hpp>
#ifdef __cplusplus

extern "C"
{
#endif

/*********************
 *      INCLUDES
 *********************/
#pragma once
#include "lvgl.h"
#include "logging.h"

    // #if LV_USE_TFT_ESPI

    /*********************
     *      DEFINES
     *********************/

    /**********************
     *      TYPEDEFS
     **********************/

    /**********************
     * GLOBAL PROTOTYPES
     **********************/
    void lv_skdk_create();

    lv_disp_drv_t *lv_skdk_get_disp_drv();
    LGFX *lv_skdk_get_lcd();

    /**********************
     *      MACROS
     **********************/

    // #endif /* LV_USE_TFT_ESPI */

#ifdef __cplusplus
} /* extern "C" */
#endif

// #endif /* LV_TFT_ESPI_H */
