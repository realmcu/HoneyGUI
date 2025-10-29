/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "app_main.h"
#include "common_data.h"
#include "gui_img.h"
#include "gui_win.h"
#include "gui_text.h"
#include "tp_algo.h"

/*============================================================================*
 *                            Macros
 *============================================================================*/
#define CURRENT_VIEW_NAME "find_buds_view"

#define ICON_START_POS_X 6
#define ICON_START_POS_Y 6
#define ICON_STOP_POS_X  212
#define ICON_SIZE        78
#define ARROW_POS_START   104
#define ARROW_POS_INTERVAL   40

/*============================================================================*
 *                           Function Declaration
 *============================================================================*/
static void find_buds_view_design(gui_view_t *view);
static void clear(gui_view_t *view);
static void win_start_buzz_design(void);

/*============================================================================*
 *                            Variables
 *============================================================================*/
/* View Management */
static gui_view_t *current_view = NULL;
static gui_view_descriptor_t const descriptor =
{
    /* change Here for current view */
    .name = (const char *)CURRENT_VIEW_NAME,
    .pView = &current_view,
    .on_switch_in = find_buds_view_design,
    .on_switch_out = clear
};

static const gui_view_descriptor_t *descriptor_rec = NULL;
static char *text_str = NULL;

/*============================================================================*
 *                           Private Functions
 *============================================================================*/
static int gui_view_descriptor_register_init(void)
{
    gui_view_descriptor_register(&descriptor);
    gui_log("File: %s, Function: %s\n", __FILE__, __func__);
    return 0;
}
static GUI_INIT_VIEW_DESCRIPTOR_REGISTER(gui_view_descriptor_register_init);


static void timer_cb(void *p)
{
    GUI_UNUSED(p);
    static uint8_t cnt = 0;
    uint8_t cnt_max = 5; //5s
    cnt++;
    gui_obj_t *win_wait = gui_list_entry(current_view->base.child_list.next, gui_obj_t, brother_list);
    gui_obj_t *control = gui_list_entry(win_wait->child_list.prev, gui_obj_t, brother_list);
    gui_obj_t *text = gui_list_entry(control->child_list.next, gui_obj_t, brother_list);
    sprintf(text_str + 24, "%d", cnt_max - cnt);
    gui_text_content_set((void *)text, text_str + 24, 2);
    if (cnt >= cnt_max)
    {
        cnt = 0;
        gui_obj_tree_free(win_wait);
        win_start_buzz_design();
        gui_obj_stop_timer((void *)p);
    }
}

static void click_cancel(void *obj, gui_event_t e, void *param)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);
    GUI_UNUSED(param);
    gui_view_set_animate_step(current_view, 400);
    gui_view_switch_direct(current_view, descriptor_rec, SWITCH_OUT_NONE_ANIMATION,
                           SWITCH_IN_NONE_ANIMATION);
}

static void show_hide_arrow(gui_obj_t *obj, int16_t deltaX)
{
    uint8_t num = 0;
    int16_t compare = ARROW_POS_START - ICON_START_POS_X - ICON_SIZE + 14;
    uint8_t i = 0;
    while (deltaX > compare)
    {
        num++;
        compare += ARROW_POS_INTERVAL;
    }
    for (i = 0; i < num; i++)
    {
        obj = gui_list_entry(obj->brother_list.next, gui_obj_t, brother_list);
        gui_obj_hidden(obj, true);
    }

    for (; i < 5; i++)
    {
        obj = gui_list_entry(obj->brother_list.next, gui_obj_t, brother_list);
        gui_obj_hidden(obj, false);
    }
}

static void pressing_icon(void *obj)
{
    GUI_UNUSED(obj);

    touch_info_t *tp = tp_get_info();
    gui_img_t *img = (gui_img_t *)obj;
    if (tp->pressing && tp->x >= (ICON_START_POS_X + 12) && tp->x <= (ICON_START_POS_X + 12 + ICON_SIZE)
        && tp->y >= (ICON_START_POS_Y + 92) && tp->y <= (ICON_START_POS_Y + 92 + ICON_SIZE))
    {
        int16_t x = tp->deltaX;
        if (x < 0)
        {
            x = 0;
        }
        else if (x > ICON_STOP_POS_X - ICON_START_POS_X)
        {
            x = ICON_STOP_POS_X - ICON_START_POS_X;
        }
        gui_img_translate(img, (float)x, 0);
        show_hide_arrow((void *)obj, x);
    }
    else if (img->t_x != 0)
    {
        int16_t x = (int16_t)img->t_x;
        const int16_t step = 20;
        if (x >= ICON_STOP_POS_X - ICON_START_POS_X)
        {
            f_status.earbuds_shock_l = f_status.earbuds_find_l;
            f_status.earbuds_shock_r = f_status.earbuds_find_r;
            gui_view_switch_direct(current_view, descriptor_rec, SWITCH_OUT_NONE_ANIMATION,
                                   SWITCH_IN_ANIMATION_FADE);
            gui_obj_stop_timer(GUI_BASE(img));
            return;
        }
        else if (x >= (ICON_STOP_POS_X - ICON_START_POS_X - 40))
        {
            x += step;
        }
        else
        {
            x -= step;
        }
        if (x <= 0)
        {
            x = 0;
        }
        else if (x >= ICON_STOP_POS_X - ICON_START_POS_X)
        {
            x = ICON_STOP_POS_X - ICON_START_POS_X;
        }
        gui_img_translate(img, (float)x, 0);
        show_hide_arrow((void *)obj, x);
    }
}

