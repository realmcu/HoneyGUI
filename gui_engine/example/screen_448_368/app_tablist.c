#include "root_image_hongkong/ui_resource.h"
#include <gui_magic_img.h>
#include "gui_watch_gradient_spot.h"
#include "gui_watchface_gradient.h"

void tablist_clock(void *parent)
{
    gui_magic_img_t *tablist_img = gui_magic_img_create_from_mem(parent, "page0", CLOCKN_BIN, 0, 0, 0,
                                                                 0);
    gui_img_scale(tablist_img, 0.6, 0.6);
    gui_magic_img_t *rect = gui_magic_img_create_from_mem(parent, "rect", RECT_221_269_BIN, 0, 0, 0, 0);
}
void tablist_watch(void *parent)
{
    gui_watch_gradient_spot_t *watch = gui_watch_gradient_spot_create(parent, "watchface", 0, 0, 221,
                                                                      269);
    gui_watch_gradient_spot_set_center(watch, 221, 269);
    gui_magic_img_t *rect = gui_magic_img_create_from_mem(parent, "rect", RECT_221_269_BIN, 0, 0, 0, 0);
}
void tablist_watchface(void *parent)
{
    gui_watchface_gradient_t *canvas = gui_watchface_gradient_create(parent, "watchface_gradient",
                                                                     (368 - 221) / 2,
                                                                     (448 - 269) / 2, 221, 269);
    gui_magic_img_t *rect = gui_magic_img_create_from_mem(parent, "rect", RECT_221_269_BIN, 0, 0, 0, 0);
}
void tablist_blood(void *parent)
{
    gui_magic_img_t *tablist_img = gui_magic_img_create_from_mem(parent, "page3", BLOODOXYGEN_BIN, 0, 0,
                                                                 0, 0);
    gui_img_scale(tablist_img, 0.6, 0.6);
    gui_magic_img_t *rect = gui_magic_img_create_from_mem(parent, "rect", RECT_221_269_BIN, 0, 0, 0, 0);
}
void tablist_weather(void *parent)
{
    gui_magic_img_create_from_mem(parent, "page4", WEATHER_BIN, 0, 0, 0, 0);
    gui_magic_img_t *rect = gui_magic_img_create_from_mem(parent, "rect", RECT_221_269_BIN, 0, 0, 0, 0);
}
void tablist_music(void *parent)
{
    gui_magic_img_create_from_mem(parent, "page5", MUSIC_BIN, 0, 0, 0, 0);
    gui_magic_img_t *rect = gui_magic_img_create_from_mem(parent, "rect", RECT_221_269_BIN, 0, 0, 0, 0);
}

