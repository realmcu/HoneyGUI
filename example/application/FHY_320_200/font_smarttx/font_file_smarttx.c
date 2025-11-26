/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "gui_components_init.h"
#include "font_file_smarttx.h"
#include "gui_img.h"
#include "gui_text.h"
#include "draw_font.h"
#include "font_custom.h"

/*============================================================================*
 *                           Types
 *============================================================================*/


/*============================================================================*
 *                            Macros
 *============================================================================*/


/*============================================================================*
 *                           Function Declaration
 *============================================================================*/


/*============================================================================*
 *                            Variables
 *============================================================================*/

uint8_t *font_root3 = NULL;
uint8_t *font_root4 = NULL;

static bool scroll_need_update = false;

/**
*@brief A:11 B:10 C:10 D:11 E:8 F:8
*/
/*ABCDEFG*/
uint8_t text[] = {0x41, 0x00, 0x42, 0x00, 0x43, 0x00, 0x44, 0x00, 0x45, 0x00, 0x46, 0x00, 0x47, 0x00};

/*============================================================================*
 *                           Private Functions
 *============================================================================*/

static uint8_t get_utf8_byte_num(uint8_t first_byte)
{
    if (first_byte < 0x80)
    {
        return 1;   // 1-byte (ASCII character)
    }
    else if ((first_byte >> 5) == 0x6)
    {
        return 2;   // 2-byte sequence
    }
    else if ((first_byte >> 4) == 0xE)
    {
        return 3;   // 3-byte sequence
    }
    else if ((first_byte >> 3) == 0x1E)
    {
        return 4;   // 4-byte sequence
    }
    else
    {
        return 0;   // Invalid first byte
    }
}

static uint16_t utf8_to_unicode_length(uint8_t *utf8, uint32_t utf8_len)
{
    uint16_t unicode_len = 0;
    uint16_t i = 0;

    while (i < utf8_len)
    {
        uint8_t char_len = get_utf8_byte_num(utf8[i]);

        if (char_len == 0 || i + char_len > utf8_len)
        {
            break;
        }

        i += char_len;
        unicode_len++;
    }

    return unicode_len;
}

static uint16_t utf8_to_utf16le(const uint8_t *utf8_str, uint16_t utf8_len, uint8_t *utf16le_buf,
                                uint16_t buf_size)
{
    uint16_t utf16_index = 0;
    uint16_t char_count = 0;

    for (uint16_t i = 0; i < utf8_len && utf16_index + 1 < buf_size;)
    {
        uint8_t utf8_bytes = get_utf8_byte_num(utf8_str[i]);
        uint32_t unicode_char = 0;

        switch (utf8_bytes)
        {
        case 1:
            unicode_char = utf8_str[i];
            i += 1;
            break;

        case 2:
            if (i + 1 < utf8_len)
            {
                unicode_char = ((utf8_str[i] & 0x1F) << 6) | (utf8_str[i + 1] & 0x3F);
                i += 2;
            }
            else
            {
                i = utf8_len;
                continue;
            }
            break;

        case 3:
            if (i + 2 < utf8_len)
            {
                unicode_char = ((utf8_str[i] & 0x0F) << 12) |
                               ((utf8_str[i + 1] & 0x3F) << 6) |
                               (utf8_str[i + 2] & 0x3F);
                i += 3;
            }
            else
            {
                i = utf8_len;
                continue;
            }
            break;

        default:
            i++;
            continue;
        }

        if (unicode_char <= 0xFFFF)
        {
            utf16le_buf[utf16_index++] = unicode_char & 0xFF;
            utf16le_buf[utf16_index++] = (unicode_char >> 8) & 0xFF;
            char_count++;
        }
        else
        {
            utf16le_buf[utf16_index++] = 0xFF;
            utf16le_buf[utf16_index++] = 0xFD;
            char_count++;
        }
    }

    return char_count;
}

