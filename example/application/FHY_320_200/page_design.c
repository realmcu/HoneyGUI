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


/*============================================================================*
 *                            Macros
 *============================================================================*/
#define PROGRESSBAR_W_MAX 116
#define TOGGLE_OFF_X 4
#define TOGGLE_ON_X 28
#define TOGGLE_Y 4

#define BUTTON_X_LEFT 0
#define BUTTON_X_MID  99
#define BUTTON_X_RIGHT 198

#define BUTTON_X_LIGHT 148
#define BUTTON_X_DARK  0

#define BUTTON_X_FORMAT_24   BUTTON_X_LIGHT
#define BUTTON_X_FORMAT_12   BUTTON_X_DARK

#define MUSIC_ARRAY_NUM 20

#define CONTROL_BG_OPACITY 102

/*============================================================================*
 *                           Function Declaration
 *============================================================================*/
static void update_icon_color(gui_obj_t *obj, uint8_t status);
static void note_timer_up_page_reorder_quick_access(void *obj);
static void note_timer_down_page_reorder_quick_access(void *obj);
/*============================================================================*
 *                            Variables
 *============================================================================*/
static const gui_view_descriptor_t *clock_view = NULL;
static const gui_view_descriptor_t *find_buds_view = NULL;
static const gui_view_descriptor_t *timer_view = NULL;
static const gui_view_descriptor_t *flashlight_view = NULL;
static const gui_view_descriptor_t *quick_view = NULL;
static const gui_view_descriptor_t *detail_view = NULL;
static const gui_view_descriptor_t *button_customize_view = NULL;
static const gui_view_descriptor_t *support_view = NULL;

static uint8_t toggle_target_x = 0;
static uint32_t toggle_on_bg_color = 0;
static bool control_invalid = false;
static void *img_data_array[10] = {0};
static bool pressed_l = false;
static bool pressed_r = false;

static int8_t euqalizer_type_index = 0;
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

static const char lyrics_str[200] =
{
    "Never gonna give you up \
    Never gonna let you down \
    Never gonna run around and desert you \
    Never gonna make you cry \
    Never gonna say goodbye \
    Never gonna tell a lie and hurt you"
};

static const char quick_wake_up_warning_str[] =
{
    "When disabled, only the\nbutton or opening the case will work. When enabled,\nusethe preferred method to\nwake up the screen."
};

static const char unlock_slider_warning_str[] =
{
    "Use unlock slider to prevent\naccidental screen touches."
};

const char *language_type[13] =
{
    "English",
    "French",
    "German",
    "Spanish",
    "Italian",
    "Dutch",
    "Portuguese",
    "Russian",
    "Chinese",
    "Japanese",
    "Korean",
    "Thai",
    "Vietnam"
};

static char page_str[5] = {0};

static int16_t page_quick_wake_up_list_offset_rec = 0;
static int16_t page_reorder_quick_access_list_offset_rec = 0;
static int16_t page_information_center_customize_list_offset_rec = 0;
/*============================================================================*
 *                           Private Functions
 *============================================================================*/
static int gui_view_get_other_view_descriptor_init(void)
{
    /* you can get other view descriptor point here */
    clock_view = gui_view_descriptor_get("clock_view");
    find_buds_view = gui_view_descriptor_get("find_buds_view");
    timer_view = gui_view_descriptor_get("timer_view");
    flashlight_view = gui_view_descriptor_get("flashlight_view");
    quick_view = gui_view_descriptor_get("quick_view");
    detail_view = gui_view_descriptor_get("detail_view");
    button_customize_view = gui_view_descriptor_get("button_customize_view");
    support_view = gui_view_descriptor_get("support_view");
    gui_log("File: %s, Function: %s\n", __FILE__, __func__);
    return 0;
}
static GUI_INIT_VIEW_DESCRIPTOR_GET(gui_view_get_other_view_descriptor_init);

static void toggle_move(void *p)
{
    gui_obj_t *obj = (gui_obj_t *)p;
    gui_obj_t *toggle_bg = obj->parent;
    int step = 2;
    gui_color_t color_origin;
    gui_color_t color_target;
    uint8_t control_alpha_origin = CONTROL_BG_OPACITY;
    uint8_t control_alpha_target = 0xFF;

    if (obj->x == toggle_target_x)
    {
        gui_obj_delete_timer(obj);
        if (strcmp(obj->name, (void *)page_name_array[NOTIFICATION]) == 0 ||
            strcmp(obj->name, (void *)page_name_array[QUICK_WAKE_UP_SCREEN]) == 0)
        {
            gui_msg_t msg =
            {
                .event = GUI_EVENT_USER_DEFINE,
                .cb = regenerate_current_view,
            };
            gui_send_msg_to_server(&msg);
        }
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
    }
    else
    {
        color_origin = BG_2_DARK;
    }
    color_target.color.argb_full = toggle_on_bg_color;
    if (toggle_target_x != TOGGLE_ON_X)
    {
        uint32_t temp = color_origin.color.argb_full;
        color_origin.color.argb_full = color_target.color.argb_full;
        color_target.color.argb_full = temp;

        control_alpha_origin = 0xFF;
        control_alpha_target = CONTROL_BG_OPACITY;
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
    if (toggle_bg->brother_list.next != &toggle_bg->parent->child_list)
    {
        gui_obj_t *control_bg = gui_list_entry(toggle_bg->brother_list.next, gui_obj_t, brother_list);
        uint8_t control_alpha = (uint8_t)(control_alpha_origin +
                                          multiply * (control_alpha_target - control_alpha_origin));
        gui_img_a8_mix_alpha((void *)control_bg, control_alpha);
    }
}

static void button_move(void *p)
{
    gui_obj_t *obj = (gui_obj_t *)p;
    int step = 20;

    if (obj->x == button_bg_target_x)
    {
        gui_obj_delete_timer(obj);
        if (!strcmp(obj->name, (void *)page_name_array[AMBIENT_SOUND]))
        {
            update_icon_color(obj->parent, ambient_sound_type);
        }
        else if (!strcmp(obj->name, (void *)page_name_array[DARK_LIGHT_MODE]) ||
                 !strcmp(obj->name, (void *)page_name_array[TIME_FORMAT]))
        {
            gui_msg_t msg =
            {
                .event = GUI_EVENT_USER_DEFINE,
                .cb = regenerate_current_view,
            };
            gui_send_msg_to_server(&msg);
        }
        else if (!strcmp(obj->name, (void *)page_name_array[SMART_TALK]))
        {
            update_icon_color(obj->parent, smart_talk_time_type);
        }
        else if (!strcmp(obj->name, (void *)page_name_array[SPATIAL_SOUND]))
        {
            update_icon_color(obj->parent, spatial_sound_type);
        }
        else if (!strcmp(obj->name, (void *)page_name_array[QUICK_WAKE_UP_SCREEN]))
        {
            update_icon_color(obj->parent, quick_wake_up_click_num);
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

static void update_icon_color(gui_obj_t *obj, uint8_t status)
{
    GUI_WIDGET_POINTER_BY_NAME_ROOT(left, "l", obj);
    GUI_WIDGET_POINTER_BY_NAME_ROOT(mid, "m", obj);
    GUI_WIDGET_POINTER_BY_NAME_ROOT(right, "r", obj);

    gui_img_a8_mix_alpha((void *)left, 122);
    gui_img_a8_mix_alpha((void *)mid, 122);
    gui_img_a8_mix_alpha((void *)right, 122);
    if (status == 0)
    {
        gui_img_a8_mix_alpha((void *)left, 0xFF);
    }
    else if (status == 1)
    {
        gui_img_a8_mix_alpha((void *)mid, 0xFF);
    }
    else
    {
        gui_img_a8_mix_alpha((void *)right, 0xFF);
    }
}

static void press_button_page_volume(void *obj)
{
    GUI_UNUSED(obj);
    gui_obj_t *o = GUI_BASE(obj);
    touch_info_t *tp = tp_get_info();
    if (tp->pressed && gui_obj_point_in_obj_rect(o, tp->x, tp->y))
    {
        if (strcmp(o->name, "l") == 0)
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

            if (strcmp(o->name, "l") == 0)
            {
                compare = 0;
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
            else
            {
                gui_img_a8_recolor(icon, theme_bg_white ? FG_1_LIGHT.color.argb_full : FG_1_DARK.color.argb_full);
            }
            gui_img_a8_mix_alpha(icon, icon->fg_color_set >> 24);
        }
        pressed_r = false;
        pressed_l = false;
    }
}

static void press_button_page_voice_aware(void *obj)
{
    GUI_UNUSED(obj);
    if (control_invalid) { return; }
    gui_obj_t *o = GUI_BASE(obj);
    touch_info_t *tp = tp_get_info();
    if (tp->pressed && gui_obj_point_in_obj_rect(o, tp->x, tp->y))
    {
        if (strcmp(o->name, "l") == 0)
        {
            if (voice_aware_val != 1)
            {
                pressed_l = true;
                gui_obj_hidden(o, false);
            }
        }
        else
        {
            if (voice_aware_val != 3)
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

            if (strcmp(o->name, "l") == 0)
            {
                compare = 1;
            }
            else
            {
                compare = 3;
            }
            if (pressed_r)
            {
                voice_aware_val++;
                gui_obj_t *progressbar = gui_list_entry(o->parent->child_list.prev, gui_obj_t, brother_list);
                gui_img_set_image_data((void *)progressbar, img_data_array[voice_aware_val - 1]);
            }
            else if (pressed_l)
            {
                voice_aware_val--;
                gui_obj_t *progressbar = gui_list_entry(o->parent->child_list.prev, gui_obj_t, brother_list);
                gui_img_set_image_data((void *)progressbar, img_data_array[voice_aware_val - 1]);
            }

            if (voice_aware_val == compare)
            {
                gui_img_a8_recolor(icon, theme_bg_white ? FG_2_LIGHT.color.argb_full : FG_2_DARK.color.argb_full);
            }
            else
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
    if (control_invalid) { return; }
    gui_obj_t *o = GUI_BASE(obj);
    touch_info_t *tp = tp_get_info();
    if (tp->pressed && gui_obj_point_in_obj_rect(o, tp->x, tp->y))
    {
        if (strcmp(o->name, "l") == 0)
        {
            if (euqalizer_type_index != 0)
            {
                pressed_l = true;
                gui_obj_hidden(o, false);
            }
        }
        else
        {
            if (euqalizer_type_index != 5)
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

            if (strcmp(o->name, "l") == 0)
            {
                compare = 0;
            }
            else
            {
                compare = 5;
            }

            if (pressed_l)
            {
                euqalizer_type_index--;
                GUI_WIDGET_POINTER_BY_NAME_ROOT(euqalizer_type, "euqalizer_type", o->parent);
                gui_img_set_image_data((gui_img_t *)euqalizer_type, img_data_array[euqalizer_type_index]);
            }
            else if (pressed_r)
            {
                euqalizer_type_index++;
                GUI_WIDGET_POINTER_BY_NAME_ROOT(euqalizer_type, "euqalizer_type", o->parent);
                gui_img_set_image_data((gui_img_t *)euqalizer_type, img_data_array[euqalizer_type_index]);
            }

            if (euqalizer_type_index == compare)
            {
                gui_img_a8_recolor(icon, theme_bg_white ? FG_2_LIGHT.color.argb_full : FG_2_DARK.color.argb_full);
            }
            else
            {
                gui_img_a8_recolor(icon, theme_bg_white ? FG_1_LIGHT.color.argb_full : FG_1_DARK.color.argb_full);
            }
            gui_img_a8_mix_alpha(icon, icon->fg_color_set >> 24);
        }
        pressed_l = false;
        pressed_r = false;
    }
}

static void press_button_page_playback(void *obj)
{
    GUI_UNUSED(obj);
    gui_obj_t *o = GUI_BASE(obj);
    touch_info_t *tp = tp_get_info();
    if (tp->pressed && gui_obj_point_in_obj_rect(o, tp->x, tp->y))
    {
        if (strcmp(o->name, "l") == 0)
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
                GUI_WIDGET_POINTER_BY_NAME_ROOT(text, (void *)page_name_array[PLAYBACK], o->parent->parent);
                gui_scroll_text_content_set((void *)text, (void *)music_array[music_index],
                                            strlen(music_array[music_index]));
            }
            else if (pressed_r)
            {
                music_index++;
                GUI_WIDGET_POINTER_BY_NAME_ROOT(text, (void *)page_name_array[PLAYBACK], o->parent->parent);
                gui_scroll_text_content_set((void *)text, (void *)music_array[music_index],
                                            strlen(music_array[music_index]));
            }

            if (strcmp(o->name, "l") == 0)
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
        if (strcmp(o->name, "l") == 0)
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

            if (strcmp(o->name, "l") == 0)
            {
                compare = 1;
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
            else
            {
                gui_img_a8_recolor(icon, theme_bg_white ? FG_1_LIGHT.color.argb_full : FG_1_DARK.color.argb_full);
            }
            gui_img_a8_mix_alpha(icon, icon->fg_color_set >> 24);
        }
        pressed_r = false;
        pressed_l = false;
    }
}

static void press_button_page_auto_dim_off_screen(void *obj)
{
    GUI_UNUSED(obj);
    if (control_invalid) { return; }
    gui_obj_t *o = GUI_BASE(obj);
    touch_info_t *tp = tp_get_info();
    if (tp->pressed && gui_obj_point_in_obj_rect(o, tp->x, tp->y))
    {
        if (strcmp(o->name, "l") == 0)
        {
            if (auto_dim_time_val != 5)
            {
                pressed_l = true;
                gui_obj_hidden(o, false);
            }
        }
        else
        {
            if (auto_dim_time_val != 60)
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

            if (strcmp(o->name, "l") == 0)
            {
                compare = 5;
            }
            else
            {
                compare = 60;
            }
            if (pressed_r)
            {
                auto_dim_time_val += 5;
            }
            else if (pressed_l)
            {
                auto_dim_time_val -= 5;
            }

            if (auto_dim_time_val == compare)
            {
                gui_img_a8_recolor(icon, theme_bg_white ? FG_2_LIGHT.color.argb_full : FG_2_DARK.color.argb_full);
            }
            else
            {
                gui_img_a8_recolor(icon, theme_bg_white ? FG_1_LIGHT.color.argb_full : FG_1_DARK.color.argb_full);
            }
            gui_img_a8_mix_alpha(icon, icon->fg_color_set >> 24);
            gui_obj_t *text = gui_list_entry(o->parent->child_list.prev, gui_obj_t, brother_list);
            sprintf(page_str, "%d", auto_dim_time_val);
            gui_text_content_set((void *)text, page_str, strlen(page_str));
        }
        pressed_r = false;
        pressed_l = false;
    }
}

static void press_button_page_timer(void *obj)
{
    GUI_UNUSED(obj);
    gui_obj_t *o = GUI_BASE(obj);
    touch_info_t *tp = tp_get_info();
    static uint32_t time_rec = 0;
    static bool flag = 0;
    if (tp->pressed && gui_obj_point_in_obj_rect(o, tp->x, tp->y))
    {
        if (strcmp(o->name, "l") == 0)
        {
            if (timer_max_min_val != 1)
            {
                pressed_l = true;
                gui_obj_hidden(o, false);
                time_rec = gui_ms_get();
            }
        }
        else
        {
            if (timer_max_min_val != 60)
            {
                pressed_r = true;
                gui_obj_hidden(o, false);
                time_rec = gui_ms_get();
            }
        }
    }
    else if (tp->pressing)
    {
        if (tp->deltaX != 0 || tp->deltaY != 0)
        {
            gui_obj_hidden(o, true);
        }
        else if ((pressed_r || pressed_l) && (gui_ms_get() - time_rec >= 300))
        {
            time_rec = gui_ms_get();
            flag = 1;
            if (pressed_r)
            {
                timer_max_min_val += 5;
                if (timer_max_min_val >= 60)
                {
                    timer_max_min_val = 60;
                }
            }
            else if (pressed_l)
            {
                timer_max_min_val -= 5;
                if (timer_max_min_val <= 1)
                {
                    timer_max_min_val = 1;
                }
            }
            gui_obj_t *text = gui_list_entry(o->parent->child_list.prev, gui_obj_t, brother_list);
            sprintf(page_str, "%d", timer_max_min_val);
            gui_text_content_set((void *)text, page_str, strlen(page_str));
        }
    }
    else if (tp->released)
    {
        gui_obj_hidden(o, true);
        if (tp->deltaX == 0 && tp->deltaY == 0)
        {
            int8_t compare;
            gui_img_t *icon = (gui_img_t *)gui_list_entry(o->brother_list.next, gui_obj_t, brother_list);

            if (strcmp(o->name, "l") == 0)
            {
                compare = 1;
            }
            else
            {
                compare = 60;
            }
            if (!flag)
            {
                if (pressed_r)
                {
                    timer_max_min_val++;
                }
                else if (pressed_l)
                {
                    timer_max_min_val--;
                }
                gui_obj_t *text = gui_list_entry(o->parent->child_list.prev, gui_obj_t, brother_list);
                sprintf(page_str, "%d", timer_max_min_val);
                gui_text_content_set((void *)text, page_str, strlen(page_str));
            }
            else
            {
                flag = 0;
            }
            if (timer_max_min_val == compare)
            {
                gui_img_a8_recolor(icon, theme_bg_white ? FG_2_LIGHT.color.argb_full : FG_2_DARK.color.argb_full);
            }
            else
            {
                gui_img_a8_recolor(icon, theme_bg_white ? FG_1_LIGHT.color.argb_full : FG_1_DARK.color.argb_full);
            }
            gui_img_a8_mix_alpha(icon, icon->fg_color_set >> 24);
        }
        pressed_r = false;
        pressed_l = false;
    }
}

static void press_button_page_language(void *obj)
{
    GUI_UNUSED(obj);
    if (control_invalid) { return; }
    gui_obj_t *o = GUI_BASE(obj);
    touch_info_t *tp = tp_get_info();
    if (tp->pressed && gui_obj_point_in_obj_rect(o, tp->x, tp->y))
    {
        if (strcmp(o->name, "l") == 0)
        {
            if (language_type_index != 0)
            {
                pressed_l = true;
                gui_obj_hidden(o, false);
            }
        }
        else
        {
            if (language_type_index != 12)
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

            if (strcmp(o->name, "l") == 0)
            {
                compare = 0;
            }
            else
            {
                compare = 12;
            }
            gui_obj_t *text = gui_list_entry(o->parent->child_list.prev, gui_obj_t, brother_list);
            if (pressed_l)
            {
                language_type_index--;
                gui_text_content_set((void *)text, (void *)language_type[language_type_index],
                                     strlen(language_type[language_type_index]));
            }
            else if (pressed_r)
            {
                language_type_index++;
                gui_text_content_set((void *)text, (void *)language_type[language_type_index],
                                     strlen(language_type[language_type_index]));
            }

            if (language_type_index == compare)
            {
                gui_img_a8_recolor(icon, theme_bg_white ? FG_2_LIGHT.color.argb_full : FG_2_DARK.color.argb_full);
            }
            else
            {
                gui_img_a8_recolor(icon, theme_bg_white ? FG_1_LIGHT.color.argb_full : FG_1_DARK.color.argb_full);
            }
            gui_img_a8_mix_alpha(icon, icon->fg_color_set >> 24);
        }
        pressed_l = false;
        pressed_r = false;
    }
}

static void click_button_page_ambient_sound(void *obj, gui_event_t e, void *param)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);
    GUI_UNUSED(param);
    if (control_invalid) { return; }
    gui_obj_t *o = GUI_BASE(obj);
    if (strcmp(o->name, "l") == 0)
    {
        ambient_sound_type = ANC;
        button_bg_target_x = BUTTON_X_LEFT;
    }
    else if (strcmp(o->name, "m") == 0)
    {
        ambient_sound_type = AA;
        button_bg_target_x = BUTTON_X_MID;
    }
    else
    {
        ambient_sound_type = TT;
        button_bg_target_x = BUTTON_X_RIGHT;
    }
    gui_obj_create_timer(gui_list_entry(o->parent->child_list.next, gui_obj_t, brother_list), 10, true,
                         button_move);
}

static void click_button_page_playback(void *obj, gui_event_t e, void *param)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);
    GUI_UNUSED(param);
    gui_obj_t *o = GUI_BASE(obj);
    f_status.playback = !f_status.playback;
    gui_scroll_text_t *name = (void *)gui_list_entry(o->parent->parent->child_list.next, gui_obj_t,
                                                     brother_list);
    gui_scroll_text_t *lyrics = (void *)gui_list_entry(GUI_BASE(name)->brother_list.next, gui_obj_t,
                                                       brother_list);
    if (f_status.playback)
    {
        gui_scroll_text_resume(name);
        gui_scroll_text_resume(lyrics);
        gui_img_set_image_data((gui_img_t *)o, PAUSE_BIN);
    }
    else
    {
        gui_scroll_text_pause(name);
        gui_scroll_text_pause(lyrics);
        gui_img_set_image_data((gui_img_t *)o, PLAY_BIN);
    }
    gui_img_refresh_size((gui_img_t *)o);
}

static void click_button_page_smart_talk(void *obj, gui_event_t e, void *param)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);
    GUI_UNUSED(param);
    if (control_invalid) { return; }
    gui_obj_t *o = GUI_BASE(obj);
    if (strcmp(o->name, "l") == 0)
    {
        smart_talk_time_type = 0;
        button_bg_target_x = BUTTON_X_LEFT;
    }
    else if (strcmp(o->name, "m") == 0)
    {
        smart_talk_time_type = 1;
        button_bg_target_x = BUTTON_X_MID;
    }
    else
    {
        smart_talk_time_type = 2;
        button_bg_target_x = BUTTON_X_RIGHT;
    }
    gui_obj_create_timer(gui_list_entry(o->parent->child_list.next, gui_obj_t, brother_list), 10, true,
                         button_move);
}

