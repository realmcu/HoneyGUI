/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "app_main.h"
#include "common_data.h"
#include "gui_img.h"
#include "gui_win.h"
#include "gui_text.h"
#include "gui_message.h"

/*============================================================================*
 *                            Macros
 *============================================================================*/
#define CURRENT_VIEW_NAME "timer_view"


/*============================================================================*
 *                           Function Declaration
 *============================================================================*/
static void timer_view_design(gui_view_t *view);
static void clear(gui_view_t *view);

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
    .on_switch_in = timer_view_design,
    .on_switch_out = clear
};

const gui_view_descriptor_t *timer_descriptor_rec = NULL;
static void **ring_data_array = NULL;
static bool reset_flag = 0;

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

static void click_button_back(void *obj, gui_event_t e, void *param)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);
    GUI_UNUSED(param);
    gui_view_switch_direct(current_view, timer_descriptor_rec, SWITCH_OUT_ANIMATION_MOVE_TO_RIGHT,
                           SWITCH_IN_ANIMATION_MOVE_FROM_LEFT);
}

static void click_button_left(void *obj, gui_event_t e, void *param)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);
    GUI_UNUSED(param);
    if (reset_flag)
    {
        reset_flag = 0;
        gui_msg_t msg =
        {
            .event = GUI_EVENT_USER_DEFINE,
            .cb = regenerate_current_view,
        };
        gui_send_msg_to_server(&msg);
        return;
    }
    f_status.timer = !f_status.timer;
    gui_obj_t *text = gui_list_entry(GUI_BASE(obj)->brother_list.prev, gui_obj_t, brother_list);
    gui_obj_t *ring = gui_list_entry(GUI_BASE(text)->brother_list.prev, gui_obj_t, brother_list);
    gui_obj_t *icon = gui_list_entry(GUI_BASE(obj)->child_list.next, gui_obj_t, brother_list);
    if (f_status.timer)
    {
        gui_obj_start_timer(ring);
        gui_img_set_image_data((void *)icon, TIMER_PAUSE_BIN);
        gui_img_refresh_size((void *)icon);
        if (theme_bg_white)
        {
            gui_img_a8_recolor((void *)icon, FG_THEME2_LIGHT.color.argb_full);
            gui_img_a8_recolor((void *)obj, BG_THEME2_MID_LIGHT.color.argb_full);
        }
        else
        {
            gui_img_a8_recolor((void *)icon, FG_THEME2_DARK.color.argb_full);
            gui_img_a8_recolor((void *)obj, BG_THEME2_MID_DARK.color.argb_full);
        }
    }
    else
    {
        gui_obj_stop_timer(ring);
        gui_img_set_image_data((void *)icon, TIMER_PLAY_BIG_BIN);
        gui_img_refresh_size((void *)icon);
        if (theme_bg_white)
        {
            gui_img_a8_recolor((void *)icon, FG_1_LIGHT.color.argb_full);
            gui_img_a8_recolor((void *)obj, BG_THEME2_BRIGHT_LIGHT.color.argb_full);
        }
        else
        {
            gui_img_a8_recolor((void *)icon, FG_1_DARK.color.argb_full);
            gui_img_a8_recolor((void *)obj, BG_THEME2_BRIGHT_DARK.color.argb_full);
        }
    }
}

static void click_button_right(void *obj, gui_event_t e, void *param)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);
    GUI_UNUSED(param);
    f_status.timer = 0;
    gui_view_switch_direct(current_view, timer_descriptor_rec, SWITCH_OUT_ANIMATION_MOVE_TO_RIGHT,
                           SWITCH_IN_ANIMATION_MOVE_FROM_LEFT);
}

static void timer_cb(void *p)
{
    GUI_UNUSED(p);
    gui_obj_t *ring = GUI_BASE(p);
    gui_obj_t *text = gui_list_entry(ring->brother_list.next, gui_obj_t, brother_list);
    if (timer_val != 0)
    {
        uint16_t max = timer_max_min_val * 60;
        gui_img_set_image_data((void *)ring, ring_data_array[49 * (max - timer_val) / max]);
        gui_text_content_set((void *)text, timer_str, strlen(timer_str));
    }
    else
    {
        f_status.timer = 0;
        gui_obj_stop_timer((void *)ring);
        gui_obj_t *ring_bg = gui_list_entry(ring->brother_list.prev, gui_obj_t, brother_list);
        gui_obj_hidden(text, true);
        gui_obj_hidden(ring, true);
        gui_obj_hidden(ring_bg, true);

        gui_obj_t *icon_bg = gui_list_entry(text->brother_list.next, gui_obj_t, brother_list);
        gui_obj_t *icon = gui_list_entry(icon_bg->child_list.next, gui_obj_t, brother_list);
        gui_img_set_image_data((void *)icon, TIMER_RESET_BIN);
        gui_img_refresh_size((void *)icon);
        gui_img_t *bell_2 = gui_img_create_from_mem(ring->parent, 0, ICON_BELL_PART_2_BIN, 156, 59, 0, 0);
        gui_img_t *bell_1 = gui_img_create_from_mem(ring->parent, 0, ICON_BELL_PART_1_BIN, 149, 47, 0, 0);
        if (theme_bg_white)
        {
            gui_img_a8_recolor((void *)icon, FG_1_LIGHT.color.argb_full);
            gui_img_a8_recolor((void *)icon_bg, BG_THEME2_BRIGHT_LIGHT.color.argb_full);
            gui_img_a8_recolor(bell_1, FG_THEME2_LIGHT.color.argb_full);
            gui_img_a8_recolor(bell_2, BG_THEME2_BRIGHT_LIGHT.color.argb_full);
        }
        else
        {
            gui_img_a8_recolor((void *)icon, FG_1_DARK.color.argb_full);
            gui_img_a8_recolor((void *)icon_bg, BG_THEME2_BRIGHT_DARK.color.argb_full);
            gui_img_a8_recolor(bell_1, FG_THEME2_DARK.color.argb_full);
            gui_img_a8_recolor(bell_2, BG_THEME2_BRIGHT_DARK.color.argb_full);
        }
        reset_flag = 1;
    }
}

