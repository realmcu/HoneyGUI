#include "root_image_hongkong/ui_resource.h"
#include <gui_img.h>
#include "gui_canvas.h"
#include "gui_win.h"
#include "gui_obj.h"
#include "gui_app.h"

// static void canvas_cb(gui_canvas_t *canvas)
// {
//     nvgRect(canvas->vg, 0, 0, 368, 448);
//     nvgFillColor(canvas->vg, nvgRGBA(0, 0, 0, 200));
//     nvgFill(canvas->vg);
// }

// static void touch_long_cb(void *obj, gui_event_t e)
// {
//     //gui_app_shutdown(gui_current_app());
//     gui_log("line = %d \n", __LINE__);
//     gui_mem_debug();
//     gui_log("line = %d \n", __LINE__);
// }

// void  page_left_figure(void *parent_widget)
// {

//     gui_win_t *parent = gui_win_create(parent_widget, NULL, 50, 30, 0, 0);
//     gui_img_t *time =  gui_img_create_from_mem(parent, "time", TIME_BIN, 142, 20, 0, 0);
//     gui_img_t *data =  gui_img_create_from_mem(parent, "data", DATA_BIN, 132, 64, 0, 0);
//     gui_img_t *week =  gui_img_create_from_mem(parent, "week", SUN_BIN, 213, 64, 0, 0);

//     gui_img_t *figure_bk0 =  gui_img_create_from_mem(parent, "figure_bk0", RECT_110_108_BIN,
//                                                      82, 98, 0, 0);
//     gui_img_set_opacity(figure_bk0, 200);
//     gui_img_t *sky =  gui_img_create_from_mem(parent, "sky", SKY1_BIN, 123, 112, 0, 0);
//     gui_img_t *centigrade0 =  gui_img_create_from_mem(parent, "centigrade", ASSEMBLY_BIN,
//                                                       106, 183, 0, 0);
//     gui_img_t *centigrade1 =  gui_img_create_from_mem(parent, "centigrade", ASSEMBLY_BIN,
//                                                       124, 183, 0, 0);
//     gui_img_t *centigrade2 =  gui_img_create_from_mem(parent, "centigrade", CENTIGRADE_BIN,
//                                                       132, 166, 0, 0);

//     gui_img_t *map =  gui_img_create_from_mem(parent, "siri", MAP_BIN, 202, 98, 0, 0);

//     gui_img_t *figure_bk1 =  gui_img_create_from_mem(parent, "figure_bk1", RECT_228_64_BIN,
//                                                      82, 214, 0, 0);
//     gui_img_set_opacity(figure_bk1, 200);
//     gui_img_t *heart_rate =  gui_img_create_from_mem(parent, "heart_rate", HEART_RATE_BIN,
//                                                      98, 225, 0, 0);
//     gui_img_t *sound =  gui_img_create_from_mem(parent, "sound", SOUND_RECORDING_BIN, 150,
//                                                 225, 0, 0);
//     gui_img_t *sport =  gui_img_create_from_mem(parent, "sport", SPORT_BIN, 202, 225, 0, 0);
//     gui_img_t *ring =  gui_img_create_from_mem(parent, "ring", RING_BIN, 254, 225, 0, 0);

//     gui_img_t *figure_bk2 =  gui_img_create_from_mem(parent, "figure_bk2", SOUND_BIN, 72,
//                                                      319, 0, 0);
//     gui_img_t *siri =  gui_img_create_from_mem(parent, "siri", SIRI_BIN, 161, 297, 0, 0);

//     gui_obj_add_event_cb(siri, (gui_event_cb_t)touch_long_cb, GUI_EVENT_TOUCH_CLICKED, NULL);

// }
// void page_ct_sidebar(void *parent)
// {
//     gui_canvas_t *canvas = gui_canvas_create(parent, "canvas", 0, 0, 0, 368, 448);
//     gui_canvas_set_canvas_cb(canvas, canvas_cb);
//     page_left_figure(parent);
// }

#include "gui_canvas_rect.h"
#include "gui_canvas_round_rect.h"
#include "gui_page.h"

