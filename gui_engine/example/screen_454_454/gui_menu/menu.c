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
static void press_callback(gui_win_t *button);
static void press_animate_cb(gui_win_t *button);
static void release_callback(gui_win_t *button);
static void release_animate_callback(gui_win_t *button);
static void deal_win_in_page(gui_obj_t *object);
static void page_callback(gui_page_t *page);
void design_tab_menu(void *parent)
{
    gui_rect(parent, 0, 0, gui_get_screen_width(), gui_get_screen_height(), gui_rgba(UINT8_MAX,
             UINT8_MAX, UINT8_MAX, 100));
    gui_page_t *page = gui_page_create(parent, 0, 0, 0, 0, 0);
    gui_page_set_animate(page, 1000, -1, page_callback, page);
    //gui_page_rebound(page, true);
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
    int array_size = sizeof(array) / sizeof(array[0]);
    static gui_win_t *button_array[sizeof(array) / sizeof(array[0])];
    gui_grid_t *grid = gui_grid_create(page, 27, 0, array_size, 1, 0, 74);
    for (size_t i = 0; i < array_size; i++)
    {

        gui_win_t *button = gui_win_create(grid, 0, 0, 0, 400, 70);
        gui_win_onPress(button, press_animate_cb, button);
        gui_win_onRelease(button, release_animate_callback, button);
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
        static char text_array[24][6];
        sprintf(text_array[i], "APP%d", (int)i);
        char *text = text_array[i];
        int font_size = 16;
        gui_text_t *t = gui_text_create(button, "txt", 70, 27, gui_get_screen_width(), font_size);
        gui_text_set(t, text, GUI_FONT_SRC_BMP, gui_rgb(42, 5, 79), strlen(text), font_size);
        void *addr1 = ARIALBD_SIZE16_BITS4_FONT_BIN;
        gui_font_mem_init(addr1);
        gui_text_type_set(t, addr1);
    }

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
    float scale = (0.7 - current_scale) * button->animate->progress_percent + current_scale;
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
static void deal_win_in_page(gui_obj_t *object)
{
    gui_list_t *node = NULL;
    gui_list_for_each(node, &object->child_list)
    {
        gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);
        if (obj->type == WINDOW)
        {
            if (obj->matrix->m[1][2] >= (float)(0 - obj->h) &&
                obj->matrix->m[1][2] < (float)gui_get_screen_height())
            {
                float scale = (1 - 0.5 * _UI_ABS(((float)(obj->matrix->m[1][2] + (float)(obj->h / 2)) -
                                                  (float)gui_get_screen_height() / 2) / (float)(gui_get_screen_height() / 2)));
                GUI_TYPE(gui_win_t, obj)->scale = scale;
                obj->not_show = false;
            }
            else
            {
                obj->not_show = true;
            }
        }
        deal_win_in_page(obj);
    }
}
static void page_callback(gui_page_t *page)
{
    deal_win_in_page(page);



    // gui_log("%f,%d,%d\n", ((float)(page->start_y - GET_BASE(page)->y)) / (float)(GET_BASE(
    //         page)->h - gui_get_screen_height()), page->start_y, GET_BASE(page)->y);
    // float progress = ((float)(page->start_y - GET_BASE(page)->y)) / (float)(GET_BASE(
    //                                                                             page)->h - gui_get_screen_height());
    // if (pro)
    // {
    //     if (progress >= 0 && progress <= 1)
    //     {
    //         {
    //             pro->color_hl = (uint32_t)scrollbar_array;
    //             pro->max = sizeof(scrollbar_array) / sizeof(void *);
    //         }
    //         gui_progressbar_set_percentage(pro, progress) ;
    //     }
    //     else if (progress < 0)
    //     {
    //         {
    //             pro->color_hl = (uint32_t)scrollbar_array_top;
    //             pro->max = sizeof(scrollbar_array_top) / sizeof(void *);
    //         }
    //         float progress_top = -progress;
    //         gui_progressbar_set_percentage(pro, progress_top) ;
    //     }
    //     else if (progress > 1)
    //     {
    //         {
    //             pro->color_hl = (uint32_t)scrollbar_array_buttom;
    //             pro->max = sizeof(scrollbar_array_buttom) / sizeof(void *);
    //         }
    //         float progress_buttom = progress - 1;
    //         gui_progressbar_set_percentage(pro, progress_buttom) ;
    //     }
    // }


}