static void click_button_page_spatial_sound(void *obj, gui_event_t e, void *param)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);
    GUI_UNUSED(param);
    if (control_invalid) { return; }
    gui_obj_t *o = GUI_BASE(obj);
    if (strcmp(o->name, "l") == 0)
    {
        spatial_sound_type = SPATIAL_SOUND_MOVIE;
        button_bg_target_x = BUTTON_X_LEFT;
    }
    else if (strcmp(o->name, "m") == 0)
    {
        spatial_sound_type = SPATIAL_SOUND_MUSIC;
        button_bg_target_x = BUTTON_X_MID;
    }
    else
    {
        spatial_sound_type = SPATIAL_SOUND_GAME;
        button_bg_target_x = BUTTON_X_RIGHT;
    }
    gui_obj_create_timer(gui_list_entry(o->parent->child_list.next, gui_obj_t, brother_list), 10, true,
                         button_move);
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
    gui_view_set_animate_step(gui_view_get_current(), 400);
    gui_view_switch_direct(gui_view_get_current(), clock_view, SWITCH_OUT_NONE_ANIMATION,
                           SWITCH_IN_NONE_ANIMATION);
}

static void click_button_page_lock_screen(void *obj, gui_event_t e, void *param)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);
    GUI_UNUSED(param);
    gui_obj_t *o = GUI_BASE(obj);
    gui_img_t *icon_l;
    gui_img_t *icon_r;
    bool is_switch_l = (strcmp(o->name, "l") == 0);
    if (is_switch_l)
    {
        clock_style--;
        icon_l = (gui_img_t *)gui_list_entry(o->child_list.next, gui_obj_t, brother_list);
        icon_r = (gui_img_t *)gui_list_entry(gui_list_entry(o->brother_list.next, gui_obj_t,
                                                            brother_list)->child_list.next, gui_obj_t, brother_list);
    }
    else
    {
        clock_style++;
        icon_l = (gui_img_t *)gui_list_entry(gui_list_entry(o->brother_list.prev, gui_obj_t,
                                                            brother_list)->child_list.next, gui_obj_t, brother_list);
        icon_r = (gui_img_t *)gui_list_entry(o->child_list.next, gui_obj_t, brother_list);
    }
    if (clock_style <= 0)
    {
        clock_style = 0;
        gui_img_a8_recolor(icon_l, theme_bg_white ? FG_2_LIGHT.color.argb_full : FG_2_DARK.color.argb_full);
    }
    else if (clock_style >= 4)
    {
        clock_style = 4;
        gui_img_a8_recolor(icon_r, theme_bg_white ? FG_2_LIGHT.color.argb_full : FG_2_DARK.color.argb_full);
    }
    else
    {
        gui_img_a8_recolor(icon_l, theme_bg_white ? FG_1_LIGHT.color.argb_full : FG_1_DARK.color.argb_full);
        gui_img_a8_recolor(icon_r, theme_bg_white ? FG_1_LIGHT.color.argb_full : FG_1_DARK.color.argb_full);
    }
    gui_img_a8_mix_alpha((void *)icon_l, icon_l->fg_color_set >> 24);
    gui_img_a8_mix_alpha((void *)icon_r, icon_r->fg_color_set >> 24);

    gui_obj_t *img = gui_list_entry(o->parent->child_list.prev, gui_obj_t, brother_list);
    gui_img_set_image_data((gui_img_t *)img, img_data_array[clock_style]);
}

static void click_button_page_quick_wake_up_screen(void *obj, gui_event_t e, void *param)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);
    GUI_UNUSED(param);
    if (control_invalid) { return; }
    gui_obj_t *o = GUI_BASE(obj);
    if (strcmp(o->name, "l") == 0)
    {
        quick_wake_up_click_num = 0;
        button_bg_target_x = BUTTON_X_LEFT;
    }
    else if (strcmp(o->name, "m") == 0)
    {
        quick_wake_up_click_num = 1;
        button_bg_target_x = BUTTON_X_MID;
    }
    else
    {
        quick_wake_up_click_num = 2;
        button_bg_target_x = BUTTON_X_RIGHT;
    }
    gui_obj_create_timer(gui_list_entry(o->parent->child_list.next, gui_obj_t, brother_list), 10, true,
                         button_move);
}

static void click_button_page_time_format(void *obj, gui_event_t e, void *param)
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

static void click_button_page_find_buds(void *obj, gui_event_t e, void *param)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);
    GUI_UNUSED(param);
    gui_obj_t *o = GUI_BASE(obj);
    if (strcmp(o->name, "l") == 0)
    {
        if (f_status.earbuds_shock_l)
        {
            f_status.earbuds_shock_l = 0;
            gui_obj_t *icon = gui_list_entry(o->child_list.next, gui_obj_t, brother_list);
            gui_img_set_image_data((void *)icon, ICON_L_BIN);
            gui_img_refresh_size((void *)icon);
            if (theme_bg_white)
            {
                gui_img_a8_recolor((void *)o, BG_THEME2_BRIGHT_LIGHT.color.argb_full);
                gui_img_a8_recolor((void *)icon, FG_1_LIGHT.color.argb_full);
            }
            else
            {
                gui_img_a8_recolor((void *)o, BG_THEME2_BRIGHT_DARK.color.argb_full);
                gui_img_a8_recolor((void *)icon, FG_1_DARK.color.argb_full);
            }
            return;
        }
        f_status.earbuds_find_l = 1;
    }
    else
    {
        if (f_status.earbuds_shock_r)
        {
            f_status.earbuds_shock_r = 0;
            gui_obj_t *icon = gui_list_entry(o->child_list.next, gui_obj_t, brother_list);
            gui_img_set_image_data((void *)icon, ICON_R_BIN);
            gui_img_refresh_size((void *)icon);
            if (theme_bg_white)
            {
                gui_img_a8_recolor((void *)o, BG_THEME2_BRIGHT_LIGHT.color.argb_full);
                gui_img_a8_recolor((void *)icon, FG_1_LIGHT.color.argb_full);
            }
            else
            {
                gui_img_a8_recolor((void *)o, BG_THEME2_BRIGHT_DARK.color.argb_full);
                gui_img_a8_recolor((void *)icon, FG_1_DARK.color.argb_full);
            }
            return;
        }
        f_status.earbuds_find_r = 1;
    }
    // gui_view_set_animate_step(gui_view_get_current(), 400);
    gui_view_switch_direct(gui_view_get_current(), find_buds_view, SWITCH_OUT_NONE_ANIMATION,
                           SWITCH_IN_NONE_ANIMATION);
}

static void click_button_page_timer(void *obj, gui_event_t e, void *param)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);
    GUI_UNUSED(param);
    f_status.timer = 0;
    gui_view_switch_direct(gui_view_get_current(), timer_view, SWITCH_OUT_ANIMATION_MOVE_TO_LEFT,
                           SWITCH_IN_ANIMATION_MOVE_FROM_RIGHT);
}

static void click_button_page_flashlight(void *obj, gui_event_t e, void *param)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);
    GUI_UNUSED(param);
    f_status.timer = 0;
    gui_view_set_animate_step(gui_view_get_current(), 400);
    gui_view_switch_direct(gui_view_get_current(), flashlight_view, SWITCH_OUT_NONE_ANIMATION,
                           SWITCH_IN_NONE_ANIMATION);
}

static void click_button_page_case_button_customize(void *obj, gui_event_t e, void *param)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);
    GUI_UNUSED(param);
    int16_t index = *((int16_t *)param);
    case_button_customize_type_index = index % 3;
    if (index < 3)
    {
        gui_obj_t *parent = GUI_BASE(obj)->parent->parent;
        gui_obj_t *bg_note = gui_list_entry(parent->child_list.next, gui_obj_t, brother_list);
        gui_obj_move(bg_note, 0, GUI_BASE(obj)->y + 60);
        gui_obj_hidden(bg_note, false);

        gui_view_switch_direct(gui_view_get_current(), button_customize_view,
                               SWITCH_OUT_ANIMATION_MOVE_TO_LEFT,
                               SWITCH_IN_ANIMATION_MOVE_FROM_RIGHT);
    }
    else
    {
        case_button_customize_index_array[0] = 0;
        case_button_customize_index_array[1] = 0;
        case_button_customize_index_array[2] = 0;
        gui_obj_t *img = obj;
        gui_obj_t *text = gui_list_entry(img->child_list.next, gui_obj_t, brother_list);
        gui_img_a8_mix_alpha((void *)img, 122);
        gui_color_t font_color;
        font_color.color.argb_full = theme_bg_white ? GUI_COLOR_ARGB8888(0xFF, 0x93, 0x93,
                                                                         0x93) : GUI_COLOR_ARGB8888(0xFF, 122, 122, 122);
        gui_text_color_set((void *)text, font_color);
    }
}

static void click_button_page_reorder_quick_access(void *obj, gui_event_t e, void *param)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);
    GUI_UNUSED(param);
    gui_obj_t *o = GUI_BASE(obj);
    gui_obj_t *parent = o->parent;
    if (control_invalid || parent->y <= -10) { return; }

    if (strcmp(o->name, "up") == 0)
    {
        control_invalid = true;
        gui_obj_create_timer(parent, 10, true, note_timer_up_page_reorder_quick_access);
        gui_obj_start_timer(parent);
    }
    else
    {
        control_invalid = true;
        gui_obj_create_timer(parent, 10, true, note_timer_down_page_reorder_quick_access);
        gui_obj_start_timer(parent);
    }
}

static void click_button_page_information_center_customize(void *obj, gui_event_t e, void *param)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);
    GUI_UNUSED(param);
    gui_obj_t *o = GUI_BASE(obj);
    int16_t index = *((int16_t *)param);
    uint8_t add;
    if (strcmp(o->name, "l") == 0)
    {
        if (index == 0)
        {
            f_status.infor_center_func_0 = !f_status.infor_center_func_0;
            add = f_status.infor_center_func_0;

        }
        else
        {
            f_status.infor_center_func_3 = !f_status.infor_center_func_3;
            add = f_status.infor_center_func_3;
        }
    }
    else if (strcmp(o->name, "m") == 0)
    {
        if (index == 0)
        {
            f_status.infor_center_func_1 = !f_status.infor_center_func_1;
            add = f_status.infor_center_func_1;
        }
        else
        {
            f_status.infor_center_func_4 = !f_status.infor_center_func_4;
            add = f_status.infor_center_func_4;
        }
    }
    else
    {
        if (index == 0)
        {
            f_status.infor_center_func_2 = !f_status.infor_center_func_2;
            add = f_status.infor_center_func_2;
        }
        else
        {
            f_status.infor_center_func_5 = !f_status.infor_center_func_5;
            add = f_status.infor_center_func_5;
        }
    }
    info_center_func_cnt += (add * 2 - 1);
    gui_msg_t msg =
    {
        .event = GUI_EVENT_USER_DEFINE,
        .cb = regenerate_current_view,
    };
    gui_send_msg_to_server(&msg);
}

static void click_button_page_support(void *obj, gui_event_t e, void *param)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);
    GUI_UNUSED(param);
    gui_obj_t *o = GUI_BASE(obj);
    gui_obj_t *bg = gui_list_entry(o->child_list.next, gui_obj_t, brother_list);
    extern bool support_reset;
    if (o->parent->child_list.next == &o->brother_list)
    {
        support_reset = false;
    }
    else
    {
        support_reset = true;
    }
    gui_obj_hidden(bg, false);
    gui_view_switch_direct(gui_view_get_current(), support_view, SWITCH_OUT_ANIMATION_MOVE_TO_LEFT,
                           SWITCH_IN_ANIMATION_MOVE_FROM_RIGHT);
}
static void click_toggle_cb(void *obj, gui_event_t e, void *param)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);
    GUI_UNUSED(param);
    gui_obj_t *o = GUI_BASE(obj);
    bool *status = param;
    gui_obj_t *toggle = gui_list_entry(o->child_list.next, gui_obj_t, brother_list);

    *status = !(*status);
    if (*status)
    {
        toggle_target_x = TOGGLE_ON_X;
    }
    else
    {
        toggle_target_x = TOGGLE_OFF_X;
    }
    control_invalid = !(*status);
    gui_obj_create_timer(toggle, 10, true, toggle_move);
}

static void click_button_no_action(void *obj, gui_event_t e, void *param)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);
    GUI_UNUSED(param);
}

static void note_design_page_quick_wake_up(gui_obj_t *obj, void *p)
{
    GUI_UNUSED(p);
    gui_color_t font_color;
    if (theme_bg_white)
    {
        font_color.color.argb_full = GUI_COLOR_ARGB8888(0xFF, 0x93, 0x93, 0x93);
    }
    else
    {
        font_color.color.argb_full = GUI_COLOR_ARGB8888(0xFF, 122, 122, 122);
    }
    gui_text_t *text = gui_text_create(obj, 0, 12, 0, 296, 170);
    gui_text_set(text, (void *)quick_wake_up_warning_str, GUI_FONT_SRC_BMP, font_color,
                 strlen(quick_wake_up_warning_str), 30);
    gui_text_type_set(text, CAPTION_3_30_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, MULTI_LEFT);
}

