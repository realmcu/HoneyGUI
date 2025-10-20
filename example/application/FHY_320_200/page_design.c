/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "app_main.h"
#include "common_data.h"
#include "gui_img.h"
#include "gui_canvas_rect.h"
#include "gui_win.h"
#include "gui_text.h"
#include "gui_scroll_text.h"
#include "gui_list.h"
#include "tp_algo.h"
#include "gui_message.h"

/*============================================================================*
 *                           Types
 *============================================================================*/
typedef enum
{
    ANC = 0,
    AA,
    TT
} AMBIENT_SOUND_STATUS;

/*============================================================================*
 *                            Macros
 *============================================================================*/
#define VOLUME              "Volume"
#define EQUALIZER           "Equalizer"
#define AMBIENT_SOUND       "Ambient Sound"
#define MUSIC               "music"
#define DARK_LIGHT_MODE     "Dark/Light Mode"
#define LOCK_SCREEN         "Lock Screen"
#define NOTIFICATION        "Notification"
#define MESSAGE_PREVIEW     "Message Preview"
#define SCREEN_BRIGHTNESS   "Screen Brightness"
#define TIME_FORMAT         "Time Format"

#define PROGRESSBAR_W_MAX 116
#define TOGGLE_OFF_X 4
#define TOGGLE_ON_X 28
#define TOGGLE_Y 4

#define BUTTON_X_ANC 0
#define BUTTON_X_AA  99
#define BUTTON_X_TT 197

#define BUTTON_X_LIGHT 148
#define BUTTON_X_DARK  0

#define BUTTON_X_FORMAT_24   BUTTON_X_LIGHT
#define BUTTON_X_FORMAT_12   BUTTON_X_DARK

#define MUSIC_ARRAY_NUM 20

/*============================================================================*
 *                           Function Declaration
 *============================================================================*/
static void update_ambient_sound_status(gui_obj_t *obj);

/*============================================================================*
 *                            Variables
 *============================================================================*/
static const gui_view_descriptor_t *clock_view = NULL;

static uint8_t toggle_target_x = 0;
static int8_t index_rec = 0;
static void *img_data_array[10] = {0};
static bool pressed_l = false;
static bool pressed_r = false;


static int8_t music_type_index = 0;

static AMBIENT_SOUND_STATUS ambient_sound_status = ANC;
static uint8_t button_bg_target_x = 0;

static int8_t music_index = 0;
static const char *music_array[MUSIC_ARRAY_NUM] =
{
    "Watermelon Sugar",
    "Save Your Tears",
    "Feels",
    "Stay",
    "Blinding Lights",
    "Levitating",
    "Peaches",
    "Montero",
    "Good 4 U",
    "Kiss Me More",
    "Industry Baby",
    "Heat Waves",
};



/*============================================================================*
 *                           Private Functions
 *============================================================================*/
static int gui_view_get_other_view_descriptor_init(void)
{
    /* you can get other view descriptor point here */
    clock_view = gui_view_descriptor_get("clock_view");
    gui_log("File: %s, Function: %s\n", __FILE__, __func__);
    return 0;
}
static GUI_INIT_VIEW_DESCRIPTOR_GET(gui_view_get_other_view_descriptor_init);

static void toggle_move(void *p)
{
    gui_obj_t *obj = (gui_obj_t *)p;
    int step = 2;
    gui_color_t color_origin;
    gui_color_t color_target;

    if (obj->x == toggle_target_x)
    {
        gui_obj_delete_timer(obj);
        return;
    }
    else if (obj->x < toggle_target_x)
    {
        obj->x += step;
    }
    else
    {
        obj->x -= step;
    }

    if (theme_bg_white)
    {
        color_origin = FG_1_LIGHT;
        color_target = BG_THEME1_BRIGHT_LIGHT;
    }
    else
    {
        color_origin = BG_2_DARK;
        color_target = BG_THEME1_BRIGHT_DARK;
    }
    if (toggle_target_x != TOGGLE_ON_X)
    {
        uint32_t temp = color_origin.color.argb_full;
        color_origin.color.argb_full = color_target.color.argb_full;
        color_target.color.argb_full = temp;
    }
    uint8_t scope = TOGGLE_ON_X - TOGGLE_OFF_X;
    float multiply = (float)(scope - abs(obj->x - toggle_target_x)) / scope;
    uint8_t a = (uint8_t)(color_origin.color.rgba.a + multiply * (color_target.color.rgba.a -
                                                                  color_origin.color.rgba.a));
    uint8_t r = (uint8_t)(color_origin.color.rgba.r + multiply * (color_target.color.rgba.r -
                                                                  color_origin.color.rgba.r));
    uint8_t g = (uint8_t)(color_origin.color.rgba.g + multiply * (color_target.color.rgba.g -
                                                                  color_origin.color.rgba.g));
    uint8_t b = (uint8_t)(color_origin.color.rgba.b + multiply * (color_target.color.rgba.b -
                                                                  color_origin.color.rgba.b));
    gui_img_a8_recolor((void *)obj->parent, GUI_COLOR_ARGB8888(a, r, g, b));
    gui_img_a8_mix_alpha((void *)obj->parent, a);
}

static void regenerate_current_view(void *msg)
{
    GUI_UNUSED(msg);

    gui_view_t *current_view = gui_view_get_current();
    const struct gui_view_descriptor *descriptor = current_view->descriptor;
    gui_obj_t *parent = current_view->base.parent;
    gui_obj_tree_free(GUI_BASE(current_view));
    gui_set_bg_color(SCREEN_BG_DARK);

    gui_view_create(parent, descriptor, 0, 0, 0, 0);
}

static void button_move(void *p)
{
    gui_obj_t *obj = (gui_obj_t *)p;
    int step = 20;

    if (obj->x == button_bg_target_x)
    {
        gui_obj_delete_timer(obj);
        if (!strcmp(obj->name, AMBIENT_SOUND))
        {
            update_ambient_sound_status(obj->parent);
        }
        else if (!strcmp(obj->name, DARK_LIGHT_MODE) || !strcmp(obj->name, TIME_FORMAT))
        {
            gui_msg_t msg =
            {
                .event = GUI_EVENT_USER_DEFINE,
                .cb = regenerate_current_view,
            };
            gui_send_msg_to_server(&msg);
        }
    }
    else if (obj->x < button_bg_target_x)
    {
        obj->x += step;
        if (obj->x > button_bg_target_x)
        {
            obj->x = button_bg_target_x;
        }
    }
    else
    {
        obj->x -= step;
        if (obj->x < button_bg_target_x)
        {
            obj->x = button_bg_target_x;
        }
    }
}

static void update_ambient_sound_status(gui_obj_t *obj)
{
    GUI_WIDGET_POINTER_BY_NAME_ROOT(anc, "anc", obj);
    GUI_WIDGET_POINTER_BY_NAME_ROOT(tt, "tt", obj);
    GUI_WIDGET_POINTER_BY_NAME_ROOT(aa, "aa", obj);

    gui_img_a8_recolor((void *)anc, FG_1_LIGHT.color.argb_full);
    gui_img_a8_recolor((void *)tt, FG_1_LIGHT.color.argb_full);
    gui_img_a8_recolor((void *)aa, FG_1_LIGHT.color.argb_full);
    if (ambient_sound_status == ANC)
    {
        gui_img_a8_recolor((void *)anc, FG_1_DARK.color.argb_full);
    }
    else if (ambient_sound_status == TT)
    {
        gui_img_a8_recolor((void *)tt, FG_1_DARK.color.argb_full);
    }
    else
    {
        gui_img_a8_recolor((void *)aa, FG_1_DARK.color.argb_full);
    }
}

