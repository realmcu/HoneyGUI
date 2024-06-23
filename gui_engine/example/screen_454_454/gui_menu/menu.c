#include "root_image/ui_resource.h"
#include "gui_curtainview.h"
#include "gui_curtain.h"
#include "gui_img.h"
#include "gui_text.h"
#include "gui_switch.h"
#include "gui_win.h"
#include "app_gui_main.h"
#include "gui_tabview.h"
#include "gui_obj.h"
#include "gui_img_scope.h"
#include "gui_common.h"
#include "gui_page.h"
#include "gui_grid.h"
#include "gui_button.h"
#include "gui_app.h"
#include "gui_progressbar.h"
#include "tp_algo.h"
#include "gui_canvas_rect.h"
static void press_callback(gui_win_t *button);
static void press_animate_cb(gui_win_t *button);
static void release_callback(gui_win_t *button);
static void release_animate_callback(gui_win_t *button);
static void deal_win_in_page(gui_obj_t *object, int page_y);
static void page_callback(gui_page_t *page);
static void heart_rate_cb(void);
static void menu_cb(void);
static void stopwatch_cb(void);
static void map_cb(void);
static void card_cb(void);
static void box2d_cb(void);
static void compass_cb(void);
static void volume_cb(void);
static void cycle_tracking_cb(void);
static int page_y_recode;
static void page_dtor(gui_obj_t *obj);
static gui_progressbar_t *pro;
static int window_index;
#define MENU_GAP 74
#define SCROLLBAR_SIZE 14
const static void *scrollbar_array[SCROLLBAR_SIZE];

static void gui_page_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_INPUT_PREPARE:
            {
                extern void gui_page_input_prepare(gui_obj_t *obj);
                gui_page_input_prepare(obj);
            }
            break;

        case OBJ_PREPARE:
            {
                extern void gui_page_update_rebound(gui_obj_t *obj);
                gui_page_update_rebound(obj);
            }
            break;

        case OBJ_DESTORY:
            page_dtor(obj);
            break;

        default:
            break;
        }
    }
}

void design_tab_menu(void *parent)
{
    gui_canvas_rect_create(parent, "canvas_rect", 0, 0, gui_get_screen_width(), gui_get_screen_height(),
                           gui_rgba(UINT8_MAX,
                                    UINT8_MAX, UINT8_MAX, 100));
    gui_page_t *page = gui_page_create(parent, 0, 0, 0, 0, 0);
    //gui_page_rebound(page, 1);
    GET_BASE(page)->obj_cb = gui_page_cb;
    GET_BASE(page)->has_input_prepare_cb = true;
    GET_BASE(page)->has_prepare_cb = true;
    GET_BASE(page)->has_destroy_cb = true;
    GET_BASE(page)->y = page_y_recode;
    page_y_recode = 0;
    gui_page_set_animate(page, 1000, -1, page_callback, page);
    pro = gui_progressbar_movie_create(parent, (void *)scrollbar_array, SCROLLBAR_SIZE, 435, 200);
    gui_img_set_mode((void *)pro->c, IMG_SRC_OVER_MODE);
    //gui_page_rebound(page, true);
    gui_page_center_alignment(page, MENU_GAP);
    static void *array[] =
    {
        ICON_MENU_BIN,
        ICON_MENU_BIN,
        ICON_MENU_BIN,
        ICON_MENU_BIN,
        ICON_MENU_BIN,
        ICON_MENU_BIN,
        ICON_MENU_BIN,
        ICON_MENU_BIN,
        ICON_MENU_BIN,
        ICON_MENU_BIN,
        ICON_MENU_BIN,
        ICON_MENU_BIN,
    };
    static char *text_array[] =
    {
        "Heart Rate",
        "Menu",
        "Stopwatch",
        "Map",
        "CARD",
        "box2d",
        "Compass",
        "Volume",
        "Cycle tracking",
        "Sport",
        "Watch Face",
        "Calculator",
        "Sport",
    };
    int array_size = sizeof(array) / sizeof(array[0]);
    static gui_win_t *button_array[sizeof(array) / sizeof(array[0])];
    gui_grid_t *grid = gui_grid_create(page, 27, 0, array_size, 1, 0, MENU_GAP);
    for (size_t i = 0; i < array_size; i++)
    {

        gui_win_t *button = gui_win_create(grid, 0, 0, 0, 400, 70);
        gui_win_press(button, press_animate_cb, button);
        gui_win_release(button, release_animate_callback, button);
        //
        {
            gui_img_t *img = gui_img_create_from_mem(button, 0, RECT1_BIN, 0, 0, 0, 0);
            gui_img_set_mode(img, IMG_SRC_OVER_MODE);
        }
        {
            gui_img_t *img = gui_img_create_from_mem(button, 0, array[i], 3, 3, 0, 0);
            gui_img_set_mode(img, IMG_SRC_OVER_MODE);
        }
        button_array[i] = button;
        char *text = text_array[i];
        int font_size = 16;
        gui_text_t *t = gui_text_create(button, "txt", 70, 27, gui_get_screen_width(), font_size);
        if (i < 9)
        {
            gui_text_set(t, text, GUI_FONT_SRC_BMP, gui_rgb(42, 5, 79), strlen(text), font_size);
        }
        else
        {
            gui_text_set(t, text, GUI_FONT_SRC_BMP, gui_rgb(200, 200, 200), strlen(text), font_size);
        }
        void *addr1 = ARIALBD_SIZE16_BITS4_FONT_BIN;
        gui_text_type_set(t, addr1);
    }
    gui_win_click(button_array[0], heart_rate_cb,     button_array[0]);
    gui_win_click(button_array[1], menu_cb,           button_array[1]);
    gui_win_click(button_array[2], stopwatch_cb,      button_array[2]);
    gui_win_click(button_array[3], map_cb,            button_array[3]);
    gui_win_click(button_array[4], card_cb,           button_array[4]);
    gui_win_click(button_array[5], box2d_cb,          button_array[5]);
    gui_win_click(button_array[6], compass_cb,        button_array[6]);
    gui_win_click(button_array[7], volume_cb,         button_array[7]);
    gui_win_click(button_array[8], cycle_tracking_cb, button_array[8]);
}

