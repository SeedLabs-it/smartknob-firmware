/**
 * @file lv_linux_fb_dev_h
 *
 */

#ifndef LV_TFT_ESPI_H
#define LV_TFT_ESPI_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lvgl.h"
#include "logging.h"

#if LV_USE_TFT_ESPI

    /*********************
     *      DEFINES
     *********************/

    /**********************
     *      TYPEDEFS
     **********************/

    /**********************
     * GLOBAL PROTOTYPES
     **********************/
    lv_display_t *lv_skdk_create(uint32_t hor_res, uint32_t ver_res, void *buf1, void *buf2, uint32_t buf_size_bytes);

    /**********************
     *      MACROS
     **********************/

#endif /* LV_USE_TFT_ESPI */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LV_TFT_ESPI_H */
