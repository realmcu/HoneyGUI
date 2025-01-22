/*******************************************************************************
 * Size: 12 px
 * Bpp: 1
 * Opts: --bpp 1 --size 12 --no-compress --font HarmonyOS_Sans_SC_Regular.ttf --range 32-126,176 --format lvgl -o SourceHanSansSC_size12_bits1_font.c
 ******************************************************************************/

#include "app_main.h"

#ifndef SOURCEHANSANSSC_SIZE12_BITS1_FONT
#define SOURCEHANSANSSC_SIZE12_BITS1_FONT 1
#endif

#if SOURCEHANSANSSC_SIZE12_BITS1_FONT

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] =
{
    /* U+0020 " " */
    0x0,

    /* U+0021 "!" */
    0xfe, 0x40,

    /* U+0022 "\"" */
    0xb6, 0x80,

    /* U+0023 "#" */
    0x12, 0x12, 0x12, 0x7f, 0x24, 0x24, 0x24, 0xfe,
    0x48, 0x48,

    /* U+0024 "$" */
    0x21, 0x1d, 0xfa, 0xd3, 0x86, 0x2d, 0x6a, 0xe2,
    0x10,

    /* U+0025 "%" */
    0x62, 0x49, 0x25, 0x12, 0x86, 0x80, 0x40, 0x4c,
    0x29, 0x24, 0x91, 0x80,

    /* U+0026 "&" */
    0x38, 0x44, 0x44, 0x4c, 0x38, 0x72, 0x8a, 0x86,
    0x86, 0x79,

    /* U+0027 "'" */
    0xe0,

    /* U+0028 "(" */
    0x29, 0x49, 0x24, 0x89, 0x10,

    /* U+0029 ")" */
    0x89, 0x12, 0x49, 0x29, 0x40,

    /* U+002A "*" */
    0x23, 0xc8, 0xf2, 0x0,

    /* U+002B "+" */
    0x20, 0x8f, 0xc8, 0x20, 0x80,

    /* U+002C "," */
    0xd8,

    /* U+002D "-" */
    0xf0,

    /* U+002E "." */
    0x80,

    /* U+002F "/" */
    0x10, 0x84, 0x42, 0x11, 0x8, 0x44, 0x0,

    /* U+0030 "0" */
    0x79, 0x28, 0x61, 0x86, 0x18, 0x61, 0x49, 0xe0,

    /* U+0031 "1" */
    0x37, 0x91, 0x11, 0x11, 0x11,

    /* U+0032 "2" */
    0x73, 0x28, 0x82, 0x18, 0x42, 0x18, 0x43, 0xf0,

    /* U+0033 "3" */
    0x76, 0x42, 0x13, 0x4, 0x21, 0xcb, 0x80,

    /* U+0034 "4" */
    0x10, 0x42, 0x8, 0x49, 0x2c, 0xbf, 0x8, 0x20,

    /* U+0035 "5" */
    0x7c, 0x21, 0xf, 0x4c, 0x21, 0x9b, 0x80,

    /* U+0036 "6" */
    0x10, 0xc2, 0x18, 0x7b, 0x38, 0x61, 0xcd, 0xe0,

    /* U+0037 "7" */
    0xf8, 0x44, 0x21, 0x10, 0x84, 0x62, 0x0,

    /* U+0038 "8" */
    0x72, 0x28, 0xa2, 0x73, 0x28, 0x61, 0xcd, 0xe0,

    /* U+0039 "9" */
    0x7b, 0x38, 0x61, 0xcd, 0xe1, 0x84, 0x30, 0x80,

    /* U+003A ":" */
    0x82,

    /* U+003B ";" */
    0xc0, 0xd, 0x80,

    /* U+003C "<" */
    0x4, 0x66, 0x30, 0x30, 0x30,

    /* U+003D "=" */
    0xfc, 0x0, 0x3f,

    /* U+003E ">" */
    0x81, 0x81, 0x83, 0x73, 0x0,

    /* U+003F "?" */
    0x76, 0x42, 0x11, 0x10, 0x84, 0x1, 0x0,

    /* U+0040 "@" */
    0x1f, 0x6, 0x11, 0x1, 0x67, 0x99, 0x13, 0x22,
    0x64, 0x4c, 0x89, 0xce, 0xc8, 0x0, 0x84, 0xf,
    0x80,

    /* U+0041 "A" */
    0x18, 0x18, 0x18, 0x24, 0x24, 0x24, 0x7e, 0x42,
    0x42, 0x81,

    /* U+0042 "B" */
    0xfa, 0x38, 0x61, 0x8f, 0xe8, 0x61, 0x87, 0xe0,

    /* U+0043 "C" */
    0x3c, 0x8f, 0x4, 0x8, 0x10, 0x20, 0x60, 0x46,
    0x78,

    /* U+0044 "D" */
    0xf9, 0x1a, 0x14, 0x18, 0x30, 0x60, 0xc2, 0x8d,
    0xf0,

    /* U+0045 "E" */
    0xfc, 0x21, 0x8, 0x7e, 0x10, 0x87, 0xc0,

    /* U+0046 "F" */
    0xfc, 0x21, 0x8, 0x7e, 0x10, 0x84, 0x0,

    /* U+0047 "G" */
    0x3c, 0x8f, 0x4, 0x8, 0x11, 0xe0, 0xe1, 0x62,
    0x78,

    /* U+0048 "H" */
    0x83, 0x6, 0xc, 0x1f, 0xf0, 0x60, 0xc1, 0x83,
    0x4,

    /* U+0049 "I" */
    0xff, 0xc0,

    /* U+004A "J" */
    0x11, 0x11, 0x11, 0x11, 0x9e,

    /* U+004B "K" */
    0x85, 0x12, 0x65, 0x8a, 0x1a, 0x26, 0x44, 0x85,
    0xc,

    /* U+004C "L" */
    0x84, 0x21, 0x8, 0x42, 0x10, 0x87, 0xc0,

    /* U+004D "M" */
    0x81, 0xc3, 0xc3, 0xa5, 0xa5, 0x99, 0x99, 0x81,
    0x81, 0x81,

    /* U+004E "N" */
    0x83, 0x87, 0x8d, 0x19, 0x32, 0x62, 0xc7, 0x87,
    0x4,

    /* U+004F "O" */
    0x3c, 0x42, 0xc3, 0x81, 0x81, 0x81, 0x81, 0xc3,
    0x42, 0x3c,

    /* U+0050 "P" */
    0xfa, 0x38, 0x61, 0x8f, 0xe8, 0x20, 0x82, 0x0,

    /* U+0051 "Q" */
    0x3c, 0x21, 0x30, 0xd0, 0x28, 0x14, 0xa, 0x5,
    0x86, 0x42, 0x1e, 0x1, 0x80, 0x20,

    /* U+0052 "R" */
    0xf9, 0x1a, 0x14, 0x28, 0xdf, 0x26, 0x44, 0x85,
    0xc,

    /* U+0053 "S" */
    0x7a, 0x38, 0x20, 0x70, 0x60, 0x61, 0x85, 0xe0,

    /* U+0054 "T" */
    0xfe, 0x20, 0x40, 0x81, 0x2, 0x4, 0x8, 0x10,
    0x20,

    /* U+0055 "U" */
    0x83, 0x6, 0xc, 0x18, 0x30, 0x60, 0xc1, 0x44,
    0x70,

    /* U+0056 "V" */
    0x81, 0x42, 0x42, 0x42, 0x24, 0x24, 0x24, 0x18,
    0x18, 0x18,

    /* U+0057 "W" */
    0x84, 0x28, 0xc5, 0x18, 0xa5, 0x14, 0xb4, 0x52,
    0x8a, 0x51, 0x8c, 0x30, 0x82, 0x10,

    /* U+0058 "X" */
    0x42, 0x46, 0x24, 0x38, 0x18, 0x18, 0x3c, 0x24,
    0x42, 0xc2,

    /* U+0059 "Y" */
    0x82, 0x89, 0x91, 0x43, 0x82, 0x4, 0x8, 0x10,
    0x20,

    /* U+005A "Z" */
    0xfc, 0x20, 0x84, 0x10, 0x82, 0x10, 0x83, 0xf0,

    /* U+005B "[" */
    0xf2, 0x49, 0x24, 0x92, 0x70,

    /* U+005C "\\" */
    0x82, 0x10, 0x82, 0x10, 0x82, 0x10, 0x80,

    /* U+005D "]" */
    0xe4, 0x92, 0x49, 0x24, 0xf0,

    /* U+005E "^" */
    0x21, 0x94, 0x94, 0x80,

    /* U+005F "_" */
    0xf8,

    /* U+0060 "`" */
    0x90,

    /* U+0061 "a" */
    0x74, 0x42, 0xf8, 0xc5, 0xe0,

    /* U+0062 "b" */
    0x82, 0x8, 0x2e, 0xce, 0x18, 0x61, 0xce, 0xe0,

    /* U+0063 "c" */
    0x76, 0x61, 0x8, 0x65, 0xc0,

    /* U+0064 "d" */
    0x4, 0x10, 0x5d, 0xce, 0x18, 0x61, 0xcd, 0xd0,

    /* U+0065 "e" */
    0x7b, 0x38, 0x7f, 0x81, 0x17, 0x80,

    /* U+0066 "f" */
    0x34, 0x4f, 0x44, 0x44, 0x44,

    /* U+0067 "g" */
    0x77, 0x38, 0x61, 0x87, 0x37, 0x41, 0x8d, 0xe0,

    /* U+0068 "h" */
    0x84, 0x21, 0x6c, 0xc6, 0x31, 0x8c, 0x40,

    /* U+0069 "i" */
    0x9f, 0xc0,

    /* U+006A "j" */
    0x20, 0x12, 0x49, 0x24, 0x9c,

    /* U+006B "k" */
    0x84, 0x21, 0x39, 0x53, 0x9a, 0x94, 0x40,

    /* U+006C "l" */
    0xff, 0xc0,

    /* U+006D "m" */
    0xb7, 0x66, 0x62, 0x31, 0x18, 0x8c, 0x46, 0x22,

    /* U+006E "n" */
    0xb6, 0x63, 0x18, 0xc6, 0x20,

    /* U+006F "o" */
    0x79, 0x28, 0x61, 0x85, 0x27, 0x80,

    /* U+0070 "p" */
    0xbb, 0x38, 0x61, 0x87, 0x3b, 0xa0, 0x82, 0x0,

    /* U+0071 "q" */
    0x77, 0x38, 0x61, 0x87, 0x37, 0x41, 0x4, 0x10,

    /* U+0072 "r" */
    0xec, 0x88, 0x88, 0x80,

    /* U+0073 "s" */
    0x74, 0x60, 0xe0, 0xc5, 0xc0,

    /* U+0074 "t" */
    0x44, 0xf4, 0x44, 0x44, 0x30,

    /* U+0075 "u" */
    0x8c, 0x63, 0x18, 0xcd, 0xa0,

    /* U+0076 "v" */
    0x85, 0x34, 0x9a, 0x30, 0xc1, 0x0,

    /* U+0077 "w" */
    0x88, 0xa6, 0x53, 0x4a, 0xa3, 0x31, 0x98, 0x88,

    /* U+0078 "x" */
    0xcd, 0x23, 0xc, 0x31, 0x2c, 0xc0,

    /* U+0079 "y" */
    0x85, 0x14, 0x92, 0x30, 0xc3, 0x8, 0x23, 0x0,

    /* U+007A "z" */
    0xf8, 0x84, 0x44, 0x23, 0xe0,

    /* U+007B "{" */
    0x39, 0x8, 0x42, 0x60, 0x84, 0x21, 0x8, 0x30,

    /* U+007C "|" */
    0xff, 0xf0,

    /* U+007D "}" */
    0xc1, 0x8, 0x42, 0xc, 0x84, 0x21, 0x9, 0x80,

    /* U+007E "~" */
    0x6a, 0x60,

    /* U+00B0 "°" */
    0x69, 0x96
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] =
{
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 52, .box_w = 1, .box_h = 1, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1, .adv_w = 45, .box_w = 1, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 3, .adv_w = 66, .box_w = 3, .box_h = 3, .ofs_x = 1, .ofs_y = 7},
    {.bitmap_index = 5, .adv_w = 124, .box_w = 8, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 15, .adv_w = 109, .box_w = 5, .box_h = 14, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 24, .adv_w = 148, .box_w = 9, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 36, .adv_w = 136, .box_w = 8, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 46, .adv_w = 36, .box_w = 1, .box_h = 3, .ofs_x = 1, .ofs_y = 7},
    {.bitmap_index = 47, .adv_w = 66, .box_w = 3, .box_h = 12, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 52, .adv_w = 66, .box_w = 3, .box_h = 12, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 57, .adv_w = 83, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 61, .adv_w = 109, .box_w = 6, .box_h = 6, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 66, .adv_w = 46, .box_w = 2, .box_h = 3, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 67, .adv_w = 94, .box_w = 4, .box_h = 1, .ofs_x = 1, .ofs_y = 3},
    {.bitmap_index = 68, .adv_w = 44, .box_w = 1, .box_h = 1, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 69, .adv_w = 75, .box_w = 5, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 76, .adv_w = 109, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 84, .adv_w = 109, .box_w = 4, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 89, .adv_w = 109, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 97, .adv_w = 109, .box_w = 5, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 104, .adv_w = 109, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 112, .adv_w = 109, .box_w = 5, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 119, .adv_w = 109, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 127, .adv_w = 109, .box_w = 5, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 134, .adv_w = 109, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 142, .adv_w = 109, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 150, .adv_w = 49, .box_w = 1, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 151, .adv_w = 51, .box_w = 2, .box_h = 9, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 154, .adv_w = 109, .box_w = 6, .box_h = 6, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 159, .adv_w = 109, .box_w = 6, .box_h = 4, .ofs_x = 1, .ofs_y = 3},
    {.bitmap_index = 162, .adv_w = 109, .box_w = 6, .box_h = 6, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 167, .adv_w = 83, .box_w = 5, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 174, .adv_w = 187, .box_w = 11, .box_h = 12, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 191, .adv_w = 127, .box_w = 8, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 201, .adv_w = 125, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 209, .adv_w = 125, .box_w = 7, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 218, .adv_w = 137, .box_w = 7, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 227, .adv_w = 114, .box_w = 5, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 234, .adv_w = 108, .box_w = 5, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 241, .adv_w = 134, .box_w = 7, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 250, .adv_w = 143, .box_w = 7, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 259, .adv_w = 50, .box_w = 1, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 261, .adv_w = 88, .box_w = 4, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 266, .adv_w = 130, .box_w = 7, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 275, .adv_w = 109, .box_w = 5, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 282, .adv_w = 166, .box_w = 8, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 292, .adv_w = 142, .box_w = 7, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 301, .adv_w = 146, .box_w = 8, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 311, .adv_w = 114, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 319, .adv_w = 146, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 333, .adv_w = 123, .box_w = 7, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 342, .adv_w = 110, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 350, .adv_w = 109, .box_w = 7, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 359, .adv_w = 140, .box_w = 7, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 368, .adv_w = 126, .box_w = 8, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 378, .adv_w = 185, .box_w = 11, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 392, .adv_w = 126, .box_w = 8, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 402, .adv_w = 118, .box_w = 7, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 411, .adv_w = 111, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 419, .adv_w = 66, .box_w = 3, .box_h = 12, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 424, .adv_w = 75, .box_w = 5, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 431, .adv_w = 66, .box_w = 3, .box_h = 12, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 436, .adv_w = 91, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 440, .adv_w = 79, .box_w = 5, .box_h = 1, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 441, .adv_w = 58, .box_w = 2, .box_h = 2, .ofs_x = 1, .ofs_y = 8},
    {.bitmap_index = 442, .adv_w = 105, .box_w = 5, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 447, .adv_w = 117, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 455, .adv_w = 95, .box_w = 5, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 460, .adv_w = 117, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 468, .adv_w = 105, .box_w = 6, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 474, .adv_w = 66, .box_w = 4, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 479, .adv_w = 117, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 487, .adv_w = 111, .box_w = 5, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 494, .adv_w = 46, .box_w = 1, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 496, .adv_w = 46, .box_w = 3, .box_h = 13, .ofs_x = -1, .ofs_y = -3},
    {.bitmap_index = 501, .adv_w = 100, .box_w = 5, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 508, .adv_w = 46, .box_w = 1, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 510, .adv_w = 164, .box_w = 9, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 518, .adv_w = 111, .box_w = 5, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 523, .adv_w = 113, .box_w = 6, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 529, .adv_w = 117, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 537, .adv_w = 117, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 545, .adv_w = 72, .box_w = 4, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 549, .adv_w = 87, .box_w = 5, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 554, .adv_w = 71, .box_w = 4, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 559, .adv_w = 112, .box_w = 5, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 564, .adv_w = 99, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 570, .adv_w = 150, .box_w = 9, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 578, .adv_w = 95, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 584, .adv_w = 100, .box_w = 6, .box_h = 10, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 592, .adv_w = 91, .box_w = 5, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 597, .adv_w = 71, .box_w = 5, .box_h = 12, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 605, .adv_w = 35, .box_w = 1, .box_h = 12, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 607, .adv_w = 71, .box_w = 5, .box_h = 12, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 615, .adv_w = 109, .box_w = 6, .box_h = 2, .ofs_x = 1, .ofs_y = 4},
    {.bitmap_index = 617, .adv_w = 58, .box_w = 4, .box_h = 4, .ofs_x = 1, .ofs_y = 7}
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
        .range_start = 176, .range_length = 1, .glyph_id_start = 96,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    }
};

