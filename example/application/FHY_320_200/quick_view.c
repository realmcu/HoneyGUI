/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "app_main.h"
#include "common_data.h"
#include "gui_img.h"
#include "gui_win.h"
#include "gui_text.h"
#include "gui_list.h"
#include "tp_algo.h"

/*============================================================================*
 *                           Types
 *============================================================================*/

/*============================================================================*
 *                            Macros
 *============================================================================*/
#define CURRENT_VIEW_NAME "quick_view"

/*============================================================================*
 *                           Function Declaration
 *============================================================================*/
static void quick_view_design(gui_view_t *view);
static void update_page_indicator(void);

/*============================================================================*
 *                            Variables
 *============================================================================*/
/* View Management */
static gui_view_t *current_view = NULL;
static const gui_view_descriptor_t *menu_view = NULL;
static const gui_view_descriptor_t *inform_center_view = NULL;
static const gui_view_descriptor_t *wallpaper_view = NULL;
static gui_view_descriptor_t const descriptor =
{
    /* change Here for current view */
    .name = (const char *)CURRENT_VIEW_NAME,
    .pView = &current_view,
    .on_switch_in = quick_view_design,
};

static char time_str[16] = {0};
static int8_t page_index = 0;
static gui_img_t *page_indicator_array[QUICK_PAGE_NUM_MAX] = {0};

int8_t quick_page_num = 4;
void (*quick_page_design_func_array[QUICK_PAGE_NUM_MAX])(gui_obj_t *parent) =
{
    // page_notification_design,
    // page_playback_design,
    // page_auto_play_pause_design,
    // page_voice_aware_design,
    // page_timer_design,
    // page_flashlight_design,
    // page_auto_dim_off_screen_design,
    // page_quick_wake_up_screen_design,
    // page_unlock_slider_design,
    // page_lock_screen_design,
    // page_clock_settings_design,
    page_language_design,
    page_dark_light_design,
    page_volume_design,
    page_ambient_sound_design,
    // page_equalizer_design,
};

static const char *codec_type[6] =
{
    "SBC",
    "AAC",
    "LC3",
    "LC3+",
    "LDAC",
    "PCM"
};

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

static int gui_view_get_other_view_descriptor_init(void)
{
    /* you can get other view descriptor point here */
    menu_view = gui_view_descriptor_get("menu_view");
    inform_center_view = gui_view_descriptor_get("inform_center_view");
    wallpaper_view = gui_view_descriptor_get("wallpaper_view");
    gui_log("File: %s, Function: %s\n", __FILE__, __func__);
    return 0;
}
static GUI_INIT_VIEW_DESCRIPTOR_GET(gui_view_get_other_view_descriptor_init);


static void time_update_cb(void *obj)
{
    if (!timeinfo)
    {
        return;
    }
    if (time_format_24)
    {
        sprintf(time_str, "%02d:%02d", timeinfo->tm_hour, timeinfo->tm_min);
    }
    else
    {
        int tm_hour = (timeinfo->tm_hour % 12 == 0 ? 12 : timeinfo->tm_hour % 12);
        sprintf(time_str, "%1d:%02d %s", tm_hour, timeinfo->tm_min,
                (timeinfo->tm_hour >= 12 ? "PM" : "AM"));
    }
    gui_text_content_set((gui_text_t *)obj, time_str, strlen(time_str));
}

static void list_timer_cb(void *obj)
{
    uint8_t index = ((gui_list_t *)obj)->last_created_note_index;
    index %= quick_page_num;
    index += quick_page_num;
    index %= quick_page_num;
    if (index != page_index)
    {
        page_index = index;
        update_page_indicator();
    }
    touch_info_t *tp = tp_get_info();
    if (!tp->pressing && quick_page_name[index] != page_name_array[language_index][SILENTNOW])
    {
        f_status.silentnow = 0;
    }
}

static void update_page_indicator(void)
{
    gui_color_t indicator_color;
    gui_color_t indicator_color_cur;
    if (theme_bg_white)
    {
        indicator_color = FG_2_LIGHT;
        indicator_color_cur = FG_1_LIGHT;
    }
    else
    {
        indicator_color = FG_2_DARK;
        indicator_color_cur = FG_1_DARK;
    }

    for (int i = 0; i < quick_page_num; i++)
    {
        if (page_indicator_array[i])
        {
            gui_img_a8_recolor((gui_img_t *)page_indicator_array[i], indicator_color.color.argb_full);
            gui_img_a8_mix_alpha((gui_img_t *)page_indicator_array[i], indicator_color.color.rgba.a);
        }
    }
    gui_img_a8_recolor(page_indicator_array[page_index], indicator_color_cur.color.argb_full);
    gui_img_a8_mix_alpha(page_indicator_array[page_index], indicator_color_cur.color.rgba.a);
}

