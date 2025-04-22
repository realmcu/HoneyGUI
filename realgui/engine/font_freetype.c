#include "draw_font.h"
#include "font_freetype.h"
#include "gui_api.h"
#include <math.h>
#include "font_rendering_utils.h"
#include <ft2build.h>
#include FT_FREETYPE_H

static FT_FONT_LIB font_lib_tab[10];
static uint8_t get_fontlib_name(uint8_t *font_file)
{
    uint8_t tab_size = sizeof(font_lib_tab) / sizeof(FT_FONT_LIB);
    for (size_t i = 0; i < tab_size; i++)
    {
        if (font_lib_tab[i].font_ttf_file == font_file)
        {
            return i;
        }
    }
    gui_log("Can not match font file, use default \n");
    if (font_lib_tab[0].font_ttf_file == 0 ||
        font_lib_tab[0].font_file_size == 0)
    {
        gui_log("There is no font file \n");
        GUI_ASSERT(font_lib_tab[0].font_ttf_file != 0)
    }
    return 0;
}

void gui_font_ft_mem_init(void *font_ttf_addr, uint32_t font_file_size)
{
    if (font_ttf_addr == 0 || font_file_size == 0)
    {
        return;
    }
    int i = 0;
    for (; i < sizeof(font_lib_tab) / sizeof(FT_FONT_LIB); i++)
    {
        if (font_lib_tab[i].font_ttf_file == NULL)
        {
            break;
        }
        if (font_lib_tab[i].font_ttf_file == font_ttf_addr)
        {
            break;
        }
    }
    if (i >= sizeof(font_lib_tab) / sizeof(FT_FONT_LIB))
    {
        return;
    }

    font_lib_tab[i].font_ttf_file = font_ttf_addr;
    font_lib_tab[i].font_file_size = font_file_size;
}

void gui_font_ft_load(gui_text_t *text, gui_text_rect_t *rect)
{

}

void gui_font_ft_unload(gui_text_t *text)
{

}


static void font_ft_draw_bitmap(gui_text_t *text,
                                FT_Bitmap *bitmap,
                                gui_text_rect_t *rect,
                                int x_start,
                                int y_start)
{
    gui_dispdev_t *dc = gui_get_dc();
    uint32_t width = bitmap->width;
    uint32_t rows = bitmap->rows;
    uint8_t *buffer = bitmap->buffer;
    // gui_log("char: , width %d, rows %d, pitch %d, x_start %d, y_start %d \n", width, rows, bitmap->pitch, x_start, y_start);
    uint8_t dc_bytes_per_pixel = dc->bit_depth >> 3;
    if (dc_bytes_per_pixel == 4)
    {
        uint32_t *writebuf = (uint32_t *)dc->frame_buf;
        uint32_t color_back;
        for (int i = 0; i < rows; i++)
        {
            int write_off = (i + y_start - dc->section.y1) * dc->fb_width + x_start;
            int read_off = i * bitmap->pitch;
            for (int j = 0; j < width; j++)
            {
                if (buffer[read_off + j] != 0)
                {
                    color_back = writebuf[write_off + j];
                    writebuf[write_off + j] = alphaBlendRGBA(text->color, color_back, buffer[read_off + j]);
                }
            }
        }
    }
    else if (dc_bytes_per_pixel == 2)
    {
        uint16_t *writebuf = (uint16_t *)dc->frame_buf;
        uint16_t color_back;
        for (int i = 0; i < rows; i++)
        {
            int write_off = (i + y_start - dc->section.y1) * dc->fb_width + x_start;
            int read_off = i * bitmap->pitch;
            for (int j = 0; j < width; j++)
            {
                if (buffer[read_off + j] != 0)
                {
                    color_back = writebuf[write_off + j];
                    writebuf[write_off + j] = alphaBlendRGB565(rgba2565(text->color), color_back, buffer[read_off + j]);
                }
            }
        }
    }
}

void gui_font_ft_draw(gui_text_t *text, gui_text_rect_t *rect)
{
    gui_dispdev_t *dc = gui_get_dc();

    if (dc->type == DC_RAMLESS)
    {
        return;
    }

    FT_Library    library;
    FT_Face       face;

    FT_GlyphSlot  slot;
    FT_Matrix     matrix;                 /* transformation matrix */
    FT_Vector     pen;                    /* untransformed origin  */
    FT_Error      error;
    FT_Bitmap     bitmap;
    char         *text_list;
    uint32_t      color;
    double        angle;
    uint32_t      font_file_length;

    color = text->color.color.argb_full;
    text_list = (void *)(text->content);

    angle = (0 / 360) * 3.14159 * 2;      /* use 0 degrees  pi    */
    error = FT_Init_FreeType(&library);              /* initialize library */
    GUI_ASSERT(error == 0)                          /* error handling omitted */

    font_file_length = font_lib_tab[get_fontlib_name((uint8_t *)text->path)].font_file_size;
    error = FT_New_Memory_Face(library, text->path, font_file_length, 0, &face);  //mem
    GUI_ASSERT(error == 0)                          /* error handling omitted */
    // error = FT_New_Face(library, text->path, 0, &face); //fs
    // GUI_ASSERT(error == 0)                          /* error handling omitted */
    error = FT_Set_Pixel_Sizes(face, text->font_height, text->font_height);
    GUI_ASSERT(error == 0)                          /* error handling omitted */
    // error = FT_Select_Charmap(face, FT_ENCODING_UNICODE);
    // GUI_ASSERT(error == 0)                          /* error handling omitted */

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

    uint32_t *unicode_buf = NULL;
    uint16_t unicode_len = 0;
    unicode_len = process_content_by_charset(text->charset, text->content, text->len, &unicode_buf);

    int x_start = slot->bitmap_left;
    int y_start = text->font_height - slot->bitmap_top;

    for (int i = 0; i < text->len; i++)
    {
        /* set transformation */
        FT_Set_Transform(face, &matrix, &pen);

        /* load glyph image into the slot (erase previous one) */
        // error = FT_Load_Char(face, text_list[i], FT_LOAD_RENDER);
        // GUI_ASSERT(error == 0)                          /* error handling omitted */

        error = FT_Load_Char(face, unicode_buf[i], FT_LOAD_NO_BITMAP);
        GUI_ASSERT(error == 0)                          /* error handling omitted */
        error = FT_Render_Glyph(slot, FT_RENDER_MODE_NORMAL);
        GUI_ASSERT(error == 0)                          /* error handling omitted */

        x_start = slot->bitmap_left + rect->x1;
        y_start = text->font_height - slot->bitmap_top + rect->y1;

        font_ft_draw_bitmap(text, &slot->bitmap, rect, x_start, y_start);
        /* increment pen position */
        pen.x += slot->advance.x;
        pen.y += slot->advance.y;
    }

    gui_free(unicode_buf);
    FT_Done_Face(face);
    FT_Done_FreeType(library);
}