#if 1
static int smarttx_font_test(void)
{
    /*Available font sizes and languages can be retrieved from the font1.json and font2.json files.*/
    uint8_t font_size = 24;
    /*Text widget width*/
    uint16_t width = 120;
    /*Text widget height is equal to font size.*/
    uint16_t height = font_size;
    /*Text widget position*/
    uint16_t x = 64;
    uint16_t y = 0;
    /*Text widget font color*/
    gui_color_t color = APP_COLOR_WHITE;

    gui_text_t *text_widget = gui_text_create(gui_obj_get_root(), "font_text", x, y, width, height);
    gui_text_set(text_widget, text, GUI_FONT_SRC_CUS, color, sizeof(text), font_size);
    gui_text_encoding_set(text_widget, UTF_16LE);
    /*Support LEFT, RIGHT, CENTER and SCROLL_X*/
    /*Only support single line*/
    gui_text_mode_set(text_widget, CENTER);
    return 0;
}
#else
static int smarttx_font_test(void)
{
    uint8_t font_size = 16;
    uint16_t text_width = FONT_RENDERING_STRIDE;
    uint16_t text_height = font_size;

    gui_dispdev_t *dc = gui_get_dc();
    uint8_t *pfb1 = dc->disp_buf_1;

    uint8_t *image_buf = gui_malloc(text_width * text_height * 2 + 8);
    memset(image_buf, 0x00, text_width * text_height * 2 + 8);

    uint8_t *font_buf = image_buf + 8;

    UI_WidgetTypeDef font =
    {
        .x = 0,
        .y = 0,
        .width = text_width,
        .height = text_height,
        .active_ys = 0,
        .active_ye = text_height,
        .addr_ptr = text,

        .string_mem = {
            .font_color = RGB565(255, 255, 255),
            .font_size = font_size,
            .string_len = sizeof(text) / 2,
            .align = 0,
            // .bold = 0,
            // .dirty = 0,
            // .scroll = 0,
            // .need_scroll = 0,
            // .reserved = 0,
            // .last_pos = 0,
            // .string_width = text_height,
        },
    };
    int zs = 0;
    int ze = FONT_RENDERING_STRIDE;
    uint8_t res = rtl_gui_show_string_transparency(&font, 0, 0, zs, ze, font_buf);
    gui_log("input font x %d, y %d, width %d, height %d, active_ys %d, active_ye %d", font.x, font.y,
            font.width, font.height, font.active_ys, font.active_ye);
    gui_log("input font size %d, string_len %d, align %d", font.string_mem.font_size,
            font.string_mem.string_len, font.string_mem.align);
    gui_log("input zs %d, ze %d", zs, ze);

    struct gui_rgb_data_head head;
    memset(&head, 0x0, sizeof(head));
    head.w = font.width;
    head.h = font.height;
    head.type = RGB565;
    memcpy(image_buf, &head, sizeof(head));

    gui_img_t *img = gui_img_create_from_mem(gui_obj_get_root(), "font_img", image_buf, 0, 0, 0, 0);
    return 0;
}
#endif
// GUI_INIT_APP_EXPORT(smarttx_font_test);

static void font_load(gui_text_t *text, gui_text_rect_t *rect)
{
    GUI_UNUSED(text);
    GUI_UNUSED(rect);

    if (text->charset == UTF_8)
    {
        uint16_t unicode_len = utf8_to_unicode_length(text->content, text->len);
        uint8_t *utf16le_buf = gui_malloc(unicode_len * 2);
        text->font_len = unicode_len;
        utf8_to_utf16le(text->content, text->len, utf16le_buf, unicode_len * 2);
        text->data = utf16le_buf;
        text->charset = UTF_16LE;
    }
    else if (text->charset == UTF_16LE)
    {
        text->font_len = text->len / 2;
        text->data = NULL;
    }
    else
    {
        gui_log("charset not support in font smarttx");
    }

    if (scroll_need_update)
    {
#if !defined(__WIN32) && !defined(_WIN32) && !defined(__linux__)
        gui_font_scroll_update();
#endif
        scroll_need_update = false;
    }
    // gui_log("font_load\n");

}

