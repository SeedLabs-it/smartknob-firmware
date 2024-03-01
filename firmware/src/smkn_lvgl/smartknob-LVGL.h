// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: Copyright 2024 mzyy94

#ifndef SMARTKNOB_LVGL_H
#define SMARTKNOB_LVGL_H

#ifndef LV_LVGL_H_INCLUDE_SIMPLE
#define LV_LVGL_H_INCLUDE_SIMPLE
#endif
#include "lvgl_conf.h"
#include "lvgl.h"
#include "lv_port_disp.h"
// #include "lv_port_indev.h"

inline void smartknob_lvgl_init()
{
    lv_init();
    lv_port_disp_init();
    //  lv_port_indev_init();
}

inline void smartknob_lvgl_next()
{

    uint32_t wait_ms = lv_timer_handler();
    lv_tick_inc(wait_ms);
}

#endif