static void note_design(gui_obj_t *obj, void *p)
{
    GUI_UNUSED(p);
    uint16_t index = ((gui_list_note_t *)obj)->index;
    index %= quick_page_num;
    index += quick_page_num;
    index %= quick_page_num;
    quick_page_design_func_array[index](obj);
    quick_page_name[index] = page_name_array[language_index][quick_page_name_index];
}

static void create_indicator(gui_obj_t *parent)
{
    switch (quick_page_num)
    {
    case 1 :
        {
            page_indicator_array[0] = gui_img_create_from_mem(parent, 0, PAGE_INDICATOR_BIN, 158, 188, 0, 0);
        }
        break;
    case 2 :
        {
            page_indicator_array[0] = gui_img_create_from_mem(parent, 0, PAGE_INDICATOR_BIN, 150, 188, 0, 0);
            page_indicator_array[1] = gui_img_create_from_mem(parent, 0, PAGE_INDICATOR_BIN, 162, 188, 0, 0);
        }
        break;
    case 3 :
        {
            page_indicator_array[0] = gui_img_create_from_mem(parent, 0, PAGE_INDICATOR_BIN, 142, 188, 0, 0);
            page_indicator_array[1] = gui_img_create_from_mem(parent, 0, PAGE_INDICATOR_BIN, 154, 188, 0, 0);
            page_indicator_array[2] = gui_img_create_from_mem(parent, 0, PAGE_INDICATOR_BIN, 166, 188, 0, 0);
        }
        break;
    case 4 :
        {
            page_indicator_array[0] = gui_img_create_from_mem(parent, 0, PAGE_INDICATOR_BIN, 140, 188, 0, 0);
            page_indicator_array[1] = gui_img_create_from_mem(parent, 0, PAGE_INDICATOR_BIN, 152, 188, 0, 0);
            page_indicator_array[2] = gui_img_create_from_mem(parent, 0, PAGE_INDICATOR_BIN, 164, 188, 0, 0);
            page_indicator_array[3] = gui_img_create_from_mem(parent, 0, PAGE_INDICATOR_BIN, 176, 188, 0, 0);
        }
        break;
    default:
        break;
    }

}

