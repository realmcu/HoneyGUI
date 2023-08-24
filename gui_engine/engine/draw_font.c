
#include <guidef.h>
#include <draw_font.h>
#include <string.h>
#include "gui_config.h"

static int get_utf8_byte_num(uint8_t firstCh)
{
    uint8_t temp = 0x80;
    int num = 0;

    while (temp & firstCh)
    {
        num++;
        temp = (temp >> 1);
    }
    return num;
}

uint16_t utf8_to_unicode(uint8_t *utf8, uint16_t len, uint16_t *unicode_array,
                         uint16_t unicode_buf_len)
{
    uint16_t k = 0;

    for (uint32_t i = 0; i < len; i++)
    {
        if (k >= unicode_buf_len || k == 0xffff)
        {
            return k;
        }
        switch (get_utf8_byte_num((uint8_t)utf8[i]))
        {
        case 0:
            {
                unicode_array[k] = *(utf8 + i);
                k++;
                break;
            }
        case 1:
            {
                //TODO
                //can not enter here
                //k++;
                break;
            }
        case 2:
            {
                unicode_array[k] = (uint16_t)(((utf8[i] & 0x03) << 6 | utf8[i + 1] & 0x3F) | ((
                        utf8[i] & 0x38) >> 2 << 8));
                k++;
                i = i + 1;
                break;
            }
        case 3:
            {
                unicode_array[k] = ((utf8[i + 1] & 0x03) << 6) + (utf8[i + 2] & 0x3F);
                unicode_array[k] |= (uint16_t)(((utf8[i] & 0x0F) << 4) | ((utf8[i + 1] >> 2) & 0x0F)) << 8;
                k++;
                i = i + 2;
                break;
            }
        case 4:
            {
                //TODO
                k++;
                i = i + 3;
                break;
            }
        case 5:
            {
                //TODO
                k++;
                i = i + 4;
                break;
            }
        case 6:
            {
                //TODO
                k++;
                i = i + 5;
                break;
            }
        default:
            //DBG_DIRECT("the len is more than 6\n");
            break;
        }
    }
    return k;
}

static gui_list_t _rtgui_system_font_list = {NULL};

/* register an image engine */
void rtgui_font_register_engine(struct rtgui_font_engine *engine)
{
    GUI_ASSERT(engine != NULL);

    gui_list_append(&_rtgui_system_font_list, &(engine->list));
}


void rtgui_system_font_init(void)
{
    gui_list_init(&_rtgui_system_font_list);
#ifdef RTK_GUI_FONT_FS
    extern void rtgui_font_rtk_init(void);
    rtgui_font_rtk_init();
#endif
#ifdef RTK_GUI_FONT_MEM
    extern void rtgui_font_mem_init(void);
    rtgui_font_mem_init();
#if RTK_GUI_FONT_FREETYPE
    extern void rtgui_freetype_init(void);
    rtgui_freetype_init();
#endif
#if RTK_GUI_FONT_STB
    extern void rtgui_font_stb_init(void);
    rtgui_font_stb_init();
#endif
#endif
}

static struct rtgui_font_engine *rtgui_font_get_engine_by_type(char *string)
{
    struct rtgui_font_engine *engine;
    gui_list_t *node = NULL;
    if (strstr(string, ".ttf") || strstr(string, ".TTF") || strstr(string, ".fon"))
    {
        string = "freetype";
    }
    gui_list_for_each(node, &_rtgui_system_font_list)
    {
        engine = gui_list_entry(node, struct rtgui_font_engine, list);
        if (strncmp(engine->name, string, strlen(engine->name)) == 0)
        {
//            gui_log("engine addr = 0x%x \n", engine);
//            gui_log("font string = %s \n", string);
            return engine;
        }
    }
    bool match_no_font_engine = false;
    GUI_ASSERT(match_no_font_engine);
    return NULL;
}

void rtgui_text_create(gui_text_t *text)
{
    struct rtgui_font_engine *engine;
    engine = rtgui_font_get_engine_by_type(text->text_type);
    engine->font_load(text);
    text->engine = engine;
}

void rtgui_text_destroy(gui_text_t *text)
{
    text->engine->font_unload(text);
}

/* draw a text */
void rtgui_font_draw(gui_text_t *text, struct rtgui_rect *rect)
{
    GUI_ASSERT(text != NULL);

    if (text->engine != NULL &&
        text->engine->font_draw != NULL)
    {
        text->engine->font_draw(text, rect);
    }
}






