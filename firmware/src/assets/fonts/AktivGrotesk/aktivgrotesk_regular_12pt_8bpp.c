/*******************************************************************************
 * Size: 8 px
 * Bpp: 4
 * Opts: --bpp 4 --size 8 --no-compress --font AktivGrotesk-Regular.ttf --range 32-126,192-255 --format lvgl -o aktivgrotesk_regular_8pt.c
 ******************************************************************************/

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

#ifndef AKTIVGROTESK_REGULAR_8PT
#define AKTIVGROTESK_REGULAR_8PT 1
#endif

#if AKTIVGROTESK_REGULAR_8PT

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+0020 " " */

    /* U+0021 "!" */
    0x64, 0x64, 0x64, 0x53, 0x64,

    /* U+0022 "\"" */
    0xa5, 0x58, 0x44,

    /* U+0023 "#" */
    0x8, 0x19, 0x4, 0xc7, 0xc4, 0xa, 0x9, 0x6,
    0xc8, 0xb2, 0x36, 0x54, 0x0,

    /* U+0024 "$" */
    0x3, 0xb1, 0x6, 0x7a, 0xb1, 0x76, 0x80, 0x0,
    0x6d, 0xa0, 0x50, 0x87, 0x45, 0xab, 0xb1, 0x1,
    0x90, 0x0,

    /* U+0025 "%" */
    0x59, 0x90, 0x82, 0x0, 0xa0, 0xa2, 0x80, 0x0,
    0x39, 0x79, 0x49, 0x70, 0x0, 0x36, 0xa0, 0xa0,
    0x0, 0xa0, 0x69, 0xa0,

    /* U+0026 "&" */
    0xa, 0x85, 0x0, 0x9, 0x35, 0x0, 0x39, 0xa0,
    0x50, 0xa1, 0x2b, 0x70, 0x4a, 0x89, 0x90,

    /* U+0027 "'" */
    0xa0, 0x80,

    /* U+0028 "(" */
    0xa, 0x4, 0x60, 0x74, 0x8, 0x30, 0x65, 0x2,
    0x90, 0x5, 0x0,

    /* U+0029 ")" */
    0xa0, 0x56, 0x29, 0x1a, 0x38, 0x73, 0x50,

    /* U+002A "*" */
    0x39, 0x33, 0xe5, 0x21, 0x30,

    /* U+002B "+" */
    0x0, 0x60, 0x0, 0xa, 0x0, 0x49, 0xd9, 0x10,
    0xa, 0x0,

    /* U+002C "," */
    0x42, 0x81,

    /* U+002D "-" */
    0x6a, 0x70,

    /* U+002E "." */
    0x74,

    /* U+002F "/" */
    0x4, 0x60, 0xa1, 0x1a, 0x6, 0x50, 0xb0, 0x0,

    /* U+0030 "0" */
    0x1a, 0x99, 0x8, 0x30, 0x83, 0xa1, 0x5, 0x58,
    0x30, 0x83, 0x1a, 0x99, 0x0,

    /* U+0031 "1" */
    0x7, 0xd1, 0x2, 0xa1, 0x0, 0xa1, 0x0, 0xa1,
    0x0, 0xa1,

    /* U+0032 "2" */
    0x1a, 0xaa, 0x2, 0x10, 0xa1, 0x0, 0x2a, 0x0,
    0x49, 0x0, 0x5e, 0x99, 0x10,

    /* U+0033 "3" */
    0x2a, 0x99, 0x2, 0x10, 0xa0, 0x2, 0xaa, 0x3,
    0x10, 0x92, 0x3a, 0x9a, 0x0,

    /* U+0034 "4" */
    0x0, 0x7a, 0x0, 0x47, 0xa0, 0x28, 0xa, 0x7,
    0x98, 0xd2, 0x0, 0xa, 0x0,

    /* U+0035 "5" */
    0x2c, 0x98, 0x4, 0xb8, 0x60, 0x12, 0x9, 0x13,
    0x20, 0x82, 0x1a, 0x99, 0x0,

    /* U+0036 "6" */
    0x8, 0x9a, 0x5, 0x98, 0x90, 0x77, 0x8, 0x36,
    0x50, 0x73, 0x1a, 0x89, 0x0,

    /* U+0037 "7" */
    0x69, 0x9e, 0x10, 0x2, 0x90, 0x0, 0xa2, 0x0,
    0x1a, 0x0, 0x7, 0x50, 0x0,

    /* U+0038 "8" */
    0x2a, 0x9a, 0x6, 0x30, 0x82, 0x1c, 0xab, 0x9,
    0x20, 0x65, 0x4a, 0x9a, 0x10,

    /* U+0039 "9" */
    0x3a, 0x87, 0x8, 0x20, 0xa1, 0x29, 0x8a, 0x22,
    0x10, 0xa1, 0x2a, 0x97, 0x0,

    /* U+003A ":" */
    0x74, 0x0, 0x0, 0x74,

    /* U+003B ";" */
    0x74, 0x0, 0x0, 0x73, 0x50,

    /* U+003C "<" */
    0x0, 0x1, 0x10, 0x28, 0x81, 0x4d, 0x20, 0x0,
    0x18, 0x91, 0x0, 0x0, 0x0,

    /* U+003D "=" */
    0x39, 0x99, 0x0, 0x0, 0x0, 0x39, 0x99, 0x0,

    /* U+003E ">" */
    0x10, 0x0, 0x2, 0x97, 0x10, 0x0, 0x5e, 0x13,
    0x96, 0x0, 0x10, 0x0, 0x0,

    /* U+003F "?" */
    0x5a, 0xa4, 0x30, 0x29, 0x0, 0x91, 0x3, 0x20,
    0x6, 0x40,

    /* U+0040 "@" */
    0x0, 0x68, 0x76, 0x0, 0x9, 0x20, 0x2, 0x90,
    0x54, 0x28, 0x76, 0x72, 0x90, 0xa0, 0x56, 0x44,
    0x81, 0xb0, 0x74, 0x72, 0x38, 0x87, 0x98, 0x70,
    0x4, 0x88, 0x77, 0x10,

    /* U+0041 "A" */
    0x0, 0xd4, 0x0, 0x5, 0x7b, 0x0, 0xb, 0x9,
    0x30, 0x3d, 0x9a, 0xa0, 0xa2, 0x0, 0xb1,

    /* U+0042 "B" */
    0x6b, 0x9a, 0x36, 0x50, 0x47, 0x6b, 0x9d, 0x36,
    0x50, 0x1a, 0x6b, 0x8a, 0x50,

    /* U+0043 "C" */
    0x8, 0x99, 0x80, 0x57, 0x0, 0x61, 0x83, 0x0,
    0x0, 0x57, 0x0, 0x62, 0x8, 0x99, 0x80,

    /* U+0044 "D" */
    0x6b, 0x9a, 0x50, 0x66, 0x0, 0xb1, 0x66, 0x0,
    0x83, 0x66, 0x0, 0xb1, 0x6b, 0x9a, 0x40,

    /* U+0045 "E" */
    0x6c, 0x99, 0x36, 0x60, 0x0, 0x6b, 0x99, 0x16,
    0x60, 0x0, 0x6c, 0x99, 0x40,

    /* U+0046 "F" */
    0x6c, 0x99, 0x16, 0x60, 0x0, 0x6c, 0xaa, 0x6,
    0x60, 0x0, 0x66, 0x0, 0x0,

    /* U+0047 "G" */
    0x8, 0x99, 0x80, 0x57, 0x0, 0x30, 0x83, 0x8,
    0xa3, 0x57, 0x0, 0x64, 0x8, 0xa9, 0x91,

    /* U+0048 "H" */
    0x66, 0x0, 0xb6, 0x60, 0xb, 0x6c, 0xaa, 0xe6,
    0x60, 0xb, 0x66, 0x0, 0xb0,

    /* U+0049 "I" */
    0x66, 0x66, 0x66, 0x66, 0x66,

    /* U+004A "J" */
    0x0, 0x8, 0x30, 0x0, 0x83, 0x0, 0x8, 0x38,
    0x0, 0x92, 0x3a, 0x99, 0x0,

    /* U+004B "K" */
    0x66, 0x7, 0x50, 0x66, 0x94, 0x0, 0x6c, 0xa4,
    0x0, 0x66, 0xb, 0x10, 0x66, 0x2, 0xb0,

    /* U+004C "L" */
    0x66, 0x0, 0x6, 0x60, 0x0, 0x66, 0x0, 0x6,
    0x60, 0x0, 0x6c, 0x99, 0x20,

    /* U+004D "M" */
    0x6c, 0x0, 0xe, 0x26, 0xc3, 0x6, 0xc2, 0x66,
    0x90, 0xa8, 0x26, 0x49, 0x65, 0x82, 0x64, 0x2d,
    0x8, 0x20,

    /* U+004E "N" */
    0x6b, 0x0, 0xb6, 0xa7, 0xb, 0x65, 0x83, 0xb6,
    0x50, 0xac, 0x65, 0x2, 0xe0,

    /* U+004F "O" */
    0x8, 0x99, 0x90, 0x57, 0x0, 0x57, 0x83, 0x0,
    0x1a, 0x57, 0x0, 0x57, 0x8, 0x99, 0x90,

    /* U+0050 "P" */
    0x6b, 0x9a, 0x46, 0x50, 0x29, 0x6b, 0x89, 0x26,
    0x50, 0x0, 0x65, 0x0, 0x0,

    /* U+0051 "Q" */
    0x8, 0x99, 0x90, 0x57, 0x0, 0x57, 0x83, 0x0,
    0x1a, 0x57, 0x5, 0x76, 0x8, 0x9a, 0xd2, 0x0,
    0x0, 0x2,

    /* U+0052 "R" */
    0x6b, 0x9a, 0x36, 0x50, 0x38, 0x6b, 0x9c, 0x16,
    0x50, 0xb1, 0x65, 0x3, 0x90,

    /* U+0053 "S" */
    0x3a, 0x9a, 0x37, 0x40, 0x3, 0x7, 0x88, 0x24,
    0x10, 0x1b, 0x3a, 0x9b, 0x40,

    /* U+0054 "T" */
    0x8a, 0xea, 0x60, 0xc, 0x0, 0x0, 0xc0, 0x0,
    0xc, 0x0, 0x0, 0xc0, 0x0,

    /* U+0055 "U" */
    0x74, 0x0, 0xb7, 0x40, 0xb, 0x64, 0x0, 0xb5,
    0x60, 0xb, 0xa, 0x9a, 0x40,

    /* U+0056 "V" */
    0xa2, 0x0, 0xb0, 0x38, 0x3, 0x90, 0xb, 0x9,
    0x20, 0x5, 0x7a, 0x0, 0x0, 0xe4, 0x0,

    /* U+0057 "W" */
    0xb1, 0xc, 0x40, 0x93, 0x65, 0x19, 0x90, 0xb0,
    0x1a, 0x63, 0xa2, 0x80, 0xb, 0x90, 0x7a, 0x30,
    0x7, 0x90, 0x2e, 0x0,

    /* U+0058 "X" */
    0x57, 0x5, 0x60, 0x9, 0x79, 0x0, 0x1, 0xf2,
    0x0, 0xa, 0x3b, 0x0, 0x84, 0x4, 0x90,

    /* U+0059 "Y" */
    0x84, 0x5, 0x70, 0xb2, 0xa0, 0x3, 0xe2, 0x0,
    0xc, 0x0, 0x0, 0xc0, 0x0,

    /* U+005A "Z" */
    0x69, 0x9d, 0x50, 0x3, 0x90, 0x2, 0xa0, 0x1,
    0xa0, 0x0, 0xab, 0x99, 0x40,

    /* U+005B "[" */
    0x6b, 0x16, 0x50, 0x65, 0x6, 0x50, 0x65, 0x6,
    0x50, 0x5a, 0x10,

    /* U+005C "\\" */
    0xa0, 0x6, 0x50, 0x1a, 0x0, 0xa1, 0x4, 0x60,

    /* U+005D "]" */
    0x8a, 0x1a, 0x1a, 0x1a, 0x1a, 0x1a, 0x88,

    /* U+005E "^" */
    0x3, 0xd0, 0x0, 0xa4, 0x70, 0x46, 0xa, 0x10,

    /* U+005F "_" */
    0x66, 0x66,

    /* U+0060 "`" */
    0x9, 0x10,

    /* U+0061 "a" */
    0x39, 0x86, 0x27, 0x7b, 0xa1, 0xb, 0x6a, 0x8b,

    /* U+0062 "b" */
    0x73, 0x0, 0x7, 0x98, 0xa0, 0x74, 0x7, 0x47,
    0x40, 0x74, 0x79, 0x8a, 0x0,

    /* U+0063 "c" */
    0x1a, 0x99, 0x8, 0x30, 0x30, 0x83, 0x4, 0x1,
    0xa9, 0x90,

    /* U+0064 "d" */
    0x0, 0x7, 0x31, 0xa9, 0xc3, 0x83, 0x8, 0x38,
    0x30, 0x83, 0x2a, 0x8b, 0x30,

    /* U+0065 "e" */
    0x18, 0x89, 0x8, 0x87, 0x91, 0x84, 0x2, 0x1,
    0xa8, 0x90,

    /* U+0066 "f" */
    0x1a, 0x58, 0xc4, 0x37, 0x3, 0x70, 0x37, 0x0,

    /* U+0067 "g" */
    0x1a, 0x8c, 0x38, 0x30, 0x83, 0x83, 0x8, 0x32,
    0xa8, 0xc2, 0x29, 0x89, 0x0,

    /* U+0068 "h" */
    0x73, 0x0, 0x7, 0x98, 0xa0, 0x74, 0xa, 0x7,
    0x30, 0xa1, 0x73, 0xa, 0x10,

    /* U+0069 "i" */
    0x53, 0x74, 0x74, 0x74, 0x74,

    /* U+006A "j" */
    0x5, 0x30, 0x74, 0x7, 0x40, 0x74, 0x7, 0x35,
    0xb1,

    /* U+006B "k" */
    0x73, 0x0, 0x7, 0x36, 0x70, 0x7b, 0xb0, 0x7,
    0x58, 0x30, 0x73, 0xb, 0x0,

    /* U+006C "l" */
    0x74, 0x74, 0x74, 0x74, 0x74,

    /* U+006D "m" */
    0x79, 0x99, 0x8b, 0x27, 0x40, 0xc0, 0x55, 0x73,
    0xb, 0x5, 0x67, 0x30, 0xb0, 0x56,

    /* U+006E "n" */
    0x79, 0x8a, 0x7, 0x40, 0xa0, 0x73, 0xa, 0x17,
    0x30, 0xa1,

    /* U+006F "o" */
    0x1a, 0x89, 0x8, 0x30, 0x65, 0x83, 0x6, 0x41,
    0xa8, 0x90,

    /* U+0070 "p" */
    0x79, 0x8a, 0x7, 0x40, 0x74, 0x74, 0x7, 0x47,
    0xa8, 0xa0, 0x73, 0x0, 0x0,

    /* U+0071 "q" */
    0x1a, 0x8b, 0x38, 0x30, 0x83, 0x83, 0x8, 0x32,
    0xb8, 0xb3, 0x0, 0x7, 0x30,

    /* U+0072 "r" */
    0x0, 0x7, 0xa6, 0x74, 0x7, 0x30, 0x73, 0x0,

    /* U+0073 "s" */
    0x49, 0x96, 0x68, 0x31, 0x22, 0x5a, 0x59, 0x97,

    /* U+0074 "t" */
    0x37, 0x8, 0xb4, 0x37, 0x3, 0x70, 0x1b, 0x50,

    /* U+0075 "u" */
    0x73, 0xb, 0x73, 0xb, 0x73, 0xc, 0x3b, 0x8c,

    /* U+0076 "v" */
    0xb1, 0xb, 0x5, 0x63, 0x70, 0xb, 0x91, 0x0,
    0x7a, 0x0,

    /* U+0077 "w" */
    0xb0, 0x87, 0x1a, 0x73, 0x89, 0x55, 0x39, 0x69,
    0xa0, 0xd, 0x24, 0xb0,

    /* U+0078 "x" */
    0x66, 0x46, 0xa, 0xa0, 0xa, 0xb0, 0x83, 0x39,

    /* U+0079 "y" */
    0xa1, 0xb, 0x3, 0x84, 0x70, 0xa, 0xa0, 0x0,
    0x48, 0x0, 0x4b, 0x10, 0x0,

    /* U+007A "z" */
    0x68, 0xc6, 0x3, 0xa0, 0x1b, 0x0, 0xaa, 0x84,

    /* U+007B "{" */
    0x1b, 0x23, 0x70, 0x37, 0xa, 0x40, 0x37, 0x3,
    0x70, 0x9, 0x20,

    /* U+007C "|" */
    0x74, 0x74, 0x74, 0x74, 0x74, 0x74, 0x32,

    /* U+007D "}" */
    0x95, 0x2, 0x80, 0x28, 0x0, 0xd0, 0x28, 0x2,
    0x80, 0x93, 0x0,

    /* U+007E "~" */
    0x3a, 0x33, 0x47, 0x6, 0x91,

    /* U+00C0 "À" */
    0x1, 0x90, 0x0, 0x0, 0x0, 0x0, 0x0, 0xd4,
    0x0, 0x5, 0x7b, 0x0, 0xb, 0x9, 0x30, 0x3d,
    0x9a, 0xa0, 0xa2, 0x0, 0xb1,

    /* U+00C1 "Á" */
    0x0, 0x65, 0x0, 0x0, 0x0, 0x0, 0x0, 0xd4,
    0x0, 0x5, 0x7b, 0x0, 0xb, 0x9, 0x30, 0x3d,
    0x9a, 0xa0, 0xa2, 0x0, 0xb1,

    /* U+00C2 "Â" */
    0x2, 0xa7, 0x0, 0x0, 0x0, 0x0, 0x0, 0xd4,
    0x0, 0x5, 0x7b, 0x0, 0xb, 0x9, 0x30, 0x3d,
    0x9a, 0xa0, 0xa2, 0x0, 0xb1,

    /* U+00C3 "Ã" */
    0x4, 0x76, 0x0, 0x0, 0x0, 0x0, 0x0, 0xd4,
    0x0, 0x5, 0x7b, 0x0, 0xb, 0x9, 0x30, 0x3d,
    0x9a, 0xa0, 0xa2, 0x0, 0xb1,

    /* U+00C4 "Ä" */
    0x4, 0x48, 0x0, 0x0, 0x0, 0x0, 0x0, 0xd4,
    0x0, 0x5, 0x7b, 0x0, 0xb, 0x9, 0x30, 0x3d,
    0x9a, 0xa0, 0xa2, 0x0, 0xb1,

    /* U+00C5 "Å" */
    0x0, 0x73, 0x0, 0x0, 0x55, 0x0, 0x1, 0xf7,
    0x0, 0x7, 0x7d, 0x0, 0xc, 0x9, 0x40, 0x4d,
    0x9a, 0xa0, 0xa2, 0x0, 0xb1,

    /* U+00C6 "Æ" */
    0x0, 0xb, 0xda, 0x98, 0x0, 0x6, 0x29, 0x10,
    0x0, 0x3, 0xa9, 0xda, 0x96, 0x0, 0x80, 0x9,
    0x10, 0x0, 0x92, 0x0, 0x9a, 0x99, 0x0,

    /* U+00C7 "Ç" */
    0x8, 0x99, 0x80, 0x57, 0x0, 0x61, 0x83, 0x0,
    0x0, 0x67, 0x0, 0x62, 0xa, 0xa9, 0x80, 0x0,
    0x46, 0x0, 0x0, 0x56, 0x0,

    /* U+00C8 "È" */
    0x2, 0x90, 0x0, 0x0, 0x0, 0x6c, 0x99, 0x36,
    0x60, 0x0, 0x6b, 0x99, 0x16, 0x60, 0x0, 0x6c,
    0x99, 0x40,

    /* U+00C9 "É" */
    0x0, 0x93, 0x0, 0x0, 0x0, 0x6c, 0x99, 0x36,
    0x60, 0x0, 0x6b, 0x99, 0x16, 0x60, 0x0, 0x6c,
    0x99, 0x40,

    /* U+00CA "Ê" */
    0x5, 0xa4, 0x0, 0x0, 0x0, 0x6c, 0x99, 0x36,
    0x60, 0x0, 0x6b, 0x99, 0x16, 0x60, 0x0, 0x6c,
    0x99, 0x40,

    /* U+00CB "Ë" */
    0x6, 0x36, 0x0, 0x0, 0x0, 0x6c, 0x99, 0x36,
    0x60, 0x0, 0x6b, 0x99, 0x16, 0x60, 0x0, 0x6c,
    0x99, 0x40,

    /* U+00CC "Ì" */
    0x9, 0x20, 0x0, 0x6, 0x60, 0x66, 0x6, 0x60,
    0x66, 0x6, 0x60,

    /* U+00CD "Í" */
    0x29, 0x0, 0x0, 0x66, 0x6, 0x60, 0x66, 0x6,
    0x60, 0x66, 0x0,

    /* U+00CE "Î" */
    0x9, 0x90, 0x0, 0x0, 0x6, 0x60, 0x6, 0x60,
    0x6, 0x60, 0x6, 0x60, 0x6, 0x60,

    /* U+00CF "Ï" */
    0x7, 0x71, 0x0, 0x0, 0x6, 0x60, 0x6, 0x60,
    0x6, 0x60, 0x6, 0x60, 0x6, 0x60,

    /* U+00D0 "Ð" */
    0x4c, 0x9a, 0x70, 0x47, 0x0, 0x93, 0xac, 0x80,
    0x65, 0x47, 0x0, 0xa2, 0x4c, 0x9a, 0x60,

    /* U+00D1 "Ñ" */
    0x3, 0x77, 0x0, 0x0, 0x0, 0x6b, 0x0, 0xb6,
    0xa7, 0xb, 0x65, 0x83, 0xb6, 0x50, 0xac, 0x65,
    0x2, 0xe0,

    /* U+00D2 "Ò" */
    0x0, 0x74, 0x0, 0x0, 0x0, 0x0, 0x8, 0x99,
    0x90, 0x57, 0x0, 0x57, 0x83, 0x0, 0x1a, 0x57,
    0x0, 0x57, 0x8, 0x99, 0x90,

    /* U+00D3 "Ó" */
    0x0, 0x39, 0x0, 0x0, 0x0, 0x0, 0x8, 0x99,
    0x90, 0x57, 0x0, 0x57, 0x83, 0x0, 0x1a, 0x57,
    0x0, 0x57, 0x8, 0x99, 0x90,

    /* U+00D4 "Ô" */
    0x0, 0x99, 0x0, 0x0, 0x0, 0x0, 0x8, 0x99,
    0x90, 0x57, 0x0, 0x57, 0x83, 0x0, 0x1a, 0x57,
    0x0, 0x57, 0x8, 0x99, 0x90,

    /* U+00D5 "Õ" */
    0x1, 0x77, 0x20, 0x0, 0x0, 0x0, 0x8, 0x99,
    0x90, 0x57, 0x0, 0x57, 0x83, 0x0, 0x1a, 0x57,
    0x0, 0x57, 0x8, 0x99, 0x90,

    /* U+00D6 "Ö" */
    0x0, 0x87, 0x10, 0x0, 0x0, 0x0, 0x8, 0x99,
    0x90, 0x57, 0x0, 0x57, 0x83, 0x0, 0x1a, 0x57,
    0x0, 0x57, 0x8, 0x99, 0x90,

    /* U+00D7 "×" */
    0x0, 0x0, 0x1, 0xa3, 0xa0, 0x5, 0xf1, 0x1,
    0x92, 0x90, 0x0, 0x0, 0x0,

    /* U+00D8 "Ø" */
    0x0, 0x0, 0x0, 0x8, 0xaa, 0xc1, 0x58, 0x3,
    0x37, 0x83, 0x12, 0x1a, 0x55, 0x40, 0x67, 0xb,
    0xa9, 0x90, 0x0, 0x0, 0x0,

    /* U+00D9 "Ù" */
    0x1, 0x91, 0x0, 0x0, 0x0, 0x74, 0x0, 0xb7,
    0x40, 0xb, 0x64, 0x0, 0xb5, 0x60, 0xb, 0xa,
    0x9a, 0x40,

    /* U+00DA "Ú" */
    0x0, 0x66, 0x0, 0x0, 0x0, 0x74, 0x0, 0xb7,
    0x40, 0xb, 0x64, 0x0, 0xb5, 0x60, 0xb, 0xa,
    0x9a, 0x40,

    /* U+00DB "Û" */
    0x2, 0x97, 0x0, 0x0, 0x0, 0x74, 0x0, 0xb7,
    0x40, 0xb, 0x64, 0x0, 0xb5, 0x60, 0xb, 0xa,
    0x9a, 0x40,

    /* U+00DC "Ü" */
    0x4, 0x48, 0x0, 0x0, 0x0, 0x74, 0x0, 0xb7,
    0x40, 0xb, 0x64, 0x0, 0xb5, 0x60, 0xb, 0xa,
    0x9a, 0x40,

    /* U+00DD "Ý" */
    0x0, 0x93, 0x0, 0x0, 0x0, 0x84, 0x5, 0x70,
    0xb2, 0xa0, 0x3, 0xe2, 0x0, 0xc, 0x0, 0x0,
    0xc0, 0x0,

    /* U+00DE "Þ" */
    0x6b, 0x99, 0x16, 0x50, 0x39, 0x65, 0x3, 0x86,
    0xb9, 0x91, 0x65, 0x0, 0x0,

    /* U+00DF "ß" */
    0x2a, 0x97, 0x6, 0x30, 0xa0, 0x74, 0xa8, 0x7,
    0x30, 0x73, 0x75, 0x8a, 0x10,

    /* U+00E0 "à" */
    0x4, 0x40, 0x39, 0x86, 0x27, 0x7b, 0xa1, 0xb,
    0x6a, 0x8b,

    /* U+00E1 "á" */
    0x1, 0x60, 0x39, 0x86, 0x27, 0x7b, 0xa1, 0xb,
    0x6a, 0x8b,

    /* U+00E2 "â" */
    0x6, 0x61, 0x39, 0x86, 0x27, 0x7b, 0xa1, 0xb,
    0x6a, 0x8b,

    /* U+00E3 "ã" */
    0x18, 0x84, 0x39, 0x86, 0x27, 0x7b, 0xa1, 0xb,
    0x6a, 0x8b,

    /* U+00E4 "ä" */
    0x8, 0x62, 0x39, 0x86, 0x27, 0x7b, 0xa1, 0xb,
    0x6a, 0x8b,

    /* U+00E5 "å" */
    0x4, 0x60, 0x4, 0x60, 0x39, 0x86, 0x27, 0x7b,
    0xa1, 0xb, 0x6a, 0x8b,

    /* U+00E6 "æ" */
    0x39, 0x89, 0x88, 0x52, 0x86, 0xe9, 0x98, 0xa0,
    0xd, 0x0, 0x26, 0xa8, 0x69, 0xa4,

    /* U+00E7 "ç" */
    0x1a, 0x99, 0x8, 0x30, 0x30, 0x83, 0x4, 0x2,
    0xa9, 0x90, 0x4, 0xb0, 0x0,

    /* U+00E8 "è" */
    0x3, 0x50, 0x1, 0x88, 0x90, 0x88, 0x79, 0x18,
    0x40, 0x20, 0x1a, 0x89, 0x0,

    /* U+00E9 "é" */
    0x0, 0x61, 0x1, 0x88, 0x90, 0x88, 0x79, 0x18,
    0x40, 0x20, 0x1a, 0x89, 0x0,

    /* U+00EA "ê" */
    0x4, 0x62, 0x1, 0x88, 0x90, 0x88, 0x79, 0x18,
    0x40, 0x20, 0x1a, 0x89, 0x0,

    /* U+00EB "ë" */
    0x8, 0x34, 0x1, 0x88, 0x90, 0x88, 0x79, 0x18,
    0x40, 0x20, 0x1a, 0x89, 0x0,

    /* U+00EC "ì" */
    0x6, 0x0, 0x74, 0x7, 0x40, 0x74, 0x7, 0x40,

    /* U+00ED "í" */
    0x16, 0x7, 0x40, 0x74, 0x7, 0x40, 0x74, 0x0,

    /* U+00EE "î" */
    0x6, 0x50, 0x7, 0x40, 0x7, 0x40, 0x7, 0x40,
    0x7, 0x40,

    /* U+00EF "ï" */
    0x16, 0x80, 0x74, 0x7, 0x40, 0x74, 0x7, 0x40,

    /* U+00F0 "ð" */
    0x6, 0x61, 0x0, 0x48, 0x80, 0x8, 0x8c, 0x27,
    0x40, 0x75, 0x83, 0x6, 0x41, 0xa8, 0x90,

    /* U+00F1 "ñ" */
    0x8, 0x85, 0x7, 0x98, 0xa0, 0x74, 0xa, 0x7,
    0x30, 0xa1, 0x73, 0xa, 0x10,

    /* U+00F2 "ò" */
    0x2, 0x60, 0x1, 0xa8, 0x90, 0x83, 0x6, 0x58,
    0x30, 0x64, 0x1a, 0x89, 0x0,

    /* U+00F3 "ó" */
    0x0, 0x61, 0x1, 0xa8, 0x90, 0x83, 0x6, 0x58,
    0x30, 0x64, 0x1a, 0x89, 0x0,

    /* U+00F4 "ô" */
    0x4, 0x62, 0x1, 0xa8, 0x90, 0x83, 0x6, 0x58,
    0x30, 0x64, 0x1a, 0x89, 0x0,

    /* U+00F5 "õ" */
    0x8, 0x86, 0x1, 0xa8, 0x90, 0x83, 0x6, 0x58,
    0x30, 0x64, 0x1a, 0x89, 0x0,

    /* U+00F6 "ö" */
    0x7, 0x35, 0x1, 0xa8, 0x90, 0x83, 0x6, 0x58,
    0x30, 0x64, 0x1a, 0x89, 0x0,

    /* U+00F7 "÷" */
    0x0, 0x90, 0x0, 0x0, 0x0, 0x39, 0x99, 0x0,
    0x9, 0x0,

    /* U+00F8 "ø" */
    0x0, 0x1, 0x1, 0xa8, 0xd0, 0x83, 0x68, 0x58,
    0x75, 0x65, 0x3d, 0x89, 0x1, 0x0, 0x0,

    /* U+00F9 "ù" */
    0x4, 0x30, 0x73, 0xb, 0x73, 0xb, 0x73, 0xc,
    0x3b, 0x8c,

    /* U+00FA "ú" */
    0x0, 0x60, 0x73, 0xb, 0x73, 0xb, 0x73, 0xc,
    0x3b, 0x8c,

    /* U+00FB "û" */
    0x5, 0x61, 0x73, 0xb, 0x73, 0xb, 0x73, 0xc,
    0x3b, 0x8c,

    /* U+00FC "ü" */
    0x8, 0x53, 0x73, 0xb, 0x73, 0xb, 0x73, 0xc,
    0x3b, 0x8c,

    /* U+00FD "ý" */
    0x1, 0x60, 0xa, 0x10, 0xb0, 0x38, 0x47, 0x0,
    0xaa, 0x0, 0x4, 0x80, 0x4, 0xb1, 0x0,

    /* U+00FE "þ" */
    0x73, 0x0, 0x7, 0x98, 0xa0, 0x74, 0x7, 0x47,
    0x40, 0x74, 0x7a, 0x8a, 0x7, 0x30, 0x0,

    /* U+00FF "ÿ" */
    0x8, 0x61, 0xa, 0x10, 0xb0, 0x38, 0x47, 0x0,
    0xaa, 0x0, 0x4, 0x80, 0x4, 0xb1, 0x0
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 29, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 0, .adv_w = 29, .box_w = 2, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 5, .adv_w = 43, .box_w = 3, .box_h = 2, .ofs_x = 0, .ofs_y = 3},
    {.bitmap_index = 8, .adv_w = 76, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 21, .adv_w = 76, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 39, .adv_w = 118, .box_w = 8, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 59, .adv_w = 82, .box_w = 6, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 74, .adv_w = 22, .box_w = 2, .box_h = 2, .ofs_x = 0, .ofs_y = 3},
    {.bitmap_index = 76, .adv_w = 34, .box_w = 3, .box_h = 7, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 87, .adv_w = 34, .box_w = 2, .box_h = 7, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 94, .adv_w = 50, .box_w = 3, .box_h = 3, .ofs_x = 0, .ofs_y = 2},
    {.bitmap_index = 99, .adv_w = 76, .box_w = 5, .box_h = 4, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 109, .adv_w = 29, .box_w = 2, .box_h = 2, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 111, .adv_w = 50, .box_w = 3, .box_h = 1, .ofs_x = 0, .ofs_y = 2},
    {.bitmap_index = 113, .adv_w = 29, .box_w = 2, .box_h = 1, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 114, .adv_w = 42, .box_w = 3, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 122, .adv_w = 76, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 135, .adv_w = 76, .box_w = 4, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 145, .adv_w = 76, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 158, .adv_w = 76, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 171, .adv_w = 76, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 184, .adv_w = 76, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 197, .adv_w = 76, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 210, .adv_w = 76, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 223, .adv_w = 76, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 236, .adv_w = 76, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 249, .adv_w = 29, .box_w = 2, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 253, .adv_w = 29, .box_w = 2, .box_h = 5, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 258, .adv_w = 76, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 271, .adv_w = 76, .box_w = 5, .box_h = 3, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 279, .adv_w = 76, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 292, .adv_w = 64, .box_w = 4, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 302, .adv_w = 122, .box_w = 8, .box_h = 7, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 330, .adv_w = 86, .box_w = 6, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 345, .adv_w = 81, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 358, .adv_w = 91, .box_w = 6, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 373, .adv_w = 91, .box_w = 6, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 388, .adv_w = 76, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 401, .adv_w = 72, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 414, .adv_w = 92, .box_w = 6, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 429, .adv_w = 90, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 442, .adv_w = 32, .box_w = 2, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 447, .adv_w = 76, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 460, .adv_w = 83, .box_w = 6, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 475, .adv_w = 72, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 488, .adv_w = 108, .box_w = 7, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 506, .adv_w = 90, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 519, .adv_w = 98, .box_w = 6, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 534, .adv_w = 80, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 547, .adv_w = 98, .box_w = 6, .box_h = 6, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 565, .adv_w = 81, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 578, .adv_w = 82, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 591, .adv_w = 77, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 604, .adv_w = 87, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 617, .adv_w = 85, .box_w = 6, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 632, .adv_w = 119, .box_w = 8, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 652, .adv_w = 82, .box_w = 6, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 667, .adv_w = 78, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 680, .adv_w = 75, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 693, .adv_w = 36, .box_w = 3, .box_h = 7, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 704, .adv_w = 42, .box_w = 3, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 712, .adv_w = 36, .box_w = 2, .box_h = 7, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 719, .adv_w = 76, .box_w = 5, .box_h = 3, .ofs_x = 0, .ofs_y = 2},
    {.bitmap_index = 727, .adv_w = 64, .box_w = 4, .box_h = 1, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 729, .adv_w = 50, .box_w = 3, .box_h = 1, .ofs_x = 0, .ofs_y = 4},
    {.bitmap_index = 731, .adv_w = 69, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 739, .adv_w = 76, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 752, .adv_w = 72, .box_w = 5, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 762, .adv_w = 76, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 775, .adv_w = 73, .box_w = 5, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 785, .adv_w = 43, .box_w = 3, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 793, .adv_w = 76, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 806, .adv_w = 73, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 819, .adv_w = 29, .box_w = 2, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 824, .adv_w = 29, .box_w = 3, .box_h = 6, .ofs_x = -1, .ofs_y = -1},
    {.bitmap_index = 833, .adv_w = 67, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 846, .adv_w = 29, .box_w = 2, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 851, .adv_w = 110, .box_w = 7, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 865, .adv_w = 73, .box_w = 5, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 875, .adv_w = 77, .box_w = 5, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 885, .adv_w = 76, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 898, .adv_w = 76, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 911, .adv_w = 47, .box_w = 3, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 919, .adv_w = 67, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 927, .adv_w = 44, .box_w = 3, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 935, .adv_w = 72, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 943, .adv_w = 67, .box_w = 5, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 953, .adv_w = 94, .box_w = 6, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 965, .adv_w = 64, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 973, .adv_w = 67, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 986, .adv_w = 60, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 994, .adv_w = 37, .box_w = 3, .box_h = 7, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 1005, .adv_w = 29, .box_w = 2, .box_h = 7, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 1012, .adv_w = 37, .box_w = 3, .box_h = 7, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 1023, .adv_w = 76, .box_w = 5, .box_h = 2, .ofs_x = 0, .ofs_y = 2},
    {.bitmap_index = 1028, .adv_w = 86, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1049, .adv_w = 86, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1070, .adv_w = 86, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1091, .adv_w = 86, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1112, .adv_w = 86, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1133, .adv_w = 86, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1154, .adv_w = 133, .box_w = 9, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1177, .adv_w = 91, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 1198, .adv_w = 76, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1216, .adv_w = 76, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1234, .adv_w = 76, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1252, .adv_w = 76, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1270, .adv_w = 32, .box_w = 3, .box_h = 7, .ofs_x = -1, .ofs_y = 0},
    {.bitmap_index = 1281, .adv_w = 32, .box_w = 3, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1292, .adv_w = 32, .box_w = 4, .box_h = 7, .ofs_x = -1, .ofs_y = 0},
    {.bitmap_index = 1306, .adv_w = 32, .box_w = 4, .box_h = 7, .ofs_x = -1, .ofs_y = 0},
    {.bitmap_index = 1320, .adv_w = 93, .box_w = 6, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1335, .adv_w = 90, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1353, .adv_w = 98, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1374, .adv_w = 98, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1395, .adv_w = 98, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1416, .adv_w = 98, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1437, .adv_w = 98, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1458, .adv_w = 76, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1471, .adv_w = 98, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 1492, .adv_w = 87, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1510, .adv_w = 87, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1528, .adv_w = 87, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1546, .adv_w = 87, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1564, .adv_w = 78, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1582, .adv_w = 80, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1595, .adv_w = 74, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1608, .adv_w = 69, .box_w = 4, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1618, .adv_w = 69, .box_w = 4, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1628, .adv_w = 69, .box_w = 4, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1638, .adv_w = 69, .box_w = 4, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1648, .adv_w = 69, .box_w = 4, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1658, .adv_w = 69, .box_w = 4, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1670, .adv_w = 114, .box_w = 7, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1684, .adv_w = 72, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 1697, .adv_w = 73, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1710, .adv_w = 73, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1723, .adv_w = 73, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1736, .adv_w = 73, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1749, .adv_w = 29, .box_w = 3, .box_h = 5, .ofs_x = -1, .ofs_y = 0},
    {.bitmap_index = 1757, .adv_w = 29, .box_w = 3, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1765, .adv_w = 29, .box_w = 4, .box_h = 5, .ofs_x = -1, .ofs_y = 0},
    {.bitmap_index = 1775, .adv_w = 29, .box_w = 3, .box_h = 5, .ofs_x = -1, .ofs_y = 0},
    {.bitmap_index = 1783, .adv_w = 77, .box_w = 5, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1798, .adv_w = 73, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1811, .adv_w = 77, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1824, .adv_w = 77, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1837, .adv_w = 77, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1850, .adv_w = 77, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1863, .adv_w = 77, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1876, .adv_w = 76, .box_w = 5, .box_h = 4, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 1886, .adv_w = 77, .box_w = 5, .box_h = 6, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 1901, .adv_w = 72, .box_w = 4, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1911, .adv_w = 72, .box_w = 4, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1921, .adv_w = 72, .box_w = 4, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1931, .adv_w = 72, .box_w = 4, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1941, .adv_w = 67, .box_w = 5, .box_h = 6, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 1956, .adv_w = 76, .box_w = 5, .box_h = 6, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 1971, .adv_w = 67, .box_w = 5, .box_h = 6, .ofs_x = 0, .ofs_y = -1}
};