static void press_callback(gui_win_t *button)
{
    touch_info_t *tp = tp_get_info();
    static bool first_flag;
    static float current_scale;
    if (!first_flag)
    {
        first_flag = true;
        current_scale = 1;
    }
    /**
     * @brief Shrunk current scale to 0.7 in animation duration
     *  y = (0.7-scale)x+scale;
     */
    float scale = (0.7f - current_scale) * button->animate->progress_percent + current_scale;
    if (tp->deltaX == 0)
    {
        matrix_translate(GET_BASE(button)->w / 2, GET_BASE(button)->h / 2, GET_BASE(button)->matrix);
        matrix_scale(scale, scale, GET_BASE(button)->matrix);
        matrix_translate(GET_BASE(button)->w / -2, GET_BASE(button)->h / -2, GET_BASE(button)->matrix);
    }
    if (button->animate->progress_percent == 1)
    {
        first_flag = false;
    }

}
static void press_animate_cb(gui_win_t *button)
{
    gui_win_set_animate(button, 1000, 0, press_callback, button);
}
static void release_callback(gui_win_t *button)
{
    static bool first_flag = false;
    static float current_scale;
    if (!first_flag)
    {
        first_flag = true;
        current_scale = GET_BASE(button)->matrix->m[0][0];
    }
    /**
     * @brief Shrunk current scale to 1 in animation duration
     *  y = (1-scale)x+scale;
     */
    float scale = (1 - current_scale) * button->animate->progress_percent + current_scale;
    {
        matrix_translate(GET_BASE(button)->w / 2, GET_BASE(button)->h / 2, GET_BASE(button)->matrix);
        matrix_scale(1, 1, GET_BASE(button)->matrix);
        matrix_translate(GET_BASE(button)->w / -2, GET_BASE(button)->h / -2, GET_BASE(button)->matrix);
    }
    if (button->animate->progress_percent == 1)
    {
        first_flag = false;
    }
}
static void release_animate_callback(gui_win_t *button)
{
    gui_win_set_animate(button, 1000, 0, release_callback, button);
}
static void deal_win_in_page(gui_obj_t *object, int page_y)
{
    gui_list_t *node = NULL;
    gui_list_for_each(node, &object->child_list)
    {
        gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);
        if (obj->type == WINDOW)
        {
            int ay = window_index * MENU_GAP + page_y;
            window_index++;
            if (ay >= (float)(0 - obj->h) &&
                ay < (float)gui_get_screen_height())
            {
                float scale = (1 - 0.5f * _UI_ABS(((float)(ay + (float)(obj->h / 2)) -
                                                   (float)gui_get_screen_height() / 2) / (float)(gui_get_screen_height() / 2)));
                GUI_TYPE(gui_win_t, obj)->scale = scale;
                obj->not_show = false;
            }
            else
            {
                obj->not_show = true;
            }
        }
        deal_win_in_page(obj, page_y);
    }
}
const static void *scrollbar_array_top[] =
{
    BARS1_BIN,
    BARS2_BIN,
    BARS3_BIN,
};
const static void *scrollbar_array_buttom[] =
{
    BAR40_BIN,
    BAR41_BIN,
    BAR42_BIN,
    BAR43_BIN,
};
const static void *scrollbar_array[SCROLLBAR_SIZE] =
{
    BAR1_BIN,  BAR4_BIN,

    BAR7_BIN,

    BAR10_BIN,

    BAR13_BIN,

    BAR16_BIN,

    BAR19_BIN,

    BAR22_BIN,

    BAR25_BIN,

    BAR28_BIN,

    BAR31_BIN,

    BAR34_BIN,

    BAR38_BIN,
    BAR39_BIN,
};
static void page_callback(gui_page_t *page)
{
    window_index = 0;
    deal_win_in_page((void *)page, GUI_BASE(page)->y);



    // gui_log("%f,%d,%d\n", ((float)(page->start_y - GET_BASE(page)->y)) / (float)(GET_BASE(
    //         page)->h - gui_get_screen_height()), page->start_y, GET_BASE(page)->y);
    float progress = ((float)(page->start_y - GET_BASE(page)->y)) / (float)(GET_BASE(
                                                                                page)->h - gui_get_screen_height());
    if (pro)
    {
        if (progress >= 0 && progress <= 1)
        {
            {
                pro->color_hl = (uint32_t)(uintptr_t)scrollbar_array;
                pro->max = sizeof(scrollbar_array) / sizeof(void *);
            }
            gui_progressbar_set_percentage(pro, progress) ;
        }
        else if (progress < 0)
        {
            {
                pro->color_hl = (uint32_t)(uintptr_t)scrollbar_array_top;
                pro->max = sizeof(scrollbar_array_top) / sizeof(void *);
            }
            float progress_top = -progress;
            gui_progressbar_set_percentage(pro, progress_top) ;
        }
        else if (progress > 1)
        {
            {
                pro->color_hl = (uint32_t)(uintptr_t)scrollbar_array_buttom;
                pro->max = sizeof(scrollbar_array_buttom) / sizeof(void *);
            }
            float progress_buttom = progress - 1;
            gui_progressbar_set_percentage(pro, progress_buttom) ;
        }
    }


}
static void heart_rate_cb()
{
    //GUI_APP_SWAP_HANDLE(get_app_watch_ui(), GUI_APP_HANDLE(APP_HEART_RATE))
    GUI_APP_STARTUP(APP_HEART_RATE) // cppcheck-suppress unknownMacro
    gui_app_layer_top();
}
static void menu_cb()
{
    GUI_APP_SWAP_HANDLE(get_app_watch_ui(), GUI_APP_HANDLE(APP_MENU))
}
static void stopwatch_cb()
{
    GUI_APP_SWAP_HANDLE(get_app_watch_ui(), GUI_APP_HANDLE(APP_STOPWATCH))
}
static void map_cb()
{
    GUI_APP_SWAP_HANDLE(get_app_watch_ui(), GUI_APP_HANDLE(APP_MAP))
}
static void card_cb()
{
    GUI_APP_SWAP_HANDLE(get_app_watch_ui(), GUI_APP_HANDLE(APP_CARDVIEW))
}
static void box2d_cb()
{
    GUI_APP_SWAP_HANDLE(get_app_watch_ui(), GUI_APP_HANDLE(APP_BOX2D))
}
static void compass_cb()
{
    GUI_APP_SWAP_HANDLE(get_app_watch_ui(), GUI_APP_HANDLE(APP_COMPASS))
}
static void volume_cb()
{
    GUI_APP_SWAP_HANDLE(get_app_watch_ui(), GUI_APP_HANDLE(APP_VOLUME))
}
static void cycle_tracking_cb()
{
    GUI_APP_SWAP_HANDLE(get_app_watch_ui(), GUI_APP_HANDLE(APP_CYCLE_TRACKING))
}
// static void calculator_cb()
// {
//     GUI_APP_SWAP_HANDLE(get_app_watch_ui(), GUI_APP_HANDLE(APP_CALCULATOR))
// }
// static void sport_cb()
// {
//     GUI_APP_SWAP_HANDLE(get_app_watch_ui(), GUI_APP_HANDLE(APP_SPORT))
// }
static void page_dtor(gui_obj_t *obj)
{
    extern void gui_page_destory(gui_obj_t *obj);
    gui_page_destory(obj);
    page_y_recode = obj->y;
}