static void note_design_page_case_button_customize(gui_obj_t *obj, void *p)
{
    GUI_UNUSED(p);
    gui_list_note_t *note = (gui_list_note_t *)obj;
    int16_t index = note->index;
    gui_color_t font_color;
    gui_color_t img_color;
    if (theme_bg_white)
    {
        font_color = FG_1_LIGHT;
        img_color = BG_THEME3_BRIGHT_LIGHT;
    }
    else
    {
        font_color = FG_1_DARK;
        img_color = BG_THEME3_BRIGHT_DARK;
    }
    if (index < 3)
    {
        char *text = (char *)case_button_customize_type[index];
        int font_size = 30;
        gui_text_t *t = gui_text_create(note, 0, 10, 0, 200, 56);
        gui_text_set(t, text, GUI_FONT_SRC_BMP, font_color, strlen(text), font_size);
        gui_text_type_set(t, CAPTION_3_30_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(t, MID_LEFT);
        gui_img_t *arrow = gui_img_create_from_mem(note, 0, ICON_ARROW_R_BIG_BIN, 291, 18, 0, 0);
        gui_img_a8_recolor(arrow, font_color.color.argb_full);
        gui_obj_add_event_cb(note, click_button_page_case_button_customize, GUI_EVENT_TOUCH_CLICKED,
                             &(note->index));
    }
    else if (index == 3)
    {
        gui_img_t *img = gui_img_create_from_mem(note, 0, BUTTON_BG_ELLIPSE_201_80_BIN, 60, 24, 0, 0);
        gui_img_a8_recolor(img, img_color.color.argb_full);
        if (case_button_customize_index_array[0] ||
            case_button_customize_index_array[1] ||
            case_button_customize_index_array[2])
        {
            gui_obj_add_event_cb(img, click_button_page_case_button_customize, GUI_EVENT_TOUCH_CLICKED,
                                 &(note->index));
        }
        else
        {
            gui_img_a8_mix_alpha(img, 122);
            font_color.color.argb_full = theme_bg_white ? GUI_COLOR_ARGB8888(0xFF, 0x93, 0x93,
                                                                             0x93) : GUI_COLOR_ARGB8888(0xFF, 122, 122, 122);
        }

        gui_text_t *t = gui_text_create(img, 0, 0, 0, 201, 80);
        gui_text_set(t, "Reset", GUI_FONT_SRC_BMP, font_color, 5, 32);
        gui_text_type_set(t, CAPTION_1_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(t, MID_CENTER);
    }
}

static void note_design_page_reorder_quick_access(gui_obj_t *obj, void *p)
{
    GUI_UNUSED(p);
    gui_list_note_t *note = (gui_list_note_t *)obj;
    int16_t index = note->index;
    gui_color_t font_color;
    gui_color_t bg_color;
    gui_color_t icon_color;
    if (theme_bg_white)
    {
        font_color.color.argb_full = GUI_COLOR_ARGB8888(0xFF, 0x93, 0x93, 0x93);
        icon_color = FG_1_LIGHT;
        bg_color = BG_1_LIGHT;
    }
    else
    {
        font_color.color.argb_full = GUI_COLOR_ARGB8888(0xFF, 122, 122, 122);
        icon_color = FG_1_DARK;
        bg_color = BG_1_DARK;
    }
    gui_text_t *text = gui_text_create(obj, 0, 0, 0, 24, 56);
    gui_text_set(text, (void *)(page_str + index), GUI_FONT_SRC_BMP, font_color, 1, 20);
    gui_text_type_set(text, HEADING_1_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, MID_CENTER);

    gui_img_t *bg_up = gui_img_create_from_mem(obj, "up", BG_CIRCLE_40_BIN, 28, 8, 0, 0);
    gui_img_t *up = gui_img_create_from_mem(bg_up, 0, ICON_ARROW_UP_BIN, 10, 14, 0, 0);
    gui_img_t *bg_down = gui_img_create_from_mem(obj, "down", BG_CIRCLE_40_BIN, 84, 8, 0, 0);
    gui_img_t *down = gui_img_create_from_mem(bg_down, 0, ICON_ARROW_DOWN_BIN, 10, 14, 0, 0);
    gui_img_a8_recolor(bg_up, bg_color.color.argb_full);
    gui_img_a8_recolor(bg_down, bg_color.color.argb_full);
    gui_img_a8_mix_alpha(bg_up, bg_up->fg_color_set >> 24);
    gui_img_a8_mix_alpha(bg_down, bg_down->fg_color_set >> 24);
    if (index == 0)
    {
        gui_img_a8_recolor(up, bg_color.color.argb_full);
        gui_img_a8_mix_alpha(up, up->fg_color_set >> 24);
    }
    else
    {
        gui_img_a8_recolor(up, icon_color.color.argb_full);
        gui_obj_add_event_cb(bg_up, click_button_page_reorder_quick_access, GUI_EVENT_TOUCH_CLICKED, NULL);
    }
    if (index == quick_page_num - 1)
    {
        gui_img_a8_recolor(down, bg_color.color.argb_full);
        gui_img_a8_mix_alpha(down, down->fg_color_set >> 24);
    }
    else
    {
        gui_img_a8_recolor(down, icon_color.color.argb_full);
        gui_obj_add_event_cb(bg_down, click_button_page_reorder_quick_access, GUI_EVENT_TOUCH_CLICKED,
                             NULL);
    }

    gui_scroll_text_t *t = gui_scroll_text_create(obj, 0, 136, 12, 170, 30);
    gui_scroll_text_type_set(t, CAPTION_3_30_BIN, FONT_SRC_MEMADDR);
    gui_scroll_text_set(t, (void *)quick_page_name[index], GUI_FONT_SRC_BMP, icon_color,
                        strlen(quick_page_name[index]), 30);
    gui_scroll_text_scroll_set(t, SCROLL_X, 170, 170, 3000, 0);
}

static void note_design_page_information_center_customize(gui_obj_t *obj, void *p)
{
    GUI_UNUSED(p);
    gui_list_note_t *note = (gui_list_note_t *)obj;
    int16_t index = note->index;
    gui_color_t font_color;
    gui_color_t font_color_invalid;
    gui_color_t bg_color;
    gui_color_t bg_theme1_color;
    gui_color_t bg_theme2_color;
    gui_color_t bg_theme3_color;
    if (theme_bg_white)
    {
        font_color_invalid.color.argb_full = GUI_COLOR_ARGB8888(0xFF, 0x93, 0x93, 0x93);
        font_color = FG_1_LIGHT;
        bg_color = SCREEN_BG_LIGHT;
        bg_theme1_color = BG_THEME1_BRIGHT_LIGHT;
        bg_theme2_color = BG_THEME2_BRIGHT_LIGHT;
        bg_theme3_color = BG_THEME3_BRIGHT_LIGHT;
    }
    else
    {
        font_color_invalid.color.argb_full = GUI_COLOR_ARGB8888(0xFF, 122, 122, 122);
        font_color = FG_1_DARK;
        bg_color = SCREEN_BG_DARK;
        bg_theme1_color = BG_THEME1_BRIGHT_DARK;
        bg_theme2_color = BG_THEME2_BRIGHT_DARK;
        bg_theme3_color = BG_THEME3_BRIGHT_DARK;
    }
    if (index == 0)
    {
        gui_img_t *bg_l = gui_img_create_from_mem(obj, "l", BUTTON_BG_ELLIPSE_90_68_BIN, 12, 0, 0, 0);
        gui_img_t *bg_m = gui_img_create_from_mem(obj, "m", BUTTON_BG_ELLIPSE_90_68_BIN, 115, 0, 0, 0);
        gui_img_t *bg_r = gui_img_create_from_mem(obj, "r", BUTTON_BG_ELLIPSE_90_68_BIN, 218, 0, 0, 0);
        gui_img_t *icon_l = gui_img_create_from_mem(bg_l, 0, PAUSE_SMALL_BIN, 36, 22, 0, 0);
        gui_img_t *icon_m = gui_img_create_from_mem(bg_m, 0, ICON_ASC_BIN, 25, 14, 0, 0);
        gui_img_t *icon_r = gui_img_create_from_mem(bg_r, 0, ICON_EQ_BIN, 25, 14, 0, 0);

        gui_text_t *text_l = gui_text_create(obj, 0, 12, 70, 98, 40);
        gui_text_set(text_l, (void *)info_center_func_name[0], GUI_FONT_SRC_BMP, font_color,
                     strlen(info_center_func_name[0]), 20);
        gui_text_type_set(text_l, HEADING_1_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(text_l, MULTI_CENTER);
        gui_text_t *text_m = gui_text_create(obj, 0, 115, 70, 98, 40);
        gui_text_set(text_m, (void *)info_center_func_name[1], GUI_FONT_SRC_BMP, font_color,
                     strlen(info_center_func_name[1]), 20);
        gui_text_type_set(text_m, HEADING_1_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(text_m, MULTI_CENTER);
        gui_text_t *text_r = gui_text_create(obj, 0, 218, 70, 98, 40);
        gui_text_set(text_r, (void *)info_center_func_name[2], GUI_FONT_SRC_BMP, font_color,
                     strlen(info_center_func_name[2]), 20);
        gui_text_type_set(text_r, HEADING_1_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(text_r, MULTI_CENTER);

        gui_img_a8_recolor(bg_l, bg_theme1_color.color.argb_full);
        gui_img_a8_recolor(bg_m, bg_l->fg_color_set);
        gui_img_a8_recolor(bg_r, bg_l->fg_color_set);
        gui_img_a8_fix_bg(bg_l, bg_color.color.argb_full);
        gui_img_a8_fix_bg(bg_m, bg_l->bg_color_fix);
        gui_img_a8_fix_bg(bg_r, bg_l->bg_color_fix);
        gui_img_a8_recolor(icon_l, FG_WHITE.color.argb_full);
        gui_img_a8_recolor(icon_m, icon_l->fg_color_set);
        gui_img_a8_recolor(icon_r, icon_l->fg_color_set);
        if (f_status.infor_center_func_0)
        {
            gui_img_t *icon_done = gui_img_create_from_mem(bg_l, 0, ICON_DONE_BIN, 0, 0, 0, 0);
            gui_img_set_mode(icon_done, IMG_2D_SW_SRC_OVER_MODE);
            gui_obj_add_event_cb(bg_l, click_button_page_information_center_customize, GUI_EVENT_TOUCH_CLICKED,
                                 &note->index);
        }
        else if (info_center_func_cnt == 3)
        {
            gui_img_a8_mix_alpha(bg_l, 122);
            gui_img_a8_mix_alpha(icon_l, 122);
            gui_text_color_set(text_l, font_color_invalid);
        }
        else
        {
            gui_obj_add_event_cb(bg_l, click_button_page_information_center_customize, GUI_EVENT_TOUCH_CLICKED,
                                 &note->index);
        }
        if (f_status.infor_center_func_1)
        {
            gui_img_t *icon_done = gui_img_create_from_mem(bg_m, 0, ICON_DONE_BIN, 0, 0, 0, 0);
            gui_img_set_mode(icon_done, IMG_2D_SW_SRC_OVER_MODE);
            gui_obj_add_event_cb(bg_m, click_button_page_information_center_customize, GUI_EVENT_TOUCH_CLICKED,
                                 &note->index);
        }
        else if (info_center_func_cnt == 3)
        {
            gui_img_a8_mix_alpha(bg_m, 122);
            gui_img_a8_mix_alpha(icon_m, 122);
            gui_text_color_set(text_m, font_color_invalid);
        }
        else
        {
            gui_obj_add_event_cb(bg_m, click_button_page_information_center_customize, GUI_EVENT_TOUCH_CLICKED,
                                 &note->index);
        }
        if (f_status.infor_center_func_2)
        {
            gui_img_t *icon_done = gui_img_create_from_mem(bg_r, 0, ICON_DONE_BIN, 0, 0, 0, 0);
            gui_img_set_mode(icon_done, IMG_2D_SW_SRC_OVER_MODE);
            gui_obj_add_event_cb(bg_r, click_button_page_information_center_customize, GUI_EVENT_TOUCH_CLICKED,
                                 &note->index);
        }
        else if (info_center_func_cnt == 3)
        {
            gui_img_a8_mix_alpha(bg_r, 122);
            gui_img_a8_mix_alpha(icon_r, 122);
            gui_text_color_set(text_r, font_color_invalid);
        }
        else
        {
            gui_obj_add_event_cb(bg_r, click_button_page_information_center_customize, GUI_EVENT_TOUCH_CLICKED,
                                 &note->index);
        }
    }
    else
    {
        gui_img_t *bg_l = gui_img_create_from_mem(obj, "l", BUTTON_BG_ELLIPSE_90_68_BIN, 12, 0, 0, 0);
        gui_img_t *bg_m = gui_img_create_from_mem(obj, "m", BUTTON_BG_ELLIPSE_90_68_BIN, 115, 0, 0, 0);
        gui_img_t *bg_r = gui_img_create_from_mem(obj, "r", BUTTON_BG_ELLIPSE_90_68_BIN, 218, 0, 0, 0);
        gui_img_t *icon_l = gui_img_create_from_mem(bg_l, 0, ICON_SPATIAL_SOUND_BIN, 25, 14, 0, 0);
        gui_img_t *icon_m = gui_img_create_from_mem(bg_m, 0, ICON_FLASHLIGHT_BIN, 25, 14, 0, 0);
        gui_img_t *icon_r = gui_img_create_from_mem(bg_r, 0, ICON_LIGHTDARKMODE_BIN, 25, 14, 0, 0);

        gui_text_t *text_l = gui_text_create(obj, 0, 12, 70, 98, 40);
        gui_text_set(text_l, (void *)info_center_func_name[3], GUI_FONT_SRC_BMP, font_color,
                     strlen(info_center_func_name[3]), 20);
        gui_text_type_set(text_l, HEADING_1_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(text_l, MULTI_CENTER);
        gui_text_t *text_m = gui_text_create(obj, 0, 115, 70, 98, 40);
        gui_text_set(text_m, (void *)info_center_func_name[4], GUI_FONT_SRC_BMP, font_color,
                     strlen(info_center_func_name[4]), 20);
        gui_text_type_set(text_m, HEADING_1_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(text_m, MULTI_CENTER);
        gui_text_t *text_r = gui_text_create(obj, 0, 218, 70, 98, 40);
        gui_text_set(text_r, (void *)info_center_func_name[5], GUI_FONT_SRC_BMP, font_color,
                     strlen(info_center_func_name[5]), 20);
        gui_text_type_set(text_r, HEADING_1_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(text_r, MULTI_CENTER);

        gui_img_a8_recolor(bg_l, bg_theme1_color.color.argb_full);
        gui_img_a8_recolor(bg_m, bg_theme2_color.color.argb_full);
        gui_img_a8_recolor(bg_r, bg_theme3_color.color.argb_full);
        gui_img_a8_fix_bg(bg_l, bg_color.color.argb_full);
        gui_img_a8_fix_bg(bg_m, bg_l->bg_color_fix);
        gui_img_a8_fix_bg(bg_r, bg_l->bg_color_fix);
        gui_img_a8_recolor(icon_l, FG_WHITE.color.argb_full);
        gui_img_a8_recolor(icon_m, icon_l->fg_color_set);
        gui_img_a8_recolor(icon_r, icon_l->fg_color_set);
        if (f_status.infor_center_func_3)
        {
            gui_img_t *icon_done = gui_img_create_from_mem(bg_l, 0, ICON_DONE_BIN, 0, 0, 0, 0);
            gui_img_set_mode(icon_done, IMG_2D_SW_SRC_OVER_MODE);
            gui_obj_add_event_cb(bg_l, click_button_page_information_center_customize, GUI_EVENT_TOUCH_CLICKED,
                                 &note->index);
        }
        else if (info_center_func_cnt == 3)
        {
            gui_img_a8_mix_alpha(bg_l, 122);
            gui_img_a8_mix_alpha(icon_l, 122);
            gui_text_color_set(text_l, font_color_invalid);
        }
        else
        {
            gui_obj_add_event_cb(bg_l, click_button_page_information_center_customize, GUI_EVENT_TOUCH_CLICKED,
                                 &note->index);
        }
        if (f_status.infor_center_func_4)
        {
            gui_img_t *icon_done = gui_img_create_from_mem(bg_m, 0, ICON_DONE_BIN, 0, 0, 0, 0);
            gui_img_set_mode(icon_done, IMG_2D_SW_SRC_OVER_MODE);
            gui_obj_add_event_cb(bg_m, click_button_page_information_center_customize, GUI_EVENT_TOUCH_CLICKED,
                                 &note->index);
        }
        else if (info_center_func_cnt == 3)
        {
            gui_img_a8_mix_alpha(bg_m, 122);
            gui_img_a8_mix_alpha(icon_m, 122);
            gui_text_color_set(text_m, font_color_invalid);
        }
        else
        {
            gui_obj_add_event_cb(bg_m, click_button_page_information_center_customize, GUI_EVENT_TOUCH_CLICKED,
                                 &note->index);
        }
        if (f_status.infor_center_func_5)
        {
            gui_img_t *icon_done = gui_img_create_from_mem(bg_r, 0, ICON_DONE_BIN, 0, 0, 0, 0);
            gui_img_set_mode(icon_done, IMG_2D_SW_SRC_OVER_MODE);
            gui_obj_add_event_cb(bg_r, click_button_page_information_center_customize, GUI_EVENT_TOUCH_CLICKED,
                                 &note->index);
        }
        else if (info_center_func_cnt == 3)
        {
            gui_img_a8_mix_alpha(bg_r, 122);
            gui_img_a8_mix_alpha(icon_r, 122);
            gui_text_color_set(text_r, font_color_invalid);
        }
        else
        {
            gui_obj_add_event_cb(bg_r, click_button_page_information_center_customize, GUI_EVENT_TOUCH_CLICKED,
                                 &note->index);
        }
    }
}

static void win_timer_page_quick_wake_up(void *obj)
{
    gui_matrix_t *matrix = GUI_BASE(obj)->matrix;

    if ((matrix->m[0][2] != 0 || matrix->m[1][2] != 0))
    {
        gui_obj_hidden((void *)obj, true);
        return;
    }
    gui_obj_hidden((void *)obj, false);
}

static void list_timer_page_quick_wake_up(void *obj)
{
    touch_info_t *tp = tp_get_info();
    gui_list_t *list = (gui_list_t *)obj;
    int16_t list_offset = list->offset;

    gui_obj_t *mask_fade_bottom = gui_list_entry(GUI_BASE(obj)->parent->child_list.prev, gui_obj_t,
                                                 brother_list);
    gui_obj_t *mask_fade_top = gui_list_entry(mask_fade_bottom->brother_list.prev, gui_obj_t,
                                              brother_list);
    gui_obj_t *scrollbar = gui_list_entry(mask_fade_top->brother_list.prev, gui_obj_t,
                                          brother_list);
    if (tp->pressing)
    {
        if (page_quick_wake_up_list_offset_rec > list_offset)
        {
            gui_obj_hidden(mask_fade_bottom, false);
            gui_obj_hidden(mask_fade_top, true);
        }
        else if (page_quick_wake_up_list_offset_rec < list_offset)
        {
            gui_obj_hidden(mask_fade_top, false);
            gui_obj_hidden(mask_fade_bottom, true);
        }
    }
    else
    {
        gui_obj_hidden(mask_fade_bottom, true);
        gui_obj_hidden(mask_fade_top, true);
    }

    page_quick_wake_up_list_offset_rec = list_offset;
    int range = list->base.h - list->total_length;
    int t_y = page_quick_wake_up_list_offset_rec * (SCROLL_BAR_BG_H - SCROLL_BAR_L_H) / range;
    gui_img_translate((void *)scrollbar, 0, t_y);
}

static void list_timer_page_case_button_customize(void *obj)
{
    gui_list_t *list = (gui_list_t *)obj;
    int16_t list_offset = list->offset;
    gui_obj_t *scrollbar = gui_list_entry(GUI_BASE(obj)->parent->child_list.prev, gui_obj_t,
                                          brother_list);

    int range = list->base.h - list->total_length;
    int t_y = list_offset * (SCROLL_BAR_BG_H - SCROLL_BAR_M_H) / range;
    gui_img_translate((void *)scrollbar, 0, t_y);
}

static void list_timer_page_reorder_quick_access(void *obj)
{
    gui_list_t *list = (gui_list_t *)obj;
    int16_t list_offset = list->offset;
    page_reorder_quick_access_list_offset_rec = list_offset;
    if (quick_page_num > 2)
    {
        gui_obj_t *scrollbar = gui_list_entry(GUI_BASE(obj)->parent->child_list.prev, gui_obj_t,
                                              brother_list);
        int range = list->base.h - list->total_length;
        int t_y = list_offset * (SCROLL_BAR_BG_H - SCROLL_BAR_L_H) / range;
        gui_img_translate((void *)scrollbar, 0, t_y);
    }
}

static void list_timer_page_information_center_customize(void *obj)
{
    gui_list_t *list = (gui_list_t *)obj;
    int16_t list_offset = list->offset;
    page_information_center_customize_list_offset_rec = list_offset;
    gui_obj_t *scrollbar = gui_list_entry(GUI_BASE(obj)->parent->child_list.prev, gui_obj_t,
                                          brother_list);
    int range = list->base.h - list->total_length;
    int t_y = list_offset * (SCROLL_BAR_BG_H - SCROLL_BAR_L_H) / range;
    gui_img_translate((void *)scrollbar, 0, t_y);
}

static void note_timer_up_page_reorder_quick_access(void *obj)
{
    static uint8_t cnt = 0;
    uint8_t cnt_max = 15;
    cnt++;
    gui_list_note_t *note = (gui_list_note_t *)obj;
    gui_list_note_t *note_last = (gui_list_note_t *)gui_list_entry(note->base.brother_list.next,
                                                                   gui_obj_t, brother_list);
    gui_obj_t *text = gui_list_entry(note->base.child_list.prev, gui_obj_t, brother_list);
    gui_obj_t *text_last = gui_list_entry(note_last->base.child_list.prev, gui_obj_t, brother_list);
    gui_scroll_text_pause((void *)text);
    gui_scroll_text_pause((void *)text_last);

    int16_t y_offset = 56 * cnt / cnt_max;
    text->y = 12 - y_offset;
    text_last->y = 12 + y_offset;
    if (cnt >= cnt_max)
    {
        cnt = 0;
        control_invalid = false;
        gui_obj_stop_timer(obj);
        int16_t index = note->index;
        void *temp = quick_page_design_func_array[index];
        quick_page_design_func_array[index] = quick_page_design_func_array[index - 1];
        quick_page_design_func_array[index - 1] = temp;

        temp = (void *)quick_page_name[index];
        quick_page_name[index] = quick_page_name[index - 1];
        quick_page_name[index - 1] = temp;
        gui_msg_t msg =
        {
            .event = GUI_EVENT_USER_DEFINE,
            .cb = regenerate_current_view,
        };
        gui_send_msg_to_server(&msg);
    }
}

static void note_timer_down_page_reorder_quick_access(void *obj)
{
    static uint8_t cnt = 0;
    uint8_t cnt_max = 15;
    cnt++;
    gui_list_note_t *note = (gui_list_note_t *)obj;
    gui_list_note_t *note_next = (gui_list_note_t *)gui_list_entry(note->base.brother_list.prev,
                                                                   gui_obj_t, brother_list);
    gui_obj_t *text = gui_list_entry(note->base.child_list.prev, gui_obj_t, brother_list);
    gui_obj_t *text_next = gui_list_entry(note_next->base.child_list.prev, gui_obj_t, brother_list);
    gui_scroll_text_pause((void *)text);
    gui_scroll_text_pause((void *)text_next);

    int16_t y_offset = 56 * cnt / cnt_max;
    text->y = 12 + y_offset;
    text_next->y = 12 - y_offset;
    if (cnt >= cnt_max)
    {
        cnt = 0;
        control_invalid = false;
        gui_obj_stop_timer(obj);
        int16_t index = note->index;
        void *temp = quick_page_design_func_array[index];
        quick_page_design_func_array[index] = quick_page_design_func_array[index + 1];
        quick_page_design_func_array[index + 1] = temp;

        temp = (void *)quick_page_name[index];
        quick_page_name[index] = quick_page_name[index + 1];
        quick_page_name[index + 1] = temp;
        gui_msg_t msg =
        {
            .event = GUI_EVENT_USER_DEFINE,
            .cb = regenerate_current_view,
        };
        gui_send_msg_to_server(&msg);
    }
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/
void regenerate_current_view(void *msg)
{
    GUI_UNUSED(msg);

    gui_view_t *current_view = gui_view_get_current();
    const struct gui_view_descriptor *descriptor = current_view->descriptor;
    gui_obj_t *parent = current_view->base.parent;
    gui_obj_tree_free(GUI_BASE(current_view));

    gui_view_create(parent, descriptor, 0, 0, 0, 0);
}

/* Audio settings page */
void page_playback_design(gui_obj_t *parent)
{
    quick_page_name_index = PLAYBACK;
    gui_color_t font_color;

    if (theme_bg_white)
    {
        font_color = FG_1_LIGHT;
    }
    else
    {
        font_color = FG_1_DARK;
    }
    gui_scroll_text_t *music_name = gui_scroll_text_create(parent, (void *)page_name_array[PLAYBACK],
                                                           12, 40, 308, 56);
    gui_scroll_text_type_set(music_name, CAPTION_3_30_BIN, FONT_SRC_MEMADDR);
    gui_scroll_text_scroll_set(music_name, SCROLL_X, 0, 308, 5000, 0);
    gui_scroll_text_t *lyrics;
    if (f_status.music_input)
    {
        gui_scroll_text_set(music_name, (char *)music_array[music_index], GUI_FONT_SRC_BMP, font_color,
                            strlen(music_array[music_index]), 30);
        lyrics = gui_scroll_text_create(parent, 0, 12, 65, 308, 20);
        gui_scroll_text_set(lyrics, (char *)lyrics_str, GUI_FONT_SRC_BMP, font_color,
                            strlen(lyrics_str), 20);
        gui_scroll_text_type_set(lyrics, HEADING_1_BIN, FONT_SRC_MEMADDR);
        gui_scroll_text_scroll_set(lyrics, SCROLL_X, 0, 308, 20000, 0);
    }
    else
    {
        gui_scroll_text_set(music_name, "No music", GUI_FONT_SRC_BMP, font_color, 9, 30);
    }

    gui_img_t *control_bg = gui_img_create_from_mem(parent, 0, CONTROL_BG_BIN, 12, 92, 0, 0);
    gui_img_t *button_last_bg = gui_img_create_from_mem(control_bg, "l", BUTTON_BG_CIRCLE_BIN, 6, 6,
                                                        0, 0);
    gui_img_t *last = gui_img_create_from_mem(control_bg, 0, SKIP_LAST_BIN, 24, 32, 0, 0);
    gui_img_t *button_next_bg = gui_img_create_from_mem(control_bg, "r", BUTTON_BG_CIRCLE_BIN, 212,
                                                        6, 0, 0);
    gui_obj_hidden((void *)button_last_bg, 1);
    gui_obj_hidden((void *)button_next_bg, 1);
    gui_img_t *next = gui_img_create_from_mem(control_bg, 0, SKIP_NEXT_BIN, 233, 32, 0, 0);
    gui_img_t *play = gui_img_create_from_mem(control_bg, 0, PLAY_BIN, 132, 24, 0, 0);

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
    if (f_status.music_input)
    {
        if (f_status.playback)
        {
            gui_img_set_image_data(play, PAUSE_BIN);
            gui_img_refresh_size(play);
        }
        else
        {
            gui_scroll_text_pause(music_name);
            gui_scroll_text_pause(lyrics);
        }
        gui_obj_add_event_cb(play, click_button_page_playback, GUI_EVENT_TOUCH_CLICKED, NULL);
        gui_obj_create_timer(GUI_BASE(button_last_bg), 10, true, press_button_page_playback);
        gui_obj_create_timer(GUI_BASE(button_next_bg), 10, true, press_button_page_playback);
    }
    else
    {
        gui_img_a8_mix_alpha(control_bg, CONTROL_BG_OPACITY);
        gui_img_a8_recolor(last, FG_2_DARK.color.argb_full);
        gui_img_a8_recolor(next, FG_2_DARK.color.argb_full);
        gui_img_a8_recolor(play, FG_2_DARK.color.argb_full);
        gui_img_a8_mix_alpha(play, play->fg_color_set >> 24);
    }
    gui_img_a8_mix_alpha(last, last->fg_color_set >> 24);
    gui_img_a8_mix_alpha(next, next->fg_color_set >> 24);
}

void page_volume_design(gui_obj_t *parent)
{
    quick_page_name_index = VOLUME;
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
    gui_text_set(text, (void *)page_name_array[VOLUME], GUI_FONT_SRC_BMP, font_color,
                 strlen((void *)page_name_array[VOLUME]), 30);
    gui_text_type_set(text, CAPTION_3_30_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, LEFT);

    gui_img_t *control_bg = gui_img_create_from_mem(parent, 0, CONTROL_BG_BIN, 12, 92, 0, 0);
    gui_img_t *button_sub_bg = gui_img_create_from_mem(control_bg, "l", BUTTON_BG_CIRCLE_BIN, 6, 6, 0,
                                                       0);
    gui_img_t *sub = gui_img_create_from_mem(control_bg, 0, SUB_BIN, 31, 42, 0, 0);
    gui_img_t *button_add_bg = gui_img_create_from_mem(control_bg, "r", BUTTON_BG_CIRCLE_BIN, 212, 6,
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

void page_ambient_sound_design(gui_obj_t *parent)
{
    quick_page_name_index = AMBIENT_SOUND;
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
    gui_text_set(text, (void *)page_name_array[AMBIENT_SOUND], GUI_FONT_SRC_BMP, font_color,
                 strlen((void *)page_name_array[AMBIENT_SOUND]), 30);
    gui_text_type_set(text, CAPTION_3_30_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, LEFT);

    gui_img_t *toggle_bg = gui_img_create_from_mem(parent, 0, TOGGLE_BG_BIN, 244, 40, 0, 0);
    gui_img_t *toggle = gui_img_create_from_mem(toggle_bg, (void *)page_name_array[AMBIENT_SOUND],
                                                TOGGLE_BIN, TOGGLE_OFF_X,
                                                TOGGLE_Y, 0, 0);
    gui_img_a8_recolor(toggle, FG_1_DARK.color.argb_full);
    gui_obj_add_event_cb(toggle_bg, click_toggle_cb, GUI_EVENT_TOUCH_CLICKED,
                         &(f_status.ambient_sound));

    gui_img_t *control_bg = gui_img_create_from_mem(parent, 0, CONTROL_BG_BIN, 12, 92, 0, 0);
    gui_img_t *button_bg = gui_img_create_from_mem(control_bg, (void *)page_name_array[AMBIENT_SOUND],
                                                   BUTTON_BG_ELLIPSE_BIN,
                                                   ambient_sound_type * 99, 0, 0, 0);
    gui_img_t *anc = gui_img_create_from_mem(control_bg, "l", ICON_ANC_BIN, 25, 21, 0, 0);
    gui_img_t *aa = gui_img_create_from_mem(control_bg, "m", ICON_AA_BIN, 124, 21, 0, 0);
    gui_img_t *tt = gui_img_create_from_mem(control_bg, "r", ICON_TT_BIN, 223, 21, 0, 0);
    update_icon_color((void *)control_bg, ambient_sound_type);
    gui_img_a8_recolor(anc, FG_WHITE.color.argb_full);
    gui_img_a8_recolor(aa, FG_WHITE.color.argb_full);
    gui_img_a8_recolor(tt, FG_WHITE.color.argb_full);
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
            gui_img_a8_mix_alpha(control_bg, CONTROL_BG_OPACITY);
        }
        toggle_on_bg_color = BG_THEME1_BRIGHT_LIGHT.color.argb_full;
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
            gui_img_a8_mix_alpha(control_bg, CONTROL_BG_OPACITY);
        }
        toggle_on_bg_color = BG_THEME1_BRIGHT_DARK.color.argb_full;
    }
    gui_img_a8_mix_alpha(toggle_bg, toggle_bg->fg_color_set >> 24);
    control_invalid = !f_status.ambient_sound;
}

void page_equalizer_design(gui_obj_t *parent)
{
    quick_page_name_index = EQUALIZER;
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
    gui_text_set(text, (void *)page_name_array[EQUALIZER], GUI_FONT_SRC_BMP, font_color,
                 strlen((void *)page_name_array[EQUALIZER]), 30);
    gui_text_type_set(text, CAPTION_3_30_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, LEFT);

    gui_img_t *toggle_bg = gui_img_create_from_mem(parent, 0, TOGGLE_BG_BIN, 244, 40, 0, 0);
    gui_img_t *toggle = gui_img_create_from_mem(toggle_bg, (void *)page_name_array[EQUALIZER],
                                                TOGGLE_BIN, TOGGLE_OFF_X,
                                                TOGGLE_Y, 0, 0);
    gui_img_a8_recolor(toggle, FG_1_DARK.color.argb_full);
    gui_obj_add_event_cb(toggle_bg, click_toggle_cb, GUI_EVENT_TOUCH_CLICKED, &(f_status.equalizer));

    gui_img_t *control_bg = gui_img_create_from_mem(parent, 0, CONTROL_BG_BIN, 12, 92, 0, 0);
    gui_img_t *button_switch_l_bg = gui_img_create_from_mem(control_bg, "l",
                                                            BUTTON_BG_CIRCLE_BIN, 6, 6, 0, 0);
    gui_img_t *switch_l = gui_img_create_from_mem(control_bg, 0, SWITCH_L_BIN, 32, 32, 0, 0);
    gui_img_t *button_switch_r_bg = gui_img_create_from_mem(control_bg, "r",
                                                            BUTTON_BG_CIRCLE_BIN, 212, 6, 0, 0);
    gui_img_t *switch_r = gui_img_create_from_mem(control_bg, 0, SWITCH_R_BIN, 241, 32, 0, 0);
    gui_img_t *euqalizer_type = gui_img_create_from_mem(control_bg, "euqalizer_type",
                                                        img_data_array[euqalizer_type_index], 86, 5, 0, 0);
    gui_obj_hidden((void *)button_switch_l_bg, 1);
    gui_obj_hidden((void *)button_switch_r_bg, 1);
    gui_obj_create_timer(GUI_BASE(button_switch_l_bg), 10, true, press_button_page_equalizer);
    gui_obj_create_timer(GUI_BASE(button_switch_r_bg), 10, true, press_button_page_equalizer);

    if (theme_bg_white)
    {
        gui_img_a8_recolor(control_bg, BG_THEME1_DARK_LIGHT.color.argb_full);
        gui_img_a8_recolor(button_switch_l_bg, BG_THEME1_MID_LIGHT.color.argb_full);
        gui_img_a8_recolor(button_switch_r_bg, BG_THEME1_MID_LIGHT.color.argb_full);
        gui_img_a8_recolor(euqalizer_type, FG_1_LIGHT.color.argb_full);
        gui_img_a8_recolor(switch_l, FG_1_LIGHT.color.argb_full);
        gui_img_a8_recolor(switch_r, FG_1_LIGHT.color.argb_full);
        if (euqalizer_type_index == 0)
        {
            gui_img_a8_recolor(switch_l, FG_2_LIGHT.color.argb_full);
        }
        else if (euqalizer_type_index == 5)
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
            gui_img_a8_mix_alpha((void *)control_bg, CONTROL_BG_OPACITY);
        }
        toggle_on_bg_color = BG_THEME1_BRIGHT_LIGHT.color.argb_full;
    }
    else
    {
        gui_img_a8_recolor(control_bg, BG_THEME1_DARK_DARK.color.argb_full);
        gui_img_a8_recolor(button_switch_l_bg, BG_THEME1_MID_DARK.color.argb_full);
        gui_img_a8_recolor(button_switch_r_bg, BG_THEME1_MID_DARK.color.argb_full);
        gui_img_a8_recolor(euqalizer_type, FG_1_DARK.color.argb_full);
        gui_img_a8_recolor(switch_l, FG_1_DARK.color.argb_full);
        gui_img_a8_recolor(switch_r, FG_1_DARK.color.argb_full);
        if (euqalizer_type_index == 0)
        {
            gui_img_a8_recolor(switch_l, FG_2_DARK.color.argb_full);
        }
        else if (euqalizer_type_index == 5)
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
            gui_img_a8_mix_alpha((void *)control_bg, CONTROL_BG_OPACITY);
        }
        toggle_on_bg_color = BG_THEME1_BRIGHT_DARK.color.argb_full;
    }
    gui_img_a8_mix_alpha(toggle_bg, toggle_bg->fg_color_set >> 24);
    gui_img_a8_mix_alpha(switch_l, switch_l->fg_color_set >> 24);
    gui_img_a8_mix_alpha(switch_r, switch_r->fg_color_set >> 24);

    control_invalid = !f_status.equalizer;
}

void page_smart_talk_design(gui_obj_t *parent)
{
    quick_page_name_index = SMART_TALK;
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
    gui_text_set(text, (void *)page_name_array[SMART_TALK], GUI_FONT_SRC_BMP, font_color,
                 strlen((void *)page_name_array[SMART_TALK]), 30);
    gui_text_type_set(text, CAPTION_3_30_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, LEFT);

    gui_img_t *toggle_bg = gui_img_create_from_mem(parent, 0, TOGGLE_BG_BIN, 244, 40, 0, 0);
    gui_img_t *toggle = gui_img_create_from_mem(toggle_bg, (void *)page_name_array[SMART_TALK],
                                                TOGGLE_BIN, TOGGLE_OFF_X,
                                                TOGGLE_Y, 0, 0);
    gui_img_a8_recolor(toggle, FG_1_DARK.color.argb_full);
    gui_obj_add_event_cb(toggle_bg, click_toggle_cb, GUI_EVENT_TOUCH_CLICKED, &(f_status.smart_talk));

    gui_img_t *control_bg = gui_img_create_from_mem(parent, 0, CONTROL_BG_BIN, 12, 92, 0, 0);
    gui_img_t *button_bg = gui_img_create_from_mem(control_bg, (void *)page_name_array[SMART_TALK],
                                                   BUTTON_BG_ELLIPSE_BIN,
                                                   smart_talk_time_type * 99, 0, 0, 0);
    gui_img_t *st_5 = gui_img_create_from_mem(control_bg, "l", SMART_TALK_5S_BIN, 22, 20, 0, 0);
    gui_img_t *st_15 = gui_img_create_from_mem(control_bg, "m", SMART_TALK_15S_BIN, 121, 20, 0, 0);
    gui_img_t *st_20 = gui_img_create_from_mem(control_bg, "r", SMART_TALK_20S_BIN, 219, 20, 0, 0);
    update_icon_color((void *)control_bg, smart_talk_time_type);
    gui_img_a8_recolor(st_5, FG_WHITE.color.argb_full);
    gui_img_a8_recolor(st_15, FG_WHITE.color.argb_full);
    gui_img_a8_recolor(st_20, FG_WHITE.color.argb_full);
    gui_obj_add_event_cb(st_5, click_button_page_smart_talk, GUI_EVENT_TOUCH_CLICKED, NULL);
    gui_obj_add_event_cb(st_15, click_button_page_smart_talk, GUI_EVENT_TOUCH_CLICKED, NULL);
    gui_obj_add_event_cb(st_20, click_button_page_smart_talk, GUI_EVENT_TOUCH_CLICKED, NULL);

    if (theme_bg_white)
    {
        gui_img_a8_recolor(control_bg, BG_THEME1_DARK_LIGHT.color.argb_full);
        gui_img_a8_recolor(button_bg, BG_THEME1_BRIGHT_LIGHT.color.argb_full);
        if (f_status.smart_talk)
        {
            gui_obj_move(GUI_BASE(toggle), TOGGLE_ON_X, TOGGLE_Y);
            gui_img_a8_recolor(toggle_bg, BG_THEME1_BRIGHT_LIGHT.color.argb_full);
        }
        else
        {
            gui_img_a8_recolor(toggle_bg, FG_1_LIGHT.color.argb_full);
            gui_img_a8_mix_alpha((void *)control_bg, CONTROL_BG_OPACITY);
        }
        toggle_on_bg_color = BG_THEME1_BRIGHT_LIGHT.color.argb_full;
    }
    else
    {
        gui_img_a8_recolor(control_bg, BG_THEME1_DARK_DARK.color.argb_full);
        gui_img_a8_recolor(button_bg, BG_THEME1_BRIGHT_DARK.color.argb_full);
        if (f_status.smart_talk)
        {
            gui_obj_move(GUI_BASE(toggle), TOGGLE_ON_X, TOGGLE_Y);
            gui_img_a8_recolor(toggle_bg, BG_THEME1_BRIGHT_DARK.color.argb_full);
        }
        else
        {
            gui_img_a8_recolor(toggle_bg, BG_2_DARK.color.argb_full);
            gui_img_a8_mix_alpha((void *)control_bg, CONTROL_BG_OPACITY);
        }
        toggle_on_bg_color = BG_THEME1_BRIGHT_DARK.color.argb_full;
    }
    gui_img_a8_mix_alpha(toggle_bg, toggle_bg->fg_color_set >> 24);
    control_invalid = !f_status.ambient_sound;
}

void page_spatial_sound_design(gui_obj_t *parent)
{
    quick_page_name_index = SPATIAL_SOUND;
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
    gui_text_set(text, (void *)page_name_array[SPATIAL_SOUND], GUI_FONT_SRC_BMP, font_color,
                 strlen((void *)page_name_array[SPATIAL_SOUND]), 30);
    gui_text_type_set(text, CAPTION_3_30_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, LEFT);

    gui_img_t *toggle_bg = gui_img_create_from_mem(parent, 0, TOGGLE_BG_BIN, 244, 40, 0, 0);
    gui_img_t *toggle = gui_img_create_from_mem(toggle_bg, (void *)page_name_array[SPATIAL_SOUND],
                                                TOGGLE_BIN, TOGGLE_OFF_X,
                                                TOGGLE_Y, 0, 0);
    gui_img_a8_recolor(toggle, FG_1_DARK.color.argb_full);
    gui_obj_add_event_cb(toggle_bg, click_toggle_cb, GUI_EVENT_TOUCH_CLICKED,
                         &(f_status.spatial_sound));

    gui_img_t *control_bg = gui_img_create_from_mem(parent, 0, CONTROL_BG_BIN, 12, 92, 0, 0);
    gui_img_t *button_bg = gui_img_create_from_mem(control_bg, (void *)page_name_array[SPATIAL_SOUND],
                                                   BUTTON_BG_ELLIPSE_BIN,
                                                   ambient_sound_type * 99, 0, 0, 0);
    gui_img_t *movie = gui_img_create_from_mem(control_bg, "l", SPATIAL_SOUND_MOVIE_BIN, 25, 21, 0,
                                               0);
    gui_img_t *music = gui_img_create_from_mem(control_bg, "m", SPATIAL_SOUND_MUSIC_BIN, 124, 21, 0,
                                               0);
    gui_img_t *game = gui_img_create_from_mem(control_bg, "r", SPATIAL_SOUND_GAME_BIN, 223, 21, 0,
                                              0);
    update_icon_color((void *)control_bg, spatial_sound_type);
    gui_img_a8_recolor(movie, FG_WHITE.color.argb_full);
    gui_img_a8_recolor(music, FG_WHITE.color.argb_full);
    gui_img_a8_recolor(game, FG_WHITE.color.argb_full);
    gui_obj_add_event_cb(movie, click_button_page_spatial_sound, GUI_EVENT_TOUCH_CLICKED, NULL);
    gui_obj_add_event_cb(music, click_button_page_spatial_sound, GUI_EVENT_TOUCH_CLICKED, NULL);
    gui_obj_add_event_cb(game, click_button_page_spatial_sound, GUI_EVENT_TOUCH_CLICKED, NULL);

    if (theme_bg_white)
    {
        gui_img_a8_recolor(control_bg, BG_THEME1_DARK_LIGHT.color.argb_full);
        gui_img_a8_recolor(button_bg, BG_THEME1_BRIGHT_LIGHT.color.argb_full);
        if (f_status.spatial_sound)
        {
            gui_obj_move(GUI_BASE(toggle), TOGGLE_ON_X, TOGGLE_Y);
            gui_img_a8_recolor(toggle_bg, BG_THEME1_BRIGHT_LIGHT.color.argb_full);
        }
        else
        {
            gui_img_a8_recolor(toggle_bg, FG_1_LIGHT.color.argb_full);
            gui_img_a8_mix_alpha((void *)control_bg, CONTROL_BG_OPACITY);
        }
        toggle_on_bg_color = BG_THEME1_BRIGHT_LIGHT.color.argb_full;
    }
    else
    {
        gui_img_a8_recolor(control_bg, BG_THEME1_DARK_DARK.color.argb_full);
        gui_img_a8_recolor(button_bg, BG_THEME1_BRIGHT_DARK.color.argb_full);
        if (f_status.spatial_sound)
        {
            gui_obj_move(GUI_BASE(toggle), TOGGLE_ON_X, TOGGLE_Y);
            gui_img_a8_recolor(toggle_bg, BG_THEME1_BRIGHT_DARK.color.argb_full);
        }
        else
        {
            gui_img_a8_recolor(toggle_bg, BG_2_DARK.color.argb_full);
            gui_img_a8_mix_alpha((void *)control_bg, CONTROL_BG_OPACITY);
        }
        toggle_on_bg_color = BG_THEME1_BRIGHT_DARK.color.argb_full;
    }
    gui_img_a8_mix_alpha(toggle_bg, toggle_bg->fg_color_set >> 24);
    control_invalid = !f_status.spatial_sound;
}

void page_voice_aware_design(gui_obj_t *parent)
{
    quick_page_name_index = VOICE_AWARE;
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
        img_data_array[0] = SECTION_BAR_1_BIN;
        img_data_array[1] = SECTION_BAR_2_BIN;
        img_data_array[2] = SECTION_BAR_3_BIN;
    }

    gui_text_t *text = gui_text_create(parent, 0, 12, 45, 200, 15);
    gui_text_set(text, (void *)page_name_array[VOICE_AWARE], GUI_FONT_SRC_BMP, font_color,
                 strlen((void *)page_name_array[VOICE_AWARE]), 30);
    gui_text_type_set(text, CAPTION_3_30_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, LEFT);

    gui_img_t *toggle_bg = gui_img_create_from_mem(parent, 0, TOGGLE_BG_BIN, 244, 40, 0, 0);
    gui_img_t *toggle = gui_img_create_from_mem(toggle_bg, (void *)page_name_array[VOICE_AWARE],
                                                TOGGLE_BIN, TOGGLE_OFF_X,
                                                TOGGLE_Y, 0, 0);
    gui_img_a8_recolor(toggle, FG_1_DARK.color.argb_full);
    gui_obj_add_event_cb(toggle_bg, click_toggle_cb, GUI_EVENT_TOUCH_CLICKED, &(f_status.voice_aware));

    gui_img_t *control_bg = gui_img_create_from_mem(parent, 0, CONTROL_BG_BIN, 12, 92, 0, 0);
    gui_img_t *button_sub_bg = gui_img_create_from_mem(control_bg, "l", BUTTON_BG_CIRCLE_BIN, 6, 6, 0,
                                                       0);
    gui_img_t *sub = gui_img_create_from_mem(control_bg, 0, SUB_BIN, 31, 42, 0, 0);
    gui_img_t *button_add_bg = gui_img_create_from_mem(control_bg, "r", BUTTON_BG_CIRCLE_BIN, 212, 6,
                                                       0, 0);
    gui_img_t *add = gui_img_create_from_mem(control_bg, 0, ADD_BIN, 237, 31, 0, 0);
    gui_img_t *progressbar_bg = gui_img_create_from_mem(control_bg, 0, img_data_array[2], 90, 27, 0,
                                                        0);
    gui_img_t *progressbar = gui_img_create_from_mem(control_bg, "progressbar",
                                                     img_data_array[voice_aware_val - 1],
                                                     90, 27, 0, 0);
    gui_obj_hidden((void *)button_sub_bg, 1);
    gui_obj_hidden((void *)button_add_bg, 1);

    gui_obj_create_timer(GUI_BASE(button_sub_bg), 10, true, press_button_page_voice_aware);
    gui_obj_create_timer(GUI_BASE(button_add_bg), 10, true, press_button_page_voice_aware);

    if (theme_bg_white)
    {
        gui_img_a8_recolor(control_bg, BG_THEME1_DARK_LIGHT.color.argb_full);
        gui_img_a8_recolor(button_sub_bg, BG_THEME1_MID_LIGHT.color.argb_full);
        gui_img_a8_recolor(button_add_bg, BG_THEME1_MID_LIGHT.color.argb_full);
        gui_img_a8_recolor(progressbar_bg, BG_THEME1_MID_LIGHT.color.argb_full);
        gui_img_a8_recolor(progressbar, FG_THEME1_LIGHT.color.argb_full);
        gui_img_a8_recolor(sub, FG_1_LIGHT.color.argb_full);
        gui_img_a8_recolor(add, FG_1_LIGHT.color.argb_full);
        if (voice_aware_val == 1)
        {
            gui_img_a8_recolor(sub, FG_2_LIGHT.color.argb_full);
        }
        else if (voice_aware_val == 3)
        {
            gui_img_a8_recolor(add, FG_2_LIGHT.color.argb_full);
        }
        if (f_status.voice_aware)
        {
            gui_obj_move(GUI_BASE(toggle), TOGGLE_ON_X, TOGGLE_Y);
            gui_img_a8_recolor(toggle_bg, BG_THEME1_BRIGHT_LIGHT.color.argb_full);
        }
        else
        {
            gui_img_a8_recolor(toggle_bg, FG_1_LIGHT.color.argb_full);
            gui_img_a8_mix_alpha((void *)control_bg, CONTROL_BG_OPACITY);
        }
        toggle_on_bg_color = BG_THEME1_BRIGHT_LIGHT.color.argb_full;
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
        if (voice_aware_val == 1)
        {
            gui_img_a8_recolor(sub, FG_2_DARK.color.argb_full);
        }
        else if (voice_aware_val == 3)
        {
            gui_img_a8_recolor(add, FG_2_DARK.color.argb_full);
        }
        if (f_status.voice_aware)
        {
            gui_obj_move(GUI_BASE(toggle), TOGGLE_ON_X, TOGGLE_Y);
            gui_img_a8_recolor(toggle_bg, BG_THEME1_BRIGHT_LIGHT.color.argb_full);
        }
        else
        {
            gui_img_a8_recolor(toggle_bg, FG_1_LIGHT.color.argb_full);
            gui_img_a8_mix_alpha((void *)control_bg, CONTROL_BG_OPACITY);
        }
        toggle_on_bg_color = BG_THEME1_BRIGHT_DARK.color.argb_full;
    }
    gui_img_a8_mix_alpha(sub, sub->fg_color_set >> 24);
    gui_img_a8_mix_alpha(add, add->fg_color_set >> 24);
    control_invalid = !f_status.voice_aware;
}

void page_auto_play_pause_design(gui_obj_t *parent)
{
    quick_page_name_index = AUTO_PLAY_PAUSE;
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
    gui_text_set(text, (void *)page_name_array[AUTO_PLAY_PAUSE], GUI_FONT_SRC_BMP, font_color,
                 strlen((void *)page_name_array[AUTO_PLAY_PAUSE]), 30);
    gui_text_type_set(text, CAPTION_3_30_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, LEFT);

    gui_img_t *toggle_bg = gui_img_create_from_mem(parent, 0, TOGGLE_BG_BIN, 244, 40, 0,
                                                   0);
    gui_img_t *toggle = gui_img_create_from_mem(toggle_bg, (void *)page_name_array[AUTO_PLAY_PAUSE],
                                                TOGGLE_BIN, TOGGLE_OFF_X,
                                                TOGGLE_Y, 0, 0);
    gui_img_a8_recolor(toggle, FG_1_DARK.color.argb_full);
    gui_obj_add_event_cb(toggle_bg, click_toggle_cb, GUI_EVENT_TOUCH_CLICKED,
                         &(f_status.auto_play_pause));

    if (theme_bg_white)
    {
        if (f_status.auto_play_pause)
        {
            gui_obj_move(GUI_BASE(toggle), TOGGLE_ON_X, TOGGLE_Y);
            gui_img_a8_recolor(toggle_bg, BG_THEME1_BRIGHT_LIGHT.color.argb_full);
        }
        else
        {
            gui_img_a8_recolor(toggle_bg, FG_1_LIGHT.color.argb_full);
        }
        toggle_on_bg_color = BG_THEME1_BRIGHT_LIGHT.color.argb_full;
    }
    else
    {
        if (f_status.auto_play_pause)
        {
            gui_obj_move(GUI_BASE(toggle), TOGGLE_ON_X, TOGGLE_Y);
            gui_img_a8_recolor(toggle_bg, BG_THEME1_BRIGHT_DARK.color.argb_full);
        }
        else
        {
            gui_img_a8_recolor(toggle_bg, BG_2_DARK.color.argb_full);
        }
        toggle_on_bg_color = BG_THEME1_BRIGHT_DARK.color.argb_full;
    }
    gui_img_a8_mix_alpha(toggle_bg, toggle_bg->fg_color_set >> 24);
}

/* Case settings page */
void page_screen_brightness_design(gui_obj_t *parent)
{
    quick_page_name_index = SCREEN_BRIGHTNESS;
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
    gui_text_set(text, (void *)page_name_array[SCREEN_BRIGHTNESS], GUI_FONT_SRC_BMP, font_color,
                 strlen((void *)page_name_array[SCREEN_BRIGHTNESS]), 30);
    gui_text_type_set(text, CAPTION_3_30_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, LEFT);

    gui_img_t *control_bg = gui_img_create_from_mem(parent, 0, CONTROL_BG_BIN, 12, 92, 0, 0);
    gui_img_t *button_sub_bg = gui_img_create_from_mem(control_bg, "l", BUTTON_BG_CIRCLE_BIN, 6, 6, 0,
                                                       0);
    gui_img_t *sub = gui_img_create_from_mem(control_bg, 0, SUB_BIN, 31, 42, 0, 0);
    gui_img_t *button_add_bg = gui_img_create_from_mem(control_bg, "r", BUTTON_BG_CIRCLE_BIN, 212, 6,
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

void page_dark_light_design(gui_obj_t *parent)
{
    quick_page_name_index = DARK_LIGHT_MODE;
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
    gui_text_set(text, (void *)page_name_array[DARK_LIGHT_MODE], GUI_FONT_SRC_BMP, font_color,
                 strlen((void *)page_name_array[DARK_LIGHT_MODE]), 30);
    gui_text_type_set(text, CAPTION_3_30_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, LEFT);

    gui_img_t *control_bg = gui_img_create_from_mem(parent, 0, CONTROL_BG_BIN, 12, 92, 0, 0);
    gui_img_t *button_bg = gui_img_create_from_mem(control_bg, (void *)page_name_array[DARK_LIGHT_MODE],
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
    quick_page_name_index = LOCK_SCREEN;
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
    gui_text_set(text, (void *)page_name_array[LOCK_SCREEN], GUI_FONT_SRC_BMP, font_color,
                 strlen((void *)page_name_array[LOCK_SCREEN]), 30);
    gui_text_type_set(text, CAPTION_3_30_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, LEFT);
    gui_win_t *win_l = gui_win_create(parent, "l", 18, 98, 78, 78);
    gui_win_t *win_r = gui_win_create(parent, "r", 224, 98, 78, 78);
    gui_img_t *switch_l = gui_img_create_from_mem(win_l, 0, SWITCH_L_BIN, 29, 26, 0, 0);
    gui_img_t *switch_r = gui_img_create_from_mem(win_r, 0, SWITCH_R_BIN, 29, 26, 0, 0);
    gui_obj_add_event_cb(win_l, click_button_page_lock_screen, GUI_EVENT_TOUCH_CLICKED, NULL);
    gui_obj_add_event_cb(win_r, click_button_page_lock_screen, GUI_EVENT_TOUCH_CLICKED, NULL);

    gui_img_t *clock = gui_img_create_from_mem(parent, 0, img_data_array[clock_style], 96, 97, 0, 0);
    gui_img_set_mode(clock, IMG_2D_SW_SRC_OVER_MODE);
    gui_obj_add_event_cb(clock, click_clock_page_lock_screen, GUI_EVENT_TOUCH_CLICKED, NULL);

    if (theme_bg_white)
    {
        gui_img_a8_recolor(switch_l, FG_1_LIGHT.color.argb_full);
        gui_img_a8_recolor(switch_r, FG_1_LIGHT.color.argb_full);
        if (clock_style == 0)
        {
            gui_img_a8_recolor(switch_l, FG_2_LIGHT.color.argb_full);
        }
        else if (clock_style == 4)
        {
            gui_img_a8_recolor(switch_r, FG_2_LIGHT.color.argb_full);
        }
    }
    else
    {
        gui_img_a8_recolor(switch_l, FG_1_DARK.color.argb_full);
        gui_img_a8_recolor(switch_r, FG_1_DARK.color.argb_full);
        if (clock_style == 0)
        {
            gui_img_a8_recolor(switch_l, FG_2_DARK.color.argb_full);
        }
        else if (clock_style == 4)
        {
            gui_img_a8_recolor(switch_r, FG_2_DARK.color.argb_full);
        }
    }
    gui_img_a8_mix_alpha(switch_l, switch_l->fg_color_set >> 24);
    gui_img_a8_mix_alpha(switch_r, switch_r->fg_color_set >> 24);
}

void page_auto_dim_off_screen_design(gui_obj_t *parent)
{
    quick_page_name_index = AUTO_DIM_OFF_SCREEN;
    gui_color_t font_color;
    if (theme_bg_white)
    {
        font_color = FG_1_LIGHT;
    }
    else
    {
        font_color = FG_1_DARK;
    }

    gui_text_t *text = gui_text_create(parent, 0, 12, 45, 250, 15);
    gui_text_set(text, (void *)page_name_array[AUTO_DIM_OFF_SCREEN], GUI_FONT_SRC_BMP, font_color,
                 strlen((void *)page_name_array[AUTO_DIM_OFF_SCREEN]), 30);
    gui_text_type_set(text, CAPTION_3_30_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, LEFT);

    gui_img_t *toggle_bg = gui_img_create_from_mem(parent, 0, TOGGLE_BG_BIN, 244, 40, 0, 0);
    gui_img_t *toggle = gui_img_create_from_mem(toggle_bg, (void *)page_name_array[AUTO_DIM_OFF_SCREEN],
                                                TOGGLE_BIN, TOGGLE_OFF_X,
                                                TOGGLE_Y, 0, 0);
    gui_img_a8_recolor(toggle, FG_1_DARK.color.argb_full);
    gui_obj_add_event_cb(toggle_bg, click_toggle_cb, GUI_EVENT_TOUCH_CLICKED,
                         &(f_status.auto_dim_off_screen));

    gui_img_t *control_bg = gui_img_create_from_mem(parent, 0, CONTROL_BG_BIN, 12, 92, 0, 0);
    gui_img_t *button_sub_bg = gui_img_create_from_mem(control_bg, "l", BUTTON_BG_CIRCLE_BIN, 6, 6, 0,
                                                       0);
    gui_img_t *sub = gui_img_create_from_mem(control_bg, 0, SUB_BIN, 31, 42, 0, 0);
    gui_img_t *button_add_bg = gui_img_create_from_mem(control_bg, "r", BUTTON_BG_CIRCLE_BIN, 212, 6,
                                                       0, 0);
    gui_img_t *add = gui_img_create_from_mem(control_bg, 0, ADD_BIN, 237, 31, 0, 0);

    text = gui_text_create(control_bg, 0, 40, 0, 256, 90);
    gui_text_set(text, "s", GUI_FONT_SRC_BMP, font_color, 1, 20);
    gui_text_type_set(text, HEADING_1_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, MID_CENTER);
    sprintf(page_str, "%d", auto_dim_time_val);
    text = gui_text_create(control_bg, 0, 0, 0, 296, 90);
    gui_text_set(text, page_str, GUI_FONT_SRC_BMP, font_color, strlen(page_str), 32);
    gui_text_type_set(text, CAPTION_1_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, MID_CENTER);

    gui_obj_hidden((void *)button_sub_bg, 1);
    gui_obj_hidden((void *)button_add_bg, 1);
    gui_obj_create_timer(GUI_BASE(button_sub_bg), 10, true, press_button_page_auto_dim_off_screen);
    gui_obj_create_timer(GUI_BASE(button_add_bg), 10, true, press_button_page_auto_dim_off_screen);

    if (theme_bg_white)
    {
        if (f_status.auto_dim_off_screen)
        {
            gui_obj_move(GUI_BASE(toggle), TOGGLE_ON_X, TOGGLE_Y);
            gui_img_a8_recolor(toggle_bg, BG_THEME3_BRIGHT_LIGHT.color.argb_full);
        }
        else
        {
            gui_img_a8_recolor(toggle_bg, FG_1_LIGHT.color.argb_full);
            gui_img_a8_mix_alpha(control_bg, CONTROL_BG_OPACITY);
        }
        gui_img_a8_recolor(control_bg, BG_THEME3_DARK_LIGHT.color.argb_full);
        gui_img_a8_recolor(button_sub_bg, BG_THEME3_MID_LIGHT.color.argb_full);
        gui_img_a8_recolor(button_add_bg, BG_THEME3_MID_LIGHT.color.argb_full);
        gui_img_a8_recolor(sub, FG_1_LIGHT.color.argb_full);
        gui_img_a8_recolor(add, FG_1_LIGHT.color.argb_full);
        toggle_on_bg_color = BG_THEME3_BRIGHT_LIGHT.color.argb_full;
    }
    else
    {
        if (f_status.auto_dim_off_screen)
        {
            gui_obj_move(GUI_BASE(toggle), TOGGLE_ON_X, TOGGLE_Y);
            gui_img_a8_recolor(toggle_bg, BG_THEME3_BRIGHT_LIGHT.color.argb_full);
        }
        else
        {
            gui_img_a8_recolor(toggle_bg, BG_2_DARK.color.argb_full);
            gui_img_a8_mix_alpha(control_bg, CONTROL_BG_OPACITY);
        }
        gui_img_a8_recolor(control_bg, BG_THEME3_DARK_DARK.color.argb_full);
        gui_img_a8_recolor(button_sub_bg, BG_THEME3_MID_DARK.color.argb_full);
        gui_img_a8_recolor(button_add_bg, BG_THEME3_MID_DARK.color.argb_full);
        gui_img_a8_recolor(sub, FG_1_DARK.color.argb_full);
        gui_img_a8_recolor(add, FG_1_DARK.color.argb_full);
        toggle_on_bg_color = BG_THEME3_BRIGHT_DARK.color.argb_full;
    }
    gui_img_a8_mix_alpha(sub, sub->fg_color_set >> 24);
    gui_img_a8_mix_alpha(add, add->fg_color_set >> 24);
    gui_img_a8_mix_alpha(toggle_bg, toggle_bg->fg_color_set >> 24);
    control_invalid = !f_status.auto_dim_off_screen;
}

void page_quick_wake_up_screen_design(gui_obj_t *parent)
{
    quick_page_name_index = QUICK_WAKE_UP_SCREEN;
    gui_color_t font_color;
    if (theme_bg_white)
    {
        font_color = FG_1_LIGHT;
    }
    else
    {
        font_color = FG_1_DARK;
    }
    gui_text_t *text = gui_text_create(parent, 0, 12, 30, 170, 60);
    gui_text_set(text, (void *)page_name_array[QUICK_WAKE_UP_SCREEN], GUI_FONT_SRC_BMP, font_color,
                 strlen((void *)page_name_array[QUICK_WAKE_UP_SCREEN]), 30);
    gui_text_type_set(text, CAPTION_3_30_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, MULTI_LEFT);

    gui_img_t *toggle_bg = gui_img_create_from_mem(parent, 0, TOGGLE_BG_BIN, 244, 40, 0, 0);
    gui_img_t *toggle = gui_img_create_from_mem(toggle_bg,
                                                (void *)page_name_array[QUICK_WAKE_UP_SCREEN], TOGGLE_BIN, TOGGLE_OFF_X,
                                                TOGGLE_Y, 0, 0);
    gui_img_a8_recolor(toggle, FG_1_DARK.color.argb_full);
    gui_obj_add_event_cb(toggle_bg, click_toggle_cb, GUI_EVENT_TOUCH_CLICKED,
                         &(f_status.quick_wake_up_screen));

    if (f_status.quick_wake_up_screen)
    {
        gui_img_t *control_bg = gui_img_create_from_mem(parent, 0, CONTROL_BG_BIN, 12, 92, 0, 0);
        gui_img_t *button_bg = gui_img_create_from_mem(control_bg,
                                                       (void *)page_name_array[QUICK_WAKE_UP_SCREEN], BUTTON_BG_ELLIPSE_BIN,
                                                       BUTTON_X_MID * quick_wake_up_click_num, 0, 0, 0);
        gui_img_t *l = gui_img_create_from_mem(control_bg, "l", ICON_CLICK_BIN, 30, 23, 0, 0);
        gui_img_t *m = gui_img_create_from_mem(control_bg, "m", ICON_CLICK_DOUBLE_BIN, 129, 23, 0, 0);
        gui_img_t *r = gui_img_create_from_mem(control_bg, "r", ICON_CLICK_TRIPLE_BIN, 227, 23, 0, 0);
        update_icon_color((void *)control_bg, quick_wake_up_click_num);
        gui_img_a8_recolor(l, FG_WHITE.color.argb_full);
        gui_img_a8_recolor(m, FG_WHITE.color.argb_full);
        gui_img_a8_recolor(r, FG_WHITE.color.argb_full);
        gui_obj_add_event_cb(l, click_button_page_quick_wake_up_screen, GUI_EVENT_TOUCH_CLICKED, NULL);
        gui_obj_add_event_cb(m, click_button_page_quick_wake_up_screen, GUI_EVENT_TOUCH_CLICKED, NULL);
        gui_obj_add_event_cb(r, click_button_page_quick_wake_up_screen, GUI_EVENT_TOUCH_CLICKED, NULL);
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
    else
    {
        gui_win_t *win = gui_win_create(gui_obj_get_fake_root(), 0, 0, 0, 0, 0);
        win->base.parent = parent;
        gui_list_remove(&win->base.brother_list);
        gui_list_insert(&(parent->child_list), &(win->base.brother_list));
        gui_obj_create_timer(GUI_BASE(win), 10, true, win_timer_page_quick_wake_up);
        gui_list_t *list = gui_list_create(win, "list", 0, 96, 0, 0, 170, 0,
                                           VERTICAL, note_design_page_quick_wake_up, NULL, 0);
        gui_list_set_style(list, LIST_CLASSIC);
        gui_list_set_note_num(list, 1);
        gui_list_set_offset(list, page_quick_wake_up_list_offset_rec);
        gui_list_set_inertia(list, false);
        gui_obj_create_timer(GUI_BASE(list), 10, true, list_timer_page_quick_wake_up);

        gui_img_t *mask_top = gui_img_create_from_mem(win, 0, MASK_320_82_BIN, 0, 0, 0, 0);
        gui_img_t *mask_bottom = gui_img_create_from_mem(win, 0, MASK_320_14_BIN, 0, 186, 0, 0);
        gui_img_t *scrollbar_bg = gui_img_create_from_mem(win, 0, SCROLLBAR_BG_BIN, 310, 86, 0, 0);
        gui_img_t *scrollbar = gui_img_create_from_mem(win, 0, SCROLLBAR_L_BIN, 310, 86, 0, 0);
        gui_img_t *mask_fade_top = gui_img_create_from_mem(win, 0, MASK_FADE_TOP_BIN, 0, 80, 0, 0);
        gui_img_t *mask_fade_bottom = gui_img_create_from_mem(win, 0, MASK_FADE_BOTTOM_BIN, 0, 142, 0, 0);
        if (theme_bg_white)
        {
            gui_img_a8_recolor(mask_top, SCREEN_BG_LIGHT.color.argb_full);
            gui_img_a8_fix_bg(mask_top, SCREEN_BG_LIGHT.color.argb_full);
            gui_img_a8_recolor(mask_bottom, SCREEN_BG_LIGHT.color.argb_full);
            gui_img_a8_fix_bg(mask_bottom, SCREEN_BG_LIGHT.color.argb_full);
            gui_img_a8_recolor(mask_fade_top, SCREEN_BG_LIGHT.color.argb_full);
            gui_img_a8_recolor(mask_fade_bottom, SCREEN_BG_LIGHT.color.argb_full);
            gui_img_a8_recolor(scrollbar_bg, FG_DARK.color.argb_full);
            gui_img_a8_recolor(scrollbar, FG_DARK.color.argb_full);
        }
        else
        {
            gui_img_a8_recolor(mask_top, 0);
            gui_img_a8_fix_bg(mask_top, SCREEN_BG_LIGHT.color.argb_full);
            gui_img_a8_recolor(mask_bottom, 0);
            gui_img_a8_fix_bg(mask_bottom, SCREEN_BG_LIGHT.color.argb_full);
            gui_img_a8_recolor(mask_fade_top, SCREEN_BG_DARK.color.argb_full);
            gui_img_a8_recolor(mask_fade_bottom, SCREEN_BG_DARK.color.argb_full);
            gui_img_a8_recolor(scrollbar_bg, FG_WHITE.color.argb_full);
            gui_img_a8_recolor(scrollbar, FG_WHITE.color.argb_full);
        }
    }

    if (theme_bg_white)
    {
        if (f_status.quick_wake_up_screen)
        {
            gui_obj_move(GUI_BASE(toggle), TOGGLE_ON_X, TOGGLE_Y);
            gui_img_a8_recolor(toggle_bg, BG_THEME3_BRIGHT_LIGHT.color.argb_full);
        }
        else
        {
            gui_img_a8_recolor(toggle_bg, FG_1_LIGHT.color.argb_full);
        }
        toggle_on_bg_color = BG_THEME3_BRIGHT_LIGHT.color.argb_full;
    }
    else
    {
        if (f_status.quick_wake_up_screen)
        {
            gui_obj_move(GUI_BASE(toggle), TOGGLE_ON_X, TOGGLE_Y);
            gui_img_a8_recolor(toggle_bg, BG_THEME3_BRIGHT_DARK.color.argb_full);
        }
        else
        {
            gui_img_a8_recolor(toggle_bg, BG_2_DARK.color.argb_full);
        }
        toggle_on_bg_color = BG_THEME3_BRIGHT_DARK.color.argb_full;
    }
    gui_img_a8_mix_alpha(toggle_bg, toggle_bg->fg_color_set >> 24);
    control_invalid = !f_status.quick_wake_up_screen;
}

void page_case_button_customize_design(gui_obj_t *parent)
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

    gui_win_t *win = gui_win_create(gui_obj_get_fake_root(), 0, 0, 0, 0, 0);
    win->base.parent = parent;
    gui_list_remove(&win->base.brother_list);
    gui_list_insert(&(parent->child_list), &(win->base.brother_list));
    parent = GUI_BASE(win);

    gui_img_t *bg_note = gui_img_create_from_mem(parent, "bg_note", MENU_LISTNOTE_BG_BIN, 0, 0, 0, 0);
    gui_obj_hidden((void *)bg_note, true);

    gui_list_t *list = gui_list_create(parent, "list", 0, 60, 0, 0, 56, 0,
                                       VERTICAL, note_design_page_case_button_customize, NULL, 0);
    gui_list_set_style(list, LIST_CLASSIC);
    gui_list_set_note_num(list, 5);
    gui_list_set_out_scope(list, 20);
    gui_obj_create_timer(GUI_BASE(list), 10, true, list_timer_page_case_button_customize);

    gui_img_t *mask = gui_img_create_from_mem(parent, 0, MASK_BIN, 0, 0, 0, 0);
    gui_scroll_text_t *text = gui_scroll_text_create(parent, 0, 44, 16, 260, 30);
    gui_scroll_text_type_set(text, CAPTION_3_30_BIN, FONT_SRC_MEMADDR);
    gui_scroll_text_set(text, (void *)page_name_array[CASE_BUTTON_CUSTOMIZE], GUI_FONT_SRC_BMP,
                        font_color, strlen((void *)page_name_array[CASE_BUTTON_CUSTOMIZE]), 30);
    gui_scroll_text_scroll_set(text, SCROLL_X, 20, 260, 5000, 0);

    gui_img_t *scrollbar_bg = gui_img_create_from_mem(parent, 0, SCROLLBAR_BG_BIN, 310, 86, 0, 0);
    gui_img_t *scrollbar = gui_img_create_from_mem(parent, 0, SCROLLBAR_M_BIN, 310, 86, 0, 0);
    if (theme_bg_white)
    {
        gui_img_a8_recolor(mask, SCREEN_BG_LIGHT.color.argb_full);
        gui_img_a8_fix_bg(mask, SCREEN_BG_LIGHT.color.argb_full);
        gui_img_a8_recolor(scrollbar_bg, FG_DARK.color.argb_full);
        gui_img_a8_recolor(scrollbar, FG_DARK.color.argb_full);
        gui_img_a8_recolor(bg_note, BG_1_LIGHT.color.argb_full);
    }
    else
    {
        gui_img_a8_recolor(mask, 0);
        gui_img_a8_fix_bg(mask, SCREEN_BG_LIGHT.color.argb_full);
        gui_img_a8_recolor(scrollbar_bg, FG_WHITE.color.argb_full);
        gui_img_a8_recolor(scrollbar, FG_WHITE.color.argb_full);
        gui_img_a8_recolor(bg_note, BG_1_DARK.color.argb_full);
    }
    gui_img_a8_mix_alpha(bg_note, bg_note->fg_color_set >> 24);
}

void page_information_center_customize_design(gui_obj_t *parent)
{
    gui_color_t font_color;
    gui_color_t mask_color;
    gui_color_t scroll_color;
    if (theme_bg_white)
    {
        font_color = FG_1_LIGHT;
        mask_color = SCREEN_BG_LIGHT;
        scroll_color = FG_DARK;
    }
    else
    {
        font_color = FG_1_DARK;
        mask_color = SCREEN_BG_DARK;
        scroll_color = FG_WHITE;
    }

    gui_win_t *win = gui_win_create(gui_obj_get_fake_root(), 0, 0, 0, 0, 0);
    win->base.parent = parent;
    gui_list_remove(&win->base.brother_list);
    gui_list_insert(&(parent->child_list), &(win->base.brother_list));
    parent = GUI_BASE(win);

    gui_list_t *list = gui_list_create(parent, "list", 0, 60, 0, 0, 110, 0,
                                       VERTICAL, note_design_page_information_center_customize, NULL, 0);
    gui_list_set_style(list, LIST_CLASSIC);
    gui_list_set_note_num(list, 2);
    gui_list_set_out_scope(list, 10);
    gui_list_set_offset(list, page_information_center_customize_list_offset_rec);
    gui_obj_create_timer(GUI_BASE(list), 10, true, list_timer_page_information_center_customize);

    gui_img_t *mask = gui_img_create_from_mem(parent, 0, MASK_BIN, 0, 0, 0, 0);
    gui_scroll_text_t *text = gui_scroll_text_create(parent, 0, 44, 16, 260, 30);
    gui_scroll_text_type_set(text, CAPTION_3_30_BIN, FONT_SRC_MEMADDR);
    gui_scroll_text_set(text, (void *)page_name_array[INFORMATION_CENTER_CUSTOMIZE], GUI_FONT_SRC_BMP,
                        font_color, strlen((void *)page_name_array[INFORMATION_CENTER_CUSTOMIZE]), 30);
    gui_scroll_text_scroll_set(text, SCROLL_X, 20, 260, 5000, 0);

    gui_img_t *scrollbar_bg = gui_img_create_from_mem(parent, 0, SCROLLBAR_BG_BIN, 310, 86, 0, 0);
    gui_img_t *scrollbar = gui_img_create_from_mem(parent, 0, SCROLLBAR_L_BIN, 310, 86, 0, 0);
    gui_img_a8_recolor(scrollbar_bg, scroll_color.color.argb_full);
    gui_img_a8_recolor(scrollbar, scroll_color.color.argb_full);
    gui_img_a8_recolor(mask, mask_color.color.argb_full);
    gui_img_a8_fix_bg(mask, mask_color.color.argb_full);
}

void page_unlock_slider_design(gui_obj_t *parent)
{
    quick_page_name_index = UNLOCK_SLIDER;
    gui_color_t font_color;
    gui_color_t font_color_warning;
    if (theme_bg_white)
    {
        font_color = FG_1_LIGHT;
        font_color_warning.color.argb_full = GUI_COLOR_ARGB8888(0xFF, 0x93, 0x93, 0x93);
    }
    else
    {
        font_color = FG_1_DARK;
        font_color_warning.color.argb_full = GUI_COLOR_ARGB8888(0xFF, 122, 122, 122);
    }

    gui_text_t *text = gui_text_create(parent, 0, 12, 45, 250, 15);
    gui_text_set(text, (void *)page_name_array[UNLOCK_SLIDER], GUI_FONT_SRC_BMP, font_color,
                 strlen((void *)page_name_array[UNLOCK_SLIDER]), 30);
    gui_text_type_set(text, CAPTION_3_30_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, LEFT);

    gui_img_t *toggle_bg = gui_img_create_from_mem(parent, 0, TOGGLE_BG_BIN, 244, 40, 0, 0);
    gui_img_t *toggle = gui_img_create_from_mem(toggle_bg, (void *)page_name_array[UNLOCK_SLIDER],
                                                TOGGLE_BIN, TOGGLE_OFF_X,
                                                TOGGLE_Y, 0, 0);
    gui_img_a8_recolor(toggle, FG_1_DARK.color.argb_full);
    gui_obj_add_event_cb(toggle_bg, click_toggle_cb, GUI_EVENT_TOUCH_CLICKED,
                         &(f_status.unlock_slider));

    text = gui_text_create(parent, 0, 12, 96, 296, 170);
    gui_text_set(text, (void *)unlock_slider_warning_str, GUI_FONT_SRC_BMP, font_color_warning,
                 strlen(unlock_slider_warning_str), 30);
    gui_text_type_set(text, CAPTION_3_30_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, MULTI_LEFT);

    if (theme_bg_white)
    {
        if (f_status.unlock_slider)
        {
            gui_obj_move(GUI_BASE(toggle), TOGGLE_ON_X, TOGGLE_Y);
            gui_img_a8_recolor(toggle_bg, BG_THEME3_BRIGHT_LIGHT.color.argb_full);
        }
        else
        {
            gui_img_a8_recolor(toggle_bg, FG_1_LIGHT.color.argb_full);
        }
        toggle_on_bg_color = BG_THEME3_BRIGHT_LIGHT.color.argb_full;
    }
    else
    {
        if (f_status.unlock_slider)
        {
            gui_obj_move(GUI_BASE(toggle), TOGGLE_ON_X, TOGGLE_Y);
            gui_img_a8_recolor(toggle_bg, BG_THEME3_BRIGHT_LIGHT.color.argb_full);
        }
        else
        {
            gui_img_a8_recolor(toggle_bg, BG_2_DARK.color.argb_full);
        }
        toggle_on_bg_color = BG_THEME3_BRIGHT_DARK.color.argb_full;
    }
    gui_img_a8_mix_alpha(toggle_bg, toggle_bg->fg_color_set >> 24);
    control_invalid = !f_status.unlock_slider;
}

void page_notification_design(gui_obj_t *parent)
{
    quick_page_name_index = NOTIFICATION;
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
    gui_text_set(text, (void *)page_name_array[NOTIFICATION], GUI_FONT_SRC_BMP, font_color,
                 strlen((void *)page_name_array[NOTIFICATION]), 30);
    gui_text_type_set(text, CAPTION_3_30_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, LEFT);

    gui_img_t *toggle_notification_bg = gui_img_create_from_mem(parent, 0, TOGGLE_BG_BIN, 244, 40, 0,
                                                                0);
    gui_img_t *toggle_notification = gui_img_create_from_mem(toggle_notification_bg,
                                                             (void *)page_name_array[NOTIFICATION],
                                                             TOGGLE_BIN, TOGGLE_OFF_X,
                                                             TOGGLE_Y, 0, 0);
    gui_img_a8_recolor(toggle_notification, FG_1_DARK.color.argb_full);
    gui_obj_add_event_cb(toggle_notification_bg, click_toggle_cb, GUI_EVENT_TOUCH_CLICKED,
                         &(f_status.notification));

    gui_img_t *toggle_message_bg = NULL;
    gui_img_t *toggle_message = NULL;
    if (f_status.notification)
    {
        text = gui_text_create(parent, 0, 12, 95, 300, 15);
        gui_text_set(text, (void *)page_name_array[NOTIFICATION], GUI_FONT_SRC_BMP, font_color,
                     strlen((void *)page_name_array[NOTIFICATION]), 30);
        gui_text_type_set(text, CAPTION_3_30_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(text, LEFT);
        toggle_message_bg = gui_img_create_from_mem(parent, 0, TOGGLE_BG_BIN, 244, 90, 0, 0);
        toggle_message = gui_img_create_from_mem(toggle_message_bg, (void *)page_name_array[NOTIFICATION],
                                                 TOGGLE_BIN,
                                                 TOGGLE_OFF_X,
                                                 TOGGLE_Y, 0, 0);
        gui_img_a8_recolor(toggle_message, FG_1_DARK.color.argb_full);

        gui_obj_add_event_cb(toggle_message_bg, click_toggle_cb, GUI_EVENT_TOUCH_CLICKED,
                             &(f_status.message_preview));
    }

    if (theme_bg_white)
    {
        if (f_status.notification)
        {
            gui_obj_move(GUI_BASE(toggle_notification), TOGGLE_ON_X, TOGGLE_Y);
            gui_img_a8_recolor(toggle_notification_bg, BG_THEME1_BRIGHT_LIGHT.color.argb_full);
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
            gui_img_a8_recolor(toggle_notification_bg, FG_1_LIGHT.color.argb_full);
        }
    }
    else
    {
        if (f_status.notification)
        {
            gui_obj_move(GUI_BASE(toggle_notification), TOGGLE_ON_X, TOGGLE_Y);
            gui_img_a8_recolor(toggle_notification_bg, BG_THEME1_BRIGHT_DARK.color.argb_full);
            if (f_status.message_preview)
            {
                gui_obj_move(GUI_BASE(toggle_message), TOGGLE_ON_X, TOGGLE_Y);
                gui_img_a8_recolor(toggle_message_bg, BG_THEME1_BRIGHT_DARK.color.argb_full);
            }
            else
            {
                gui_img_a8_recolor(toggle_message_bg, BG_2_DARK.color.argb_full);
                gui_img_a8_mix_alpha(toggle_message_bg, toggle_message_bg->fg_color_set >> 24);
            }
        }
        else
        {
            gui_img_a8_recolor(toggle_notification_bg, BG_2_DARK.color.argb_full);
        }
    }
    gui_img_a8_mix_alpha(toggle_notification_bg, toggle_notification_bg->fg_color_set >> 24);
}

void page_time_format_design(gui_obj_t *parent)
{
    quick_page_name_index = TIME_FORMAT;
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
    gui_text_set(text, (void *)page_name_array[TIME_FORMAT], GUI_FONT_SRC_BMP, font_color,
                 strlen((void *)page_name_array[TIME_FORMAT]), 30);
    gui_text_type_set(text, CAPTION_3_30_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, LEFT);

    gui_img_t *control_bg = gui_img_create_from_mem(parent, 0, CONTROL_BG_BIN, 12, 92, 0, 0);
    gui_img_t *button_bg = gui_img_create_from_mem(control_bg, (void *)page_name_array[DARK_LIGHT_MODE],
                                                   BUTTON_BG_ELLIPSE_148_88_BIN, BUTTON_X_FORMAT_12, 0, 0, 0);
    gui_img_t *format_12 = gui_img_create_from_mem(control_bg, 0, TIME_FORMAT_12_BIN, 45, 34, 0, 0);
    gui_img_t *format_24 = gui_img_create_from_mem(control_bg, 0, TIME_FORMAT_24_BIN, 186, 34, 0, 0);
    gui_obj_add_event_cb(control_bg, click_button_page_time_format, GUI_EVENT_TOUCH_CLICKED, NULL);
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

void page_language_design(gui_obj_t *parent)
{
    quick_page_name_index = LANGUAGE;
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
    gui_text_set(text, (void *)page_name_array[LANGUAGE], GUI_FONT_SRC_BMP, font_color,
                 strlen((void *)page_name_array[LANGUAGE]), 30);
    gui_text_type_set(text, CAPTION_3_30_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, LEFT);

    gui_img_t *control_bg = gui_img_create_from_mem(parent, 0, CONTROL_BG_BIN, 12, 92, 0, 0);
    gui_img_t *button_switch_l_bg = gui_img_create_from_mem(control_bg, "l",
                                                            BUTTON_BG_CIRCLE_BIN, 6, 6, 0, 0);
    gui_img_t *switch_l = gui_img_create_from_mem(control_bg, 0, SWITCH_L_BIN, 32, 32, 0, 0);
    gui_img_t *button_switch_r_bg = gui_img_create_from_mem(control_bg, "r",
                                                            BUTTON_BG_CIRCLE_BIN, 212, 6, 0, 0);
    gui_img_t *switch_r = gui_img_create_from_mem(control_bg, 0, SWITCH_R_BIN, 241, 32, 0, 0);
    gui_obj_hidden((void *)button_switch_l_bg, 1);
    gui_obj_hidden((void *)button_switch_r_bg, 1);
    gui_obj_create_timer(GUI_BASE(button_switch_l_bg), 10, true, press_button_page_language);
    gui_obj_create_timer(GUI_BASE(button_switch_r_bg), 10, true, press_button_page_language);

    text = gui_text_create(control_bg, 0, 0, 0, 296, 90);
    gui_text_set(text, (void *)language_type[language_type_index], GUI_FONT_SRC_BMP, font_color,
                 strlen(language_type[language_type_index]), 30);
    gui_text_type_set(text, CAPTION_3_30_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, MID_CENTER);

    if (theme_bg_white)
    {
        gui_img_a8_recolor(control_bg, BG_THEME3_DARK_LIGHT.color.argb_full);
        gui_img_a8_recolor(button_switch_l_bg, BG_THEME3_MID_LIGHT.color.argb_full);
        gui_img_a8_recolor(button_switch_r_bg, BG_THEME3_MID_LIGHT.color.argb_full);
        gui_img_a8_recolor(switch_l, FG_1_LIGHT.color.argb_full);
        gui_img_a8_recolor(switch_r, FG_1_LIGHT.color.argb_full);
        if (language_type_index == 0)
        {
            gui_img_a8_recolor(switch_l, FG_2_LIGHT.color.argb_full);
        }
        else if (language_type_index == 12)
        {
            gui_img_a8_recolor(switch_r, FG_2_LIGHT.color.argb_full);
        }
        toggle_on_bg_color = BG_THEME3_BRIGHT_LIGHT.color.argb_full;
    }
    else
    {
        gui_img_a8_recolor(control_bg, BG_THEME3_DARK_DARK.color.argb_full);
        gui_img_a8_recolor(button_switch_l_bg, BG_THEME3_MID_DARK.color.argb_full);
        gui_img_a8_recolor(button_switch_r_bg, BG_THEME3_MID_DARK.color.argb_full);
        gui_img_a8_recolor(switch_l, FG_1_DARK.color.argb_full);
        gui_img_a8_recolor(switch_r, FG_1_DARK.color.argb_full);
        if (language_type_index == 0)
        {
            gui_img_a8_recolor(switch_l, FG_2_DARK.color.argb_full);
        }
        else if (language_type_index == 12)
        {
            gui_img_a8_recolor(switch_r, FG_2_DARK.color.argb_full);
        }
        toggle_on_bg_color = BG_THEME3_BRIGHT_DARK.color.argb_full;
    }
    gui_img_a8_mix_alpha(switch_l, switch_l->fg_color_set >> 24);
    gui_img_a8_mix_alpha(switch_r, switch_r->fg_color_set >> 24);
}

void page_reorder_quick_access_design(gui_obj_t *parent)
{
    gui_color_t font_color;
    gui_color_t mask_color;
    gui_color_t scroll_color;
    if (theme_bg_white)
    {
        font_color = FG_1_LIGHT;
        mask_color = SCREEN_BG_LIGHT;
        scroll_color = FG_DARK;
    }
    else
    {
        font_color = FG_1_DARK;
        mask_color = SCREEN_BG_DARK;
        scroll_color = FG_WHITE;
    }

    gui_win_t *win = gui_win_create(gui_obj_get_fake_root(), 0, 0, 0, 0, 0);
    win->base.parent = parent;
    gui_list_remove(&win->base.brother_list);
    gui_list_insert(&(parent->child_list), &(win->base.brother_list));
    parent = GUI_BASE(win);
    sprintf(page_str, "1234");

    gui_list_t *list = gui_list_create(parent, "list", 0, 60, 0, 0, 56, 0,
                                       VERTICAL, note_design_page_reorder_quick_access, NULL, 0);
    gui_list_set_style(list, LIST_CLASSIC);
    gui_list_set_note_num(list, quick_page_num);
    gui_list_set_out_scope(list, 10);
    gui_list_set_offset(list, page_reorder_quick_access_list_offset_rec);
    gui_obj_create_timer(GUI_BASE(list), 10, true, list_timer_page_reorder_quick_access);

    gui_img_t *mask = gui_img_create_from_mem(parent, 0, MASK_BIN, 0, 0, 0, 0);
    gui_scroll_text_t *text = gui_scroll_text_create(parent, 0, 44, 16, 260, 30);
    gui_scroll_text_type_set(text, CAPTION_3_30_BIN, FONT_SRC_MEMADDR);
    gui_scroll_text_set(text, (void *)page_name_array[REORDER_QUICK_ACCESS], GUI_FONT_SRC_BMP,
                        font_color, strlen((void *)page_name_array[REORDER_QUICK_ACCESS]), 30);
    gui_scroll_text_scroll_set(text, SCROLL_X, 20, 260, 5000, 0);

    if (quick_page_num > 2)
    {
        gui_img_t *scrollbar_bg = gui_img_create_from_mem(parent, 0, SCROLLBAR_BG_BIN, 310, 86, 0, 0);
        gui_img_t *scrollbar = gui_img_create_from_mem(parent, 0, SCROLLBAR_L_BIN, 310, 86, 0, 0);
        gui_img_a8_recolor(scrollbar_bg, scroll_color.color.argb_full);
        gui_img_a8_recolor(scrollbar, scroll_color.color.argb_full);
    }
    gui_img_a8_recolor(mask, mask_color.color.argb_full);
    gui_img_a8_fix_bg(mask, mask_color.color.argb_full);
    control_invalid = false;
}

void page_support_design(gui_obj_t *parent)
{
    gui_color_t font_color;
    gui_color_t img_color;
    gui_color_t bg_color;
    if (theme_bg_white)
    {
        font_color = FG_1_LIGHT;
        img_color = FG_THEME3_LIGHT;
        bg_color = BG_1_LIGHT;
    }
    else
    {
        font_color = FG_1_DARK;
        img_color = FG_THEME3_DARK;
        bg_color = BG_1_DARK;
    }
    gui_win_t *win = gui_win_create(gui_obj_get_fake_root(), 0, 0, 0, 0, 0);
    win->base.parent = parent;
    gui_list_remove(&win->base.brother_list);
    gui_list_insert(&(parent->child_list), &(win->base.brother_list));
    parent = GUI_BASE(win);

    gui_win_t *win_tips = gui_win_create(parent, 0, 0, 60, 0, 56);
    gui_win_t *win_reset = gui_win_create(parent, 0, 0, 116, 0, 56);
    gui_obj_add_event_cb(win_tips, click_button_page_support, GUI_EVENT_TOUCH_CLICKED, NULL);
    gui_obj_add_event_cb(win_reset, click_button_page_support, GUI_EVENT_TOUCH_CLICKED, NULL);

    gui_img_t *win_tips_bg = gui_img_create_from_mem(win_tips, 0, MENU_LISTNOTE_BG_BIN, 0, 0, 0, 0);
    gui_img_t *win_reset_bg = gui_img_create_from_mem(win_reset, 0, MENU_LISTNOTE_BG_BIN, 0, 0, 0, 0);
    gui_img_a8_recolor(win_tips_bg, bg_color.color.argb_full);
    gui_img_a8_recolor(win_reset_bg, bg_color.color.argb_full);
    gui_img_a8_mix_alpha(win_tips_bg, win_tips_bg->fg_color_set >> 24);
    gui_img_a8_mix_alpha(win_reset_bg, win_reset_bg->fg_color_set >> 24);
    gui_obj_hidden((void *)win_tips_bg, true);
    gui_obj_hidden((void *)win_reset_bg, true);

    gui_img_t *tips = gui_img_create_from_mem(win_tips, 0, ICON_PRODUCT_TIPS_BIN, 8, 8, 0, 0);
    gui_img_t *reset = gui_img_create_from_mem(win_reset, 0, ICON_FACTORY_RESET_BIN, 8, 8, 0, 0);
    gui_img_a8_recolor(tips, img_color.color.argb_full);
    gui_img_a8_recolor(reset, img_color.color.argb_full);

    gui_text_t *text = gui_text_create(parent, 0, 0, 0, 320, 60);
    gui_text_set(text, (void *)page_name_array[SUPPORT], GUI_FONT_SRC_BMP, font_color,
                 strlen((void *)page_name_array[SUPPORT]), 30);
    gui_text_type_set(text, CAPTION_3_30_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, MID_CENTER);
    text = gui_text_create(win_tips, 0, 56, 8, 248, 40);
    gui_text_set(text, (void *)page_name_array[PRODUCT_TIPS], GUI_FONT_SRC_BMP, font_color,
                 strlen((void *)page_name_array[PRODUCT_TIPS]), 30);
    gui_text_type_set(text, CAPTION_3_30_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, MID_LEFT);
    text = gui_text_create(win_reset, 0, 56, 8, 248, 40);
    gui_text_set(text, (void *)page_name_array[FACTORY_RESET], GUI_FONT_SRC_BMP, font_color,
                 strlen((void *)page_name_array[FACTORY_RESET]), 30);
    gui_text_type_set(text, CAPTION_3_30_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, MID_LEFT);
}

/* Tools page */
void page_find_buds_design(gui_obj_t *parent)
{
    quick_page_name_index = FIND_MY_BUDS;
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
    gui_text_set(text, (void *)page_name_array[FIND_MY_BUDS], GUI_FONT_SRC_BMP, font_color,
                 strlen((void *)page_name_array[FIND_MY_BUDS]), 30);
    gui_text_type_set(text, CAPTION_3_30_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, LEFT);

    gui_img_t *button_bg_l = gui_img_create_from_mem(parent, "l", BUTTON_BG_ELLIPSE_134_66_BIN, 12, 92,
                                                     0, 0);
    gui_img_t *button_bg_r = gui_img_create_from_mem(parent, "r", BUTTON_BG_ELLIPSE_134_66_BIN, 174, 92,
                                                     0, 0);
    gui_img_t *icon_l = gui_img_create_from_mem(button_bg_l, 0, ICON_L_BIN, 58, 21, 0, 0);
    gui_img_t *icon_r = gui_img_create_from_mem(button_bg_r, 0, ICON_R_BIN, 58, 21, 0, 0);
    gui_obj_add_event_cb(button_bg_l, click_button_page_find_buds, GUI_EVENT_TOUCH_CLICKED, NULL);
    gui_obj_add_event_cb(button_bg_r, click_button_page_find_buds, GUI_EVENT_TOUCH_CLICKED, NULL);

    if (f_status.earbuds_shock_l)
    {
        gui_img_set_image_data(icon_l, TIMER_PAUSE_BIN);
        gui_img_refresh_size(icon_l);
    }
    else if (f_status.earbuds_in_ear_l)
    {
        gui_img_t *note = gui_img_create_from_mem(parent, 0, ICON_NOTE_BIN, 44, 162, 0, 0);
        gui_img_set_mode(note, IMG_2D_SW_SRC_OVER_MODE);

        gui_text_t *text = gui_text_create(parent, 0, 68, 163, 50, 20);
        gui_text_set(text, "In ear", GUI_FONT_SRC_BMP, FG_WARNING, 7, 20);
        gui_text_type_set(text, HEADING_1_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(text, LEFT);
    }
    if (f_status.earbuds_shock_r)
    {
        gui_img_set_image_data(icon_r, TIMER_PAUSE_BIN);
        gui_img_refresh_size(icon_r);
    }
    else if (f_status.earbuds_in_ear_r)
    {
        gui_img_t *note = gui_img_create_from_mem(parent, 0, ICON_NOTE_BIN, 204, 162, 0, 0);
        gui_img_set_mode(note, IMG_2D_SW_SRC_OVER_MODE);

        gui_text_t *text = gui_text_create(parent, 0, 228, 163, 50, 20);
        gui_text_set(text, "In ear", GUI_FONT_SRC_BMP, FG_WARNING, 7, 20);
        gui_text_type_set(text, HEADING_1_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(text, LEFT);
    }

    if (theme_bg_white)
    {
        if (f_status.earbuds_shock_l)
        {
            gui_img_a8_recolor(button_bg_l, BG_THEME2_MID_LIGHT.color.argb_full);
            gui_img_a8_recolor(icon_l, FG_THEME2_LIGHT.color.argb_full);
        }
        else
        {
            gui_img_a8_recolor(button_bg_l, BG_THEME2_BRIGHT_LIGHT.color.argb_full);
            gui_img_a8_recolor(icon_l, FG_1_LIGHT.color.argb_full);
        }
        if (f_status.earbuds_shock_r)
        {
            gui_img_a8_recolor(button_bg_r, BG_THEME2_MID_LIGHT.color.argb_full);
            gui_img_a8_recolor(icon_r, FG_THEME2_LIGHT.color.argb_full);
        }
        else
        {
            gui_img_a8_recolor(button_bg_r, BG_THEME2_BRIGHT_LIGHT.color.argb_full);
            gui_img_a8_recolor(icon_r, FG_1_LIGHT.color.argb_full);
        }
    }
    else
    {
        if (f_status.earbuds_shock_l)
        {
            gui_img_a8_recolor(button_bg_l, BG_THEME2_MID_DARK.color.argb_full);
            gui_img_a8_recolor(icon_l, FG_THEME2_DARK.color.argb_full);
        }
        else
        {
            gui_img_a8_recolor(button_bg_l, BG_THEME2_BRIGHT_DARK.color.argb_full);
            gui_img_a8_recolor(icon_l, FG_1_DARK.color.argb_full);
        }
        if (f_status.earbuds_shock_r)
        {
            gui_img_a8_recolor(button_bg_r, BG_THEME2_MID_DARK.color.argb_full);
            gui_img_a8_recolor(icon_r, FG_THEME2_DARK.color.argb_full);
        }
        else
        {
            gui_img_a8_recolor(button_bg_r, BG_THEME2_BRIGHT_DARK.color.argb_full);
            gui_img_a8_recolor(icon_r, FG_1_DARK.color.argb_full);
        }
    }
}

void page_timer_design(gui_obj_t *parent)
{
    quick_page_name_index = TIMER;
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
    gui_text_set(text, (void *)page_name_array[TIMER], GUI_FONT_SRC_BMP, font_color,
                 strlen((void *)page_name_array[TIMER]), 30);
    gui_text_type_set(text, CAPTION_3_30_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, LEFT);

    gui_img_t *play_bg = gui_img_create_from_mem(parent, 0, BUTTON_BG_ELLIPSE_64_40_BIN, 244, 40, 0, 0);
    gui_img_t *play = gui_img_create_from_mem(play_bg, 0, TIMER_PLAY_BIN, 24, 10, 0, 0);
    gui_obj_add_event_cb(play_bg, click_button_page_timer, GUI_EVENT_TOUCH_CLICKED, NULL);

    gui_img_t *control_bg = gui_img_create_from_mem(parent, 0, CONTROL_BG_BIN, 12, 92, 0, 0);
    gui_img_t *button_sub_bg = gui_img_create_from_mem(control_bg, "l", BUTTON_BG_CIRCLE_BIN, 6, 6, 0,
                                                       0);
    gui_img_t *sub = gui_img_create_from_mem(control_bg, 0, SUB_BIN, 31, 42, 0, 0);
    gui_img_t *button_add_bg = gui_img_create_from_mem(control_bg, "r", BUTTON_BG_CIRCLE_BIN, 212, 6,
                                                       0, 0);
    gui_img_t *add = gui_img_create_from_mem(control_bg, 0, ADD_BIN, 237, 31, 0, 0);

    text = gui_text_create(control_bg, 0, 30, 0, 266, 90);
    gui_text_set(text, "min", GUI_FONT_SRC_BMP, font_color, 3, 20);
    gui_text_type_set(text, HEADING_1_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, MID_CENTER);
    sprintf(page_str, "%d", timer_max_min_val);
    text = gui_text_create(control_bg, 0, -15, 0, 296, 90);
    gui_text_set(text, page_str, GUI_FONT_SRC_BMP, font_color, strlen(page_str), 32);
    gui_text_type_set(text, CAPTION_1_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, MID_CENTER);

    gui_obj_hidden((void *)button_sub_bg, 1);
    gui_obj_hidden((void *)button_add_bg, 1);
    gui_obj_create_timer(GUI_BASE(button_sub_bg), 10, true, press_button_page_timer);
    gui_obj_create_timer(GUI_BASE(button_add_bg), 10, true, press_button_page_timer);
    gui_obj_add_event_cb(button_sub_bg, click_button_no_action, GUI_EVENT_TOUCH_LONG, NULL);
    gui_obj_add_event_cb(button_add_bg, click_button_no_action, GUI_EVENT_TOUCH_LONG, NULL);

    if (theme_bg_white)
    {
        gui_img_a8_recolor(play_bg, BG_THEME2_BRIGHT_LIGHT.color.argb_full);
        gui_img_a8_recolor(play, FG_WHITE.color.argb_full);
        gui_img_a8_recolor(control_bg, BG_THEME2_DARK_LIGHT.color.argb_full);
        gui_img_a8_recolor(button_sub_bg, BG_THEME2_MID_LIGHT.color.argb_full);
        gui_img_a8_recolor(button_add_bg, BG_THEME2_MID_LIGHT.color.argb_full);
        gui_img_a8_recolor(sub, FG_1_LIGHT.color.argb_full);
        gui_img_a8_recolor(add, FG_1_LIGHT.color.argb_full);
    }
    else
    {
        gui_img_a8_recolor(play_bg, BG_THEME2_BRIGHT_DARK.color.argb_full);
        gui_img_a8_recolor(play, FG_WHITE.color.argb_full);
        gui_img_a8_recolor(control_bg, BG_THEME2_DARK_DARK.color.argb_full);
        gui_img_a8_recolor(button_sub_bg, BG_THEME2_MID_DARK.color.argb_full);
        gui_img_a8_recolor(button_add_bg, BG_THEME2_MID_DARK.color.argb_full);
        gui_img_a8_recolor(sub, FG_1_DARK.color.argb_full);
        gui_img_a8_recolor(add, FG_1_DARK.color.argb_full);
    }
    gui_img_a8_mix_alpha(sub, sub->fg_color_set >> 24);
    gui_img_a8_mix_alpha(add, add->fg_color_set >> 24);
}

void page_flashlight_design(gui_obj_t *parent)
{
    quick_page_name_index = FLASHLIGHT;
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
    gui_text_set(text, (void *)page_name_array[FLASHLIGHT], GUI_FONT_SRC_BMP, font_color,
                 strlen((void *)page_name_array[FLASHLIGHT]), 30);
    gui_text_type_set(text, CAPTION_3_30_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, LEFT);

    gui_img_t *control_bg = gui_img_create_from_mem(parent, 0, CONTROL_BG_BIN, 12, 92, 0, 0);
    gui_img_t *icon = gui_img_create_from_mem(control_bg, 0, ICON_FLASHLIGHT_ON_BIN, 138, 28, 0, 0);
    gui_img_a8_recolor(control_bg,
                       theme_bg_white ? BG_THEME2_BRIGHT_LIGHT.color.argb_full : BG_THEME2_BRIGHT_DARK.color.argb_full);
    gui_img_a8_recolor(icon, FG_WHITE.color.argb_full);
    gui_obj_add_event_cb(control_bg, click_button_page_flashlight, GUI_EVENT_TOUCH_CLICKED, NULL);
}

void page_qrcode_design(gui_obj_t *parent)
{
    quick_page_name_index = JBL_HEADPHONES_APP;
    gui_img_t *img = gui_img_create_from_mem(parent, 0, QRCODE_BIN, 104, 40, 0, 0);
    gui_img_set_mode(img, IMG_2D_SW_SRC_OVER_MODE);
}