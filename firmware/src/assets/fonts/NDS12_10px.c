/*******************************************************************************
 * Size: 10 px
 * Bpp: 4
 * Opts: --bpp 4 --size 10 --no-compress --font NDS12.woff --range 32-127 --format lvgl -o NDS12_10px.c
 ******************************************************************************/

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

#ifndef NDS12_10PX
#define NDS12_10PX 1
#endif

#if NDS12_10PX

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+0020 " " */

    /* U+0021 "!" */
    0xff, 0xff, 0xff, 0xf, 0xf0,

    /* U+0022 "\"" */
    0xf0, 0xff, 0xf,

    /* U+0023 "#" */
    0x0, 0xf, 0xf, 0x0, 0x0, 0xf0, 0xf0, 0xf,
    0xff, 0xff, 0xf0, 0xf, 0xf, 0x0, 0x0, 0xf0,
    0xf0, 0x0, 0xf, 0xf, 0x0, 0xff, 0xff, 0xff,
    0x0, 0xf0, 0xf0, 0x0, 0xf, 0xf, 0x0, 0x0,

    /* U+0024 "$" */
    0x0, 0xf0, 0x0, 0xff, 0xf0, 0xf0, 0xf0, 0xff,
    0xf, 0x0, 0xf, 0xff, 0x0, 0xf, 0xf, 0xf0,
    0xf0, 0xf0, 0xff, 0xf0, 0x0, 0xf0, 0x0,

    /* U+0025 "%" */
    0xf, 0x0, 0xf, 0xf, 0xf, 0x0, 0xf0, 0xf0,
    0xf0, 0xf0, 0x0, 0xf0, 0xf, 0x0, 0x0, 0xf,
    0x0, 0x0, 0xf, 0x0, 0xf0, 0x0, 0xf0, 0xf0,
    0xf0, 0xf0, 0xf, 0xf, 0xf, 0x0, 0xf, 0x0,

    /* U+0026 "&" */
    0x0, 0xf0, 0x0, 0xf, 0xf, 0x0, 0xf, 0xf,
    0x0, 0xf, 0xf, 0x0, 0x0, 0xf0, 0x0, 0xf,
    0xf, 0xf, 0xf0, 0x0, 0xf0, 0xf0, 0x0, 0xf0,
    0xf, 0xff, 0xf,

    /* U+0027 "'" */
    0xff,

    /* U+0028 "(" */
    0x0, 0xf0, 0xf0, 0xf, 0xf, 0x0, 0xf0, 0xf,
    0x0, 0xf, 0x0, 0xf0, 0x0, 0xf0,

    /* U+0029 ")" */
    0xf0, 0x0, 0xf0, 0xf, 0x0, 0xf, 0x0, 0xf0,
    0xf, 0xf, 0x0, 0xf0, 0xf0, 0x0,

    /* U+002A "*" */
    0x0, 0xf, 0x0, 0xf, 0x0, 0xf0, 0xf, 0xf,
    0xf, 0xf, 0x0, 0xf, 0xff, 0x0, 0xf, 0xf,
    0xf, 0xf, 0x0, 0xf0, 0xf, 0x0, 0xf, 0x0,
    0x0,

    /* U+002B "+" */
    0x0, 0xf0, 0x0, 0xf, 0x0, 0xff, 0xff, 0xf0,
    0xf, 0x0, 0x0, 0xf0, 0x0,

    /* U+002C "," */
    0xf, 0xf0,

    /* U+002D "-" */
    0xff, 0xff, 0xf0,

    /* U+002E "." */
    0xf0,

    /* U+002F "/" */
    0x0, 0xf0, 0xf, 0x0, 0xf0, 0xf0, 0xf, 0x0,
    0xf0, 0xf0, 0xf, 0x0, 0xf0, 0x0,

    /* U+0030 "0" */
    0xf, 0xff, 0xf, 0x0, 0xf, 0xf0, 0xf, 0xff,
    0x0, 0xff, 0xf0, 0xf0, 0xff, 0xf0, 0xf, 0xff,
    0x0, 0xff, 0x0, 0xf, 0xf, 0xff, 0x0,

    /* U+0031 "1" */
    0xff, 0xf, 0xf, 0xf, 0xf, 0xf, 0xf, 0xf,
    0xf,

    /* U+0032 "2" */
    0xff, 0xff, 0x0, 0x0, 0xf, 0x0, 0x0, 0xf0,
    0x0, 0xf, 0xf, 0xff, 0xf, 0x0, 0x0, 0xf0,
    0x0, 0xf, 0x0, 0x0, 0xff, 0xff, 0xf0,

    /* U+0033 "3" */
    0xff, 0xff, 0x0, 0x0, 0xf, 0x0, 0x0, 0xf0,
    0x0, 0xf, 0xf, 0xff, 0x0, 0x0, 0xf, 0x0,
    0x0, 0xf0, 0x0, 0xf, 0xff, 0xff, 0x0,

    /* U+0034 "4" */
    0xf0, 0x0, 0xf0, 0xf0, 0x0, 0xf0, 0xf0, 0x0,
    0xf0, 0xf0, 0x0, 0xf0, 0xf0, 0x0, 0xf0, 0xf,
    0xff, 0xff, 0x0, 0x0, 0xf0, 0x0, 0x0, 0xf0,
    0x0, 0x0, 0xf0,

    /* U+0035 "5" */
    0xff, 0xff, 0xff, 0x0, 0x0, 0xf0, 0x0, 0xf,
    0x0, 0x0, 0xff, 0xff, 0x0, 0x0, 0xf, 0x0,
    0x0, 0xf0, 0x0, 0xf, 0xff, 0xff, 0x0,

    /* U+0036 "6" */
    0xf, 0xff, 0xf, 0x0, 0x0, 0xf0, 0x0, 0xf,
    0x0, 0x0, 0xff, 0xff, 0xf, 0x0, 0xf, 0xf0,
    0x0, 0xff, 0x0, 0xf, 0xf, 0xff, 0x0,

    /* U+0037 "7" */
    0xff, 0xff, 0xf0, 0x0, 0xf, 0x0, 0x0, 0xf0,
    0x0, 0xf0, 0x0, 0xf0, 0x0, 0xf, 0x0, 0x0,
    0xf0, 0x0, 0xf, 0x0, 0x0, 0xf0, 0x0,

    /* U+0038 "8" */
    0xf, 0xff, 0xf, 0x0, 0xf, 0xf0, 0x0, 0xff,
    0x0, 0xf, 0xf, 0xff, 0xf, 0x0, 0xf, 0xf0,
    0x0, 0xff, 0x0, 0xf, 0xf, 0xff, 0x0,

    /* U+0039 "9" */
    0xf, 0xff, 0xf, 0x0, 0xf, 0xf0, 0x0, 0xff,
    0x0, 0xf, 0xf, 0xff, 0xf0, 0x0, 0xf, 0x0,
    0x0, 0xf0, 0x0, 0xf, 0xf, 0xff, 0x0,

    /* U+003A ":" */
    0xf0, 0x0, 0xf0,

    /* U+003B ";" */
    0xf, 0x0, 0x0, 0x0, 0xf, 0xf0,

    /* U+003C "<" */
    0x0, 0xf0, 0xf0, 0xf0, 0x0, 0xf0, 0x0, 0xf0,

    /* U+003D "=" */
    0xff, 0xff, 0xf0, 0x0, 0x0, 0xff, 0xff, 0xf0,

    /* U+003E ">" */
    0xf0, 0x0, 0xf0, 0x0, 0xf0, 0xf0, 0xf0, 0x0,

    /* U+003F "?" */
    0xff, 0xf0, 0x0, 0xf, 0x0, 0xf, 0x0, 0xf0,
    0xf, 0x0, 0xf, 0x0, 0x0, 0x0, 0xf, 0x0,
    0xf, 0x0,

    /* U+0040 "@" */
    0x0, 0xff, 0xf0, 0x0, 0xf0, 0x0, 0xf0, 0xf0,
    0xf, 0xf0, 0xff, 0xf, 0xf, 0xf, 0xf0, 0xf0,
    0xf0, 0xff, 0xf, 0xf, 0xf, 0xf0, 0xf, 0xff,
    0x0, 0xf0, 0x0, 0x0, 0x0, 0xff, 0xff, 0x0,

    /* U+0041 "A" */
    0x0, 0xf0, 0x0, 0xf, 0x0, 0xf, 0xf, 0x0,
    0xf0, 0xf0, 0xf, 0xf, 0xf, 0xff, 0xff, 0xf0,
    0x0, 0xff, 0x0, 0xf, 0xf0, 0x0, 0xf0,

    /* U+0042 "B" */
    0xff, 0xff, 0xf, 0x0, 0xf, 0xf0, 0x0, 0xff,
    0x0, 0xf, 0xff, 0xff, 0xf, 0x0, 0xf, 0xf0,
    0x0, 0xff, 0x0, 0xf, 0xff, 0xff, 0x0,

    /* U+0043 "C" */
    0xf, 0xff, 0xff, 0x0, 0x0, 0xf0, 0x0, 0xf,
    0x0, 0x0, 0xf0, 0x0, 0xf, 0x0, 0x0, 0xf0,
    0x0, 0xf, 0x0, 0x0, 0xf, 0xff, 0xf0,

    /* U+0044 "D" */
    0xff, 0xff, 0xf, 0x0, 0xf, 0xf0, 0x0, 0xff,
    0x0, 0xf, 0xf0, 0x0, 0xff, 0x0, 0xf, 0xf0,
    0x0, 0xff, 0x0, 0xf, 0xff, 0xff, 0x0,

    /* U+0045 "E" */
    0xff, 0xff, 0xf0, 0x0, 0xf0, 0x0, 0xf0, 0x0,
    0xff, 0xff, 0xf0, 0x0, 0xf0, 0x0, 0xf0, 0x0,
    0xff, 0xff,

    /* U+0046 "F" */
    0xff, 0xff, 0xf0, 0x0, 0xf0, 0x0, 0xf0, 0x0,
    0xff, 0xff, 0xf0, 0x0, 0xf0, 0x0, 0xf0, 0x0,
    0xf0, 0x0,

    /* U+0047 "G" */
    0xf, 0xff, 0xff, 0x0, 0x0, 0xf0, 0x0, 0xf,
    0x0, 0x0, 0xf0, 0xff, 0xff, 0x0, 0xf, 0xf0,
    0x0, 0xff, 0x0, 0xf, 0xf, 0xff, 0x0,

    /* U+0048 "H" */
    0xf0, 0x0, 0xff, 0x0, 0xf, 0xf0, 0x0, 0xff,
    0x0, 0xf, 0xff, 0xff, 0xff, 0x0, 0xf, 0xf0,
    0x0, 0xff, 0x0, 0xf, 0xf0, 0x0, 0xf0,

    /* U+0049 "I" */
    0xff, 0xff, 0xff, 0xff, 0xf0,

    /* U+004A "J" */
    0x0, 0xf, 0x0, 0xf, 0x0, 0xf, 0x0, 0xf,
    0x0, 0xf, 0x0, 0xf, 0x0, 0xf, 0x0, 0xf,
    0xff, 0xf0,

    /* U+004B "K" */
    0xf0, 0x0, 0xff, 0x0, 0xf0, 0xf0, 0xf, 0xf,
    0xf, 0x0, 0xff, 0x0, 0xf, 0xf, 0x0, 0xf0,
    0xf, 0xf, 0x0, 0xf0, 0xf0, 0x0, 0xf0,

    /* U+004C "L" */
    0xf0, 0x0, 0xf0, 0x0, 0xf0, 0x0, 0xf0, 0x0,
    0xf0, 0x0, 0xf0, 0x0, 0xf0, 0x0, 0xf0, 0x0,
    0xff, 0xff,

    /* U+004D "M" */
    0xf0, 0x0, 0xff, 0x0, 0xf, 0xff, 0xf, 0xff,
    0xf0, 0xff, 0xf0, 0xf0, 0xff, 0xf, 0xf, 0xf0,
    0x0, 0xff, 0x0, 0xf, 0xf0, 0x0, 0xf0,

    /* U+004E "N" */
    0xf0, 0x0, 0xff, 0xf0, 0xf, 0xff, 0x0, 0xff,
    0xf, 0xf, 0xf0, 0xf0, 0xff, 0x0, 0xff, 0xf0,
    0xf, 0xff, 0x0, 0xf, 0xf0, 0x0, 0xf0,

    /* U+004F "O" */
    0xf, 0xff, 0xf, 0x0, 0xf, 0xf0, 0x0, 0xff,
    0x0, 0xf, 0xf0, 0x0, 0xff, 0x0, 0xf, 0xf0,
    0x0, 0xff, 0x0, 0xf, 0xf, 0xff, 0x0,

    /* U+0050 "P" */
    0xff, 0xff, 0xf, 0x0, 0xf, 0xf0, 0x0, 0xff,
    0x0, 0xf, 0xff, 0xff, 0xf, 0x0, 0x0, 0xf0,
    0x0, 0xf, 0x0, 0x0, 0xf0, 0x0, 0x0,

    /* U+0051 "Q" */
    0xf, 0xff, 0xf, 0x0, 0xf, 0xf0, 0x0, 0xff,
    0x0, 0xf, 0xf0, 0x0, 0xff, 0x0, 0xf, 0xf0,
    0x0, 0xff, 0xf, 0xf, 0xf, 0xff, 0x0, 0x0,
    0xff,

    /* U+0052 "R" */
    0xff, 0xff, 0xf, 0x0, 0xf, 0xf0, 0x0, 0xff,
    0x0, 0xf, 0xff, 0xff, 0xf, 0x0, 0xf0, 0xf0,
    0xf, 0xf, 0x0, 0xf, 0xf0, 0x0, 0xf0,

    /* U+0053 "S" */
    0xf, 0xff, 0xf0, 0x0, 0xf0, 0x0, 0xf0, 0x0,
    0xf, 0xf0, 0x0, 0xf, 0x0, 0xf, 0x0, 0xf,
    0xff, 0xf0,

    /* U+0054 "T" */
    0xff, 0xff, 0xf0, 0xf, 0x0, 0x0, 0xf0, 0x0,
    0xf, 0x0, 0x0, 0xf0, 0x0, 0xf, 0x0, 0x0,
    0xf0, 0x0, 0xf, 0x0, 0x0, 0xf0, 0x0,

    /* U+0055 "U" */
    0xf0, 0x0, 0xff, 0x0, 0xf, 0xf0, 0x0, 0xff,
    0x0, 0xf, 0xf0, 0x0, 0xff, 0x0, 0xf, 0xf0,
    0x0, 0xff, 0x0, 0xf, 0xf, 0xff, 0x0,

    /* U+0056 "V" */
    0xf0, 0x0, 0xff, 0x0, 0xf, 0xf0, 0x0, 0xf0,
    0xf0, 0xf0, 0xf, 0xf, 0x0, 0xf0, 0xf0, 0xf,
    0xf, 0x0, 0xf, 0x0, 0x0, 0xf0, 0x0,

    /* U+0057 "W" */
    0xf0, 0x0, 0xff, 0x0, 0xf, 0xf0, 0xf0, 0xff,
    0xf, 0xf, 0xf0, 0xf0, 0xff, 0xf, 0xf, 0xf,
    0xf, 0x0, 0xf0, 0xf0, 0xf, 0xf, 0x0,

    /* U+0058 "X" */
    0xf0, 0x0, 0xff, 0x0, 0xf, 0xf, 0xf, 0x0,
    0xf0, 0xf0, 0x0, 0xf0, 0x0, 0xf0, 0xf0, 0xf,
    0xf, 0xf, 0x0, 0xf, 0xf0, 0x0, 0xf0,

    /* U+0059 "Y" */
    0xf0, 0x0, 0xff, 0x0, 0xf, 0xf, 0xf, 0x0,
    0xf0, 0xf0, 0x0, 0xf0, 0x0, 0xf, 0x0, 0x0,
    0xf0, 0x0, 0xf, 0x0, 0x0, 0xf0, 0x0,

    /* U+005A "Z" */
    0xff, 0xff, 0x0, 0xf, 0x0, 0xf0, 0x0, 0xf0,
    0xf, 0xf0, 0xf, 0x0, 0xf, 0x0, 0xf0, 0x0,
    0xff, 0xff,

    /* U+005B "[" */
    0xff, 0xff, 0x0, 0xf0, 0xf, 0x0, 0xf0, 0xf,
    0x0, 0xf0, 0xf, 0x0, 0xff, 0xf0,

    /* U+005C "\\" */
    0xf0, 0xf, 0x0, 0xf0, 0x0, 0xf0, 0xf, 0x0,
    0xf0, 0x0, 0xf0, 0xf, 0x0, 0xf0,

    /* U+005D "]" */
    0xff, 0xf0, 0xf, 0x0, 0xf0, 0xf, 0x0, 0xf0,
    0xf, 0x0, 0xf0, 0xf, 0xff, 0xf0,

    /* U+005E "^" */
    0xf, 0xf, 0xf,

    /* U+005F "_" */
    0xff, 0xff, 0xf0,

    /* U+0060 "`" */
    0xf0, 0xf,

    /* U+0061 "a" */
    0xff, 0xff, 0x0, 0x0, 0xf, 0xf, 0xff, 0xff,
    0x0, 0xf, 0xf0, 0x0, 0xf0, 0xff, 0xff,

    /* U+0062 "b" */
    0xf0, 0x0, 0xf, 0x0, 0x0, 0xf0, 0x0, 0xf,
    0xff, 0xf0, 0xf0, 0x0, 0xff, 0x0, 0xf, 0xf0,
    0x0, 0xff, 0x0, 0xf, 0xff, 0xff, 0x0,

    /* U+0063 "c" */
    0xf, 0xff, 0xff, 0x0, 0x0, 0xf0, 0x0, 0xf,
    0x0, 0x0, 0xf0, 0x0, 0x0, 0xff, 0xff,

    /* U+0064 "d" */
    0x0, 0x0, 0xf0, 0x0, 0xf, 0x0, 0x0, 0xf0,
    0xff, 0xff, 0xf0, 0x0, 0xff, 0x0, 0xf, 0xf0,
    0x0, 0xff, 0x0, 0xf, 0xf, 0xff, 0xf0,

    /* U+0065 "e" */
    0xf, 0xff, 0xf, 0x0, 0xf, 0xff, 0xff, 0xff,
    0x0, 0x0, 0xf0, 0x0, 0x0, 0xff, 0xff,

    /* U+0066 "f" */
    0x0, 0xff, 0xf, 0x0, 0xf, 0x0, 0xff, 0xff,
    0xf, 0x0, 0xf, 0x0, 0xf, 0x0, 0xf, 0x0,
    0xf, 0x0,

    /* U+0067 "g" */
    0xf, 0xff, 0xff, 0x0, 0xf, 0xf0, 0x0, 0xff,
    0x0, 0xf, 0xf, 0xff, 0xf0, 0x0, 0xf, 0xff,
    0xff, 0x0,

    /* U+0068 "h" */
    0xf0, 0x0, 0xf, 0x0, 0x0, 0xf0, 0x0, 0xf,
    0xff, 0xf0, 0xf0, 0x0, 0xff, 0x0, 0xf, 0xf0,
    0x0, 0xff, 0x0, 0xf, 0xf0, 0x0, 0xf0,

    /* U+0069 "i" */
    0xf0, 0xff, 0xff, 0xff,

    /* U+006A "j" */
    0x0, 0xf0, 0x0, 0x0, 0xf0, 0xf, 0x0, 0xf0,
    0xf, 0x0, 0xf0, 0xf, 0xff, 0x0,

    /* U+006B "k" */
    0xf0, 0x0, 0xf0, 0x0, 0xf0, 0x0, 0xf0, 0xf,
    0xf0, 0xf0, 0xff, 0x0, 0xf0, 0xf0, 0xf0, 0xf,
    0xf0, 0xf,

    /* U+006C "l" */
    0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0,
    0xf,

    /* U+006D "m" */
    0xff, 0xff, 0xff, 0xf, 0x0, 0xf0, 0xf, 0xf0,
    0xf, 0x0, 0xff, 0x0, 0xf0, 0xf, 0xf0, 0xf,
    0x0, 0xff, 0x0, 0xf0, 0xf,

    /* U+006E "n" */
    0xff, 0xff, 0xf, 0x0, 0xf, 0xf0, 0x0, 0xff,
    0x0, 0xf, 0xf0, 0x0, 0xff, 0x0, 0xf,

    /* U+006F "o" */
    0xf, 0xff, 0xf, 0x0, 0xf, 0xf0, 0x0, 0xff,
    0x0, 0xf, 0xf0, 0x0, 0xf0, 0xff, 0xf0,

    /* U+0070 "p" */
    0xff, 0xff, 0xf, 0x0, 0xf, 0xf0, 0x0, 0xff,
    0x0, 0xf, 0xff, 0xff, 0xf, 0x0, 0x0, 0xf0,
    0x0, 0x0,

    /* U+0071 "q" */
    0xf, 0xff, 0xff, 0x0, 0xf, 0xf0, 0x0, 0xff,
    0x0, 0xf, 0xf, 0xff, 0xf0, 0x0, 0xf, 0x0,
    0x0, 0xf0,

    /* U+0072 "r" */
    0xf0, 0xff, 0xff, 0x0, 0xf0, 0x0, 0xf0, 0x0,
    0xf0, 0x0, 0xf0, 0x0,

    /* U+0073 "s" */
    0xf, 0xff, 0xf0, 0x0, 0xf, 0xf0, 0x0, 0xf,
    0x0, 0xf, 0xff, 0xf0,

    /* U+0074 "t" */
    0xf, 0x0, 0xf, 0x0, 0xff, 0xff, 0xf, 0x0,
    0xf, 0x0, 0xf, 0x0, 0xf, 0x0, 0x0, 0xff,

    /* U+0075 "u" */
    0xf0, 0x0, 0xff, 0x0, 0xf, 0xf0, 0x0, 0xff,
    0x0, 0xf, 0xf0, 0x0, 0xf0, 0xff, 0xff,

    /* U+0076 "v" */
    0xf0, 0x0, 0xff, 0x0, 0xf, 0xf, 0xf, 0x0,
    0xf0, 0xf0, 0x0, 0xf0, 0x0, 0xf, 0x0,

    /* U+0077 "w" */
    0xf0, 0x0, 0xff, 0xf, 0xf, 0xf0, 0xf0, 0xff,
    0xf, 0xf, 0xf, 0xf, 0x0, 0xf0, 0xf0,

    /* U+0078 "x" */
    0xf0, 0x0, 0xf0, 0xf0, 0xf0, 0x0, 0xf0, 0x0,
    0xf, 0x0, 0xf, 0xf, 0xf, 0x0, 0xf,

    /* U+0079 "y" */
    0xf0, 0x0, 0xff, 0x0, 0xf, 0xf, 0xf, 0x0,
    0xf0, 0xf0, 0x0, 0xf0, 0x0, 0xf, 0x0, 0xff,
    0x0, 0x0,

    /* U+007A "z" */
    0xff, 0xff, 0xf0, 0x0, 0xf0, 0x0, 0xf0, 0x0,
    0xf0, 0x0, 0xf0, 0x0, 0xf, 0xff, 0xff,

    /* U+007B "{" */
    0x0, 0xff, 0xf, 0x0, 0xf, 0x0, 0xf, 0x0,
    0xf0, 0x0, 0xf, 0x0, 0xf, 0x0, 0xf, 0x0,
    0x0, 0xff,

    /* U+007C "|" */
    0xff, 0xff, 0xff, 0xff, 0xf0,

    /* U+007D "}" */
    0xff, 0x0, 0x0, 0xf0, 0x0, 0xf0, 0x0, 0xf0,
    0x0, 0xf, 0x0, 0xf0, 0x0, 0xf0, 0x0, 0xf0,
    0xff, 0x0,

    /* U+007E "~" */
    0xf, 0x0, 0xf, 0xf, 0xf, 0x0, 0xf, 0x0,

    /* U+007F "" */
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 48, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 0, .adv_w = 64, .box_w = 1, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 5, .adv_w = 64, .box_w = 3, .box_h = 2, .ofs_x = 0, .ofs_y = 7},
    {.bitmap_index = 8, .adv_w = 128, .box_w = 7, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 40, .adv_w = 96, .box_w = 5, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 63, .adv_w = 128, .box_w = 7, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 95, .adv_w = 112, .box_w = 6, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 122, .adv_w = 32, .box_w = 1, .box_h = 2, .ofs_x = 0, .ofs_y = 7},
    {.bitmap_index = 123, .adv_w = 64, .box_w = 3, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 137, .adv_w = 64, .box_w = 3, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 151, .adv_w = 128, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 176, .adv_w = 96, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 2},
    {.bitmap_index = 189, .adv_w = 64, .box_w = 2, .box_h = 2, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 191, .adv_w = 96, .box_w = 5, .box_h = 1, .ofs_x = 0, .ofs_y = 4},
    {.bitmap_index = 194, .adv_w = 64, .box_w = 1, .box_h = 1, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 195, .adv_w = 96, .box_w = 3, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 209, .adv_w = 96, .box_w = 5, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 232, .adv_w = 64, .box_w = 2, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 241, .adv_w = 96, .box_w = 5, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 264, .adv_w = 96, .box_w = 5, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 287, .adv_w = 112, .box_w = 6, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 314, .adv_w = 96, .box_w = 5, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 337, .adv_w = 96, .box_w = 5, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 360, .adv_w = 96, .box_w = 5, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 383, .adv_w = 96, .box_w = 5, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 406, .adv_w = 96, .box_w = 5, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 429, .adv_w = 64, .box_w = 1, .box_h = 5, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 432, .adv_w = 80, .box_w = 2, .box_h = 6, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 438, .adv_w = 64, .box_w = 3, .box_h = 5, .ofs_x = 0, .ofs_y = 2},
    {.bitmap_index = 446, .adv_w = 96, .box_w = 5, .box_h = 3, .ofs_x = 0, .ofs_y = 3},
    {.bitmap_index = 454, .adv_w = 64, .box_w = 3, .box_h = 5, .ofs_x = 0, .ofs_y = 2},
    {.bitmap_index = 462, .adv_w = 80, .box_w = 4, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 480, .adv_w = 128, .box_w = 7, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 512, .adv_w = 96, .box_w = 5, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 535, .adv_w = 96, .box_w = 5, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 558, .adv_w = 96, .box_w = 5, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 581, .adv_w = 96, .box_w = 5, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 604, .adv_w = 80, .box_w = 4, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 622, .adv_w = 80, .box_w = 4, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 640, .adv_w = 96, .box_w = 5, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 663, .adv_w = 96, .box_w = 5, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 686, .adv_w = 32, .box_w = 1, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 691, .adv_w = 80, .box_w = 4, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 709, .adv_w = 96, .box_w = 5, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 732, .adv_w = 80, .box_w = 4, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 750, .adv_w = 96, .box_w = 5, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 773, .adv_w = 96, .box_w = 5, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 796, .adv_w = 96, .box_w = 5, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 819, .adv_w = 96, .box_w = 5, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 842, .adv_w = 96, .box_w = 5, .box_h = 10, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 867, .adv_w = 96, .box_w = 5, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 890, .adv_w = 80, .box_w = 4, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 908, .adv_w = 96, .box_w = 5, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 931, .adv_w = 96, .box_w = 5, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 954, .adv_w = 96, .box_w = 5, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 977, .adv_w = 96, .box_w = 5, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1000, .adv_w = 96, .box_w = 5, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1023, .adv_w = 96, .box_w = 5, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1046, .adv_w = 80, .box_w = 4, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1064, .adv_w = 64, .box_w = 3, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1078, .adv_w = 96, .box_w = 3, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1092, .adv_w = 64, .box_w = 3, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1106, .adv_w = 64, .box_w = 3, .box_h = 2, .ofs_x = 0, .ofs_y = 7},
    {.bitmap_index = 1109, .adv_w = 96, .box_w = 5, .box_h = 1, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1112, .adv_w = 48, .box_w = 2, .box_h = 2, .ofs_x = 0, .ofs_y = 7},
    {.bitmap_index = 1114, .adv_w = 96, .box_w = 5, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1129, .adv_w = 96, .box_w = 5, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1152, .adv_w = 96, .box_w = 5, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1167, .adv_w = 96, .box_w = 5, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1190, .adv_w = 96, .box_w = 5, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1205, .adv_w = 80, .box_w = 4, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1223, .adv_w = 96, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 1241, .adv_w = 96, .box_w = 5, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1264, .adv_w = 32, .box_w = 1, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1268, .adv_w = 64, .box_w = 3, .box_h = 9, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 1282, .adv_w = 80, .box_w = 4, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1300, .adv_w = 48, .box_w = 2, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1309, .adv_w = 128, .box_w = 7, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1330, .adv_w = 96, .box_w = 5, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1345, .adv_w = 96, .box_w = 5, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1360, .adv_w = 96, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 1378, .adv_w = 96, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 1396, .adv_w = 80, .box_w = 4, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1408, .adv_w = 80, .box_w = 4, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1420, .adv_w = 80, .box_w = 4, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1436, .adv_w = 96, .box_w = 5, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1451, .adv_w = 96, .box_w = 5, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1466, .adv_w = 96, .box_w = 5, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1481, .adv_w = 96, .box_w = 5, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1496, .adv_w = 96, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 1514, .adv_w = 96, .box_w = 5, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1529, .adv_w = 80, .box_w = 4, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1547, .adv_w = 32, .box_w = 1, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1552, .adv_w = 80, .box_w = 4, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1570, .adv_w = 96, .box_w = 5, .box_h = 3, .ofs_x = 0, .ofs_y = 6},
    {.bitmap_index = 1578, .adv_w = 160, .box_w = 9, .box_h = 12, .ofs_x = 0, .ofs_y = -1}
};