#define SCREEN_WIDTH 368   //410
#define SCREEN_HEIGHT 448  //502
#define SCREEN_X_OFF 0     //21
#define SCREEN_Y_OFF 0     //18
#define APP_NUM 4

typedef struct gui_sidebar_app
{
    void *img_addr;
    gui_event_cb_t callback_function;
} gui_sidebar_app_t;

gui_sidebar_app_t sidebar_app_array[APP_NUM] = {0};

extern void switch_BOX2D_RING();
extern void switch_APP_FRUIT_NINJA();
extern void switch_APP_MUSIC();
extern void switch_heart_rate();

void sidebar_app_array_fill(void *img_addr, gui_event_cb_t callback_function)
{
    uint8_t index;
    for (index = 0; index < APP_NUM;)
    {
        if (sidebar_app_array[index++].img_addr == img_addr)
        {
            break;
        }
    }
    index--;
    gui_log("index = %d\n", index);
    while (1)
    {
        if (index == 0)
        {
            break;
        }
        sidebar_app_array[index].callback_function = sidebar_app_array[index - 1].callback_function;
        sidebar_app_array[index].img_addr = sidebar_app_array[index - 1].img_addr;
        index--;
    }
    sidebar_app_array[0].callback_function = callback_function;
    sidebar_app_array[0].img_addr = img_addr;
}

void page_ct_sidebar(void *parent)
{
    gui_canvas_round_rect_t *canvas_timecard = gui_canvas_round_rect_create(GUI_BASE(parent), "",
                                                                            0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 20, gui_rgba(39, 43, 44, 255 * 0.7));

    extern void switch_BOX2D_RING();
    extern void switch_APP_FRUIT_NINJA();
    extern void switch_APP_MUSIC();
    extern void switch_heart_rate();

    gui_win_t *win = gui_win_create(parent, NULL, 0, 0, 0, 0);
    uint8_t index = 0;
    while (1)
    {
        if (sidebar_app_array[index].img_addr)
        {
            gui_img_t *img = gui_img_create_from_mem(win, 0, sidebar_app_array[index].img_addr,
                                                     SCREEN_WIDTH - 120, 10 + 105 * index, 0, 0);
            gui_img_set_mode(img, IMG_SRC_OVER_MODE);
            gui_obj_add_event_cb(GUI_BASE(img), sidebar_app_array[index].callback_function, GUI_EVENT_1, NULL);
            index++;
        }
        else
        {
            break;
        }
    }

    // gui_img_t *img = gui_img_create_from_mem(win, 0, UI_CLOCK_HEARTRATE_ICON_BIN, SCREEN_WIDTH - 120, 10, 0, 0);
    // gui_img_set_mode(img, IMG_SRC_OVER_MODE);
    // gui_obj_add_event_cb(GUI_BASE(img), switch_heart_rate, GUI_EVENT_1, NULL);
    // img = gui_img_create_from_mem(win, 0, UI_CLOCK_MUSIC_ICON_BIN, SCREEN_WIDTH - 120, 10 + 105, 0, 0);
    // gui_img_set_mode(img, IMG_SRC_OVER_MODE);
    // gui_obj_add_event_cb(GUI_BASE(img), switch_APP_MUSIC, GUI_EVENT_1, NULL);
    // img = gui_img_create_from_mem(win, 0, UI_CLOCK_FRUIT_NINJA_ICON_BIN, SCREEN_WIDTH - 120, 10 + 105 * 2, 0, 0);
    // gui_img_set_mode(img, IMG_SRC_OVER_MODE);
    // gui_obj_add_event_cb(GUI_BASE(img), switch_APP_FRUIT_NINJA, GUI_EVENT_1, NULL);
    // img = gui_img_create_from_mem(win, 0, UI_CLOCK_BOX2D_RING_ICON_BIN, SCREEN_WIDTH - 120, 10 + 105 * 3, 0, 0);
    // gui_img_set_mode(img, IMG_SRC_OVER_MODE);
    // gui_obj_add_event_cb(GUI_BASE(img), switch_BOX2D_RING, GUI_EVENT_1, NULL);
}
