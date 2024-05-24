
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

#ifndef LV_ATTRIBUTE_LIGHTBULB_REGULAR
#define LV_ATTRIBUTE_LIGHTBULB_REGULAR
#endif

static const
LV_ATTRIBUTE_MEM_ALIGN LV_ATTRIBUTE_LARGE_CONST LV_ATTRIBUTE_LIGHTBULB_REGULAR
uint8_t lightbulb_regular_map[] = {

    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x14,0x7b,0xde,0xff,0xff,0xed,0xb7,0x41,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x5a,0xdf,0xff,0xff,0xff,0xff,0xff,0xff,0xfd,0xa5,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x7d,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xd7,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x7d,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xd7,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x4c,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xc4,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x07,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x70,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x01,0xaf,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xfa,0x10,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x1b,0xff,0xff,0xff,0xff,0xff,0xff,0xfe,0xba,0x97,0x79,0xab,0xef,0xff,0xff,0xff,0xff,0xff,0xff,0xb1,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x01,0xbf,0xff,0xff,0xff,0xff,0xff,0xd9,0x41,0x00,0x00,0x00,0x00,0x14,0x9d,0xff,0xff,0xff,0xff,0xff,0xfb,0x10,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x1b,0xff,0xff,0xff,0xff,0xff,0xc6,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x6c,0xff,0xff,0xff,0xff,0xff,0xb1,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x9f,0xff,0xff,0xff,0xff,0xe7,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x7e,0xff,0xff,0xff,0xff,0xf9,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x06,0xff,0xff,0xff,0xff,0xfc,0x40,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0xcf,0xff,0xff,0xff,0xff,0x60,0x00,0x00,0x00,
    0x00,0x00,0x00,0x2d,0xff,0xff,0xff,0xff,0xb1,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1b,0xff,0xff,0xff,0xff,0xd2,0x00,0x00,0x00,
    0x00,0x00,0x00,0xaf,0xff,0xff,0xff,0xfa,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xaf,0xff,0xff,0xff,0xfa,0x00,0x00,0x00,
    0x00,0x00,0x04,0xff,0xff,0xff,0xff,0xb1,0x00,0x00,0x00,0x00,0x00,0x00,0x11,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x1b,0xff,0xff,0xff,0xff,0x40,0x00,0x00,
    0x00,0x00,0x0b,0xff,0xff,0xff,0xfd,0x20,0x00,0x00,0x00,0x00,0x03,0x7b,0xdd,0xd7,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0xdf,0xff,0xff,0xff,0xb0,0x00,0x00,
    0x00,0x00,0x4f,0xff,0xff,0xff,0xf5,0x00,0x00,0x00,0x00,0x04,0xbf,0xff,0xff,0xff,0x40,0x00,0x00,0x00,0x00,0x00,0x00,0x5f,0xff,0xff,0xff,0xf4,0x00,0x00,
    0x00,0x00,0x9f,0xff,0xff,0xff,0xa0,0x00,0x00,0x00,0x00,0x7e,0xff,0xff,0xff,0xff,0x70,0x00,0x00,0x00,0x00,0x00,0x00,0x0a,0xff,0xff,0xff,0xf9,0x00,0x00,
    0x00,0x01,0xdf,0xff,0xff,0xfe,0x30,0x00,0x00,0x00,0x09,0xff,0xff,0xff,0xff,0xff,0x40,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xef,0xff,0xff,0xfd,0x10,0x00,
    0x00,0x05,0xff,0xff,0xff,0xfa,0x00,0x00,0x00,0x00,0x9f,0xff,0xff,0xff,0xfe,0xd7,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xaf,0xff,0xff,0xff,0x50,0x00,
    0x00,0x09,0xff,0xff,0xff,0xf4,0x00,0x00,0x00,0x06,0xff,0xff,0xff,0xc7,0x31,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x4f,0xff,0xff,0xff,0x90,0x00,
    0x00,0x0c,0xff,0xff,0xff,0xd1,0x00,0x00,0x00,0x1d,0xff,0xff,0xf7,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1d,0xff,0xff,0xff,0xc0,0x00,
    0x00,0x2e,0xff,0xff,0xff,0x90,0x00,0x00,0x00,0x7f,0xff,0xff,0x70,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x09,0xff,0xff,0xff,0xe2,0x00,
    0x00,0x4f,0xff,0xff,0xff,0x60,0x00,0x00,0x00,0xcf,0xff,0xfa,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x06,0xff,0xff,0xff,0xf4,0x00,
    0x00,0x5f,0xff,0xff,0xff,0x40,0x00,0x00,0x03,0xff,0xff,0xe3,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0xff,0xff,0xff,0xf5,0x00,
    0x00,0x7f,0xff,0xff,0xfe,0x20,0x00,0x00,0x05,0xff,0xff,0xb0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0xef,0xff,0xff,0xf7,0x00,
    0x00,0x7f,0xff,0xff,0xfe,0x10,0x00,0x00,0x07,0xff,0xff,0x90,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xef,0xff,0xff,0xf7,0x00,
    0x00,0x7f,0xff,0xff,0xfe,0x10,0x00,0x00,0x07,0xff,0xff,0x70,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xef,0xff,0xff,0xf7,0x00,
    0x00,0x7f,0xff,0xff,0xfe,0x10,0x00,0x00,0x04,0xff,0xff,0x40,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xef,0xff,0xff,0xf7,0x00,
    0x00,0x7f,0xff,0xff,0xfe,0x20,0x00,0x00,0x00,0x7d,0xd7,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0xef,0xff,0xff,0xf7,0x00,
    0x00,0x5f,0xff,0xff,0xff,0x40,0x00,0x00,0x00,0x01,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0xff,0xff,0xff,0xf5,0x00,
    0x00,0x4f,0xff,0xff,0xff,0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x06,0xff,0xff,0xff,0xf4,0x00,
    0x00,0x2e,0xff,0xff,0xff,0x90,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x09,0xff,0xff,0xff,0xe2,0x00,
    0x00,0x0c,0xff,0xff,0xff,0xd1,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1d,0xff,0xff,0xff,0xc0,0x00,
    0x00,0x09,0xff,0xff,0xff,0xf4,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x4f,0xff,0xff,0xff,0x90,0x00,
    0x00,0x05,0xff,0xff,0xff,0xfa,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xaf,0xff,0xff,0xff,0x50,0x00,
    0x00,0x01,0xdf,0xff,0xff,0xfe,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xef,0xff,0xff,0xfd,0x10,0x00,
    0x00,0x00,0x9f,0xff,0xff,0xff,0xa0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0a,0xff,0xff,0xff,0xf9,0x00,0x00,
    0x00,0x00,0x4f,0xff,0xff,0xff,0xf5,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x5f,0xff,0xff,0xff,0xf4,0x00,0x00,
    0x00,0x00,0x0b,0xff,0xff,0xff,0xfd,0x20,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0xdf,0xff,0xff,0xff,0xb0,0x00,0x00,
    0x00,0x00,0x04,0xff,0xff,0xff,0xff,0xa0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0a,0xff,0xff,0xff,0xff,0x40,0x00,0x00,
    0x00,0x00,0x00,0xaf,0xff,0xff,0xff,0xf7,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7f,0xff,0xff,0xff,0xfa,0x00,0x00,0x00,
    0x00,0x00,0x00,0x2d,0xff,0xff,0xff,0xfe,0x40,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0xef,0xff,0xff,0xff,0xd2,0x00,0x00,0x00,
    0x00,0x00,0x00,0x06,0xff,0xff,0xff,0xff,0xc1,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1c,0xff,0xff,0xff,0xff,0x60,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x9f,0xff,0xff,0xff,0xf9,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x9f,0xff,0xff,0xff,0xf9,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x1c,0xff,0xff,0xff,0xff,0x50,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x05,0xff,0xff,0xff,0xff,0xc1,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x04,0xef,0xff,0xff,0xff,0xd2,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1d,0xff,0xff,0xff,0xfe,0x40,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x7f,0xff,0xff,0xff,0xf9,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x9f,0xff,0xff,0xff,0xf7,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x0b,0xff,0xff,0xff,0xff,0x50,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x05,0xff,0xff,0xff,0xff,0xb0,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x02,0xdf,0xff,0xff,0xff,0xc1,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1c,0xff,0xff,0xff,0xfd,0x20,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x6f,0xff,0xff,0xff,0xf7,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7f,0xff,0xff,0xff,0xf6,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x0a,0xff,0xff,0xff,0xfe,0x20,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0xdf,0xff,0xff,0xff,0xa0,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x02,0xdf,0xff,0xff,0xff,0x70,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0xff,0xff,0xff,0xfd,0x20,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7f,0xff,0xff,0xff,0xd1,0x00,0x00,0x00,0x00,0x00,0x00,0x1d,0xff,0xff,0xff,0xf7,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1c,0xff,0xff,0xff,0xf5,0x00,0x00,0x00,0x00,0x00,0x00,0x5f,0xff,0xff,0xff,0xc1,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x06,0xff,0xff,0xff,0xfa,0x00,0x00,0x00,0x00,0x00,0x00,0x9f,0xff,0xff,0xff,0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xdf,0xff,0xff,0xfd,0x10,0x00,0x00,0x00,0x00,0x01,0xdf,0xff,0xff,0xfd,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x9f,0xff,0xff,0xff,0x40,0x00,0x00,0x00,0x00,0x03,0xff,0xff,0xff,0xf9,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x5f,0xff,0xff,0xff,0x60,0x00,0x00,0x00,0x00,0x06,0xff,0xff,0xff,0xf5,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x2c,0xdd,0xdd,0xde,0x70,0x00,0x00,0x00,0x00,0x07,0xee,0xee,0xee,0xc2,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x11,0x11,0x11,0x10,0x00,0x00,0x00,0x00,0x01,0x11,0x11,0x11,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0x70,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x70,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x70,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x70,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x05,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x50,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xcf,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xfc,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7f,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xf7,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1d,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xd1,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x06,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x9f,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xf9,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x09,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x90,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7e,0xff,0xff,0xff,0xff,0xff,0xff,0xe7,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0xbe,0xff,0xff,0xff,0xff,0xeb,0x40,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x9c,0xff,0xff,0xc9,0x40,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

};

const lv_image_dsc_t lightbulb_regular = {
  .header.magic = LV_IMAGE_HEADER_MAGIC,
  .header.cf = LV_COLOR_FORMAT_A4,
  .header.flags = 0,
  .header.w = 60,
  .header.h = 80,
  .header.stride = 30,
  .data_size = sizeof(lightbulb_regular_map),
  .data = lightbulb_regular_map,
};