/*---------------------
 *  CHARACTER MAPPING
 *--------------------*/



/*Collect the unicode lists and glyph_id offsets*/
static const lv_font_fmt_txt_cmap_t cmaps[] =
{
    {
        .range_start = 32, .range_length = 95, .glyph_id_start = 1,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    },
    {
        .range_start = 192, .range_length = 64, .glyph_id_start = 96,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    }
};

/*-----------------
 *    KERNING
 *----------------*/


/*Map glyph_ids to kern left classes*/
static const uint8_t kern_left_class_mapping[] =
{
    0, 1, 0, 2, 3, 0, 4, 5,
    2, 6, 7, 8, 0, 9, 10, 9,
    11, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 12, 12, 0, 0, 0,
    0, 13, 14, 15, 16, 17, 18, 19,
    20, 21, 21, 22, 23, 24, 21, 21,
    17, 25, 17, 26, 27, 28, 29, 30,
    31, 32, 33, 34, 35, 36, 37, 0,
    0, 0, 38, 39, 40, 41, 42, 43,
    44, 45, 46, 47, 48, 49, 45, 45,
    39, 39, 50, 51, 52, 53, 54, 55,
    56, 57, 55, 58, 35, 0, 37, 0,
    14, 14, 14, 14, 14, 14, 18, 16,
    18, 18, 18, 18, 21, 21, 21, 21,
    17, 21, 17, 17, 17, 17, 17, 0,
    17, 29, 29, 29, 29, 33, 59, 60,
    38, 38, 38, 38, 38, 38, 42, 40,
    42, 42, 42, 42, 46, 61, 62, 63,
    64, 45, 39, 39, 39, 39, 39, 0,
    39, 54, 54, 54, 54, 55, 39, 55
};