/*-----------------
 *    KERNING
 *----------------*/


/*Map glyph_ids to kern left classes*/
static const uint8_t kern_left_class_mapping[] =
{
    0, 0, 0, 1, 0, 0, 0, 0,
    1, 2, 3, 0, 4, 0, 4, 0,
    5, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 4,
    6, 7, 8, 9, 10, 7, 11, 12,
    13, 14, 14, 15, 16, 17, 14, 14,
    7, 18, 0, 19, 20, 21, 15, 5,
    22, 23, 24, 25, 2, 8, 3, 0,
    0, 0, 26, 27, 28, 29, 30, 31,
    32, 26, 0, 33, 34, 29, 26, 26,
    27, 27, 0, 35, 36, 37, 32, 38,
    38, 39, 38, 40, 2, 0, 3, 4,
    0
};

/*Map glyph_ids to kern right classes*/
static const uint8_t kern_right_class_mapping[] =
{
    0, 1, 0, 2, 0, 0, 0, 0,
    2, 0, 3, 0, 4, 5, 4, 5,
    6, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 4, 0, 0,
    7, 8, 6, 9, 8, 9, 9, 9,
    8, 9, 9, 10, 9, 9, 9, 9,
    8, 9, 8, 9, 11, 12, 13, 14,
    15, 16, 17, 18, 0, 14, 3, 0,
    5, 0, 19, 20, 21, 21, 21, 22,
    21, 20, 0, 23, 20, 20, 24, 24,
    21, 0, 21, 24, 25, 26, 27, 28,
    28, 29, 28, 30, 0, 0, 3, 4,
    0
};

