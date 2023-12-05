#include "app_gui_main.h"
#include "root_image/ui_resource.h"
#include "gui_img.h"
#include "gui_curtainview.h"
#include <time.h>
#include <stdio.h>
#include "gui_text.h"
//#include "os_timer.h"

void *gui_watchface_turn_around_timer = NULL;

gui_img_t *img_clock = NULL;
gui_curtainview_t *curtainview_menu = NULL;

gui_img_t *watchface_hand;
gui_img_t *watchface_hour;
gui_img_t *watchface_minute;
gui_img_t *watchface_second;
float watchface_degree = 0;

/* static void watchface_turn_around_timer_callback(void *pxTimer)
{
    watchface_degree += 0.1;
} */

static void img_animate_watchface_callback(void *obj)
{
    //os_timer_create(&gui_watchface_turn_around_timer, "watchface_turn_around_timer", 1, 1000, false, watchface_turn_around_timer_callback);
    //os_timer_start(&gui_watchface_turn_around_timer);
    watchface_degree += 0.0022;
    gui_img_translate(watchface_hour, watchface_hour->base.w / 2, watchface_hour->base.h);
    gui_img_translate(watchface_minute, watchface_minute->base.w / 2, watchface_minute->base.h);
    gui_img_translate(watchface_second, watchface_second->base.w / 2, watchface_second->base.h);

    gui_img_rotation(watchface_hour, watchface_degree / 12,  watchface_hour->base.w / 2,
                     watchface_hour->base.h);
    gui_img_rotation(watchface_minute, watchface_degree, watchface_minute->base.w / 2,
                     watchface_minute->base.h);
    gui_img_rotation(watchface_second, watchface_degree * 60, watchface_second->base.w / 2,
                     watchface_second->base.h);
}

void design_tab_watchface(void *parent)
{
    img_clock = gui_img_create_from_mem(parent, "img_clock", CLOCK_BASE_BIN, 0, 0, 454, 454);

    watchface_hour = gui_img_create_from_mem(parent, "hour", ICON_CLOCK_HOUR_HAND_BIN, 223, 131, 8, 96);
    watchface_minute = gui_img_create_from_mem(parent, "minute", ICON_CLOCK_MINUTE_HAND_BIN, 223, 99, 8,
                                               128);
    watchface_second = gui_img_create_from_mem(parent, "second", ICON_CLOCK_SECOND_HAND_BIN, 225, 83, 4,
                                               144);
    watchface_hand = gui_img_create_from_mem(parent, "hand", ICON_CLOCK_HAND_DOT_BIN, 218, 218, 0, 0);

    gui_img_set_animate(watchface_second, 33, -1, img_animate_watchface_callback, NULL);

    curtainview_menu = gui_curtainview_create(parent, "curtainview_menu", 0, 0, 454, 454);
    void design_curtain_menu(void *parent);
    design_curtain_menu(curtainview_menu);
//Get the current real-time time and print it
//    struct tm *ptr;
//    time_t lt;
//    lt = time(NULL);
//    ptr = localtime(&lt);
//    printf(asctime(ptr));

//    int tm_sec = ptr->tm_sec;
//    int tm_min = ptr->tm_min;
//    int tm_hour = ptr->tm_hour;
//    printf("time: %d:%d:%d \n", tm_hour, tm_min, tm_sec);
}