/*Map glyph_ids to kern right classes*/
static const uint8_t kern_right_class_mapping[] =
{
    0, 1, 2, 3, 4, 0, 5, 6,
    3, 7, 8, 9, 0, 10, 11, 10,
    12, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 13, 13, 0, 0, 0,
    14, 15, 16, 17, 18, 17, 17, 17,
    18, 17, 17, 19, 17, 17, 17, 17,
    18, 17, 18, 17, 20, 21, 22, 23,
    24, 25, 26, 27, 28, 29, 30, 0,
    0, 0, 31, 32, 33, 33, 33, 34,
    33, 35, 36, 37, 38, 39, 40, 40,
    33, 40, 33, 40, 41, 42, 43, 44,
    45, 46, 44, 47, 28, 0, 30, 0,
    16, 16, 16, 16, 16, 16, 48, 18,
    17, 17, 17, 17, 17, 17, 17, 17,
    17, 17, 18, 18, 18, 18, 18, 0,
    18, 22, 22, 22, 22, 26, 17, 49,
    31, 31, 31, 50, 51, 31, 31, 33,
    33, 33, 33, 33, 52, 36, 53, 54,
    55, 40, 33, 33, 33, 33, 33, 0,
    33, 43, 43, 43, 43, 44, 39, 44
};