/*Kern values between classes*/
static const int8_t kern_class_values[] =
{
    0, 0, 0, 0, 0, 0, 3, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 4, 0, 4, 4, 2,
    0, 3, 0, 0, 13, 0, 0, 4,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 6, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -10, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -8, 4, 4, -8, -26, -16, 4, -6,
    0, -22, -1, 4, 0, 0, 0, 0,
    0, 0, -13, 0, -13, -4, 0, -8,
    -10, 0, -8, -8, -10, -10, 0, 0,
    0, -6, -18, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -5, 0, 0,
    -8, -6, 0, 0, 0, -7, 0, -6,
    0, -6, -3, -6, -10, -5, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -8, -23, 0, -12, 6, 0,
    -13, -6, 0, 0, 0, -15, -2, -16,
    -12, 0, -20, 4, 0, 0, -2, 0,
    0, 0, 0, 0, 0, -7, 0, 0,
    0, -2, 0, 0, 0, -2, 0, 0,
    0, 3, 0, -7, 0, -8, -2, 0,
    -10, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 2,
    0, -6, 5, 0, 6, -3, 0, 0,
    0, 1, 0, 0, 0, 0, 1, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -4, 0, 0, 0, 0, 0, 0,
    2, 0, 2, -2, 0, 2, 0, 0,
    0, -2, 0, 0, -2, 0, -2, 0,
    -2, -3, 0, 0, -2, -2, -2, -4,
    -2, 0, -2, 5, 0, 1, -25, -11,
    7, -1, 0, -27, 0, 4, 0, 0,
    0, 0, 0, 0, -8, 0, -6, -2,
    0, -4, 0, -3, 0, -4, -7, -5,
    0, 0, 0, 0, 4, 0, 2, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 1, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -6, -2, 0, 0, 0, -6, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -8, 0, 0, -21, 4, 0,
    0, -11, -2, 0, -2, 0, -4, 0,
    0, 0, 0, 0, -5, 0, -6, -7,
    0, -2, -2, -6, -7, -12, -6, 0,
    -10, -19, 0, -17, 6, 0, -13, -10,
    0, 3, -2, -25, -8, -28, -21, 0,
    -34, 0, -1, 0, -4, -4, 0, 0,
    0, -5, -6, -18, 0, 0, -2, 2,
    0, 2, -27, -15, 2, 0, 0, -31,
    0, 0, 0, 0, 0, -4, 0, -6,
    -6, 0, -6, 0, 0, 0, 0, 0,
    0, 2, 0, 0, 0, -2, 0, -2,
    8, 0, -1, -2, 0, 0, 1, -2,
    -2, -5, -2, 0, -8, 0, 0, 0,
    -2, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    2, 0, 0, 0, 4, 0, 0, -2,
    0, 0, -3, 1, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -6, 6, 0, -15, -21, -15, 6, -6,
    0, -27, 0, 4, 0, 4, 4, 0,
    0, 0, -22, 0, -21, -9, 0, -17,
    -22, -7, -16, -20, -20, -17, -2, 4,
    0, -4, -14, -12, 0, -3, 0, -13,
    0, 4, 0, 0, 0, 0, 0, 0,
    -14, 0, -12, -3, 0, -7, -8, 0,
    -5, -4, -6, -6, 0, 0, 4, -17,
    2, 0, 3, -6, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, -4, 0,
    -6, 0, 0, -3, -3, -5, -5, -11,
    0, 0, -6, 3, 4, -13, -26, -20,
    2, -10, 0, -26, -3, 0, 0, 0,
    0, 0, 0, 0, -21, 0, -19, -9,
    0, -15, -17, -6, -14, -13, -12, -14,
    0, 0, 2, -10, 4, 0, 2, -6,
    0, 0, -2, 0, 0, 0, 0, 0,
    0, 0, -1, 0, -3, 0, 0, 0,
    0, 0, 0, -6, 0, 0, 0, -8,
    0, 0, 0, 0, -5, 0, 0, 0,
    0, -16, 0, -13, -12, -1, -18, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -2, 0, 0, 0, -10, 0, -2,
    -6, 0, -7, 0, 0, 0, 0, -21,
    0, -13, -12, -6, -19, 0, -2, 0,
    0, -2, 0, 0, 0, -1, 0, -3,
    -4, -5, 0, 1, 0, 4, 5, 0,
    -2, 0, 0, 0, 0, -15, 0, -9,
    -6, 4, -13, 0, 0, 0, -1, 2,
    0, 0, 0, 4, 0, 0, 2, 2,
    0, 0, 3, 0, 0, 0, 2, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 3, 0, 0, -4, 0, 0, 0,
    0, -13, 0, -10, -8, -2, -15, 0,
    0, 0, 0, 0, 0, 0, 2, 0,
    0, 1, -1, 0, 0, 8, 0, -2,
    -15, 0, 8, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, -5, 0,
    -5, 0, 0, 0, 0, 2, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -5, 0, 0, 0, 0, -17, 0, -8,
    -7, 0, -15, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 2, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 6, 0,
    0, 0, 0, 0, 0, 0, -4, 0,
    0, -5, 5, 0, -9, 0, 0, 0,
    0, -18, 0, -11, -10, 0, -15, 0,
    -6, 0, -4, 0, 0, 0, -2, 0,
    -1, 0, 0, 0, 0, 4, 0, 2,
    -20, -7, -6, 0, 0, -21, 0, 0,
    0, -7, 0, -8, -13, 0, -8, 0,
    -6, 0, 0, 0, -1, 5, 0, 0,
    0, 0, 0, -6, 0, 0, 0, 0,
    -6, 0, 0, 0, 0, -20, 0, -13,
    -9, 0, -19, 0, 0, 0, 0, 0,
    0, 0, 2, 0, 0, -1, 0, 2,
    0, -2, 2, 0, 5, 0, -4, 0,
    0, 0, 0, -14, 0, -8, 0, 0,
    -12, 0, 0, 0, -1, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -17, -7, -5, 0, 0, -14,
    0, -20, 0, -8, -4, -11, -13, 0,
    -3, 0, -3, 0, 0, 0, -1, 0,
    0, 0, 0, 0, 0, 0, 0, -4,
    2, 0, -8, 0, 0, 0, 0, -20,
    0, -10, -6, 0, -12, 0, -3, 0,
    -4, 0, 0, 0, 0, 2, 0, 0,
    0, 0, 0, 0, 0, 0, 2, 0,
    -6, 0, 0, 0, 0, -19, 0, -10,
    -5, 0, -13, 0, -3, 0, -4, 0,
    0, 0, 0, 0, 0, 0, 0, 0
};


