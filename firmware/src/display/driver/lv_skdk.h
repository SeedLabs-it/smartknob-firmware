/**
 * @file lv_linux_fb_dev_h
 *
 */

// #include "LGFX_SKDK.hpp"

// #include <LovyanGFX.hpp>
#include "Arduino_GFX_Library.h"
// #include "Arduino_DriveBus_Library.h"
#ifdef __cplusplus

extern "C"
{
#endif

/*********************
 *      INCLUDES
 *********************/
#pragma once
#include "lvgl.h"

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
    // LGFX *lv_skdk_get_lcd();
    // Arduino_GFX *lv_skdk_get_lcd();

    /**********************
     *      MACROS
     **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif