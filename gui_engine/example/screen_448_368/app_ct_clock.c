#include "root_image_hongkong/ui_resource.h"
#include <gui_magic_img.h>
#include "gui_win.h"
#include "gui_watch_gradient_spot.h"
#include "gui_text.h"
#include "gui_watchface_gradient.h"
gui_win_t *win_watch;
gui_magic_img_t *img;
gui_watch_gradient_spot_t *watch;
gui_text_t *rate;
static gui_watchface_gradient_t *canvas;
static void callback_watchface()
{
    static char clock;
    if (clock == 1)
    {

        GET_BASE(img)->not_show = false;
        GET_BASE(watch)->not_show = true;
        GET_BASE(canvas)->not_show = true;
        clock = 2;
    }
    else if (clock == 2)
    {
        GET_BASE(img)->not_show = true;
        GET_BASE(watch)->not_show = false;
        GET_BASE(canvas)->not_show = true;
        clock = 0;
    }
    else if (clock == 0)
    {
        GET_BASE(img)->not_show = true;
        GET_BASE(watch)->not_show = true;
        GET_BASE(canvas)->not_show = false;
        clock = 1;
    }

}

static void text_cb()
{
    int32_t p = (int32_t)(rate->animate->progress_percent * 100) % 21 + 80;
    static char str[10];
    sprintf(str, "%03d", p);
    gui_text_set(rate, str, "rtk_font_mem", 0xffcfcfcf, 3, 32);
}
extern void callback_interface(void *obj, gui_event_t e);
void page_ct_clock(void *parent)
{
    win_watch = gui_win_create(parent, "win", 0, 0, 368, 448);
    gui_obj_add_event_cb(win_watch, (gui_event_cb_t)callback_watchface, GUI_EVENT_TOUCH_CLICKED, NULL);
    gui_obj_add_event_cb(win_watch, (gui_event_cb_t)callback_interface, GUI_EVENT_TOUCH_LONG, NULL);
    // img = gui_magic_img_create_from_mem(parent, "page0", CLOCKN_BIN, 0, 0, 0, 0);
    img = gui_magic_img_create_from_mem(parent, "page0", BACKGROUND_BIN, 0, 0, 0, 0);
    watch = gui_watch_gradient_spot_create(win_watch, "watchface", 0, 0, 0, 0);
    gui_watch_gradient_spot_set_center(watch, 368 / 2, 448 / 2);
    canvas = gui_watchface_gradient_create(parent, "watchface_gradient", (368 - 368) / 2,
                                           (448 - 448) / 2, 368, 448);
    GET_BASE(watch)->not_show = true;
    GET_BASE(canvas)->not_show = true;

    static void *Heart_list[] = {R1_BIN, R2_BIN, R3_BIN, R4_BIN, R5_BIN, R6_BIN, R7_BIN, R8_BIN, R9_BIN, R10_BIN,
                                 R11_BIN, R12_BIN, R13_BIN, R14_BIN, R15_BIN, R16_BIN, R17_BIN, R18_BIN, R19_BIN, R20_BIN
                                };
    gui_dynamic_create_from_mem(img, "heart", Heart_list, 45, 54, 20, 100, 100 * 10000);
    gui_font_mem_init(DINCOND_MEDIUM_SIZE32_BITS8_FONT_BIN);
    rate = gui_text_create(img,  "rate", 45, 90, 200, 200);
    gui_text_set_animate(rate, 10000, -1, text_cb, NULL);
    gui_text_type_set(rate, DINCOND_MEDIUM_SIZE32_BITS8_FONT_BIN);
    gui_text_mode_set(rate, LEFT);
}