static void press_button_page_volume(void *obj)
{
    GUI_UNUSED(obj);
    gui_obj_t *o = GUI_BASE(obj);
    touch_info_t *tp = tp_get_info();
    if (tp->pressed && gui_obj_point_in_obj_rect(o, tp->x, tp->y))
    {
        if (strcmp(o->name, "sub") == 0)
        {
            if (volume_val != 0)
            {
                pressed_l = true;
                gui_obj_hidden(o, false);
            }
        }
        else
        {
            if (volume_val != 10)
            {
                pressed_r = true;
                gui_obj_hidden(o, false);
            }
        }
    }

    else if (tp->pressing && (tp->deltaX != 0 || tp->deltaY != 0))
    {
        gui_obj_hidden(o, true);
    }
    else if (tp->released)
    {
        gui_obj_hidden(o, true);
        if (tp->deltaX == 0 && tp->deltaY == 0)
        {
            int8_t compare;
            gui_img_t *icon = (gui_img_t *)gui_list_entry(o->brother_list.next, gui_obj_t, brother_list);

            if (strcmp(o->name, "sub") == 0)
            {
                compare = 0;
                index_rec = volume_val;
            }
            else
            {
                compare = 10;
            }
            if (pressed_r)
            {
                volume_val++;
                gui_obj_t *progressbar = gui_list_entry(o->parent->child_list.prev, gui_obj_t, brother_list);
                gui_obj_hidden((void *)progressbar, false);
                gui_img_set_image_data((void *)progressbar, img_data_array[volume_val - 1]);
            }
            else if (pressed_l)
            {
                volume_val--;
                gui_obj_t *progressbar = gui_list_entry(o->parent->child_list.prev, gui_obj_t, brother_list);
                if (volume_val == 0)
                {
                    gui_obj_hidden((void *)progressbar, true);
                }
                else
                {
                    gui_obj_hidden((void *)progressbar, false);
                    gui_img_set_image_data((void *)progressbar, img_data_array[volume_val - 1]);
                }
            }

            if (volume_val == compare)
            {
                gui_img_a8_recolor(icon, theme_bg_white ? FG_2_LIGHT.color.argb_full : FG_2_DARK.color.argb_full);
            }
            else if (index_rec == compare)
            {
                gui_img_a8_recolor(icon, theme_bg_white ? FG_1_LIGHT.color.argb_full : FG_1_DARK.color.argb_full);
            }
            gui_img_a8_mix_alpha(icon, icon->fg_color_set >> 24);
        }
        pressed_r = false;
        pressed_l = false;
    }
}

static void press_button_page_equalizer(void *obj)
{
    GUI_UNUSED(obj);
    gui_obj_t *o = GUI_BASE(obj);
    touch_info_t *tp = tp_get_info();
    if (tp->pressed && gui_obj_point_in_obj_rect(o, tp->x, tp->y))
    {
        if (strcmp(o->name, "switch_l") == 0)
        {
            if (music_type_index != 0)
            {
                pressed_l = true;
                gui_obj_hidden(o, false);
            }
        }
        else
        {
            if (music_type_index != 5)
            {
                pressed_r = true;
                gui_obj_hidden(o, false);
            }
        }
    }
    else if (tp->pressing && (tp->deltaX != 0 || tp->deltaY != 0))
    {
        gui_obj_hidden(o, true);
    }
    else if (tp->released)
    {
        gui_obj_hidden(o, true);
        if (tp->deltaX == 0 && tp->deltaY == 0)
        {
            int8_t compare;
            gui_img_t *icon = (gui_img_t *)gui_list_entry(o->brother_list.next, gui_obj_t, brother_list);

            if (strcmp(o->name, "switch_l") == 0)
            {
                compare = 0;
                index_rec = music_type_index;
            }
            else
            {
                compare = 5;
            }

            if (pressed_l)
            {
                music_type_index--;
                GUI_WIDGET_POINTER_BY_NAME_ROOT(music_type, "music_type", o->parent);
                gui_img_set_image_data((gui_img_t *)music_type, img_data_array[music_type_index]);
            }
            else if (pressed_r)
            {
                music_type_index++;
                GUI_WIDGET_POINTER_BY_NAME_ROOT(music_type, "music_type", o->parent);
                gui_img_set_image_data((gui_img_t *)music_type, img_data_array[music_type_index]);
            }

            if (music_type_index == compare)
            {
                gui_img_a8_recolor(icon, theme_bg_white ? FG_2_LIGHT.color.argb_full : FG_2_DARK.color.argb_full);
            }
            else if (index_rec == compare)
            {
                gui_img_a8_recolor(icon, theme_bg_white ? FG_1_LIGHT.color.argb_full : FG_1_DARK.color.argb_full);
            }
            gui_img_a8_mix_alpha(icon, icon->fg_color_set >> 24);
        }
        pressed_l = false;
        pressed_r = false;
    }
}


static void press_button_page_music(void *obj)
{
    GUI_UNUSED(obj);
    gui_obj_t *o = GUI_BASE(obj);
    touch_info_t *tp = tp_get_info();
    if (tp->pressed && gui_obj_point_in_obj_rect(o, tp->x, tp->y))
    {
        if (strcmp(o->name, "last") == 0)
        {
            if (music_index != 0)
            {
                pressed_l = true;
                gui_obj_hidden(o, false);
            }
        }
        else
        {
            if (music_index != MUSIC_ARRAY_NUM - 1 && music_array[music_index + 1] != NULL)
            {
                pressed_r = true;
                gui_obj_hidden(o, false);
            }
        }
    }
    else if (tp->pressing && (tp->deltaX != 0 || tp->deltaY != 0))
    {
        gui_obj_hidden(o, true);
    }
    else if (tp->released)
    {
        gui_obj_hidden(o, true);
        if (tp->deltaX == 0 && tp->deltaY == 0)
        {
            gui_img_t *icon = (gui_img_t *)gui_list_entry(o->brother_list.next, gui_obj_t, brother_list);
            if (pressed_l)
            {
                music_index--;
                GUI_WIDGET_POINTER_BY_NAME_ROOT(text, MUSIC, o->parent->parent);
                gui_scroll_text_content_set((void *)text, (void *)music_array[music_index],
                                            strlen(music_array[music_index]));
            }
            else if (pressed_r)
            {
                music_index++;
                GUI_WIDGET_POINTER_BY_NAME_ROOT(text, MUSIC, o->parent->parent);
                gui_scroll_text_content_set((void *)text, (void *)music_array[music_index],
                                            strlen(music_array[music_index]));
            }

            if (strcmp(o->name, "last") == 0)
            {
                if (music_index == 0)
                {
                    gui_img_a8_recolor(icon, theme_bg_white ? FG_2_LIGHT.color.argb_full : FG_2_DARK.color.argb_full);
                }
                else
                {
                    gui_img_a8_recolor(icon, theme_bg_white ? FG_1_LIGHT.color.argb_full : FG_1_DARK.color.argb_full);
                }
            }
            else
            {
                if (music_index == MUSIC_ARRAY_NUM - 1 || music_array[music_index + 1] == NULL)
                {
                    gui_img_a8_recolor(icon, theme_bg_white ? FG_2_LIGHT.color.argb_full : FG_2_DARK.color.argb_full);
                }
                else
                {
                    gui_img_a8_recolor(icon, theme_bg_white ? FG_1_LIGHT.color.argb_full : FG_1_DARK.color.argb_full);
                }
            }
            gui_img_a8_mix_alpha(icon, icon->fg_color_set >> 24);
        }
        pressed_l = false;
        pressed_r = false;
    }
}

static void press_button_page_screen_brightness(void *obj)
{
    GUI_UNUSED(obj);
    gui_obj_t *o = GUI_BASE(obj);
    touch_info_t *tp = tp_get_info();
    if (tp->pressed && gui_obj_point_in_obj_rect(o, tp->x, tp->y))
    {
        if (strcmp(o->name, "sub") == 0)
        {
            if (screen_brightness_val != 1)
            {
                pressed_l = true;
                gui_obj_hidden(o, false);
            }
        }
        else
        {
            if (screen_brightness_val != 3)
            {
                pressed_r = true;
                gui_obj_hidden(o, false);
            }
        }
    }

    else if (tp->pressing && (tp->deltaX != 0 || tp->deltaY != 0))
    {
        gui_obj_hidden(o, true);
    }
    else if (tp->released)
    {
        gui_obj_hidden(o, true);
        if (tp->deltaX == 0 && tp->deltaY == 0)
        {
            int8_t compare;
            gui_img_t *icon = (gui_img_t *)gui_list_entry(o->brother_list.next, gui_obj_t, brother_list);

            if (strcmp(o->name, "sub") == 0)
            {
                compare = 1;
                index_rec = screen_brightness_val;
            }
            else
            {
                compare = 3;
            }
            if (pressed_r)
            {
                screen_brightness_val++;
                gui_obj_t *progressbar = gui_list_entry(o->parent->child_list.prev, gui_obj_t, brother_list);
                gui_img_set_image_data((void *)progressbar, img_data_array[screen_brightness_val - 1]);
            }
            else if (pressed_l)
            {
                screen_brightness_val--;
                gui_obj_t *progressbar = gui_list_entry(o->parent->child_list.prev, gui_obj_t, brother_list);
                gui_img_set_image_data((void *)progressbar, img_data_array[screen_brightness_val - 1]);
            }

            if (screen_brightness_val == compare)
            {
                gui_img_a8_recolor(icon, theme_bg_white ? FG_2_LIGHT.color.argb_full : FG_2_DARK.color.argb_full);
            }
            else if (index_rec == compare)
            {
                gui_img_a8_recolor(icon, theme_bg_white ? FG_1_LIGHT.color.argb_full : FG_1_DARK.color.argb_full);
            }
            gui_img_a8_mix_alpha(icon, icon->fg_color_set >> 24);
        }
        pressed_r = false;
        pressed_l = false;
    }
}

static void click_button_page_ambient_sound(void *obj, gui_event_t e, void *param)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);
    GUI_UNUSED(param);
    gui_obj_t *o = GUI_BASE(obj);
    if (strcmp(o->name, "anc") == 0)
    {
        ambient_sound_status = ANC;
        button_bg_target_x = BUTTON_X_ANC;
    }
    else if (strcmp(o->name, "tt") == 0)
    {
        ambient_sound_status = TT;
        button_bg_target_x = BUTTON_X_TT;
    }
    else
    {
        ambient_sound_status = AA;
        button_bg_target_x = BUTTON_X_AA;
    }
    gui_obj_create_timer(gui_list_entry(o->parent->child_list.next, gui_obj_t, brother_list), 10, true,
                         button_move);
}

static void click_button_page_music(void *obj, gui_event_t e, void *param)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);
    GUI_UNUSED(param);
    gui_obj_t *o = GUI_BASE(obj);
    f_status.music_play = !f_status.music_play;
    if (f_status.music_play)
    {
        gui_img_set_image_data((gui_img_t *)o, PAUSE_BIN);
    }
    else
    {
        gui_img_set_image_data((gui_img_t *)o, PLAY_BIN);
    }
    gui_img_refresh_size((gui_img_t *)o);
}

static void click_button_page_dark_light(void *obj, gui_event_t e, void *param)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);
    GUI_UNUSED(param);
    gui_obj_t *o = GUI_BASE(obj);
    theme_bg_white = !theme_bg_white;
    if (theme_bg_white)
    {
        button_bg_target_x = BUTTON_X_LIGHT;
    }
    else
    {
        button_bg_target_x = BUTTON_X_DARK;
    }
    gui_obj_create_timer(gui_list_entry(o->child_list.next, gui_obj_t, brother_list), 10, true,
                         button_move);
}

static void click_clock_page_lock_screen(void *obj, gui_event_t e, void *param)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);
    GUI_UNUSED(param);
    switch_from_lock_screen = true;
    gui_view_switch_direct(gui_view_get_current(), clock_view, SWITCH_OUT_NONE_ANIMATION,
                           SWITCH_IN_NONE_ANIMATION);
}

static void click_button_page_lock_screen(void *obj, gui_event_t e, void *param)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);
    GUI_UNUSED(param);
    gui_obj_t *o = GUI_BASE(obj);
    if (strcmp(o->name, "switch_l") == 0)
    {
        clock_style--;
    }
    else
    {
        clock_style++;
    }
    if (clock_style < 0)
    {
        clock_style = 0;
    }
    else if (clock_style > 4)
    {
        clock_style = 4;
    }
    gui_obj_t *img = gui_list_entry(o->parent->child_list.prev, gui_obj_t, brother_list);
    gui_img_set_image_data((gui_img_t *)img, img_data_array[clock_style]);
}

static void click_button_time_format_light(void *obj, gui_event_t e, void *param)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);
    GUI_UNUSED(param);
    gui_obj_t *o = GUI_BASE(obj);
    time_format_24 = !time_format_24;
    if (time_format_24)
    {
        button_bg_target_x = BUTTON_X_FORMAT_24;
    }
    else
    {
        button_bg_target_x = BUTTON_X_FORMAT_12;
    }
    gui_obj_create_timer(gui_list_entry(o->child_list.next, gui_obj_t, brother_list), 10, true,
                         button_move);
}

static void click_toggle_cb(void *obj, gui_event_t e, void *param)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);
    GUI_UNUSED(param);
    gui_obj_t *o = GUI_BASE(obj);
    gui_obj_t *toggle = gui_list_entry(o->child_list.next, gui_obj_t, brother_list);

    if (strcmp(toggle->name, EQUALIZER) == 0)
    {
        f_status.equalizer = !f_status.equalizer;
        if (f_status.equalizer)
        {
            toggle_target_x = TOGGLE_ON_X;
        }
        else
        {
            toggle_target_x = TOGGLE_OFF_X;
        }
    }
    else if (strcmp(toggle->name, AMBIENT_SOUND) == 0)
    {
        f_status.ambient_sound = !f_status.ambient_sound;
        if (f_status.ambient_sound)
        {
            toggle_target_x = TOGGLE_ON_X;
        }
        else
        {
            toggle_target_x = TOGGLE_OFF_X;
        }
    }
    else if (strcmp(toggle->name, NOTIFICATION) == 0)
    {
        f_status.notification = !f_status.notification;
        if (f_status.notification)
        {
            toggle_target_x = TOGGLE_ON_X;
        }
        else
        {
            toggle_target_x = TOGGLE_OFF_X;
        }
    }
    else if (strcmp(toggle->name, MESSAGE_PREVIEW) == 0)
    {
        f_status.message_preview = !f_status.message_preview;
        if (f_status.message_preview)
        {
            toggle_target_x = TOGGLE_ON_X;
        }
        else
        {
            toggle_target_x = TOGGLE_OFF_X;
        }
    }
    gui_obj_create_timer(toggle, 10, true, toggle_move);
}

static void click_button_no_action(void *obj, gui_event_t e, void *param)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);
    GUI_UNUSED(param);
}


/*============================================================================*
 *                           Public Functions
 *============================================================================*/
void page_volume_design(gui_obj_t *parent)
{
    gui_color_t font_color;
    gui_color_t progressbar_color;
    if (theme_bg_white)
    {
        font_color = FG_1_LIGHT;
        progressbar_color = FG_THEME1_LIGHT;
    }
    else
    {
        font_color = FG_1_DARK;
        progressbar_color = FG_THEME1_DARK;
    }
    {
        img_data_array[0] = PROGRESSBAR_1_BIN;
        img_data_array[1] = PROGRESSBAR_2_BIN;
        img_data_array[2] = PROGRESSBAR_3_BIN;
        img_data_array[3] = PROGRESSBAR_4_BIN;
        img_data_array[4] = PROGRESSBAR_5_BIN;
        img_data_array[5] = PROGRESSBAR_6_BIN;
        img_data_array[6] = PROGRESSBAR_7_BIN;
        img_data_array[7] = PROGRESSBAR_8_BIN;
        img_data_array[8] = PROGRESSBAR_9_BIN;
        img_data_array[9] = PROGRESSBAR_10_BIN;
    }

    gui_text_t *text = gui_text_create(parent, 0, 12, 45, 200, 15);
    gui_text_set(text, VOLUME, GUI_FONT_SRC_BMP, font_color, strlen(VOLUME), 30);
    gui_text_type_set(text, CAPTION_3_30_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, LEFT);

    gui_img_t *control_bg = gui_img_create_from_mem(parent, 0, CONTROL_BG_BIN, 12, 92, 0, 0);
    gui_img_t *button_sub_bg = gui_img_create_from_mem(control_bg, "sub", BUTTON_BG_CIRCLE_BIN, 6, 6, 0,
                                                       0);
    gui_img_t *sub = gui_img_create_from_mem(control_bg, 0, SUB_BIN, 31, 42, 0, 0);
    gui_img_t *button_add_bg = gui_img_create_from_mem(control_bg, "add", BUTTON_BG_CIRCLE_BIN, 212, 6,
                                                       0, 0);
    gui_img_t *add = gui_img_create_from_mem(control_bg, 0, ADD_BIN, 237, 31, 0, 0);
    gui_img_t *progressbar_bg = gui_img_create_from_mem(control_bg, 0, PROGRESSBAR_BG_BIN, 90, 27, 0,
                                                        0);
    gui_img_t *progressbar = gui_img_create_from_mem(control_bg, "progressbar", img_data_array[0],
                                                     90, 27,
                                                     0, 0);
    gui_obj_hidden((void *)button_sub_bg, 1);
    gui_obj_hidden((void *)button_add_bg, 1);
    if (volume_val == 0)
    {
        gui_obj_hidden((void *)progressbar, 1);
    }
    else
    {
        gui_img_set_image_data(progressbar, img_data_array[volume_val - 1]);
    }

    gui_obj_create_timer(GUI_BASE(button_sub_bg), 10, true, press_button_page_volume);
    gui_obj_create_timer(GUI_BASE(button_add_bg), 10, true, press_button_page_volume);

    if (theme_bg_white)
    {
        gui_img_a8_recolor(control_bg, BG_THEME1_DARK_LIGHT.color.argb_full);
        gui_img_a8_recolor(button_sub_bg, BG_THEME1_MID_LIGHT.color.argb_full);
        gui_img_a8_recolor(button_add_bg, BG_THEME1_MID_LIGHT.color.argb_full);
        gui_img_a8_recolor(progressbar_bg, BG_THEME1_MID_LIGHT.color.argb_full);
        gui_img_a8_recolor(progressbar, FG_THEME1_LIGHT.color.argb_full);
        gui_img_a8_recolor(sub, FG_1_LIGHT.color.argb_full);
        gui_img_a8_recolor(add, FG_1_LIGHT.color.argb_full);
        if (volume_val == 0)
        {
            gui_img_a8_recolor(sub, FG_2_LIGHT.color.argb_full);
        }
        else if (volume_val == 10)
        {
            gui_img_a8_recolor(add, FG_2_LIGHT.color.argb_full);
        }
    }
    else
    {
        gui_img_a8_recolor(control_bg, BG_THEME1_DARK_DARK.color.argb_full);
        gui_img_a8_recolor(button_sub_bg, BG_THEME1_MID_DARK.color.argb_full);
        gui_img_a8_recolor(button_add_bg, BG_THEME1_MID_DARK.color.argb_full);
        gui_img_a8_recolor(progressbar_bg, BG_THEME1_MID_DARK.color.argb_full);
        gui_img_a8_recolor(progressbar, FG_THEME1_DARK.color.argb_full);
        gui_img_a8_recolor(sub, FG_1_DARK.color.argb_full);
        gui_img_a8_recolor(add, FG_1_DARK.color.argb_full);
        if (volume_val == 0)
        {
            gui_img_a8_recolor(sub, FG_2_DARK.color.argb_full);
        }
        else if (volume_val == 10)
        {
            gui_img_a8_recolor(add, FG_2_DARK.color.argb_full);
        }
    }
    gui_img_a8_mix_alpha(sub, sub->fg_color_set >> 24);
    gui_img_a8_mix_alpha(add, add->fg_color_set >> 24);
}

void page_equalizer_design(gui_obj_t *parent)
{
    gui_color_t font_color;
    {
        img_data_array[0] = STUDIO_BIN;
        img_data_array[1] = BASS_BIN;
        img_data_array[2] = CLUB_BIN;
        img_data_array[3] = EXTRE_BASS_BIN;
        img_data_array[4] = VOCAL_BIN;
        img_data_array[5] = JAZZ_BIN;
    }
    if (theme_bg_white)
    {
        font_color = FG_1_LIGHT;
    }
    else
    {
        font_color = FG_1_DARK;
    }
    gui_text_t *text = gui_text_create(parent, 0, 12, 45, 200, 15);
    gui_text_set(text, EQUALIZER, GUI_FONT_SRC_BMP, font_color, strlen(EQUALIZER), 30);
    gui_text_type_set(text, CAPTION_3_30_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, LEFT);

    gui_img_t *toggle_bg = gui_img_create_from_mem(parent, 0, TOGGLE_BG_BIN, 244, 40, 0, 0);
    gui_img_t *toggle = gui_img_create_from_mem(toggle_bg, EQUALIZER, TOGGLE_BIN, TOGGLE_OFF_X,
                                                TOGGLE_Y, 0, 0);
    gui_img_a8_recolor(toggle, FG_1_DARK.color.argb_full);
    gui_obj_add_event_cb(toggle_bg, click_toggle_cb, GUI_EVENT_TOUCH_CLICKED, NULL);

    gui_img_t *control_bg = gui_img_create_from_mem(parent, 0, CONTROL_BG_BIN, 12, 92, 0, 0);
    gui_img_t *button_switch_l_bg = gui_img_create_from_mem(control_bg, "switch_l",
                                                            BUTTON_BG_CIRCLE_BIN, 6, 6, 0, 0);
    gui_img_t *switch_l = gui_img_create_from_mem(control_bg, 0, SWITCH_L_BIN, 32, 32, 0, 0);
    gui_img_t *button_switch_r_bg = gui_img_create_from_mem(control_bg, "switch_r",
                                                            BUTTON_BG_CIRCLE_BIN, 212, 6, 0, 0);
    gui_img_t *switch_r = gui_img_create_from_mem(control_bg, 0, SWITCH_R_BIN, 241, 32, 0, 0);
    gui_img_t *music_type = gui_img_create_from_mem(control_bg, "music_type",
                                                    img_data_array[music_type_index], 86, 5, 0, 0);
    gui_obj_hidden((void *)button_switch_l_bg, 1);
    gui_obj_hidden((void *)button_switch_r_bg, 1);
    gui_obj_create_timer(GUI_BASE(button_switch_l_bg), 10, true, press_button_page_equalizer);
    gui_obj_create_timer(GUI_BASE(button_switch_r_bg), 10, true, press_button_page_equalizer);

    if (theme_bg_white)
    {
        gui_img_a8_recolor(control_bg, BG_THEME1_DARK_LIGHT.color.argb_full);
        gui_img_a8_recolor(button_switch_l_bg, BG_THEME1_MID_LIGHT.color.argb_full);
        gui_img_a8_recolor(button_switch_r_bg, BG_THEME1_MID_LIGHT.color.argb_full);
        gui_img_a8_recolor(music_type, FG_1_LIGHT.color.argb_full);
        gui_img_a8_recolor(switch_l, FG_1_LIGHT.color.argb_full);
        gui_img_a8_recolor(switch_r, FG_1_LIGHT.color.argb_full);
        if (music_type_index == 0)
        {
            gui_img_a8_recolor(switch_l, FG_2_LIGHT.color.argb_full);
        }
        else if (music_type_index == 5)
        {
            gui_img_a8_recolor(switch_r, FG_2_LIGHT.color.argb_full);
        }
        if (f_status.equalizer)
        {
            gui_obj_move(GUI_BASE(toggle), TOGGLE_ON_X, TOGGLE_Y);
            gui_img_a8_recolor(toggle_bg, BG_THEME1_BRIGHT_LIGHT.color.argb_full);
        }
        else
        {
            gui_img_a8_recolor(toggle_bg, FG_1_LIGHT.color.argb_full);
        }
    }
    else
    {
        gui_img_a8_recolor(control_bg, BG_THEME1_DARK_DARK.color.argb_full);
        gui_img_a8_recolor(button_switch_l_bg, BG_THEME1_MID_DARK.color.argb_full);
        gui_img_a8_recolor(button_switch_r_bg, BG_THEME1_MID_DARK.color.argb_full);
        gui_img_a8_recolor(music_type, FG_1_DARK.color.argb_full);
        gui_img_a8_recolor(switch_l, FG_1_DARK.color.argb_full);
        gui_img_a8_recolor(switch_r, FG_1_DARK.color.argb_full);
        if (music_type_index == 0)
        {
            gui_img_a8_recolor(switch_l, FG_2_DARK.color.argb_full);
        }
        else if (music_type_index == 5)
        {
            gui_img_a8_recolor(switch_r, FG_2_DARK.color.argb_full);
        }
        if (f_status.equalizer)
        {
            gui_obj_move(GUI_BASE(toggle), TOGGLE_ON_X, TOGGLE_Y);
            gui_img_a8_recolor(toggle_bg, BG_THEME1_BRIGHT_DARK.color.argb_full);
        }
        else
        {
            gui_img_a8_recolor(toggle_bg, BG_2_DARK.color.argb_full);
        }
    }
    gui_img_a8_mix_alpha(toggle_bg, toggle_bg->fg_color_set >> 24);
    gui_img_a8_mix_alpha(switch_l, switch_l->fg_color_set >> 24);
    gui_img_a8_mix_alpha(switch_r, switch_r->fg_color_set >> 24);
}

void page_ambient_sound_design(gui_obj_t *parent)
{
    gui_color_t font_color;
    if (theme_bg_white)
    {
        font_color = FG_1_LIGHT;
    }
    else
    {
        font_color = FG_1_DARK;
    }
    gui_text_t *text = gui_text_create(parent, 0, 12, 45, 200, 15);
    gui_text_set(text, AMBIENT_SOUND, GUI_FONT_SRC_BMP, font_color, strlen(AMBIENT_SOUND), 30);
    gui_text_type_set(text, CAPTION_3_30_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, LEFT);

    gui_img_t *toggle_bg = gui_img_create_from_mem(parent, 0, TOGGLE_BG_BIN, 244, 40, 0, 0);
    gui_img_t *toggle = gui_img_create_from_mem(toggle_bg, AMBIENT_SOUND, TOGGLE_BIN, TOGGLE_OFF_X,
                                                TOGGLE_Y, 0, 0);
    gui_img_a8_recolor(toggle, FG_1_DARK.color.argb_full);
    gui_obj_add_event_cb(toggle_bg, click_toggle_cb, GUI_EVENT_TOUCH_CLICKED, NULL);

    gui_img_t *control_bg = gui_img_create_from_mem(parent, 0, CONTROL_BG_BIN, 12, 92, 0, 0);
    int button_bg_x;
    if (ambient_sound_status == ANC)
    {
        button_bg_x = BUTTON_X_ANC;
    }
    else if (ambient_sound_status == TT)
    {
        button_bg_x = BUTTON_X_TT;
    }
    else
    {
        button_bg_x = BUTTON_X_AA;
    }
    gui_img_t *button_bg = gui_img_create_from_mem(control_bg, AMBIENT_SOUND, BUTTON_BG_ELLIPSE_BIN,
                                                   button_bg_x, 0, 0, 0);
    gui_img_t *anc = gui_img_create_from_mem(control_bg, "anc", ICON_ANC_BIN, 25, 21, 0, 0);
    gui_img_t *aa = gui_img_create_from_mem(control_bg, "aa", ICON_AA_BIN, 124, 21, 0, 0);
    gui_img_t *tt = gui_img_create_from_mem(control_bg, "tt", ICON_TT_BIN, 223, 21, 0, 0);
    update_ambient_sound_status((void *)control_bg);
    gui_obj_add_event_cb(anc, click_button_page_ambient_sound, GUI_EVENT_TOUCH_CLICKED, NULL);
    gui_obj_add_event_cb(tt, click_button_page_ambient_sound, GUI_EVENT_TOUCH_CLICKED, NULL);
    gui_obj_add_event_cb(aa, click_button_page_ambient_sound, GUI_EVENT_TOUCH_CLICKED, NULL);

    if (theme_bg_white)
    {
        gui_img_a8_recolor(control_bg, BG_THEME1_DARK_LIGHT.color.argb_full);
        gui_img_a8_recolor(button_bg, BG_THEME1_BRIGHT_LIGHT.color.argb_full);
        if (f_status.ambient_sound)
        {
            gui_obj_move(GUI_BASE(toggle), TOGGLE_ON_X, TOGGLE_Y);
            gui_img_a8_recolor(toggle_bg, BG_THEME1_BRIGHT_LIGHT.color.argb_full);
        }
        else
        {
            gui_img_a8_recolor(toggle_bg, FG_1_LIGHT.color.argb_full);
        }
    }
    else
    {
        gui_img_a8_recolor(control_bg, BG_THEME1_DARK_DARK.color.argb_full);
        gui_img_a8_recolor(button_bg, BG_THEME1_BRIGHT_DARK.color.argb_full);
        if (f_status.ambient_sound)
        {
            gui_obj_move(GUI_BASE(toggle), TOGGLE_ON_X, TOGGLE_Y);
            gui_img_a8_recolor(toggle_bg, BG_THEME1_BRIGHT_DARK.color.argb_full);
        }
        else
        {
            gui_img_a8_recolor(toggle_bg, BG_2_DARK.color.argb_full);
            gui_img_a8_mix_alpha(toggle_bg, BG_2_DARK.color.rgba.a);
        }
    }
}

void page_music_design(gui_obj_t *parent)
{
    gui_color_t font_color;

    if (theme_bg_white)
    {
        font_color = FG_1_LIGHT;
    }
    else
    {
        font_color = FG_1_DARK;
    }
    gui_scroll_text_t *t = gui_scroll_text_create(parent, MUSIC, 12, 40, 160, 56);
    gui_scroll_text_set(t, (char *)music_array[music_index], GUI_FONT_SRC_BMP, font_color,
                        strlen(music_array[music_index]), 30);
    gui_scroll_text_type_set(t, CAPTION_3_30_BIN, FONT_SRC_MEMADDR);
    gui_scroll_text_scroll_set(t, SCROLL_X, 160, 160, 5000, 0);

    gui_text_t *text = gui_text_create(parent, 0, 12, 65, 200, 15);
    gui_text_set(text, "Artist-Album", GUI_FONT_SRC_BMP, font_color, 13, 20);
    gui_text_type_set(text, HEADING_1_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, LEFT);

    gui_img_t *control_bg = gui_img_create_from_mem(parent, 0, CONTROL_BG_BIN, 12, 92, 0, 0);
    gui_img_t *button_last_bg = gui_img_create_from_mem(control_bg, "last", BUTTON_BG_CIRCLE_BIN, 6, 6,
                                                        0, 0);
    gui_img_t *last = gui_img_create_from_mem(control_bg, 0, SKIP_LAST_BIN, 24, 32, 0, 0);
    gui_img_t *button_next_bg = gui_img_create_from_mem(control_bg, "next", BUTTON_BG_CIRCLE_BIN, 212,
                                                        6, 0, 0);
    gui_obj_hidden((void *)button_last_bg, 1);
    gui_obj_hidden((void *)button_next_bg, 1);
    gui_img_t *next = gui_img_create_from_mem(control_bg, 0, SKIP_NEXT_BIN, 233, 32, 0, 0);
    gui_img_t *play = gui_img_create_from_mem(control_bg, 0, PLAY_BIN, 132, 24, 0, 0);
    gui_obj_add_event_cb(play, click_button_page_music, GUI_EVENT_TOUCH_CLICKED, NULL);
    if (f_status.music_play)
    {
        gui_img_set_image_data(play, PAUSE_BIN);
        gui_img_refresh_size(play);
    }
    gui_obj_create_timer(GUI_BASE(button_last_bg), 10, true, press_button_page_music);
    gui_obj_create_timer(GUI_BASE(button_next_bg), 10, true, press_button_page_music);

    if (theme_bg_white)
    {
        gui_img_a8_recolor(control_bg, BG_THEME1_DARK_LIGHT.color.argb_full);
        gui_img_a8_recolor(button_last_bg, BG_THEME1_MID_LIGHT.color.argb_full);
        gui_img_a8_recolor(button_next_bg, BG_THEME1_MID_LIGHT.color.argb_full);
        gui_img_a8_recolor(play, FG_1_LIGHT.color.argb_full);
        gui_img_a8_recolor(last, FG_1_LIGHT.color.argb_full);
        gui_img_a8_recolor(next, FG_1_LIGHT.color.argb_full);
        if (music_index == 0)
        {
            gui_img_a8_recolor(last, FG_2_LIGHT.color.argb_full);
        }
        else if (music_index == MUSIC_ARRAY_NUM - 1 || music_array[music_index + 1] == NULL)
        {
            gui_img_a8_recolor(next, FG_2_LIGHT.color.argb_full);
        }
    }
    else
    {
        gui_img_a8_recolor(control_bg, BG_THEME1_DARK_DARK.color.argb_full);
        gui_img_a8_recolor(button_last_bg, BG_THEME1_MID_DARK.color.argb_full);
        gui_img_a8_recolor(button_next_bg, BG_THEME1_MID_DARK.color.argb_full);
        gui_img_a8_recolor(play, FG_1_DARK.color.argb_full);
        gui_img_a8_recolor(last, FG_1_DARK.color.argb_full);
        gui_img_a8_recolor(next, FG_1_DARK.color.argb_full);
        if (music_index == 0)
        {
            gui_img_a8_recolor(last, FG_2_DARK.color.argb_full);
        }
        else if (music_index == MUSIC_ARRAY_NUM - 1 || music_array[music_index + 1] == NULL)
        {
            gui_img_a8_recolor(next, FG_2_DARK.color.argb_full);
        }
    }
    gui_img_a8_mix_alpha(last, last->fg_color_set >> 24);
    gui_img_a8_mix_alpha(next, next->fg_color_set >> 24);
}



void page_dark_light_design(gui_obj_t *parent)
{
    gui_color_t font_color;
    if (theme_bg_white)
    {
        font_color = FG_1_LIGHT;
    }
    else
    {
        font_color = FG_1_DARK;
    }
    gui_text_t *text = gui_text_create(parent, 0, 12, 45, 200, 15);
    gui_text_set(text, DARK_LIGHT_MODE, GUI_FONT_SRC_BMP, font_color, strlen(DARK_LIGHT_MODE), 30);
    gui_text_type_set(text, CAPTION_3_30_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, LEFT);

    gui_img_t *control_bg = gui_img_create_from_mem(parent, 0, CONTROL_BG_BIN, 12, 92, 0, 0);
    gui_img_t *button_bg = gui_img_create_from_mem(control_bg, DARK_LIGHT_MODE,
                                                   BUTTON_BG_ELLIPSE_148_88_BIN, BUTTON_X_DARK, 0, 0, 0);
    gui_img_t *light = gui_img_create_from_mem(control_bg, "light", ICON_LIGHT_BIN, 198, 21, 0, 0);
    gui_img_t *dark = gui_img_create_from_mem(control_bg, "dark", ICON_DARK_BIN, 50, 21, 0, 0);

    gui_obj_add_event_cb(control_bg, click_button_page_dark_light, GUI_EVENT_TOUCH_CLICKED, NULL);
    gui_obj_add_event_cb(button_bg, click_button_no_action, GUI_EVENT_TOUCH_CLICKED,
                         NULL); //stop event bubble

    if (theme_bg_white)
    {
        gui_img_a8_recolor(control_bg, BG_THEME3_DARK_LIGHT.color.argb_full);
        gui_img_a8_recolor(button_bg, BG_THEME3_BRIGHT_LIGHT.color.argb_full);
        gui_img_a8_recolor(light, FG_1_LIGHT.color.argb_full);
        gui_img_a8_recolor(dark, FG_1_DARK.color.argb_full);
        button_bg->base.x = BUTTON_X_LIGHT;
    }
    else
    {
        gui_img_a8_recolor(control_bg, BG_THEME3_DARK_DARK.color.argb_full);
        gui_img_a8_recolor(button_bg, BG_THEME3_BRIGHT_DARK.color.argb_full);
        gui_img_a8_recolor(light, FG_1_LIGHT.color.argb_full);
        gui_img_a8_recolor(dark, FG_1_DARK.color.argb_full);
    }
}

void page_lock_screen_design(gui_obj_t *parent)
{
    {
        img_data_array[0] = CLOCK1_BIN;
        img_data_array[1] = CLOCK2_BIN;
        img_data_array[2] = CLOCK3_BIN;
        img_data_array[3] = CLOCK4_BIN;
        img_data_array[4] = CLOCK5_BIN;
    }

    gui_color_t font_color;

    if (theme_bg_white)
    {
        font_color = FG_1_LIGHT;
    }
    else
    {
        font_color = FG_1_DARK;
    }

    gui_text_t *text = gui_text_create(parent, 0, 12, 45, 200, 15);
    gui_text_set(text, LOCK_SCREEN, GUI_FONT_SRC_BMP, font_color, strlen(LOCK_SCREEN), 30);
    gui_text_type_set(text, CAPTION_3_30_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, LEFT);
    gui_win_t *win_l = gui_win_create(parent, "switch_l", 18, 98, 78, 78);
    gui_win_t *win_r = gui_win_create(parent, "switch_r", 224, 98, 78, 78);
    gui_img_t *switch_l = gui_img_create_from_mem(win_l, 0, SWITCH_L_BIN, 29, 26, 0, 0);
    gui_img_t *switch_r = gui_img_create_from_mem(win_r, 0, SWITCH_R_BIN, 29, 26, 0, 0);
    gui_obj_add_event_cb(win_l, click_button_page_lock_screen, GUI_EVENT_TOUCH_CLICKED, NULL);
    gui_obj_add_event_cb(win_r, click_button_page_lock_screen, GUI_EVENT_TOUCH_CLICKED, NULL);

    gui_img_t *clock = gui_img_create_from_mem(parent, 0, img_data_array[clock_style], 96, 97, 0, 0);
    gui_img_set_mode(clock, IMG_BYPASS_MODE);
    gui_obj_add_event_cb(clock, click_clock_page_lock_screen, GUI_EVENT_TOUCH_CLICKED, NULL);

    if (theme_bg_white)
    {
        gui_img_a8_recolor(switch_l, FG_1_LIGHT.color.argb_full);
        gui_img_a8_recolor(switch_r, FG_1_LIGHT.color.argb_full);
    }
    else
    {
        gui_img_a8_recolor(switch_l, FG_1_DARK.color.argb_full);
        gui_img_a8_recolor(switch_r, FG_1_DARK.color.argb_full);
    }
}

void page_qrcode_design(gui_obj_t *parent)
{
    gui_img_t *img = gui_img_create_from_mem(parent, 0, QRCODE_BIN, 104, 40, 0, 0);
    gui_img_set_mode(img, IMG_BYPASS_MODE);
}

void page_notification_design(gui_obj_t *parent)
{
    gui_color_t font_color;
    if (theme_bg_white)
    {
        font_color = FG_1_LIGHT;
    }
    else
    {
        font_color = FG_1_DARK;
    }
    gui_text_t *text = gui_text_create(parent, 0, 12, 45, 200, 15);
    gui_text_set(text, NOTIFICATION, GUI_FONT_SRC_BMP, font_color, strlen(NOTIFICATION), 30);
    gui_text_type_set(text, CAPTION_3_30_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, LEFT);

    text = gui_text_create(parent, 0, 12, 95, 300, 15);
    gui_text_set(text, MESSAGE_PREVIEW, GUI_FONT_SRC_BMP, font_color, strlen(MESSAGE_PREVIEW), 30);
    gui_text_type_set(text, CAPTION_3_30_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, LEFT);

    gui_img_t *toggle_notification_bg = gui_img_create_from_mem(parent, 0, TOGGLE_BG_BIN, 244, 40, 0,
                                                                0);
    gui_img_t *toggle_notification = gui_img_create_from_mem(toggle_notification_bg, NOTIFICATION,
                                                             TOGGLE_BIN, TOGGLE_OFF_X,
                                                             TOGGLE_Y, 0, 0);
    gui_img_t *toggle_message_bg = gui_img_create_from_mem(parent, 0, TOGGLE_BG_BIN, 244, 90, 0, 0);
    gui_img_t *toggle_message = gui_img_create_from_mem(toggle_message_bg, MESSAGE_PREVIEW, TOGGLE_BIN,
                                                        TOGGLE_OFF_X,
                                                        TOGGLE_Y, 0, 0);
    gui_img_a8_recolor(toggle_notification, FG_1_DARK.color.argb_full);
    gui_img_a8_recolor(toggle_message, FG_1_DARK.color.argb_full);
    gui_obj_add_event_cb(toggle_notification_bg, click_toggle_cb, GUI_EVENT_TOUCH_CLICKED, NULL);
    gui_obj_add_event_cb(toggle_message_bg, click_toggle_cb, GUI_EVENT_TOUCH_CLICKED, NULL);

    if (theme_bg_white)
    {
        if (f_status.notification)
        {
            gui_obj_move(GUI_BASE(toggle_notification), TOGGLE_ON_X, TOGGLE_Y);
            gui_img_a8_recolor(toggle_notification_bg, BG_THEME1_BRIGHT_LIGHT.color.argb_full);
        }
        else
        {
            gui_img_a8_recolor(toggle_notification_bg, FG_1_LIGHT.color.argb_full);
        }
        if (f_status.message_preview)
        {
            gui_obj_move(GUI_BASE(toggle_message), TOGGLE_ON_X, TOGGLE_Y);
            gui_img_a8_recolor(toggle_message_bg, BG_THEME1_BRIGHT_LIGHT.color.argb_full);
        }
        else
        {
            gui_img_a8_recolor(toggle_message_bg, FG_1_LIGHT.color.argb_full);
        }
    }
    else
    {
        if (f_status.notification)
        {
            gui_obj_move(GUI_BASE(toggle_notification), TOGGLE_ON_X, TOGGLE_Y);
            gui_img_a8_recolor(toggle_notification_bg, BG_THEME1_BRIGHT_DARK.color.argb_full);
        }
        else
        {
            gui_img_a8_recolor(toggle_notification_bg, BG_2_DARK.color.argb_full);
            gui_img_a8_mix_alpha(toggle_notification_bg, BG_2_DARK.color.rgba.a);
        }
        if (f_status.message_preview)
        {
            gui_obj_move(GUI_BASE(toggle_message), TOGGLE_ON_X, TOGGLE_Y);
            gui_img_a8_recolor(toggle_message_bg, BG_THEME1_BRIGHT_DARK.color.argb_full);
        }
        else
        {
            gui_img_a8_recolor(toggle_message_bg, BG_2_DARK.color.argb_full);
            gui_img_a8_mix_alpha(toggle_message_bg, BG_2_DARK.color.rgba.a);
        }
    }
}

void page_screen_brightness_design(gui_obj_t *parent)
{
    gui_color_t font_color;
    gui_color_t progressbar_color;
    if (theme_bg_white)
    {
        font_color = FG_1_LIGHT;
        progressbar_color = FG_THEME3_LIGHT;
    }
    else
    {
        font_color = FG_1_DARK;
        progressbar_color = FG_THEME3_DARK;
    }
    {
        img_data_array[0] = SECTION_BAR_1_BIN;
        img_data_array[1] = SECTION_BAR_2_BIN;
        img_data_array[2] = SECTION_BAR_3_BIN;
    }

    gui_text_t *text = gui_text_create(parent, 0, 12, 45, 200, 15);
    gui_text_set(text, SCREEN_BRIGHTNESS, GUI_FONT_SRC_BMP, font_color, strlen(SCREEN_BRIGHTNESS), 30);
    gui_text_type_set(text, CAPTION_3_30_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, LEFT);

    gui_img_t *control_bg = gui_img_create_from_mem(parent, 0, CONTROL_BG_BIN, 12, 92, 0, 0);
    gui_img_t *button_sub_bg = gui_img_create_from_mem(control_bg, "sub", BUTTON_BG_CIRCLE_BIN, 6, 6, 0,
                                                       0);
    gui_img_t *sub = gui_img_create_from_mem(control_bg, 0, SUB_BIN, 31, 42, 0, 0);
    gui_img_t *button_add_bg = gui_img_create_from_mem(control_bg, "add", BUTTON_BG_CIRCLE_BIN, 212, 6,
                                                       0, 0);
    gui_img_t *add = gui_img_create_from_mem(control_bg, 0, ADD_BIN, 237, 31, 0, 0);
    gui_img_t *progressbar_bg = gui_img_create_from_mem(control_bg, 0, img_data_array[2], 90, 27, 0,
                                                        0);
    gui_img_t *progressbar = gui_img_create_from_mem(control_bg, "progressbar",
                                                     img_data_array[screen_brightness_val - 1],
                                                     90, 27, 0, 0);
    gui_obj_hidden((void *)button_sub_bg, 1);
    gui_obj_hidden((void *)button_add_bg, 1);

    gui_obj_create_timer(GUI_BASE(button_sub_bg), 10, true, press_button_page_screen_brightness);
    gui_obj_create_timer(GUI_BASE(button_add_bg), 10, true, press_button_page_screen_brightness);

    if (theme_bg_white)
    {
        gui_img_a8_recolor(control_bg, BG_THEME3_DARK_LIGHT.color.argb_full);
        gui_img_a8_recolor(button_sub_bg, BG_THEME3_MID_LIGHT.color.argb_full);
        gui_img_a8_recolor(button_add_bg, BG_THEME3_MID_LIGHT.color.argb_full);
        gui_img_a8_recolor(progressbar_bg, BG_THEME3_MID_LIGHT.color.argb_full);
        gui_img_a8_recolor(progressbar, FG_THEME3_LIGHT.color.argb_full);
        gui_img_a8_recolor(sub, FG_1_LIGHT.color.argb_full);
        gui_img_a8_recolor(add, FG_1_LIGHT.color.argb_full);
        if (screen_brightness_val == 1)
        {
            gui_img_a8_recolor(sub, FG_2_LIGHT.color.argb_full);
        }
        else if (screen_brightness_val == 3)
        {
            gui_img_a8_recolor(add, FG_2_LIGHT.color.argb_full);
        }
    }
    else
    {
        gui_img_a8_recolor(control_bg, BG_THEME3_DARK_DARK.color.argb_full);
        gui_img_a8_recolor(button_sub_bg, BG_THEME3_MID_DARK.color.argb_full);
        gui_img_a8_recolor(button_add_bg, BG_THEME3_MID_DARK.color.argb_full);
        gui_img_a8_recolor(progressbar_bg, BG_THEME3_MID_DARK.color.argb_full);
        gui_img_a8_recolor(progressbar, FG_THEME3_DARK.color.argb_full);
        gui_img_a8_recolor(sub, FG_1_DARK.color.argb_full);
        gui_img_a8_recolor(add, FG_1_DARK.color.argb_full);
        if (screen_brightness_val == 1)
        {
            gui_img_a8_recolor(sub, FG_2_DARK.color.argb_full);
        }
        else if (screen_brightness_val == 3)
        {
            gui_img_a8_recolor(add, FG_2_DARK.color.argb_full);
        }
    }
    gui_img_a8_mix_alpha(sub, sub->fg_color_set >> 24);
    gui_img_a8_mix_alpha(add, add->fg_color_set >> 24);
}

void page_time_format_design(gui_obj_t *parent)
{
    gui_color_t font_color;
    if (theme_bg_white)
    {
        font_color = FG_1_LIGHT;
    }
    else
    {
        font_color = FG_1_DARK;
    }
    gui_text_t *text = gui_text_create(parent, 0, 12, 45, 200, 15);
    gui_text_set(text, TIME_FORMAT, GUI_FONT_SRC_BMP, font_color, strlen(TIME_FORMAT), 30);
    gui_text_type_set(text, CAPTION_3_30_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, LEFT);

    gui_img_t *control_bg = gui_img_create_from_mem(parent, 0, CONTROL_BG_BIN, 12, 92, 0, 0);
    gui_img_t *button_bg = gui_img_create_from_mem(control_bg, DARK_LIGHT_MODE,
                                                   BUTTON_BG_ELLIPSE_148_88_BIN, BUTTON_X_FORMAT_12, 0, 0, 0);
    gui_img_t *format_12 = gui_img_create_from_mem(control_bg, 0, TIME_FORMAT_12_BIN, 45, 34, 0, 0);
    gui_img_t *format_24 = gui_img_create_from_mem(control_bg, 0, TIME_FORMAT_24_BIN, 186, 34, 0, 0);
    gui_obj_add_event_cb(control_bg, click_button_time_format_light, GUI_EVENT_TOUCH_CLICKED, NULL);
    gui_obj_add_event_cb(button_bg, click_button_no_action, GUI_EVENT_TOUCH_CLICKED,
                         NULL); //stop event bubble

    if (time_format_24)
    {
        gui_img_a8_recolor(format_12, FG_1_LIGHT.color.argb_full);
        gui_img_a8_recolor(format_24, FG_1_DARK.color.argb_full);
        button_bg->base.x = BUTTON_X_FORMAT_24;
    }
    else
    {
        gui_img_a8_recolor(format_12, FG_1_DARK.color.argb_full);
        gui_img_a8_recolor(format_24, FG_1_LIGHT.color.argb_full);
    }
    if (theme_bg_white)
    {
        gui_img_a8_recolor(control_bg, BG_THEME3_DARK_LIGHT.color.argb_full);
        gui_img_a8_recolor(button_bg, BG_THEME3_BRIGHT_LIGHT.color.argb_full);
    }
    else
    {
        gui_img_a8_recolor(control_bg, BG_THEME3_DARK_DARK.color.argb_full);
        gui_img_a8_recolor(button_bg, BG_THEME3_BRIGHT_DARK.color.argb_full);
    }
}