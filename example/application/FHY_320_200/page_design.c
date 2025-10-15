/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "app_main.h"
#include "common_data.h"
#include "gui_img.h"
#include "gui_canvas_rect.h"
#include "gui_win.h"
#include "gui_text.h"
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
#define EQUALIZER        "Equalizer"
#define AMBIENT_SOUND    "Ambient Sound"
#define MUSIC            "music"
#define DARK_LIGHT_MODE  "Dark/Light Mode"

#define PROGRESSBAR_W_MAX 116
#define TOGGLE_OFF_X 4
#define TOGGLE_ON_X 28
#define TOGGLE_Y 4

#define BUTTON_X_ANC 0
#define BUTTON_X_AA  99
#define BUTTON_X_TT 197

#define BUTTON_X_LIGHT 148
#define BUTTON_X_DARK  0

#define MUSIC_ARRAY_NUM 20

/*============================================================================*
 *                           Function Declaration
 *============================================================================*/
static void update_ambient_sound_status(gui_obj_t *obj);

/*============================================================================*
 *                            Variables
 *============================================================================*/
static uint8_t toggle_target_x = 0;

static bool pressed_sub = false;
static bool pressed_add = false;

static bool pressed_switch_l = false;
static bool pressed_switch_r = false;
void *music_type_addr[6] = {0};
static int8_t music_type_index = 0;
static bool equalizer_on = false;

static bool ambient_sound_on = false;
static AMBIENT_SOUND_STATUS ambient_sound_status = ANC;
static uint8_t button_bg_target_x = 0;

static bool music_play = false;
static bool pressed_last = false;
static bool pressed_next = false;
static int8_t music_index = 0;
static const char *music_array[MUSIC_ARRAY_NUM] =
{
    "Feels",
    "Stay",
    "Blinding Lights",
    "Levitating",
    "Peaches",
    "Montero",
    "Watermelon Sugar",
    "Good 4 U",
    "Kiss Me More",
    "Save Your Tears",
    "Industry Baby",
    "Heat Waves",
};

static void *progressbar_array[10] = {0};

/*============================================================================*
 *                           Private Functions
 *============================================================================*/
static void toggle_move(void *p)
{
    gui_obj_t *obj = (gui_obj_t *)p;
    int step = 4;

    if (obj->x == toggle_target_x)
    {
        gui_obj_delete_timer(obj);
        if (toggle_target_x == TOGGLE_ON_X)
        {
            if (theme_bg_white)
            {
                gui_img_a8_recolor((gui_img_t *)(obj->parent), BG_THEME1_BRIGHT_LIGHT.color.argb_full);
            }
            else
            {
                gui_img_a8_recolor((gui_img_t *)(obj->parent), BG_THEME1_BRIGHT_DARK.color.argb_full);
            }
        }
        else
        {
            if (theme_bg_white)
            {
                gui_img_a8_recolor((gui_img_t *)(obj->parent), FG_1_LIGHT.color.argb_full);
            }
            else
            {
                gui_img_a8_recolor((gui_img_t *)(obj->parent), BG_2_DARK.color.argb_full);
            }
        }
    }
    else if (obj->x < toggle_target_x)
    {
        obj->x += step;
    }
    else
    {
        obj->x -= step;
    }
}

