/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "gui_img.h"
#include "tp_algo.h"
#include "gui_view.h"
#include "root_image/ui_resource.h"
#include "gui_view_macro.h"

/*============================================================================*
 *                            Macros
 *============================================================================*/
#define TAB_START_POS_X 160
#define TAB_START_POS_Y 102
#define TAB_STOP_POS_Y  310
#define TAB_SIZE        80

/*============================================================================*
 *                           Function Declaration
 *============================================================================*/
void lock_view_design(gui_view_t *view);

/*============================================================================*
 *                            Variables
 *============================================================================*/

/*============================================================================*
 *                           Private Functions
 *============================================================================*/
static void pressing_tab(void *obj)
{
    GUI_UNUSED(obj);

    touch_info_t *tp = tp_get_info();
    gui_img_t *img = (gui_img_t *)obj;
    if (tp->pressing && tp->x >= TAB_START_POS_X && tp->x <= (TAB_START_POS_X + TAB_SIZE)
        && tp->y >= TAB_START_POS_Y && tp->y <= (TAB_START_POS_Y + TAB_SIZE))
    {
        int16_t y = tp->deltaY;
        if (y < 0)
        {
            y = 0;
        }
        else if (y > TAB_STOP_POS_Y - TAB_START_POS_Y)
        {
            y = TAB_STOP_POS_Y - TAB_START_POS_Y;
        }
        gui_img_translate(img, 0, (float)y);
    }
    else if (img->t_y != 0)
    {
        int16_t y = (int16_t)img->t_y;
        const int16_t step = 20;
        if (y >= TAB_STOP_POS_Y - TAB_START_POS_Y)
        {
            gui_view_switch_direct(gui_view_get_current(), "charging", SWITCH_OUT_NONE_ANIMATION,
                                   SWITCH_IN_ANIMATION_FADE);
            gui_obj_stop_timer(GUI_BASE(img));
            return;
        }
        else if (y >= (TAB_STOP_POS_Y - TAB_START_POS_Y - 40))
        {
            y += step;
        }
        else
        {
            y -= step;
        }
        if (y <= 0)
        {
            y = 0;
            gui_img_set_image_data(img, (void *)FILE_POINTER(ICON_PRESS_BIN));
            gui_obj_stop_timer(GUI_BASE(img));
        }
        else if (y >= TAB_STOP_POS_Y - TAB_START_POS_Y)
        {
            y = TAB_STOP_POS_Y - TAB_START_POS_Y;
        }
        gui_img_translate(img, 0, (float)y);
    }
    else
    {
        gui_img_set_image_data(img, (void *)FILE_POINTER(ICON_PRESS_BIN));
        gui_obj_stop_timer(GUI_BASE(img));
    }
}

static void tab_scale(void *obj)
{
    static uint8_t cnt = 0;
    cnt++;
    uint8_t cnt_max = 5;
    gui_img_t *img = (gui_img_t *)obj;
    float scale = 0.8f + 0.2f * cnt / (float)cnt_max;
    gui_img_scale(img, scale, scale);
    if (cnt >= cnt_max)
    {
        cnt = 0;
        gui_obj_create_timer(GUI_BASE(img), 10, true, pressing_tab);
    }
}

static void press_tab(void *obj, gui_event_t e, void *param)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);
    GUI_UNUSED(param);

    gui_img_t *img = (gui_img_t *)obj;
    gui_img_set_image_data(img, (void *)FILE_POINTER(ICON_PRESSING_BIN));
    gui_img_scale(img, 0.8f, 0.8f);
    gui_obj_create_timer(GUI_BASE(img), 10, true, tab_scale);
    gui_obj_start_timer(GUI_BASE(img));
}

void lock_view_design(gui_view_t *view)
{
    gui_obj_t *parent = GUI_BASE(view);
    gui_img_t *bg = gui_img_create_from_mem(parent, "bg", (void *)FILE_POINTER(LOCK_SCROLL_BAR_BIN),
                                            165, 107, 0, 0);
    gui_img_set_mode(bg, IMG_SRC_OVER_MODE);

    int16_t focus = 45;
    gui_img_t *tab = gui_img_create_from_mem(parent, "tab_bg", (void *)FILE_POINTER(ICON_PRESS_BIN),
                                             TAB_START_POS_X + focus,
                                             TAB_START_POS_Y + focus, 0, 0);
    gui_img_set_focus(tab, focus, focus);
    gui_img_set_mode(tab, IMG_SRC_OVER_MODE);
    gui_obj_add_event_cb(tab, press_tab, GUI_EVENT_TOUCH_PRESSED, NULL);

    gui_img_create_from_mem(parent, 0, (void *)FILE_POINTER(LOCK_OPEN_BIN), 187, 339, 0, 0);
}