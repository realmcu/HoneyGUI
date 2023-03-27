#include "gui_config.h"
#if RTK_GUI_FONT_FREETYPE
#include <draw_font.h>
#include <math.h>

typedef struct
{
    uint16_t unicode;
    int16_t x;
    int16_t y;
    int16_t w;
    int16_t h;
    int16_t char_w;
    uint32_t dot_off;
    uint32_t font_area;
    int16_t x_bound;
    int16_t y_bound;
    int16_t w_bound;
    int16_t h_bound;
} rtk_char_t;

static void freetype_load(struct rtgui_font *font)
{

}

static void freetype_unload(struct rtgui_font *font)
{

}
#include <stdio.h>
#include <string.h>
#include <math.h>

#include <ft2build.h>
#include FT_FREETYPE_H
#include <gui_img.h>
/* Replace this function with something useful. */

static void draw_bitmap(FT_Bitmap *bitmap, FT_Int x, FT_Int y, uint16_t *target, char **image,
                        struct rtgui_rect *rect, uint32_t color)
{
    FT_Int  i, j, p, q;
    FT_Int  x_max = x + bitmap->width;
    FT_Int  y_max = y + bitmap->rows;

    for (i = x, p = 0; i < x_max; i++, p++)
    {
        for (j = y, q = 0; j < y_max; j++, q++)
        {
            if (i < 0 || j < 0 || i >= rect->x2 - rect->x1 || j >= rect->y2 - rect->y1)
            {
                continue;
            }
            //blend_rgba_to_screen(target+(j + i * bitmap->rows), rgba_set_alpha(0xffff00ff, bitmap->buffer[q * bitmap->width + p]));
            blend_rgba_to_screen(target + ((rect->x1 + i) + (rect->y1 + j) * gui_get_screen_width()),
                                 rgba_set_alpha(color, bitmap->buffer[q * bitmap->width + p]));

        }
    }
}


static void freetype_draw(struct rtgui_font *font, struct gui_dispdev *dc, struct rtgui_rect *rect)
{

    FT_Library    library;
    FT_Face       face;

    FT_GlyphSlot  slot;
    FT_Matrix     matrix;                 /* transformation matrix */
    FT_Vector     pen;                    /* untransformed origin  */
    FT_Error      error;

    char         *text;
    uint32_t      color;
    double        angle;
    int           target_height;
    int           n, num_chars;


    color = font->color;
    text = (void *)(font->utf8);

    num_chars = strlen(text);
    angle = (0 / 360) * 3.14159 * 2;      /* use 0 degrees  pi    */
    target_height = font->font_height;

    error = FT_Init_FreeType(&library);              /* initialize library */
    /* error handling omitted */
//error = FT_New_Memory_Face(library, (void *)0x00a00b5c, 206784, 0, &face);
    error = FT_New_Face(library, font->file, 0, &face);/* create face object */
    /* error handling omitted */
    /* use 50pt at 100dpi */
    // error = FT_Set_Char_Size(face, 10 * 64, 0, 300, 300);   /* set character size */
    /* error handling omitted */
    error = FT_Set_Pixel_Sizes(face, font->font_width, font->font_height);
    slot = face->glyph;

    /* set up matrix */
    matrix.xx = (FT_Fixed)(cos(angle) * 0x10000L);
    matrix.xy = (FT_Fixed)(-sin(angle) * 0x10000L);
    matrix.yx = (FT_Fixed)(sin(angle) * 0x10000L);
    matrix.yy = (FT_Fixed)(cos(angle) * 0x10000L);

    /* the pen position in 26.6 cartesian space coordinates; */
    /* start at (40,0) relative to the upper left corner  */
    pen.x = 0 * 64;
    pen.y = 0 * 64;

//  uint8_t** image = (uint8_t **)malloc(buffer_height*sizeof(uint8_t*));
//  for(int i=0; i<buffer_height; i++)
//  {
//      image[i] = (uint8_t*)malloc(buffer_width*sizeof(uint8_t));
//      memset(image[i],0,buffer_width*sizeof(uint8_t));
//  }

    uint16_t *writebuf = (uint16_t *)dc->frame_buf;
    for (n = 0; n < num_chars; n++)
    {
        /* set transformation */
        FT_Set_Transform(face, &matrix, &pen);

        /* load glyph image into the slot (erase previous one) */
        error = FT_Load_Char(face, text[n], FT_LOAD_RENDER);
        if (error)
        {
            continue;    /* ignore errors */
        }
        /* now, draw to our target surface (convert position) */
        draw_bitmap(&slot->bitmap, slot->bitmap_left, target_height - slot->bitmap_top, writebuf, NULL,
                    rect, color);

        /* increment pen position */
        pen.x += slot->advance.x;
        pen.y += slot->advance.y;
    }
    //show_image(writebuf,color,image,rect);

//  for(int i=0; i<buffer_height; i++)
//  {
//      free(image[i]);
//  }
//  free(image);

    FT_Done_Face(face);
    FT_Done_FreeType(library);
}

struct rtgui_font_engine rtgui_freetype_engine =
{
    "freetype",
    { NULL },
    freetype_load,
    freetype_unload,
    freetype_draw,
};

void rtgui_freetype_init(void)
{
    rtgui_font_register_engine(&rtgui_freetype_engine);
}
#endif