static void regenerate_for_dark_light_change(void *msg)
{
    GUI_UNUSED(msg);

    gui_view_t *current_view = gui_view_get_current();
    const struct gui_view_descriptor *descriptor = current_view->descriptor;
    gui_obj_t *parent = current_view->base.parent;
    gui_obj_tree_free(GUI_BASE(current_view));
    gui_set_bg_color(BG_1_LIGHT);

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
        else if (!strcmp(obj->name, DARK_LIGHT_MODE))
        {
            gui_msg_t msg =
            {
                .event = GUI_EVENT_USER_DEFINE,
                .cb = regenerate_for_dark_light_change,
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
        gui_obj_hidden(o, false);
        if (strcmp(o->name, "add") == 0)
        {
            pressed_add = true;
        }
        else
        {
            pressed_sub = true;
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
            if (pressed_add)
            {
                volume_val ++;
            }
            else if (pressed_sub)
            {
                volume_val --;
            }
            GUI_WIDGET_POINTER_BY_NAME_ROOT(progressbar, "progressbar", o->parent);
            if (volume_val < 0)
            {
                volume_val = 0;

            }
            else if (volume_val > 10)
            {
                volume_val = 10;
            }

            if (volume_val == 0)
            {
                gui_obj_hidden((void *)progressbar, true);
            }
            else
            {
                gui_obj_hidden((void *)progressbar, false);
                gui_img_set_image_data((void *)progressbar, progressbar_array[volume_val - 1]);
            }
        }
        pressed_add = false;
        pressed_sub = false;
    }
}

static void press_button_page_equalizer(void *obj)
{
    GUI_UNUSED(obj);
    gui_obj_t *o = GUI_BASE(obj);
    touch_info_t *tp = tp_get_info();
    if (tp->pressed && gui_obj_point_in_obj_rect(o, tp->x, tp->y))
    {
        gui_obj_hidden(o, false);
        if (strcmp(o->name, "switch_l") == 0)
        {
            pressed_switch_l = true;
        }
        else
        {
            pressed_switch_r = true;
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
            if (pressed_switch_l)
            {
                music_type_index--;
            }
            else if (pressed_switch_r)
            {
                music_type_index++;
            }
            if (music_type_index < 0)
            {
                music_type_index = 0;
            }
            else if (music_type_index > 5)
            {
                music_type_index = 5;
            }
            GUI_WIDGET_POINTER_BY_NAME_ROOT(music_type, "music_type", o->parent);
            gui_img_set_image_data((gui_img_t *)music_type, music_type_addr[music_type_index]);
        }
        pressed_switch_l = false;
        pressed_switch_r = false;
    }
}


static void press_button_page_music(void *obj)
{
    GUI_UNUSED(obj);
    gui_obj_t *o = GUI_BASE(obj);
    touch_info_t *tp = tp_get_info();
    if (tp->pressed && gui_obj_point_in_obj_rect(o, tp->x, tp->y))
    {
        gui_obj_hidden(o, false);
        if (strcmp(o->name, "last") == 0)
        {
            pressed_last = true;
        }
        else
        {
            pressed_next = true;
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
            if (pressed_last)
            {
                music_index--;
            }
            else if (pressed_next)
            {
                music_index++;
            }
            if (music_index < 0)
            {
                music_index = 0;
            }
            else if (!music_array[music_index])
            {
                music_index--;
            }
            GUI_WIDGET_POINTER_BY_NAME_ROOT(text, MUSIC, o->parent->parent);
            gui_text_content_set((gui_text_t *)text, (void *)music_array[music_index],
                                 strlen(music_array[music_index]));
        }
        pressed_last = false;
        pressed_next = false;
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
    music_play = !music_play;
    if (music_play)
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

static void click_toggle_cb(void *obj, gui_event_t e, void *param)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);
    GUI_UNUSED(param);
    gui_obj_t *o = GUI_BASE(obj);
    gui_obj_t *toggle = gui_list_entry(o->child_list.next, gui_obj_t, brother_list);

    if (strcmp(toggle->name, EQUALIZER) == 0)
    {
        equalizer_on = !equalizer_on;
        if (equalizer_on)
        {
            toggle_target_x = TOGGLE_ON_X;
        }
        else
        {
            toggle_target_x = TOGGLE_OFF_X;
        }
    }
    else
    {
        ambient_sound_on = !ambient_sound_on;
        if (ambient_sound_on)
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

static void note_design(gui_obj_t *obj, void *p)
{
    GUI_UNUSED(p);
    uint16_t index = ((gui_list_note_t *)obj)->index;
    switch (index)
    {
    case 0:
        page_dark_light_design(obj);
        break;
    case 1:
        page_equalizer_design(obj);
        break;
    case 2:
        page_ambient_sound_design(obj);
        break;
    case 3:
        page_music_design(obj);
        break;
    default:
        break;
    }
}

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
    if (!progressbar_array[0])
    {
        progressbar_array[0] = PROGRESSBAR_1_BIN;
        progressbar_array[1] = PROGRESSBAR_2_BIN;
        progressbar_array[2] = PROGRESSBAR_3_BIN;
        progressbar_array[3] = PROGRESSBAR_4_BIN;
        progressbar_array[4] = PROGRESSBAR_5_BIN;
        progressbar_array[5] = PROGRESSBAR_6_BIN;
        progressbar_array[6] = PROGRESSBAR_7_BIN;
        progressbar_array[7] = PROGRESSBAR_8_BIN;
        progressbar_array[8] = PROGRESSBAR_9_BIN;
        progressbar_array[9] = PROGRESSBAR_10_BIN;
    }

    gui_text_t *text = gui_text_create(parent, 0, 12, 45, 100, 15);
    gui_text_set(text, "Volume", GUI_FONT_SRC_BMP, font_color, 6, 28);
    gui_text_type_set(text, CAPTION_2_BIN, FONT_SRC_MEMADDR);
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
    gui_img_t *progressbar = gui_img_create_from_mem(control_bg, "progressbar", progressbar_array[0],
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
        gui_img_set_image_data(progressbar, progressbar_array[volume_val - 1]);
    }

    gui_obj_create_timer(GUI_BASE(button_sub_bg), 10, true, press_button_page_volume);
    gui_obj_create_timer(GUI_BASE(button_add_bg), 10, true, press_button_page_volume);

    if (theme_bg_white)
    {
        gui_img_a8_recolor(control_bg, BG_THEME1_DARK_LIGHT.color.argb_full);
        gui_img_a8_recolor(button_sub_bg, BG_THEME1_MID_LIGHT.color.argb_full);
        gui_img_a8_recolor(sub, FG_1_LIGHT.color.argb_full);
        gui_img_a8_recolor(button_add_bg, BG_THEME1_MID_LIGHT.color.argb_full);
        gui_img_a8_recolor(add, FG_1_LIGHT.color.argb_full);
        gui_img_a8_recolor(progressbar_bg, BG_THEME1_MID_LIGHT.color.argb_full);
        gui_img_a8_recolor(progressbar, FG_THEME1_LIGHT.color.argb_full);
    }
    else
    {
        gui_img_a8_recolor(control_bg, BG_THEME1_DARK_DARK.color.argb_full);
        gui_img_a8_recolor(button_sub_bg, BG_THEME1_MID_DARK.color.argb_full);
        gui_img_a8_recolor(sub, FG_1_DARK.color.argb_full);
        gui_img_a8_recolor(button_add_bg, BG_THEME1_MID_DARK.color.argb_full);
        gui_img_a8_recolor(add, FG_1_DARK.color.argb_full);
        gui_img_a8_recolor(progressbar_bg, BG_THEME1_MID_DARK.color.argb_full);
        gui_img_a8_recolor(progressbar, FG_THEME1_DARK.color.argb_full);
    }
}

void page_equalizer_design(gui_obj_t *parent)
{
    gui_color_t font_color;
    if (!music_type_addr[0])
    {
        music_type_addr[0] = STUDIO_BIN;
        music_type_addr[1] = BASS_BIN;
        music_type_addr[2] = CLUB_BIN;
        music_type_addr[3] = EXTRE_BASS_BIN;
        music_type_addr[4] = VOCAL_BIN;
        music_type_addr[5] = JAZZ_BIN;
    }
    if (theme_bg_white)
    {
        font_color = FG_1_LIGHT;
    }
    else
    {
        font_color = FG_1_DARK;
    }
    gui_text_t *text = gui_text_create(parent, 0, 12, 45, 100, 15);
    gui_text_set(text, EQUALIZER, GUI_FONT_SRC_BMP, font_color, 9, 28);
    gui_text_type_set(text, CAPTION_2_BIN, FONT_SRC_MEMADDR);
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
                                                    music_type_addr[music_type_index], 86, 5, 0, 0);
    gui_obj_hidden((void *)button_switch_l_bg, 1);
    gui_obj_hidden((void *)button_switch_r_bg, 1);
    gui_obj_create_timer(GUI_BASE(button_switch_l_bg), 10, true, press_button_page_equalizer);
    gui_obj_create_timer(GUI_BASE(button_switch_r_bg), 10, true, press_button_page_equalizer);

    if (theme_bg_white)
    {
        gui_img_a8_recolor(control_bg, BG_THEME1_DARK_LIGHT.color.argb_full);
        gui_img_a8_recolor(button_switch_l_bg, BG_THEME1_MID_LIGHT.color.argb_full);
        gui_img_a8_recolor(switch_l, FG_1_LIGHT.color.argb_full);
        gui_img_a8_recolor(button_switch_r_bg, BG_THEME1_MID_LIGHT.color.argb_full);
        gui_img_a8_recolor(switch_r, FG_1_LIGHT.color.argb_full);
        gui_img_a8_recolor(music_type, FG_1_LIGHT.color.argb_full);
        if (equalizer_on)
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
        gui_img_a8_recolor(switch_l, FG_1_DARK.color.argb_full);
        gui_img_a8_recolor(button_switch_r_bg, BG_THEME1_MID_DARK.color.argb_full);
        gui_img_a8_recolor(switch_r, FG_1_DARK.color.argb_full);
        gui_img_a8_recolor(music_type, FG_1_DARK.color.argb_full);
        if (equalizer_on)
        {
            gui_obj_move(GUI_BASE(toggle), TOGGLE_ON_X, TOGGLE_Y);
            gui_img_a8_recolor(toggle_bg, BG_THEME1_BRIGHT_DARK.color.argb_full);
        }
        else
        {
            gui_img_a8_recolor(toggle_bg, BG_2_DARK.color.argb_full);
        }
    }
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
    gui_text_t *text = gui_text_create(parent, 0, 12, 45, 150, 15);
    gui_text_set(text, AMBIENT_SOUND, GUI_FONT_SRC_BMP, font_color, strlen(AMBIENT_SOUND), 28);
    gui_text_type_set(text, CAPTION_2_BIN, FONT_SRC_MEMADDR);
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
        if (ambient_sound_on)
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
        if (ambient_sound_on)
        {
            gui_obj_move(GUI_BASE(toggle), TOGGLE_ON_X, TOGGLE_Y);
            gui_img_a8_recolor(toggle_bg, BG_THEME1_BRIGHT_DARK.color.argb_full);
        }
        else
        {
            gui_img_a8_recolor(toggle_bg, BG_2_DARK.color.argb_full);
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
    gui_text_t *text = gui_text_create(parent, MUSIC, 12, 40, 200, 15);
    gui_text_set(text, (char *)music_array[music_index], GUI_FONT_SRC_BMP, font_color,
                 strlen(music_array[music_index]), 28);
    gui_text_type_set(text, CAPTION_2_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, LEFT);
    text = gui_text_create(parent, 0, 12, 65, 200, 15);
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
    if (music_play)
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
        gui_img_a8_recolor(last, FG_1_LIGHT.color.argb_full);
        gui_img_a8_recolor(button_next_bg, BG_THEME1_MID_LIGHT.color.argb_full);
        gui_img_a8_recolor(next, FG_1_LIGHT.color.argb_full);
        gui_img_a8_recolor(play, FG_1_LIGHT.color.argb_full);
    }
    else
    {
        gui_img_a8_recolor(control_bg, BG_THEME1_DARK_DARK.color.argb_full);
        gui_img_a8_recolor(button_last_bg, BG_THEME1_MID_DARK.color.argb_full);
        gui_img_a8_recolor(last, FG_1_DARK.color.argb_full);
        gui_img_a8_recolor(button_next_bg, BG_THEME1_MID_DARK.color.argb_full);
        gui_img_a8_recolor(next, FG_1_DARK.color.argb_full);
        gui_img_a8_recolor(play, FG_1_DARK.color.argb_full);
    }
}

static void click_button_no_action(void *obj, gui_event_t e, void *param)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);
    GUI_UNUSED(param);
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
    gui_text_t *text = gui_text_create(parent, 0, 12, 45, 150, 15);
    gui_text_set(text, DARK_LIGHT_MODE, GUI_FONT_SRC_BMP, font_color, strlen(DARK_LIGHT_MODE), 28);
    gui_text_type_set(text, CAPTION_2_BIN, FONT_SRC_MEMADDR);
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