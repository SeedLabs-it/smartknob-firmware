
#if defined(LV_LVGL_H_INCLUDE_SIMPLE)
#include "lvgl.h"
#elif defined(LV_BUILD_TEST)
#include "../lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif


#ifndef LV_ATTRIBUTE_MEM_ALIGN
#define LV_ATTRIBUTE_MEM_ALIGN
#endif

#ifndef LV_ATTRIBUTE_X40_LIGHT_FILLED
#define LV_ATTRIBUTE_X40_LIGHT_FILLED
#endif

static const
LV_ATTRIBUTE_MEM_ALIGN LV_ATTRIBUTE_LARGE_CONST LV_ATTRIBUTE_X40_LIGHT_FILLED
uint8_t x40_light_filled_map[] = {

    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x11,0x11,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x9e,0xe9,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xaf,0xfa,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xaf,0xfa,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xaf,0xfa,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xaf,0xfa,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x15,0x8a,0xef,0xfe,0xa8,0x41,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x28,0xcf,0xff,0xff,0xff,0xff,0xfc,0x72,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x05,0xdf,0xff,0xff,0xff,0xff,0xff,0xff,0xfc,0x50,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x01,0x8f,0xff,0xff,0xd9,0x76,0x67,0x9c,0xff,0xff,0xf8,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x19,0xff,0xff,0xa4,0x10,0x00,0x00,0x01,0x5b,0xff,0xff,0x91,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x8f,0xff,0xe6,0x00,0x00,0x00,0x00,0x00,0x00,0x6e,0xff,0xf8,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x05,0xff,0xfd,0x40,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0xdf,0xfe,0x50,0x00,0x00,0x00,
    0x00,0x00,0x00,0x2c,0xff,0xe4,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x4e,0xff,0xd1,0x00,0x00,0x00,
    0x00,0x00,0x00,0x7f,0xff,0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x06,0xff,0xf7,0x00,0x00,0x00,
    0x00,0x00,0x01,0xdf,0xfb,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xbf,0xfd,0x10,0x00,0x00,
    0x00,0x00,0x04,0xff,0xf5,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x5f,0xff,0x40,0x00,0x00,
    0x00,0x00,0x08,0xff,0xc1,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1d,0xff,0x80,0x00,0x00,
    0x00,0x00,0x0b,0xff,0xa0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x09,0xff,0xb0,0x00,0x00,
    0x00,0x00,0x1c,0xff,0x70,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0xff,0xc1,0x00,0x00,
    0x00,0x00,0x1d,0xff,0x71,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x17,0xff,0xd1,0x00,0x00,
    0x00,0x00,0x1d,0xff,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xff,0xc1,0x00,0x00,
    0x00,0x00,0x08,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x80,0x00,0x00,
    0x00,0x00,0x01,0xae,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xfa,0x10,0x00,0x00,
    0x00,0x00,0x00,0x03,0x55,0x55,0x5c,0xff,0xff,0xff,0xff,0xff,0xff,0xc5,0x55,0x55,0x30,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x08,0xff,0xff,0xff,0xff,0xff,0xff,0x80,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x05,0xff,0xff,0xff,0xff,0xff,0xff,0x50,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xbf,0xff,0xff,0xff,0xff,0xfc,0x10,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x4e,0xff,0xff,0xff,0xff,0xe4,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x05,0xdf,0xff,0xff,0xfd,0x40,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x27,0xbd,0xdb,0x72,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

};

const lv_image_dsc_t x40_light_filled = {
  .header.magic = LV_IMAGE_HEADER_MAGIC,
  .header.cf = LV_COLOR_FORMAT_A4,
  .header.flags = 0,
  .header.w = 40,
  .header.h = 40,
  .header.stride = 20,
  .data_size = sizeof(x40_light_filled_map),
  .data = x40_light_filled_map,
};

