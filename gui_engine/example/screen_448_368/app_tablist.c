#include "root_image_hongkong/ui_resource.h"
#include <gui_magic_img.h>

void tablist_clock(void *parent)
{
    gui_magic_img_t *tablist_img = gui_magic_img_create_from_mem(parent, "page0", CLOCKN_BIN, 0, 0, 0,
                                                                 0);
    gui_magic_img_t *rect = gui_magic_img_create_from_mem(parent, "rect", RECT_BIN, 0, 0, 0, 0);
}
void tablist_activity(void *parent)
{
    gui_magic_img_create_from_mem(parent, "page1", ACTIVITY_BIN, 0, 0, 0, 0);
    gui_magic_img_t *rect = gui_magic_img_create_from_mem(parent, "rect", RECT_BIN, 0, 0, 0, 0);
}
void tablist_heart(void *parent)
{
    gui_magic_img_create_from_mem(parent, "page2", HEARTRATE_BIN, 0, 0, 0, 0);
    gui_magic_img_t *rect = gui_magic_img_create_from_mem(parent, "rect", RECT_BIN, 0, 0, 0, 0);
}
void tablist_blood(void *parent)
{
    gui_magic_img_create_from_mem(parent, "page3", BLOODOXYGEN_BIN, 0, 0, 0, 0);
    gui_magic_img_t *rect = gui_magic_img_create_from_mem(parent, "rect", RECT_BIN, 0, 0, 0, 0);
}
void tablist_weather(void *parent)
{
    gui_magic_img_create_from_mem(parent, "page4", WEATHER_BIN, 0, 0, 0, 0);
    gui_magic_img_t *rect = gui_magic_img_create_from_mem(parent, "rect", RECT_BIN, 0, 0, 0, 0);
}
void tablist_music(void *parent)
{
    gui_magic_img_create_from_mem(parent, "page5", MUSIC_BIN, 0, 0, 0, 0);
    gui_magic_img_t *rect = gui_magic_img_create_from_mem(parent, "rect", RECT_BIN, 0, 0, 0, 0);
}