static void timer_view_design(gui_view_t *view)
{
    gui_view_set_animate_step(view, 10);
    if (gui_view_get_current())
    {
        timer_descriptor_rec = gui_view_get_current()->descriptor;
    }
    ring_data_array = gui_malloc(sizeof(void *) * 50);

    {
        ring_data_array[0] = PROGRESS_CIRCLE_0_BIN;
        ring_data_array[1] = PROGRESS_CIRCLE_1_BIN;
        ring_data_array[2] = PROGRESS_CIRCLE_2_BIN;
        ring_data_array[3] = PROGRESS_CIRCLE_3_BIN;
        ring_data_array[4] = PROGRESS_CIRCLE_4_BIN;
        ring_data_array[5] = PROGRESS_CIRCLE_5_BIN;
        ring_data_array[6] = PROGRESS_CIRCLE_6_BIN;
        ring_data_array[7] = PROGRESS_CIRCLE_7_BIN;
        ring_data_array[8] = PROGRESS_CIRCLE_8_BIN;
        ring_data_array[9] = PROGRESS_CIRCLE_9_BIN;
        ring_data_array[10] = PROGRESS_CIRCLE_10_BIN;
        ring_data_array[11] = PROGRESS_CIRCLE_11_BIN;
        ring_data_array[12] = PROGRESS_CIRCLE_12_BIN;
        ring_data_array[13] = PROGRESS_CIRCLE_13_BIN;
        ring_data_array[14] = PROGRESS_CIRCLE_14_BIN;
        ring_data_array[15] = PROGRESS_CIRCLE_15_BIN;
        ring_data_array[16] = PROGRESS_CIRCLE_16_BIN;
        ring_data_array[17] = PROGRESS_CIRCLE_17_BIN;
        ring_data_array[18] = PROGRESS_CIRCLE_18_BIN;
        ring_data_array[19] = PROGRESS_CIRCLE_19_BIN;
        ring_data_array[20] = PROGRESS_CIRCLE_20_BIN;
        ring_data_array[21] = PROGRESS_CIRCLE_21_BIN;
        ring_data_array[22] = PROGRESS_CIRCLE_22_BIN;
        ring_data_array[23] = PROGRESS_CIRCLE_23_BIN;
        ring_data_array[24] = PROGRESS_CIRCLE_24_BIN;
        ring_data_array[25] = PROGRESS_CIRCLE_25_BIN;
        ring_data_array[26] = PROGRESS_CIRCLE_26_BIN;
        ring_data_array[27] = PROGRESS_CIRCLE_27_BIN;
        ring_data_array[28] = PROGRESS_CIRCLE_28_BIN;
        ring_data_array[29] = PROGRESS_CIRCLE_29_BIN;
        ring_data_array[30] = PROGRESS_CIRCLE_30_BIN;
        ring_data_array[31] = PROGRESS_CIRCLE_31_BIN;
        ring_data_array[32] = PROGRESS_CIRCLE_32_BIN;
        ring_data_array[33] = PROGRESS_CIRCLE_33_BIN;
        ring_data_array[34] = PROGRESS_CIRCLE_34_BIN;
        ring_data_array[35] = PROGRESS_CIRCLE_35_BIN;
        ring_data_array[36] = PROGRESS_CIRCLE_36_BIN;
        ring_data_array[37] = PROGRESS_CIRCLE_37_BIN;
        ring_data_array[38] = PROGRESS_CIRCLE_38_BIN;
        ring_data_array[39] = PROGRESS_CIRCLE_39_BIN;
        ring_data_array[40] = PROGRESS_CIRCLE_40_BIN;
        ring_data_array[41] = PROGRESS_CIRCLE_41_BIN;
        ring_data_array[42] = PROGRESS_CIRCLE_42_BIN;
        ring_data_array[43] = PROGRESS_CIRCLE_43_BIN;
        ring_data_array[44] = PROGRESS_CIRCLE_44_BIN;
        ring_data_array[45] = PROGRESS_CIRCLE_45_BIN;
        ring_data_array[46] = PROGRESS_CIRCLE_46_BIN;
        ring_data_array[47] = PROGRESS_CIRCLE_47_BIN;
        ring_data_array[48] = PROGRESS_CIRCLE_48_BIN;
        ring_data_array[49] = PROGRESS_CIRCLE_49_BIN;
    }

    gui_color_t font_color;
    gui_obj_t *parent = GUI_BASE(view);
    if (theme_bg_white)
    {
        gui_set_bg_color(SCREEN_BG_LIGHT);
        font_color = FG_1_LIGHT;
    }
    else
    {
        gui_set_bg_color(SCREEN_BG_DARK);
        font_color = FG_1_DARK;
    }

    gui_win_t *win_icon_back = (gui_win_t *)gui_win_create(parent, 0, 0, 0, 52, 52);
    gui_img_t *icon_back = gui_img_create_from_mem(win_icon_back, 0, ICON_BACK_BIN, 0, 0, 0, 0);
    gui_obj_add_event_cb(win_icon_back, click_button_back, GUI_EVENT_TOUCH_CLICKED, NULL);

    gui_img_t *ring_bg = gui_img_create_from_mem(parent, 0, PROGRESS_CIRCLE_0_BIN, 201, 47, 0, 0);
    gui_img_t *ring = gui_img_create_from_mem(parent, 0, PROGRESS_CIRCLE_0_BIN, 201, 47, 0, 0);

    if (f_status.timer)
    {
        uint16_t max = timer_max_min_val * 60;
        gui_img_set_image_data(ring, ring_data_array[49 * (max - timer_val) / max]);
    }
    else
    {
        timer_val = timer_max_min_val * 60;
        f_status.timer = 1;
    }
    sprintf(timer_str, "%02d:%02d", timer_val / 60, timer_val % 60);
    gui_text_t *text = gui_text_create(parent, 0, 0, 40, 320, 40);
    gui_text_set(text, timer_str, GUI_FONT_SRC_BMP, font_color, strlen(timer_str), 32);
    gui_text_type_set(text, CAPTION_1_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, MID_CENTER);
    gui_obj_create_timer(GUI_BASE(ring), 1000, true, timer_cb);
    gui_obj_start_timer(GUI_BASE(ring));

    gui_img_t *button_bg_l = gui_img_create_from_mem(parent, "l", BUTTON_BG_ELLIPSE_134_66_BIN, 12, 92,
                                                     0, 0);
    gui_img_t *button_bg_r = gui_img_create_from_mem(parent, "r", BUTTON_BG_ELLIPSE_134_66_BIN, 174, 92,
                                                     0, 0);
    gui_img_t *icon_l = gui_img_create_from_mem(button_bg_l, 0, TIMER_PAUSE_BIN, 55, 21, 0, 0);
    gui_img_t *icon_r = gui_img_create_from_mem(button_bg_r, 0, TIMER_CANCEL_BIN, 54, 20, 0, 0);
    gui_obj_add_event_cb(button_bg_l, click_button_left, GUI_EVENT_TOUCH_CLICKED, NULL);
    gui_obj_add_event_cb(button_bg_r, click_button_right, GUI_EVENT_TOUCH_CLICKED, NULL);

    if (theme_bg_white)
    {
        gui_img_a8_recolor(ring_bg, BG_THEME2_MID_LIGHT.color.argb_full);
        gui_img_a8_recolor(ring, FG_THEME2_LIGHT.color.argb_full);
        gui_img_a8_recolor(icon_back, FG_1_LIGHT.color.argb_full);
        gui_img_a8_recolor(button_bg_l, BG_THEME2_MID_LIGHT.color.argb_full);
        gui_img_a8_recolor(button_bg_r, BG_2_LIGHT.color.argb_full);
        gui_img_a8_recolor(icon_l, FG_THEME2_LIGHT.color.argb_full);
        gui_img_a8_recolor(icon_r, FG_1_LIGHT.color.argb_full);
    }
    else
    {
        gui_img_a8_recolor(ring_bg, BG_THEME2_MID_DARK.color.argb_full);
        gui_img_a8_recolor(ring, FG_THEME2_DARK.color.argb_full);
        gui_img_a8_recolor(icon_back, FG_1_DARK.color.argb_full);
        gui_img_a8_recolor(button_bg_l, BG_THEME2_MID_DARK.color.argb_full);
        gui_img_a8_recolor(button_bg_r, BG_2_DARK.color.argb_full);
        gui_img_a8_recolor(icon_l, FG_THEME2_DARK.color.argb_full);
        gui_img_a8_recolor(icon_r, FG_1_DARK.color.argb_full);
    }
    gui_img_a8_mix_alpha(button_bg_r, button_bg_r->fg_color_set >> 24);
}

static void clear(gui_view_t *view)
{
    GUI_UNUSED(view);
    if (ring_data_array)
    {
        gui_free(ring_data_array);
    }
}