static void quick_view_design(gui_view_t *view)
{
    gui_view_switch_on_event(view, menu_view->name, SWITCH_OUT_TO_TOP_USE_TRANSLATION,
                             SWITCH_IN_FROM_BOTTOM_USE_TRANSLATION,
                             GUI_EVENT_TOUCH_MOVE_UP);
    gui_view_switch_on_event(view, inform_center_view->name, SWITCH_OUT_TO_BOTTOM_USE_TRANSLATION,
                             SWITCH_IN_FROM_TOP_USE_TRANSLATION,
                             GUI_EVENT_TOUCH_MOVE_DOWN);

    // gui_view_switch_on_event(view, wallpaper_view->name, SWITCH_OUT_TO_RIGHT_USE_TRANSLATION,
    //                          SWITCH_IN_FROM_LEFT_USE_TRANSLATION,
    //                          GUI_EVENT_TOUCH_MOVE_RIGHT);

    gui_obj_t *parent = GUI_BASE(view);
    gui_list_t *list = gui_list_create(parent, 0, 0, 0, 0, 0, 320, 0, HORIZONTAL,
                                       note_design, NULL,
                                       false);
    gui_list_set_style(list, LIST_CLASSIC);
    gui_list_set_note_num(list, quick_page_num);
    gui_list_set_auto_align(list, true);
    gui_list_set_offset(list, -page_index * 320);
    gui_list_set_inertia(list, false);
    gui_list_enable_loop(list, true);
    gui_obj_create_timer(GUI_BASE(list), 10, true, list_timer_cb);

    create_indicator(parent);

    status_bar_design(parent);

    if (quick_page_num)
    {
        update_page_indicator();
    }
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/
void status_bar_design(gui_obj_t *parent)
{
    gui_color_t font_color;
    gui_color_t bg_color;
    gui_color_t charging_color;
    gui_color_t unvalid_color;
    if (theme_bg_white)
    {
        gui_set_bg_color(SCREEN_BG_LIGHT);
        font_color = FG_1_LIGHT;
        bg_color = BG_2_LIGHT;
        charging_color = FG_THEME1_LIGHT;
        unvalid_color = FG_2_LIGHT;
    }
    else
    {
        gui_set_bg_color(SCREEN_BG_DARK);
        font_color = FG_1_DARK;
        bg_color = BG_2_DARK;
        charging_color = FG_THEME1_DARK;
        unvalid_color = FG_2_DARK;
    }
    gui_img_t *bt = gui_img_create_from_mem(parent, 0, ICON_BT_CONNECT_BIN, 12, 13, 0, 0);
    int pos_x = 28;
    if (f_status.bt != BT_UNKNOWN)
    {
        gui_text_t *text = gui_text_create(parent, 0, pos_x, 12, 32, 20);
        gui_text_set(text, (void *)codec_type[codec_type_index], GUI_FONT_SRC_BMP, font_color,
                     strlen(codec_type[codec_type_index]), 20);
        gui_text_type_set(text, HEADING_1_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(text, LEFT);
        pos_x += 36;
    }
    if (f_status.notification_new)
    {
        gui_img_t *icon = gui_img_create_from_mem(parent, 0, ICON_MESSAGE_SMALL_BIN, pos_x, 14, 0, 0);
        gui_img_a8_recolor(icon, theme_bg_white ? FG_1_LIGHT.color.argb_full : FG_1_DARK.color.argb_full);
        pos_x += 22;
    }
    if (f_status.call == CALL_ING)
    {
        gui_img_t *icon = gui_img_create_from_mem(parent, 0, ICON_CALL_BIN, pos_x, 13, 0, 0);
        gui_img_a8_recolor(icon, FG_NORMAL.color.argb_full);
        pos_x += 20;
    }
    if (f_status.timer)
    {
        gui_img_t *icon = gui_img_create_from_mem(parent, 0, ICON_TIMER_SMALL_BIN, pos_x, 13, 0, 0);
        gui_img_a8_recolor(icon, theme_bg_white ? FG_THEME2_LIGHT.color.argb_full :
                           FG_THEME2_DARK.color.argb_full);
    }

    gui_text_t *text = gui_text_create(parent, 0, 200, 13, 110, 15);
    gui_text_set(text, time_str, GUI_FONT_SRC_BMP, font_color, strlen(time_str), 20);
    gui_text_type_set(text, HEADING_1_BIN, FONT_SRC_MEMADDR);
    gui_obj_create_timer(GUI_BASE(text), 3000, true, time_update_cb);
    gui_text_mode_set(text, RIGHT);

    gui_img_t *home_bg = gui_img_create_from_mem(parent, 0, STATUSBAR_HOME_BG_BIN, 131, 8, 0, 0);
    gui_img_t *tx_inner = gui_img_create_from_mem(home_bg, 0, ICON_TX_INNER_SMALL_BIN, 34, 9, 0, 0);
    gui_img_t *tx_outer = gui_img_create_from_mem(home_bg, 0, ICON_TX_OUTER_SMALL_BIN, 32, 7, 0, 0);
    gui_img_t *headband = gui_img_create_from_mem(home_bg, 0, ICON_HEADBAND_SMALL_BIN, 11, 5, 0, 0);

    {
        if (f_status.bt == 0)
        {
            gui_img_set_image_data(bt, ICON_BT_DISCONNECT_BIN);
            gui_img_refresh_size(bt);
            gui_img_a8_recolor(bt, font_color.color.argb_full);
        }
        else if (f_status.bt == 1)
        {
            gui_img_a8_recolor(bt, font_color.color.argb_full);
        }
        else
        {
            gui_img_a8_recolor(bt, unvalid_color.color.argb_full);
        }

        gui_img_a8_recolor(home_bg, bg_color.color.argb_full);
        {
            uint32_t color;
            if (f_status.tx_charging)
            {
                color = charging_color.color.argb_full;
            }
            else if (battery_tx_val > 10)
            {
                color = font_color.color.argb_full;
            }
            else
            {
                color = FG_WARNING.color.argb_full;
            }
            gui_img_a8_recolor(tx_inner, color);
            gui_img_a8_recolor(tx_outer, color);
        }
        if (f_status.headband_connect)
        {
            uint32_t color;
            if (f_status.headband_charging)
            {
                color = charging_color.color.argb_full;
            }
            else if (battery_headband_val > 10)
            {
                color = font_color.color.argb_full;
            }
            else
            {
                color = FG_WARNING.color.argb_full;
            }
            gui_img_a8_recolor(headband, color);
        }
        else
        {
            gui_img_a8_recolor(headband, unvalid_color.color.argb_full);
        }
    }
    gui_img_a8_mix_alpha(bt, bt->fg_color_set >> 24);
    gui_img_a8_mix_alpha(home_bg, home_bg->fg_color_set >> 24);
    gui_img_a8_mix_alpha(headband, headband->fg_color_set >> 24);
}