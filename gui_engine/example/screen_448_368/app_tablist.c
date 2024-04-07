#include "root_image_hongkong/ui_resource.h"
#include "gui_img.h"
#include "gui_watch_gradient_spot.h"
#include "gui_watchface_gradient.h"

void tablist_clock(void *parent)
{
    gui_img_t *tablist_img = gui_img_create_from_mem(parent, "page0", CLOCKN_BIN, 0, 0, 0,
                                                     0);
    gui_img_scale(tablist_img, 0.6, 0.6);
    gui_img_set_mode(tablist_img, IMG_SRC_OVER_MODE);
    gui_img_t *rect = gui_img_create_from_mem(parent, "rect", RECT_221_269_BIN, 0, 0, 0, 0);
    gui_img_set_mode(rect, IMG_SRC_OVER_MODE);
}
void tablist_watch(void *parent)
{
    gui_watch_gradient_spot_t *watch = gui_watch_gradient_spot_create(parent, "watchface", 0, 0, 221,
                                                                      269);
    gui_watch_gradient_spot_set_center(watch, 221, 269);
    gui_img_t *rect = gui_img_create_from_mem(parent, "rect", RECT_221_269_BIN, 0, 0, 0, 0);
    gui_img_set_mode(rect, IMG_SRC_OVER_MODE);
}
void tablist_watchface(void *parent)
{
    T_GUI_WATCHFACE_GRADIENT *canvas = gui_watchface_gradient_create(parent, "watchface_gradient",
                                                                     (368 - 368) / 2,
                                                                     (448 - 448) / 2, 221, 269);
    gui_img_t *rect = gui_img_create_from_mem(parent, "rect", RECT_221_269_BIN, 0, 0, 0, 0);
    gui_img_set_mode(rect, IMG_SRC_OVER_MODE);
}
void tablist_blood(void *parent)
{
    gui_img_t *tablist_img = gui_img_create_from_mem(parent, "page3", WEATHER_BIN, 0, 0,
                                                     0, 0);
    gui_img_scale(tablist_img, 0.6, 0.6);
    gui_img_t *rect = gui_img_create_from_mem(parent, "rect", RECT_221_269_BIN, 0, 0, 0, 0);
}
void tablist_weather(void *parent)
{
    gui_img_create_from_mem(parent, "page4", WEATHER_BIN, 0, 0, 0, 0);
    gui_img_t *rect = gui_img_create_from_mem(parent, "rect", RECT_221_269_BIN, 0, 0, 0, 0);
}
void tablist_music(void *parent)
{
    gui_img_create_from_mem(parent, "page5", MUSIC_BIN, 0, 0, 0, 0);
    gui_img_t *rect = gui_img_create_from_mem(parent, "rect", RECT_221_269_BIN, 0, 0, 0, 0);
}

