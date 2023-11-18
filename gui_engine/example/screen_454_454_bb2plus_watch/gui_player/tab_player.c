#include "app_gui_main.h"
#include "gui_text.h"

gui_text_t *text_player_title = NULL;
void design_tab_player(void *parent)
{
    char *string_player_title = "音乐";
    int font_size = 32;
    text_player_title = gui_text_create(parent, "text_player_title", 195, 24,
                                        strlen(string_player_title) / FONT_CHINESE_BYTE * FONT_CHINESE_W, font_size);
    gui_text_set(text_player_title, string_player_title, "rtk_font_mem", 0xffffffff,
                 strlen(string_player_title), font_size);

}