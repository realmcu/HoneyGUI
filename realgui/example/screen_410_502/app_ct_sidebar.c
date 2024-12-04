#include "root_image_hongkong/ui_resource.h"
#include <gui_img.h>
#include "gui_canvas.h"
#include "gui_win.h"
#include "gui_obj.h"
#include "gui_app.h"
#include "gui_canvas_rect.h"
#include "gui_canvas_round_rect.h"
#include "gui_page.h"

#define SCREEN_WIDTH 410
#define SCREEN_HEIGHT 502
#define SCREEN_X_OFF 21
#define SCREEN_Y_OFF 18
#define APP_NUM 4

typedef struct gui_sidebar_app
{
    void *img_addr;
    gui_event_cb_t callback_function;
} gui_sidebar_app_t;

gui_sidebar_app_t sidebar_app_array[APP_NUM] = {0};

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
}