/*---------------------
 *  CHARACTER MAPPING
 *--------------------*/



/*Collect the unicode lists and glyph_id offsets*/
static const lv_font_fmt_txt_cmap_t cmaps[] =
{
    {
        .range_start = 32, .range_length = 96, .glyph_id_start = 1,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    }
};



/*--------------------
 *  ALL CUSTOM DATA
 *--------------------*/

#if LV_VERSION_CHECK(8, 0, 0)
/*Store all the custom data of the font*/
static  lv_font_fmt_txt_glyph_cache_t cache;
static const lv_font_fmt_txt_dsc_t font_dsc = {
#else
static lv_font_fmt_txt_dsc_t font_dsc = {
#endif
    .glyph_bitmap = glyph_bitmap,
    .glyph_dsc = glyph_dsc,
    .cmaps = cmaps,
    .kern_dsc = NULL,
    .kern_scale = 0,
    .cmap_num = 1,
    .bpp = 4,
    .kern_classes = 0,
    .bitmap_format = 0,
#if LV_VERSION_CHECK(8, 0, 0)
    .cache = &cache
#endif
};


/*-----------------
 *  PUBLIC FONT
 *----------------*/

/*Initialize a public general font descriptor*/
#if LV_VERSION_CHECK(8, 0, 0)
const lv_font_t NDS12_10px = {
#else
const lv_font_t NDS12_10px = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 12,          /*The maximum line height required by the font*/
    .base_line = 1,             /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_NONE,
#endif
#if LV_VERSION_CHECK(7, 4, 0) || LVGL_VERSION_MAJOR >= 8
    .underline_position = 1,
    .underline_thickness = 0,
#endif
    .dsc = &font_dsc           /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
};



#endif /*#if NDS12_10PX*/