/*Kern values between classes*/
static const int8_t kern_class_values[] =
{
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -1, 0, -2, 0,
    -2, -3, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -2, 0, -1, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, -7, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 1, 2, 0, 0, 0, 0, -2,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    2, 0, 0, 0, -2, -6, 2, 2,
    1, 0, -6, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -2, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 2, 0, 0, 0, 0,
    -10, 0, -9, -5, 2, -12, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -1,
    0, 2, 2, 3, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, -1, 0,
    0, -2, 4, 0, -1, -1, 1, -1,
    0, -2, 0, 0, -2, -2, 0, 3,
    0, 3, 3, 2, 3, 1, 0, 0,
    4, 0, 0, -2, 0, 0, 0, 9,
    0, 0, 0, -2, 0, -2, 0, 0,
    0, 0, 1, 0, 0, 0, 0, 0,
    0, -3, 0, 0, 0, 0, 0, 0,
    0, -2, -2, -2, 0, 0, -1, 0,
    0, 0, 0, 0, 0, 0, -2, 0,
    -3, -1, 0, -3, -1, 0, 0, -2,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -1,
    0, 1, -5, -1, -5, -1, 2, -3,
    -10, 0, -2, -11, 0, 0, 0, 0,
    0, -2, -1, -2, 0, 0, -3, -1,
    0, -4, 0, 0, 0, 0, 0, 0,
    0, -2, 0, -1, 1, 1, 0, 1,
    -18, 0, -1, -1, 0, 0, 0, -6,
    0, 0, 0, 0, -9, 0, -2, -1,
    -5, 0, -2, 1, 0, -7, 0, 3,
    0, -7, 0, -3, -10, -6, -11, -6,
    2, -10, 1, 0, 0, -3, -1, 0,
    -5, 0, 0, 0, 0, 0, 0, 0,
    -2, -1, -2, -9, -6, 0, 2, 2,
    0, -1, -1, 0, 0, 0, -5, 0,
    0, 0, 0, 0, 0, 1, -1, -1,
    -2, 1, 0, 0, -5, 1, -3, 0,
    2, -2, -1, -10, 0, -8, -3, -9,
    -12, -5, 0, 0, -2, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -1, 0, -5, -5, -7, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -1, 0, 0, -2, 4, 0, -8,
    -2, -4, -1, 0, -4, -10, 0, -3,
    -8, 0, 4, 0, 4, 4, 2, 3,
    2, 0, 0, 5, -6, 0, -9, 0,
    0, 0, 0, 0, 0, -5, -6, 0,
    -5, 0, 0, -3, -1, -15, -3, -6,
    -6, 0, 0, 0, -6, 0, 0, 0,
    0, 0, 0, -1, -2, -1, 0, -1,
    1, -1, -1, 0, 0, 0, 0, 0,
    -1, -10, 0, -4, -1, 0, -7, 0,
    0, 0, -4, 0, 0, -2, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    2, 2, 0, 0, 1, 0, 0, 0,
    0, 0, 0, -3, 0, 0, 0, 2,
    0, 0, 0, -2, 0, 0, 3, 0,
    0, -2, 0, -3, 0, 0, 2, 0,
    -10, 0, -4, -2, -3, -9, -2, 0,
    0, -2, 0, 0, 0, 1, 0, 0,
    0, 0, 0, 0, 0, 1, 0, 0,
    1, -1, 0, -2, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -7, 0, 0,
    0, 0, 0, -10, 3, -3, 5, 0,
    -8, 0, 4, 0, -3, 0, 0, -12,
    -4, -11, -5, 3, -11, 2, 0, -11,
    0, 0, 0, -1, 0, 0, 0, 0,
    0, 0, 0, 0, -2, 0, -5, -3,
    2, 3, 3, 0, 0, 0, 0, 0,
    0, -2, 0, 0, 0, 0, 0, 0,
    0, 0, -2, -2, 1, 0, 0, -1,
    0, -1, 0, -1, 1, 0, -1, 0,
    -2, 0, -2, -3, 0, 0, -2, -1,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -1,
    0, -3, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -2, -1, -5, -1, 0, 0, 0, 0,
    -2, 0, -1, 0, 0, -3, 0, -3,
    -1, -3, -4, -3, 0, -2, -3, 0,
    0, -1, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 1, 1, 0, 0,
    -5, 0, 0, 0, 0, 0, 0, -1,
    0, 0, 0, 0, 0, 0, 0, -2,
    -2, -7, 2, -2, 0, 0, 0, -3,
    0, 0, 0, -1, -5, 0, -3, -1,
    -5, -5, -3, 0, -3, -2, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -3, 0, -4,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -1, 0, -3,
    0, -1, 2, 0, 0, 0, 2, 0,
    -1, 0, -1, 0, -1, 0, 0, 1,
    0, 0, 0, 0, 2, 0, 0, -2,
    -1, 0, 0, 0, 0, 0, 0, 0,
    -1, 0, -1, 0, 0, 1, 1, 0,
    0, 0, 0, 0, 0, -2, 0, 0,
    0, 0, 0, -4, 0, 2, -2, -10,
    0, -6, -2, 0, -3, -9, 0, -1,
    -9, -2, 1, 0, 1, 1, 0, 1,
    -1, 0, 0, 2, -1, 0, -2, 0,
    0, 0, 0, 0, 0, 0, -2, 0,
    -1, -1, 0, -5, -2, -15, -2, -1,
    -1, 0, 0, 0, -2, 0, 0, 0,
    0, 0, 0, 0, 0, -2, 0, 2,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -3, 0, -3, -1, 0, -4, 0,
    0, -2, -1, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -1, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -3, 0, -1, 0,
    0, 0, -2, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -3, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -2,
    0, 1, -3, 2, -7, 4, 0, -3,
    -1, 2, 0, -6, -1, -2, 0, -1,
    0, 0, 2, 0, 1, 0, 0, 2,
    0, 0, -4, 0, 0, 0, 0, 0,
    0, 0, 0, -2, -1, -7, -4, 0,
    3, 2, 0, 0, 0, 4, 0, 4,
    -5, -2, 0, 0, 0, 0, -1, 0,
    0, -15, 0, -10, 4, 0, -12, 0,
    2, 0, -6, 0, 0, -11, -2, -13,
    -8, 2, -17, 2, 0, -13, 0, 1,
    0, -1, 0, 0, 0, 0, 0, 0,
    0, 0, -1, 0, -6, -3, 1, 3,
    2, 0, 1, 1, 0, 0, 0, -3,
    0, 0, 0, 0, 0, -7, 0, -1,
    0, -13, -3, -9, 0, 0, -6, -10,
    -1, -1, -12, 0, -1, 0, -1, -1,
    -4, -2, -3, 0, 0, 0, -4, -1,
    -4, 1, -1, -1, -1, -1, -1, -1,
    -2, 1, -1, 2, 2, -1, 0, -20,
    -1, -4, -4, -1, -1, -1, -5, 0,
    0, 0, 0, 0, -1, 0, 0, -1,
    0, -3, 3, 0, 0, -1, 2, 0,
    -1, 0, 0, -2, 0, -1, -1, 1,
    -2, 1, 0, 0, -1, -1, 0, -3,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 2, 1, 0, 2, 2, 0,
    -1, -1, 0, 0, 0, -4, 0, 0,
    0, 0, 0, 0, 0, -1, 0, -3,
    0, 0, -1, 0, 0, 0, 0, -1,
    0, 0, -1, 0, -1, -1, -2, -2,
    -1, 0, -2, -1, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -2, 0, -3, 0, 0,
    0, 0, 0, 0, 0, -1, 0, 0,
    0, 0, -8, -2, 3, 0, -10, -10,
    -12, -10, 1, -13, -12, 0, -5, -12,
    0, 4, 0, 3, 3, 1, 2, 2,
    0, 5, 4, -14, 0, -16, -2, 0,
    0, 0, 0, 0, -14, -15, -2, -14,
    -10, -10, -13, -12, -19, -1, -7, -10,
    9, 6, 9, -7, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -6, 0, -3,
    0, 0, 0, -4, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -7, 0, 0, 0, 0,
    0, 0, 0, -2, 0, 0, 0, 0,
    -7, -3, 3, 0, -11, -8, -9, -4,
    0, -7, -11, 0, -3, -9, 0, 3,
    0, 3, 3, 1, 2, 1, 0, 5,
    4, -7, 0, -8, 0, 0, 0, 0,
    0, 0, -3, -5, 0, -3, 0, 0,
    -2, -1, -17, -2, -7, -7, 8, 6,
    7, -7, 0, 0, 0, 0, 0, -3,
    0, 3, 0, -6, -3, -5, -1, 1,
    -4, -5, 0, -1, -5, 0, 3, 0,
    3, 4, 2, 4, 1, 0, 5, 4,
    -3, 0, -4, 0, 0, 0, 0, 0,
    0, -1, -2, 0, -2, 1, 0, -1,
    0, -12, 0, -3, -3, 8, 8, 8,
    -5, -2, 0, 0, 0, 0, -2, 0,
    2, -2, 2, -9, 5, 0, -2, 0,
    3, 0, -5, 0, -2, 1, 0, 1,
    2, 4, 1, 3, 0, 0, 3, 0,
    0, -3, 0, 0, 0, 0, 0, 0,
    0, -2, -2, -1, -4, -3, 0, 3,
    3, 0, 0, 0, 5, 0, 4, -3,
    -3, 0, 0, 0, 0, -8, -3, 3,
    -1, -10, -12, -10, -7, 0, -11, -11,
    0, -5, -12, -2, 2, 0, 2, 4,
    1, 4, 1, 0, 3, 4, -12, 0,
    -14, -2, 0, 0, 0, 0, 0, -8,
    -11, -2, -8, -5, -5, -7, -6, -17,
    -3, -12, -12, 8, 3, 5, -8, 0,
    0, 0, 0, 0, -2, 0, 2, 0,
    0, -9, 2, 0, 0, 0, 2, 0,
    -3, 0, -1, 2, 0, 1, 1, 2,
    1, 2, 0, 2, 3, 0, 0, -2,
    -1, 0, 0, 0, 0, 0, 0, 0,
    -1, 0, -2, -3, 0, 2, 2, 0,
    0, 0, 6, 4, 6, -2, 0, 0,
    0, -2, 0, 0, -2, 4, -3, -3,
    -2, 0, -4, 0, -1, 0, 0, -2,
    -2, 0, 4, 0, 4, 4, 3, 4,
    2, 0, 0, 4, 0, 0, -3, -2,
    0, 0, 12, 0, 0, 0, -2, -2,
    0, 0, 0, -1, 0, 0, -1, 0,
    0, 0, 0, 0, -3, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 5, 0, -2, 0,
    0, -12, -1, -9, -6, 3, -12, 2,
    0, 0, 0, 2, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -2, 0, 3, 3, 5, 0, 2, 2,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    5, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -2, 0, 0, 0, 0,
    -5, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -6,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -2, -4, -5, 0, 0, -2, -6,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, -7, -3,
    0, 0, 0, -1, 0, 0, 0, 0,
    0, 0, 0, -1, 0, -1, 0, -4,
    -1, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -2, -3, -3, -2, 0, -1, -5, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -7, -2, 0,
    0, -1, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, -3, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 1, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 3, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, -1, -3,
    -3, 0, 0, -1, -5, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -6, -2, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -2, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 1,
    0, 0, 0, -3, 0, 6, 1, -5,
    -4, -3, 0, 2, -2, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 1, 6, 6, -2, 2, -3, 0,
    2, 1, 2, 2, 2, 0, 0, 2,
    0, 4, 4, 2, 2, 0, 0, -2,
    -2, 11, 4, 9, -5, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    1, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -5, 0, 0, 0, 0, 0, 0,
    0, 4, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -3, 0, 0, 0,
    0, -5, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -8, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -1,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 5, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 3,
    0, 0, 0, 0, 0, 0, -2, 0,
    0, 0, 1, -6, 5, 1, 0, -1,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, -1, 0,
    0, -3, 2, 0, 0, 0, 0, 0,
    0, -2, 0, -1, 1, 1, 1, 3,
    0, 0, 0, 0, 0, 0, 0, -4,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 4, 0, 2, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 1, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -5, 0, 0, 0, 0,
    0, 0, 0, 6, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -3, 0, 0, 1, -7,
    -6, -5, 0, 2, -2, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -1, 0, -4, 3,
    0, 0, 0, 0, 0, 0, -1, 3,
    0, 3, 3, 1, 2, 0, 0, -1,
    -1, 0, 0, 0, -6, 0, 0, 0,
    0, 0, 0, 0, -2, -3, -2, -1,
    0, 0, -4, -1, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -7, -2, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -1, 0, -1, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, -5, 4,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -1, -1, 0, 0, -2, 3, 0, 0,
    0, 0, 0, 0, 0, 0, -1, 2,
    2, 3, 3, 0, 0, 0, 0, 0,
    0, 0, -1, 0, 0, 0, 0, 0,
    0, 0, -1, 0, 0, 0, 0, 0,
    -1, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -5,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -2, 0, 0, 0, 0, 0,
    0, 0, 1, -9, -1, -2, 2, 1,
    -1, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -1, 2, 0, 0, 0, 0,
    0, 0, 0, 2, 0, 3, 3, 2,
    3, 0, 0, 0, 0, 0, 0, 0,
    -2, 0, 0, 0, 0, 0, 0, 0,
    0, 1, -6, 0, 0, 2, 1, -1,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 4, 0, 0, 0, 0, 0,
    0, 0, 3, 0, 3, 4, 3, 3,
    0, 0, 0, 0, 0, 0, 0, -1,
    -1, 0, 0, 0, 0, -2, 0, 0,
    0, 0, -5, 5, 0, 0, -1, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -3, -1, 0, 0,
    -4, 2, 0, 0, 0, 0, 0, 0,
    -1, 2, 0, 2, 3, 0, 2, 0,
    0, 0, 0, 0, 0, 0, -3, 0,
    0, 0, 0, 0, -1, 0, 0, 1,
    0, -3, 4, 0, 0, 1, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -2, 0, 1, 0, -2,
    2, 0, 0, 0, 0, 0, 0, 0,
    2, 0, 3, 3, 1, 3, 0, 0,
    1, 1, 0, 0, 0, -1, 0, 0,
    0, 0, 0, -1, 0, -3, -1, -10,
    1, -4, 0, -2, 0, -5, -1, 0,
    -1, -1, -8, 0, -4, -2, -6, -7,
    -8, 0, -6, -3, -1, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -2, -1, -7, 0, -1,
    -1, 0, 0, 0, -1, 0, 0, 0,
    0, 0, 0, 0, 0, -4, -2, 0,
    0, 0, -2, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -7, -2, 0, 0, 0, -1, 0,
    0, 0, 0, 0, 0, 0, -1, 0,
    -3, -1, -1, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 3, 0, 0, 2, 0,
    0, 2, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 3, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 3, 0, 0, 2, 0, 0,
    2, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 2, 0, 0, 2, 0, 0, 2,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 3,
    0, 0, 0, 0, 0, 0, 0, 0,
    -2, -1, -5, 0, 0, -2, -2, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -3, -2, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, -1, 0,
    0, 0, 0, 0, 0, 0, 0, 0
};


