#include "app_gui_main.h"
#include "gui_text.h"

gui_text_t *text_call_title = NULL;

void design_tab_call(void *parent)
{
    char *string_call_title = "电话";
    int font_size = 32;
    text_call_title = gui_text_create(parent, "text_call_title", 195, 24,
                                      strlen(string_call_title) / FONT_CHINESE_BYTE * FONT_CHINESE_W, font_size);
    gui_text_set(text_call_title, string_call_title, "rtk_font_mem", 0xffffffff,
                 strlen(string_call_title), font_size);
}