/*Collect the kern class' data in one place*/
static const lv_font_fmt_txt_kern_classes_t kern_classes =
{
    .class_pair_values   = kern_class_values,
    .left_class_mapping  = kern_left_class_mapping,
    .right_class_mapping = kern_right_class_mapping,
    .left_class_cnt      = 40,
    .right_class_cnt     = 30,
};

/*--------------------
 *  ALL CUSTOM DATA
 *--------------------*/

#if LVGL_VERSION_MAJOR == 8
/*Store all the custom data of the font*/
static  lv_font_fmt_txt_glyph_cache_t cache;
#endif

#if LVGL_VERSION_MAJOR >= 8
static const lv_font_fmt_txt_dsc_t font_dsc =
{
#else
static lv_font_fmt_txt_dsc_t font_dsc =
{
#endif
    .glyph_bitmap = glyph_bitmap,
    .glyph_dsc = glyph_dsc,
    .cmaps = cmaps,
    .kern_dsc = &kern_classes,
    .kern_scale = 16,
    .cmap_num = 2,
    .bpp = 1,
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
const lv_font_t SourceHanSansSC_size12_bits1_font =
{
#else
lv_font_t SourceHanSansSC_size12_bits1_font =
{
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 15,          /*The maximum line height required by the font*/
    .base_line = 3,             /*Baseline measured from the bottom of the line*/
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



#endif /*#if SOURCEHANSANSSC_SIZE12_BITS1_FONT*/