/*Collect the kern class' data in one place*/
static const lv_font_fmt_txt_kern_classes_t kern_classes =
{
    .class_pair_values   = kern_class_values,
    .left_class_mapping  = kern_left_class_mapping,
    .right_class_mapping = kern_right_class_mapping,
    .left_class_cnt      = 64,
    .right_class_cnt     = 55,
};

/*--------------------
 *  ALL CUSTOM DATA
 *--------------------*/

#if LVGL_VERSION_MAJOR == 8
/*Store all the custom data of the font*/
static  lv_font_fmt_txt_glyph_cache_t cache;
#endif

#if LVGL_VERSION_MAJOR >= 8
static const lv_font_fmt_txt_dsc_t font_dsc = {
#else
static lv_font_fmt_txt_dsc_t font_dsc = {
#endif
    .glyph_bitmap = glyph_bitmap,
    .glyph_dsc = glyph_dsc,
    .cmaps = cmaps,
    .kern_dsc = &kern_classes,
    .kern_scale = 16,
    .cmap_num = 2,
    .bpp = 4,
    .kern_classes = 1,
    .bitmap_format = 0,
#if LVGL_VERSION_MAJOR == 8
    .cache = &cache
#endif
};



/*-----------------
 *  PUBLIC FONT
 *----------------*/

/*Initialize a public general font descriptor*/
#if LVGL_VERSION_MAJOR >= 8
const lv_font_t aktivgrotesk_regular_8pt = {
#else
lv_font_t aktivgrotesk_regular_8pt = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 9,          /*The maximum line height required by the font*/
    .base_line = 2,             /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_NONE,
#endif
#if LV_VERSION_CHECK(7, 4, 0) || LVGL_VERSION_MAJOR >= 8
    .underline_position = -1,
    .underline_thickness = 1,
#endif
    .dsc = &font_dsc,          /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
#if LV_VERSION_CHECK(8, 2, 0) || LVGL_VERSION_MAJOR >= 9
    .fallback = NULL,
#endif
    .user_data = NULL,
};



#endif /*#if AKTIVGROTESK_REGULAR_8PT*/

