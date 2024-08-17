#ifdef __has_include
    #if __has_include("lvgl.h")
        #ifndef LV_LVGL_H_INCLUDE_SIMPLE
            #define LV_LVGL_H_INCLUDE_SIMPLE
        #endif
    #endif
#endif

#if defined(LV_LVGL_H_INCLUDE_SIMPLE)
    #include "lvgl.h"
#else
    #include "lvgl/lvgl.h"
#endif


#ifndef LV_ATTRIBUTE_MEM_ALIGN
#define LV_ATTRIBUTE_MEM_ALIGN
#endif

#ifndef LV_ATTRIBUTE_IMG_X40_SETTINGS
#define LV_ATTRIBUTE_IMG_X40_SETTINGS
#endif

const LV_ATTRIBUTE_MEM_ALIGN LV_ATTRIBUTE_LARGE_CONST LV_ATTRIBUTE_IMG_X40_SETTINGS uint8_t x40_settings_map[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x8a, 0xaa, 0xaa, 0xa8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0xef, 0xff, 0xff, 0xfe, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0xdc, 0xcd, 0xff, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0xfe, 0x30, 0x03, 0xef, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xfd, 0x10, 0x01, 0xdf, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x29, 0x40, 0x00, 0x00, 0x2b, 0xfc, 0x00, 0x00, 0xcf, 0xb2, 0x00, 0x00, 0x04, 0x92, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x8f, 0xfb, 0x51, 0x18, 0xef, 0xfa, 0x00, 0x00, 0xaf, 0xfe, 0x81, 0x15, 0xbf, 0xf8, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x03, 0xef, 0xff, 0xfc, 0xcf, 0xff, 0xc5, 0x00, 0x00, 0x5b, 0xef, 0xfc, 0xcf, 0xff, 0xfe, 0x30, 0x00, 0x00, 
  0x00, 0x00, 0x0a, 0xff, 0x9c, 0xff, 0xff, 0xb4, 0x00, 0x00, 0x00, 0x00, 0x3a, 0xff, 0xff, 0xc9, 0xff, 0xa0, 0x00, 0x00, 
  0x00, 0x00, 0x4f, 0xfa, 0x01, 0x5a, 0xe8, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x8e, 0xa5, 0x10, 0xaf, 0xf4, 0x00, 0x00, 
  0x00, 0x01, 0xbf, 0xe3, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x3e, 0xfb, 0x10, 0x00, 
  0x00, 0x06, 0xff, 0xd3, 0x00, 0x00, 0x00, 0x00, 0x01, 0x46, 0x64, 0x10, 0x00, 0x00, 0x00, 0x00, 0x3d, 0xff, 0x60, 0x00, 
  0x00, 0x03, 0xcf, 0xfd, 0x50, 0x00, 0x00, 0x00, 0x3b, 0xff, 0xff, 0xb3, 0x00, 0x00, 0x00, 0x05, 0xdf, 0xfb, 0x30, 0x00, 
  0x00, 0x00, 0x19, 0xff, 0xf9, 0x10, 0x00, 0x03, 0xdf, 0xff, 0xff, 0xfd, 0x30, 0x00, 0x01, 0x9f, 0xff, 0x81, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x5d, 0xff, 0x70, 0x00, 0x0b, 0xff, 0xff, 0xff, 0xff, 0xb0, 0x00, 0x07, 0xff, 0xd5, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x05, 0xff, 0x50, 0x00, 0x4f, 0xff, 0xff, 0xff, 0xff, 0xf4, 0x00, 0x05, 0xff, 0x50, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x04, 0xff, 0x40, 0x00, 0x6f, 0xff, 0xff, 0xff, 0xff, 0xf6, 0x00, 0x04, 0xff, 0x40, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x04, 0xff, 0x40, 0x00, 0x6f, 0xff, 0xff, 0xff, 0xff, 0xf6, 0x00, 0x04, 0xff, 0x40, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x05, 0xff, 0x50, 0x00, 0x4f, 0xff, 0xff, 0xff, 0xff, 0xf4, 0x00, 0x05, 0xff, 0x50, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x5d, 0xff, 0x70, 0x00, 0x1b, 0xff, 0xff, 0xff, 0xff, 0xb0, 0x00, 0x07, 0xff, 0xd5, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x19, 0xff, 0xf9, 0x10, 0x00, 0x03, 0xdf, 0xff, 0xff, 0xfd, 0x30, 0x00, 0x01, 0x9f, 0xff, 0x81, 0x00, 0x00, 
  0x00, 0x03, 0xcf, 0xfd, 0x50, 0x00, 0x00, 0x00, 0x3b, 0xff, 0xff, 0xb3, 0x00, 0x00, 0x00, 0x05, 0xdf, 0xfc, 0x30, 0x00, 
  0x00, 0x06, 0xff, 0xd3, 0x00, 0x00, 0x00, 0x00, 0x01, 0x46, 0x64, 0x10, 0x00, 0x00, 0x00, 0x00, 0x3d, 0xff, 0x60, 0x00, 
  0x00, 0x01, 0xbf, 0xe3, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x3e, 0xfb, 0x10, 0x00, 
  0x00, 0x00, 0x4f, 0xfa, 0x01, 0x5a, 0xe9, 0x10, 0x00, 0x00, 0x00, 0x00, 0x01, 0x9e, 0xa5, 0x10, 0xaf, 0xf4, 0x00, 0x00, 
  0x00, 0x00, 0x0a, 0xff, 0x9c, 0xff, 0xff, 0xc5, 0x10, 0x00, 0x00, 0x01, 0x5c, 0xff, 0xff, 0xc9, 0xff, 0xa0, 0x00, 0x00, 
  0x00, 0x00, 0x03, 0xef, 0xff, 0xfc, 0xcf, 0xff, 0xc5, 0x00, 0x00, 0x5c, 0xff, 0xfc, 0xcf, 0xff, 0xfe, 0x30, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x8f, 0xfb, 0x51, 0x18, 0xef, 0xfa, 0x00, 0x00, 0xaf, 0xfe, 0x81, 0x15, 0xbf, 0xf8, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x29, 0x40, 0x00, 0x00, 0x2b, 0xfc, 0x00, 0x00, 0xcf, 0xb2, 0x00, 0x00, 0x04, 0x92, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xfd, 0x10, 0x01, 0xdf, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0xfe, 0x30, 0x03, 0xef, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0xdc, 0xcd, 0xff, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0xef, 0xff, 0xff, 0xfe, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x8a, 0xaa, 0xaa, 0xa8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
};

const lv_img_dsc_t x40_settings = {
  .header.cf = LV_IMG_CF_ALPHA_4BIT,
  .header.always_zero = 0,
  .header.reserved = 0,
  .header.w = 40,
  .header.h = 40,
  .data_size = 800,
  .data = x40_settings_map,
};
