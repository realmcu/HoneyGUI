#include "app_gui_main.h"
#include "gui_text.h"

gui_text_t *text_contacts_title = NULL;

void design_tab_contacts(void *parent)
{
    char *string_contacts_title = "联系人";
    int font_size = 32;
    text_contacts_title = gui_text_create(parent, "text_contacts_title", 195, 24,
                                          strlen(string_contacts_title) / FONT_CHINESE_BYTE * FONT_CHINESE_W, font_size);
    gui_text_set(text_contacts_title, string_contacts_title, "rtk_font_mem", 0xffffffff,
                 strlen(string_contacts_title), font_size);
}

