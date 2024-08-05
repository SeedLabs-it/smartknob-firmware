/**
 * @file lv_linux_fb_dev_h
 *
 */

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

#ifdef __cplusplus
} /* extern "C" */
#endif