static void win_wait_design(gui_obj_t *parent)
{
    gui_color_t font_color = theme_bg_white ? FG_1_LIGHT : FG_1_DARK;

    text_str = gui_malloc(32);
    if ((f_status.earbuds_find_l && f_status.earbuds_in_ear_l) ||
        (f_status.earbuds_find_r && f_status.earbuds_in_ear_r))
    {
        sprintf(text_str, "Take off earbuds");

        gui_text_t *text = gui_text_create(parent, 0, 35, 43, 216, 30);
        gui_text_set(text, text_str, GUI_FONT_SRC_BMP, font_color, strlen(text_str), 30);
        gui_text_type_set(text, CAPTION_3_30_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(text, LEFT);
    }
    sprintf(text_str + 24, "5");
    gui_img_t *note = gui_img_create_from_mem(parent, 0, ICON_NOTE_BIN, 8, 48, 0, 0);
    gui_img_set_mode(note, IMG_2D_SW_SRC_OVER_MODE);

    gui_win_t *win_cancel = gui_win_create(parent, 0, 268, 40, 0, 0);
    gui_img_t *cancel = gui_img_create_from_mem(win_cancel, 0, ICON_CANCEL_BIN, 10, 10, 0, 0);
    gui_obj_add_event_cb(win_cancel, click_cancel, GUI_EVENT_TOUCH_CLICKED, NULL);

    gui_img_t *control_bg = gui_img_create_from_mem(parent, 0, CONTROL_BG_BIN, 12, 92, 0, 0);
    gui_text_t *text = gui_text_create(control_bg, 0, 0, 0, 296, 90);
    gui_text_set(text, text_str + 24, GUI_FONT_SRC_BMP, font_color, 2, 32);
    gui_text_type_set(text, CAPTION_1_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, MID_CENTER);
    text = gui_text_create(control_bg, 0, 30, 0, 266, 90);
    gui_text_set(text, "s", GUI_FONT_SRC_BMP, font_color, 2, 20);
    gui_text_type_set(text, HEADING_1_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, MID_CENTER);


    if (theme_bg_white)
    {
        gui_img_a8_recolor(control_bg, BG_1_LIGHT.color.argb_full);
        gui_img_a8_recolor(cancel, FG_3_LIGHT.color.argb_full);
    }
    else
    {
        gui_img_a8_recolor(control_bg, BG_1_DARK.color.argb_full);
        gui_img_a8_recolor(cancel, FG_3_DARK.color.argb_full);
    }
    gui_img_a8_mix_alpha(control_bg, control_bg->fg_color_set >> 24);
    gui_img_a8_mix_alpha(cancel, cancel->fg_color_set >> 24);
}

static void win_start_buzz_design(void)
{
    gui_win_t *win_start = gui_win_create(current_view, 0, 0, 0, 0, 0);
    gui_obj_t *parent = GUI_BASE(win_start);

    gui_color_t font_color = theme_bg_white ? FG_1_LIGHT : FG_1_DARK;

    if ((f_status.earbuds_find_l && f_status.earbuds_in_ear_l) ||
        (f_status.earbuds_find_r && f_status.earbuds_in_ear_r))
    {
        sprintf(text_str, "Swipe to start buzzer");
        gui_text_t *text = gui_text_create(parent, 0, 35, 43, 216, 30);
        gui_text_set(text, text_str, GUI_FONT_SRC_BMP, font_color, strlen(text_str), 30);
        gui_text_type_set(text, CAPTION_3_30_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(text, LEFT);
    }

    gui_img_t *note = gui_img_create_from_mem(parent, 0, ICON_NOTE_BIN, 8, 48, 0, 0);
    gui_img_set_mode(note, IMG_2D_SW_SRC_OVER_MODE);

    gui_win_t *win_cancel = gui_win_create(parent, 0, 268, 40, 0, 0);
    gui_img_t *cancel = gui_img_create_from_mem(win_cancel, 0, ICON_CANCEL_BIN, 10, 10, 0, 0);
    gui_obj_add_event_cb(win_cancel, click_cancel, GUI_EVENT_TOUCH_CLICKED, NULL);

    gui_img_t *control_bg = gui_img_create_from_mem(parent, 0, CONTROL_BG_BIN, 12, 92, 0, 0);
    gui_img_t *icon_bg = gui_img_create_from_mem(control_bg, 0, BUTTON_BG_CIRCLE_BIN, ICON_START_POS_X,
                                                 ICON_START_POS_Y, 0, 0);
    gui_img_t *icon_bell = gui_img_create_from_mem(icon_bg, 0, FIND_BUDS_BELL_BIN, 24, 24, 0, 0);
    gui_img_t *arrow_0 = gui_img_create_from_mem(control_bg, 0, FIND_BUDS_ARROW_BIN, 104, 33, 0, 0);
    gui_img_t *arrow_1 = gui_img_create_from_mem(control_bg, 0, FIND_BUDS_ARROW_BIN, 144, 33, 0, 0);
    gui_img_t *arrow_2 = gui_img_create_from_mem(control_bg, 0, FIND_BUDS_ARROW_BIN, 184, 33, 0, 0);
    gui_img_t *arrow_3 = gui_img_create_from_mem(control_bg, 0, FIND_BUDS_ARROW_BIN, 224, 33, 0, 0);
    gui_img_t *arrow_4 = gui_img_create_from_mem(control_bg, 0, FIND_BUDS_ARROW_BIN, 264, 33, 0, 0);
    gui_obj_create_timer(GUI_BASE(icon_bg), 10, true, pressing_icon);
    gui_img_a8_mix_alpha(arrow_0, 102);
    gui_img_a8_mix_alpha(arrow_1, 128);
    gui_img_a8_mix_alpha(arrow_2, 153);
    gui_img_a8_mix_alpha(arrow_3, 179);
    gui_img_a8_mix_alpha(arrow_4, 204);

    if (theme_bg_white)
    {
        gui_img_a8_recolor(control_bg, BG_THEME2_MID_LIGHT.color.argb_full);
        gui_img_a8_recolor(icon_bg, BG_THEME2_BRIGHT_LIGHT.color.argb_full);
        gui_img_a8_recolor(icon_bell, FG_1_LIGHT.color.argb_full);
        gui_img_a8_recolor(arrow_0, BG_THEME2_BRIGHT_LIGHT.color.argb_full);
        gui_img_a8_recolor(arrow_1, BG_THEME2_BRIGHT_LIGHT.color.argb_full);
        gui_img_a8_recolor(arrow_2, BG_THEME2_BRIGHT_LIGHT.color.argb_full);
        gui_img_a8_recolor(arrow_3, BG_THEME2_BRIGHT_LIGHT.color.argb_full);
        gui_img_a8_recolor(arrow_4, BG_THEME2_BRIGHT_LIGHT.color.argb_full);
        gui_img_a8_recolor(cancel, FG_3_LIGHT.color.argb_full);
    }
    else
    {
        gui_img_a8_recolor(control_bg, BG_THEME2_MID_DARK.color.argb_full);
        gui_img_a8_recolor(icon_bg, BG_THEME2_BRIGHT_DARK.color.argb_full);
        gui_img_a8_recolor(icon_bell, FG_1_DARK.color.argb_full);
        gui_img_a8_recolor(arrow_0, BG_THEME2_BRIGHT_DARK.color.argb_full);
        gui_img_a8_recolor(arrow_1, BG_THEME2_BRIGHT_DARK.color.argb_full);
        gui_img_a8_recolor(arrow_2, BG_THEME2_BRIGHT_DARK.color.argb_full);
        gui_img_a8_recolor(arrow_3, BG_THEME2_BRIGHT_DARK.color.argb_full);
        gui_img_a8_recolor(arrow_4, BG_THEME2_BRIGHT_DARK.color.argb_full);
        gui_img_a8_recolor(cancel, FG_3_DARK.color.argb_full);
    }
    gui_img_a8_mix_alpha(control_bg, control_bg->fg_color_set >> 24);
    gui_img_a8_mix_alpha(cancel, cancel->fg_color_set >> 24);
}

static void find_buds_view_design(gui_view_t *view)
{
    descriptor_rec = gui_view_get_current()->descriptor;
    gui_win_t *win_wait = gui_win_create(view, 0, 0, 0, 0, 0);
    win_wait_design((void *)win_wait);

    gui_obj_create_timer(GUI_BASE(view), 1000, true, timer_cb);
    gui_obj_start_timer(GUI_BASE(view));
}

static void clear(gui_view_t *view)
{
    GUI_UNUSED(view);
    if (text_str)
    {
        gui_free(text_str);
    }
}