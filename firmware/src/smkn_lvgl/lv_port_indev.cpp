// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: Copyright 2024 mzyy94
#if 0
#include "lv_port_indev.h"

static void encoder_init(void);
static void encoder_read(lv_indev_t *indev, lv_indev_data_t *data);

lv_indev_t *indev_encoder;

Encoder encoder;

void lv_port_indev_init(void)
{
    touchpad_init();

    indev_touchpad = lv_indev_create();
    lv_indev_set_type(indev_touchpad, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev_touchpad, touchpad_read);

    encoder_init();

    indev_encoder = lv_indev_create();
    lv_indev_set_type(indev_encoder, LV_INDEV_TYPE_ENCODER);
    lv_indev_set_read_cb(indev_encoder, encoder_read);
}

static void touchpad_init(void)
{
}

static void touchpad_read(lv_indev_t *indev_drv, lv_indev_data_t *data)
{
    static int32_t last_x = 0;
    static int32_t last_y = 0;

    if (touchpad_is_pressed())
    {
        touchpad_get_xy(&last_x, &last_y);
        data->state = LV_INDEV_STATE_PR;
    }
    else
    {
        data->state = LV_INDEV_STATE_REL;
    }

    data->point.x = last_x;
    data->point.y = last_y;
}

static bool touchpad_is_pressed(void)
{
    return M5.Touch.getCount() > 0;
}

static void touchpad_get_xy(int32_t *x, int32_t *y)
{
    auto detail = M5.Touch.getDetail();

    (*x) = detail.x;
    (*y) = detail.y;
}

static void encoder_init(void)
{
    encoder.setup();
}

static void encoder_read(lv_indev_t *indev_drv, lv_indev_data_t *data)
{
    data->enc_diff = encoder.getCount(true);
    data->state = M5.BtnA.isPressed() ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;
}
#endif