static void font_draw(gui_text_t *text, gui_text_rect_t *rect)
{
    GUI_UNUSED(text);
    GUI_UNUSED(rect);

    // gui_log("font_draw\n");

    gui_dispdev_t *dc = gui_get_dc();

    UI_WidgetTypeDef font =
    {
        .x = text->base.y,
        .y = text->base.x,
        // .x = text->base.x,
        // .y = text->base.y,
        .width = text->base.w,
        .height = text->base.h,
        .active_ys = 0,
        .active_ye = 0,
        .addr_ptr = text->data == NULL ? text->content : text->data,

        .string_mem = {
            .font_color = RGB565(text->color.color.rgba.r, text->color.color.rgba.g, text->color.color.rgba.b),
            .font_size = text->font_height,
            .string_len = text->len,
            // .align = 0,
            // .bold = 0,
            // .dirty = 0,
            // .scroll = 0,
            // .need_scroll = 0,
            // .reserved = 0,
            // .last_pos = 0,
            // .string_width = text_height,
        },
    };
    int zs = 0;
    int ze = 0;

    if (dc->pfb_type == PFB_Y_DIRECTION)
    {
        zs = dc->section.y1;
        ze = dc->section.y2;
    }
    else if (dc->pfb_type == PFB_X_DIRECTION)
    {
        zs = dc->section.x1;
        ze = dc->section.x2;
    }
    switch (text->mode)
    {
    case LEFT:
        font.string_mem.align = 0;
        break;
    case CENTER:
        font.string_mem.align = 1;
        break;
    case RIGHT:
        font.string_mem.align = 2;
        break;
    case SCROLL_X:
        font.string_mem.scroll = 1;
        font.string_mem.need_scroll = 1;
        break;
    default:
        break;
    }

    uint8_t *buf = dc->frame_buf;
    // gui_log("input font x %d, y %d, width %d, height %d, active_ys %d, active_ye %d \n", font.x, font.y,
    //         font.width, font.height, font.active_ys, font.active_ye);
    // gui_log("input font size %d, string_len %d, align %d \n", font.string_mem.font_size,
    //         font.string_mem.string_len, font.string_mem.align);
    // gui_log("input zs %d, ze %d, buf %p \n", zs, ze, buf);

#if !defined(__WIN32) && !defined(_WIN32) && !defined(__linux__)
    // Only call on embedded platform, not on simulator
    uint8_t res = rtl_gui_show_string_transparency(&font, 0, 0, zs, ze, buf);
    (void)res;
#else
    GUI_UNUSED(buf);
#endif

}

static void font_unload(gui_text_t *text)
{
    GUI_UNUSED(text);

    scroll_need_update = true;
    // gui_log("font_unload\n");

}

static void font_destroy(gui_text_t *text)
{
    GUI_UNUSED(text);
    if (text->data)
    {
        gui_free(text->data);
        text->data = NULL;
    }

    // gui_log("font_destroy\n");

}

static int custom_font_rendering_init(void)
{
    gui_font_custom_set_callbacks(font_load, font_draw, font_unload, font_destroy);
}
GUI_INIT_APP_EXPORT(custom_font_rendering_init);


/*============================================================================*
 *                           Public Functions
 *============================================================================*/

/*lib debug*/
#if !defined(__WIN32) && !defined(_WIN32) && !defined(__linux__)
#include "trace.h"  // Only for embedded platform, not needed for simulation
// void dump_font_align_info(uint32_t line, uint8_t dirty, uint8_t align, int16_t delta_ys,
//                           int16_t string_width, int16_t icon_height)
// {
//     gui_log("output: line %d, dirty %d, align %d, delta_ys %d, string_width %d, icon_height %d", line,
//             dirty, align, delta_ys, string_width, icon_height);
// }

// void dump_font_info(int16_t xs, int16_t ys, int16_t zs, int16_t ze, int8_t top, int8_t bottom,
//                     int8_t left, int8_t right)
// {
//     gui_log("output: xs %d, ys %d, zs %d, ze %d, top %d, bottom %d, left %d, right %d", xs, ys, zs, ze,
//             top, bottom, left, right);
// }
#endif


