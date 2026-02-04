/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "app_main.h"
#include "common_data.h"
#include "gui_img.h"
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

#define INVALID_OPACITY 102

/*============================================================================*
 *                           Function Declaration
 *============================================================================*/
static void note_timer_up_page_reorder_quick_access(void *obj);
static void note_timer_down_page_reorder_quick_access(void *obj);
static void disconnect_timer_page_audio_source(void *obj);
static void connect_timer_page_audio_source(void *obj);
static void delete_timer_page_audio_source(void *obj);
static void pair_timer_page_audio_source(void *obj);
static void scan_timer_page_audio_source(void *obj);
static void connect_auracast_timer_page_audio_source(void *obj);
static void delete_timer_page_tx_management(void *obj);
static void disconnect_timer_page_tx_management(void *obj);
static void connect_timer_page_tx_management(void *obj);
static void connect_discovered_timer_page_tx_management(void *obj);
static void pair_timer_page_tx_management(void *obj);


/*============================================================================*
 *                            Variables
 *============================================================================*/
static uint8_t toggle_target_x = 0;
static uint32_t toggle_on_bg_color = 0;
static bool control_invalid = false;
static void *img_data_array[16] = {0};
static bool pressed_l = false;
static bool pressed_r = false;

static int8_t euqalizer_type_index = 0;
static uint8_t button_bg_target_x = 0;

static const char quick_wake_up_warning_str[] =
{
    "When disabled, only the\nbutton or opening the case will work. When enabled,\nusethe preferred method to\nwake up the screen."
};

static const char *unlock_slider_warning_str[13] =
{
    "Use unlock slider to prevent\naccidental screen touches.",
    "Utilisez le curseur de déverrouillage pour éviter les actions accidentelles à l'écran",
    "Verwende den Entsperrschieber, um versehentliche Berührungen des Bildschirms zu verhindern",
    "Usa el deslizador de desbloqueo para evitar tocar la pantalla accidentalmente",
    "Utilizza il cursore di sblocco per evitare tocchi accidentali dello schermo",
    "Gebruik de schuifontgrendelaar om onbedoelde aanrakingen van het scherm te voorkomen",
    "Use o controlo deslizante de desbloqueio para evitar toques acidentais no ecrã",
    "Используйте ползунок разблокировки для предотвращения случайных касаний экрана",
    "使用解锁滑块以避免误触屏幕",
    "スライダーのアンロックを使って 予想外の画面タッチを防ぐ",
    "잠금 해제 슬라이더를 사용하여 잘못된 화면 터치 방지",
    "ใช้แถบเลื่อนปลดล็อกเพื่อป้องกันการสัมผัสหน้าจอโดยไม่ตั้งใจ",
    "Sử dụng thanh trượt mở khóa để tránh chạm nhầm màn hình"
};

static const char *language_type[13] =
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

static const char *audio_source_warning_str[2] =
{
    "Your device can connect up to 2\n other devices at the same time.",
    "No Auracast discovered."
};

static const char *audio_source_pair_str[] =
{
    "Pair Bluetooth",
    "Pairing..."
};

static const char *audio_source_scan_str[] =
{
    "Scan",
    "Scanning..."
};

static const char *tx_management_pair_str[] =
{
    "Start Pairing",
    "Pairing..."
};

static const char *min_str[] =
{
    "min",
    "min",
    "Min.",
    "min",
    "min",
    "min",
    "min",
    "мин",
    "分钟",
    "分",
    "분",
    "นาที",
    "phút"
};

static char page_str[15] = {0};

static int16_t page_audio_source_list_offset_rec = 0; // page tx management also use
static int16_t page_quick_wake_up_list_offset_rec = 0;
static int16_t page_reorder_quick_access_list_offset_rec = 0;
static int16_t page_information_center_customize_list_offset_rec = 0;

/*============================================================================*
 *                           Private Functions
 *============================================================================*/
static void regenerate_current_view(void *msg)
{
    GUI_UNUSED(msg);

    gui_view_t *current_view = gui_view_get_current();
    const char *name = current_view->descriptor->name;
    gui_obj_t *parent = current_view->base.parent;
    gui_obj_tree_free(GUI_BASE(current_view));

    gui_view_create(parent, name, 0, 0, 0, 0);
}

static void toggle_move(void *p)
{
    gui_obj_t *obj = (gui_obj_t *)p;
    gui_obj_t *toggle_bg = obj->parent;
    int step = 2;
    gui_color_t color_origin;
    gui_color_t color_target;
    uint8_t control_alpha_origin = INVALID_OPACITY;
    uint8_t control_alpha_target = 0xFF;

    if (obj->x == toggle_target_x)
    {
        gui_obj_delete_timer(obj);
        if (obj->name == (void *)page_name_array[language_index][NOTIFICATION] ||
            obj->name == (void *)page_name_array[language_index][QUICK_WAKE_UP_SCREEN])
        {
            msg_2_regenerate_current_view();
        }
        else if (obj->name == (void *)page_name_array[language_index][AURACAST_BROADCAST])
        {
            //to do: Enable/Disable Auracast.
        }
        else if (obj->name == (void *)page_name_array[language_index][PASSWORD])
        {
            if (f_status.auracast)
            {
                gui_view_t *current_view = gui_view_get_current();
                gui_view_set_animate_step(current_view, 10);
                gui_view_switch_direct(current_view, AURACAST_VIEW,
                                       SWITCH_OUT_ANIMATION_MOVE_TO_LEFT,
                                       SWITCH_IN_ANIMATION_MOVE_FROM_RIGHT);
            }
            else
            {
                msg_2_regenerate_current_view();
            }
        }
        else if (obj->name == (void *)page_name_array[language_index][AUDIO_SOURCE])
        {
            if (audio_source.auracast_receiver)
            {
                gui_color_t font_color;
                gui_color_t bg_color;
                if (theme_bg_white)
                {
                    font_color = FG_1_LIGHT;
                    bg_color = BG_THEME1_MID_LIGHT;
                }
                else
                {
                    font_color = FG_1_DARK;
                    bg_color = BG_THEME1_MID_DARK;
                }
                audio_source.wait = true;
                // to do: scan auracast
                gui_obj_t *parent = obj->parent->parent;
                gui_img_t *bg = gui_img_create_from_mem(parent, 0, BUTTON_BG_ELLIPSE_240_80_BIN, 40,
                                                        toggle_bg->y + 52, 0, 0);
                gui_img_a8_recolor(bg, bg_color.color.argb_full);
                gui_obj_create_timer((void *)bg, 2000, true, scan_timer_page_audio_source);
                gui_obj_start_timer((void *)bg);
                gui_text_t *t = gui_text_create(bg, 0, 0, 0, 240, 80);
                gui_text_set(t, (void *)audio_source_scan_str[1], GUI_FONT_SRC_BMP, font_color,
                             strlen(audio_source_scan_str[1]), 32);
                gui_text_type_set(t, CAPTION_1_BIN, FONT_SRC_MEMADDR);
                gui_text_mode_set(t, MID_CENTER);
                ((gui_list_t *)parent->parent)->total_length += 116;
            }
            else
            {
                audio_source.auracast_num = 0;
                if (audio_source.connected_num)
                {
                    if (audio_source.connected_source_is_cast[0])
                    {
                        if (audio_source.connected_source_is_cast[1])
                        {
                            audio_source.connected_num = 0;
                            audio_source.streaming_index = 0;
                        }
                        else
                        {
                            sprintf(audio_source.connected_name[0], "%s", audio_source.connected_name[1]);
                            audio_source.connected_source_is_cast[0] = audio_source.connected_source_is_cast[1];
                            audio_source.connected_num--;
                            if (audio_source.streaming_index == 2)
                            {
                                audio_source.streaming_index = 1;
                            }
                        }
                    }
                    else if (audio_source.connected_source_is_cast[1])
                    {
                        audio_source.connected_num--;
                    }
                }
                msg_2_regenerate_current_view();
            }
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
        control_alpha_target = INVALID_OPACITY;
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
        if (control_bg->type != IMAGE) { return; }
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
        if (obj->name == (void *)page_name_array[language_index][AMBIENT_SOUND])
        {
            update_icon_color(obj->parent, ambient_sound_type);
        }
        else if (obj->name == (void *)page_name_array[language_index][DARK_LIGHT_MODE] ||
                 obj->name == (void *)page_name_array[language_index][TIME_FORMAT])
        {
            msg_2_regenerate_current_view();
        }
        else if (obj->name == (void *)page_name_array[language_index][SMART_TALK])
        {
            update_icon_color(obj->parent, smart_talk_time_type);
        }
        else if (obj->name == (void *)page_name_array[language_index][SPATIAL_SOUND])
        {
#if SS_WITH_HEAD_TRACKING
            update_icon_color(obj->parent, spatial_sound_status);
            gui_obj_t *toggle_bg = gui_list_entry(obj->parent->brother_list.prev, gui_obj_t, brother_list);
            gui_obj_t *toggle = gui_list_entry(toggle_bg->child_list.next, gui_obj_t, brother_list);
            uint8_t alpha = 0xFF;
            if (spatial_sound_status == SPATIAL_SOUND_OFF)
            {
                alpha = INVALID_OPACITY;
            }
            gui_img_a8_mix_alpha((void *)toggle_bg, alpha);
            gui_img_a8_mix_alpha((void *)toggle, alpha);
#else
            update_icon_color(obj->parent, spatial_sound_type);
#endif
        }
        else if (obj->name == (void *)page_name_array[language_index][QUICK_WAKE_UP_SCREEN])
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

static void update_strength_bar(gui_obj_t *parent, uint8_t vu_strength)
{
    gui_color_t bg_2_color;
    gui_color_t fg_theme_1_color;
    if (theme_bg_white)
    {
        bg_2_color = BG_2_LIGHT;
        fg_theme_1_color = FG_THEME1_LIGHT;
    }
    else
    {
        bg_2_color = BG_2_DARK;
        fg_theme_1_color = FG_THEME1_DARK;
    }

    gui_obj_t *obj = gui_list_entry(parent->child_list.next, gui_obj_t, brother_list);
    uint8_t i = 0;
    for (; i < vu_strength; i++)
    {
        if (i >= 10)
        {
            if (i == 12)
            {
                gui_img_a8_recolor((void *)obj, FG_WARNING.color.argb_full);
            }
            else
            {
                gui_img_a8_recolor((void *)obj, fg_theme_1_color.color.argb_full);
            }
        }
        else
        {
            gui_img_a8_recolor((void *)obj, FG_NORMAL.color.argb_full);
        }
        gui_img_a8_mix_alpha((void *)obj, ((gui_img_t *)obj)->fg_color_set >> 24);
        obj = gui_list_entry(obj->brother_list.next, gui_obj_t, brother_list);
    }
    for (; i < 13; i++)
    {
        gui_img_a8_recolor((void *)obj, bg_2_color.color.argb_full);
        gui_img_a8_mix_alpha((void *)obj, ((gui_img_t *)obj)->fg_color_set >> 24);
        obj = gui_list_entry(obj->brother_list.next, gui_obj_t, brother_list);
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
            if (volume_val != 16)
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
                compare = 16;
            }
            if (pressed_r)
            {
                volume_val++;
                gui_obj_t *progressbar = gui_list_entry(o->parent->child_list.prev, gui_obj_t, brother_list);
                gui_obj_hidden((void *)progressbar, false);
                gui_img_set_image_data((void *)progressbar, img_data_array[volume_val - 1]);
                gui_img_refresh_size((void *)progressbar);
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
                    gui_img_refresh_size((void *)progressbar);
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
            if (music_index != MUSIC_MAX_NUM - 1 && strlen(music_array[music_index + 1]) != 0)
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
                GUI_WIDGET_POINTER_BY_NAME_ROOT(text, (void *)page_name_array[language_index][PLAYBACK],
                                                o->parent->parent);
                gui_scroll_text_content_set((void *)text, (void *)music_array[music_index],
                                            strlen(music_array[music_index]));
            }
            else if (pressed_r)
            {
                music_index++;
                GUI_WIDGET_POINTER_BY_NAME_ROOT(text, (void *)page_name_array[language_index][PLAYBACK],
                                                o->parent->parent);
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
                if (music_index == MUSIC_MAX_NUM - 1 || strlen(music_array[music_index + 1]) == 0)
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

static void press_button_page_clock_settings(void *obj)
{
    GUI_UNUSED(obj);
    if (f_status.clock_settings == 0) { return; }
    gui_obj_t *o = GUI_BASE(obj);
    touch_info_t *tp = tp_get_info();
    static uint32_t time_rec = 0;
    static bool flag = 0;
    if (tp->pressed && gui_obj_point_in_obj_rect(o, tp->x, tp->y))
    {
        if (strcmp(o->name, "l") == 0)
        {
            pressed_l = true;
            gui_obj_hidden(o, false);
            time_rec = gui_ms_get();
        }
        else
        {
            pressed_r = true;
            gui_obj_hidden(o, false);
            time_rec = gui_ms_get();
        }
        return;
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
            uint8_t *val = (f_status.clock_settings == 1) ? &hour_val : &minutes_val;

            if (pressed_r)
            {
                *val += 5;
            }
            else if (pressed_l)
            {
                *val -= 5;
            }
            hour_val %= 24;
            minutes_val %= 60;
            gui_obj_t *min_bg = gui_list_entry(o->parent->child_list.prev, gui_obj_t, brother_list);
            if (f_status.clock_settings == 1)
            {
                gui_obj_t *hour_bg = gui_list_entry(min_bg->brother_list.prev, gui_obj_t, brother_list);
                gui_obj_t *text = gui_list_entry(hour_bg->child_list.prev, gui_obj_t, brother_list);
                sprintf(hour_str, "%02d", hour_val);
                gui_text_content_set((void *)text, hour_str, 2);
            }
            else
            {
                gui_obj_t *text = gui_list_entry(min_bg->child_list.prev, gui_obj_t, brother_list);
                sprintf(minutes_str, "%02d", minutes_val);
                gui_text_content_set((void *)text, minutes_str, 2);
            }
        }
    }
    else if (tp->released)
    {
        gui_obj_hidden(o, true);
        if (tp->deltaX == 0 && tp->deltaY == 0)
        {
            if (!flag)
            {
                uint8_t *val = (f_status.clock_settings == 1) ? &hour_val : &minutes_val;
                if (pressed_r)
                {
                    *val += 1;
                }
                else if (pressed_l)
                {
                    *val -= 1;
                }
            }
            else
            {
                flag = 0;
            }
            hour_val %= 24;
            minutes_val %= 60;
            gui_obj_t *min_bg = gui_list_entry(o->parent->child_list.prev, gui_obj_t, brother_list);
            if (f_status.clock_settings == 1)
            {
                gui_obj_t *hour_bg = gui_list_entry(min_bg->brother_list.prev, gui_obj_t, brother_list);
                gui_obj_t *text = gui_list_entry(hour_bg->child_list.prev, gui_obj_t, brother_list);
                sprintf(hour_str, "%02d", hour_val);
                gui_text_content_set((void *)text, hour_str, 2);
            }
            else
            {
                gui_obj_t *text = gui_list_entry(min_bg->child_list.prev, gui_obj_t, brother_list);
                sprintf(minutes_str, "%02d", minutes_val);
                gui_text_content_set((void *)text, minutes_str, 2);
            }
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
    gui_obj_t *o = GUI_BASE(obj);
    touch_info_t *tp = tp_get_info();
    if (tp->pressed && gui_obj_point_in_obj_rect(o, tp->x, tp->y))
    {
        if (strcmp(o->name, "l") == 0)
        {
            if (language_index != 0)
            {
                pressed_l = true;
                gui_obj_hidden(o, false);
            }
        }
        else
        {
            if (language_index != 12)
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
                language_index--;
                gui_text_content_set((void *)text, (void *)language_type[language_index],
                                     strlen(language_type[language_index]));
                msg_2_regenerate_current_view();
            }
            else if (pressed_r)
            {
                language_index++;
                gui_text_content_set((void *)text, (void *)language_type[language_index],
                                     strlen(language_type[language_index]));
                msg_2_regenerate_current_view();
            }

            if (language_index == compare)
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

static void click_button_page_ambient_sound(void *obj, gui_event_t *e)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);

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

static void click_button_page_playback(void *obj, gui_event_t *e)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);

    gui_obj_t *o = GUI_BASE(obj);
    f_status.playback = !f_status.playback;
    gui_scroll_text_t *lyrics = (void *)gui_list_entry(o->parent->brother_list.prev, gui_obj_t,
                                                       brother_list);
    gui_scroll_text_t *name = (void *)gui_list_entry(GUI_BASE(lyrics)->brother_list.prev, gui_obj_t,
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

static void click_button_page_smart_talk(void *obj, gui_event_t *e)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);

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
#if SS_WITH_HEAD_TRACKING
static void click_button_page_spatial_sound_with_head_tracking(void *obj, gui_event_t *e)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);

    gui_obj_t *o = GUI_BASE(obj);
    if (strcmp(o->name, "l") == 0)
    {
        spatial_sound_status = SPATIAL_SOUND_OFF;
        button_bg_target_x = BUTTON_X_LEFT;
    }
    else if (strcmp(o->name, "m") == 0)
    {
        spatial_sound_status = SPATIAL_SOUND_ON;
        button_bg_target_x = BUTTON_X_MID;
    }
    else if (strcmp(o->name, "r") == 0)
    {
        spatial_sound_status = SPATIAL_SOUND_HEAD_TRACKING;
        button_bg_target_x = BUTTON_X_RIGHT;
    }
    else
    {
        if (spatial_sound_status != SPATIAL_SOUND_OFF)
        {
            gui_view_t *current_view = gui_view_get_current();
            gui_view_set_animate_step(current_view, 10);
            gui_view_switch_direct(current_view, SPATIAL_SOUND_VIEW, SWITCH_OUT_ANIMATION_MOVE_TO_LEFT,
                                   SWITCH_IN_ANIMATION_MOVE_FROM_RIGHT);
        }
        return;
    }
    gui_obj_create_timer(gui_list_entry(o->parent->child_list.next, gui_obj_t, brother_list), 10, true,
                         button_move);
}
#else
static void click_button_page_spatial_sound_without_head_tracking(void *obj, gui_event_code_t e,
                                                                  void *param)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);

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
#endif

static void click_button_page_dark_light(void *obj, gui_event_t *e)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);

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

static void click_wallpaper_page_lock_screen(void *obj, gui_event_t *e)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);

    switch_from_lock_screen = true;
    gui_view_set_animate_step(gui_view_get_current(), 400);
    gui_view_switch_direct(gui_view_get_current(), LOCK_VIEW, SWITCH_OUT_NONE_ANIMATION,
                           SWITCH_IN_NONE_ANIMATION);
}

static void click_button_page_lock_screen(void *obj, gui_event_t *e)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);

    gui_obj_t *o = GUI_BASE(obj);
    gui_img_t *icon_l;
    gui_img_t *icon_r;
    bool is_switch_l = (strcmp(o->name, "l") == 0);
    if (is_switch_l)
    {
        wallpaper_index--;
        icon_l = (gui_img_t *)gui_list_entry(o->child_list.next, gui_obj_t, brother_list);
        icon_r = (gui_img_t *)gui_list_entry(gui_list_entry(o->brother_list.next, gui_obj_t,
                                                            brother_list)->child_list.next, gui_obj_t, brother_list);
    }
    else
    {
        wallpaper_index++;
        icon_l = (gui_img_t *)gui_list_entry(gui_list_entry(o->brother_list.prev, gui_obj_t,
                                                            brother_list)->child_list.next, gui_obj_t, brother_list);
        icon_r = (gui_img_t *)gui_list_entry(o->child_list.next, gui_obj_t, brother_list);
    }
    if (wallpaper_index <= 0)
    {
        wallpaper_index = 0;
        gui_img_a8_recolor(icon_l, theme_bg_white ? FG_2_LIGHT.color.argb_full : FG_2_DARK.color.argb_full);
    }
    else if (wallpaper_index >= 4)
    {
        wallpaper_index = 4;
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
    gui_img_set_image_data((gui_img_t *)img, img_data_array[wallpaper_index]);
}

static void click_button_page_quick_wake_up_screen(void *obj, gui_event_t *e)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);

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

static void click_button_page_clock_settings(void *obj, gui_event_t *e)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);

    if (f_status.clock_settings == 0) { return; }

    gui_obj_t *o = GUI_BASE(obj);
    gui_color_t fg_1_color;
    gui_color_t fg_2_color;
    gui_color_t bg_theme3_mid_color;
    if (theme_bg_white)
    {
        fg_1_color = FG_1_LIGHT;
        bg_theme3_mid_color = BG_THEME3_MID_LIGHT;
        fg_2_color = FG_2_LIGHT;
    }
    else
    {
        fg_1_color = FG_1_DARK;
        bg_theme3_mid_color = BG_THEME3_MID_DARK;
        fg_2_color = FG_2_DARK;
    }

    if (strcmp(o->name, "l") == 0)
    {
        if (f_status.clock_settings != 1)
        {
            f_status.clock_settings = 1;
            gui_obj_t *hour_bg = obj;
            gui_img_a8_recolor((void *)hour_bg, fg_1_color.color.argb_full);
            gui_obj_t *hour_bg_inner = gui_list_entry(hour_bg->child_list.next, gui_obj_t, brother_list);
            gui_obj_hidden(hour_bg_inner, false);

            gui_obj_t *min_bg = gui_list_entry(hour_bg->brother_list.next, gui_obj_t, brother_list);
            gui_img_a8_recolor((void *)min_bg, bg_theme3_mid_color.color.argb_full);
            gui_obj_t *min_bg_inner = gui_list_entry(min_bg->child_list.next, gui_obj_t, brother_list);
            gui_obj_hidden(min_bg_inner, true);
        }
    }
    else
    {
        if (f_status.clock_settings != 2)
        {
            f_status.clock_settings = 2;
            gui_obj_t *min_bg = obj;
            gui_img_a8_recolor((void *)min_bg, fg_1_color.color.argb_full);
            gui_obj_t *min_bg_inner = gui_list_entry(min_bg->child_list.next, gui_obj_t, brother_list);
            gui_obj_hidden(min_bg_inner, false);

            gui_obj_t *hour_bg = gui_list_entry(min_bg->brother_list.prev, gui_obj_t, brother_list);
            gui_img_a8_recolor((void *)hour_bg, bg_theme3_mid_color.color.argb_full);
            gui_obj_t *hour_bg_inner = gui_list_entry(hour_bg->child_list.next, gui_obj_t, brother_list);
            gui_obj_hidden(hour_bg_inner, true);
        }
    }
}

static void click_button_settings_page_clock_settings(void *obj, gui_event_t *e)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);


    gui_color_t fg_1_color;
    gui_color_t fg_2_color;
    gui_color_t bg_theme3_mid_color;
    if (theme_bg_white)
    {
        fg_1_color = FG_1_LIGHT;
        bg_theme3_mid_color = BG_THEME3_MID_LIGHT;
        fg_2_color = FG_2_LIGHT;
    }
    else
    {
        fg_1_color = FG_1_DARK;
        bg_theme3_mid_color = BG_THEME3_MID_DARK;
        fg_2_color = FG_2_DARK;
    }
    gui_obj_t *settings_bg = obj;
    gui_obj_t *settings = gui_list_entry(settings_bg->child_list.next, gui_obj_t, brother_list);
    gui_obj_t *control_bg = gui_list_entry(settings_bg->brother_list.next, gui_obj_t, brother_list);
    GUI_WIDGET_POINTER_BY_NAME_ROOT(add, "add", control_bg);
    GUI_WIDGET_POINTER_BY_NAME_ROOT(sub, "sub", control_bg);
    gui_obj_t *min_bg = gui_list_entry(control_bg->child_list.prev, gui_obj_t, brother_list);
    gui_obj_t *hour_bg = gui_list_entry(min_bg->brother_list.prev, gui_obj_t, brother_list);
    if (f_status.clock_settings)
    {
        if (f_status.clock_settings == 1)
        {
            gui_obj_t *hour_bg_inner = gui_list_entry(hour_bg->child_list.next, gui_obj_t, brother_list);
            gui_obj_hidden(hour_bg_inner, true);
            gui_img_a8_recolor((void *)hour_bg, bg_theme3_mid_color.color.argb_full);
        }
        else
        {
            gui_obj_t *min_bg_inner = gui_list_entry(min_bg->child_list.next, gui_obj_t, brother_list);
            gui_obj_hidden(min_bg_inner, true);
            gui_img_a8_recolor((void *)min_bg, bg_theme3_mid_color.color.argb_full);
        }
        f_status.clock_settings = 0;
        gui_img_set_image_data((void *)settings, ICON_SETTINGS_BIN);
        gui_img_a8_recolor((void *)add, fg_2_color.color.argb_full);
        gui_img_a8_recolor((void *)sub, fg_2_color.color.argb_full);

        timeinfo->tm_hour = hour_val;
        timeinfo->tm_min = minutes_val;
    }
    else
    {
        gui_obj_t *hour_bg_inner = gui_list_entry(hour_bg->child_list.next, gui_obj_t, brother_list);
        gui_obj_hidden(hour_bg_inner, false);
        gui_img_a8_recolor((void *)hour_bg, fg_1_color.color.argb_full);

        f_status.clock_settings = 1;
        gui_img_set_image_data((void *)settings, ICON_SELECTED_BIN);
        gui_img_a8_recolor((void *)add, fg_1_color.color.argb_full);
        gui_img_a8_recolor((void *)sub, fg_1_color.color.argb_full);
    }
    gui_img_a8_mix_alpha((void *)add, ((gui_img_t *)add)->fg_color_set >> 24);
    gui_img_a8_mix_alpha((void *)sub, ((gui_img_t *)sub)->fg_color_set >> 24);
}

static void click_button_page_time_format(void *obj, gui_event_t *e)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);

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

static void click_button_page_timer(void *obj, gui_event_t *e)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);

    f_status.timer = 0;
    gui_view_switch_direct(gui_view_get_current(), TIMER_VIEW, SWITCH_OUT_ANIMATION_MOVE_TO_LEFT,
                           SWITCH_IN_ANIMATION_MOVE_FROM_RIGHT);
}

static void click_button_page_flashlight(void *obj, gui_event_t *e)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);

    f_status.timer = 0;
    gui_view_set_animate_step(gui_view_get_current(), 400);
    gui_view_switch_direct(gui_view_get_current(), FLASHLIGHT_VIEW, SWITCH_OUT_NONE_ANIMATION,
                           SWITCH_IN_NONE_ANIMATION);
}

static void click_button_page_silentnow(void *obj, gui_event_t *e)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);

    f_status.silentnow = !f_status.silentnow;
    msg_2_regenerate_current_view();
}

static void click_button_page_case_button_customize(void *obj, gui_event_t *e)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);

    int16_t index = *((int16_t *)e->user_data);
    case_button_customize_type_index = index % 3;
    if (index < 3)
    {
        gui_obj_t *parent = GUI_BASE(obj)->parent->parent;
        gui_obj_t *bg_note = gui_list_entry(parent->child_list.next, gui_obj_t, brother_list);
        gui_obj_move(bg_note, 0, GUI_BASE(obj)->y + 60);
        gui_obj_hidden(bg_note, false);

        gui_view_switch_direct(gui_view_get_current(), BUTTON_CUSTOMIZE_VIEW,
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

static void click_button_page_reorder_quick_access(void *obj, gui_event_t *e)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);

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

static void click_button_page_information_center_customize(void *obj, gui_event_t *e)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);

    gui_obj_t *o = GUI_BASE(obj);
    int16_t index = *((int16_t *)e->user_data);
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
    msg_2_regenerate_current_view();
}

static void click_button_disconnect_page_audio_source(void *obj, gui_event_t *e)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);

    if (audio_source.wait) { return; }
    // to do : disconnect audio source clicked
    audio_source.wait = true;

    gui_obj_create_timer(obj, 50, true, disconnect_timer_page_audio_source);
    gui_obj_start_timer(obj);
}

static void click_button_sub_channel_connect_page_audio_source(void *obj, gui_event_t *e)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);

    if (audio_source.wait) { return; }
    if (audio_source.auracast_streaming_index == 1)
    {
        audio_source.auracast_streaming_index = 2;
    }
    else
    {
        audio_source.auracast_streaming_index = 1;
    }

    // to do : connect other sub channel clicked
    msg_2_regenerate_current_view();
}

static void click_button_pair_page_audio_source(void *obj, gui_event_t *e)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);

    if (audio_source.wait) { return; }
    // to do : pair device

    audio_source.wait = true;
    gui_obj_t *bg = GUI_BASE(obj);
    gui_obj_t *text = gui_list_entry(bg->child_list.next, gui_obj_t, brother_list);
    gui_text_content_set((void *)text, (void *)audio_source_pair_str[1],
                         strlen(audio_source_pair_str[1]));
    gui_img_a8_recolor((void *)bg,
                       theme_bg_white ? BG_THEME1_MID_LIGHT.color.argb_full : BG_THEME1_MID_DARK.color.argb_full);

    gui_obj_create_timer(obj, 2000, true, pair_timer_page_audio_source);
    gui_obj_start_timer(obj);
}

static void click_button_scan_page_audio_source(void *obj, gui_event_t *e)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);

    if (audio_source.wait) { return; }
    // to do : scan device

    audio_source.wait = true;
    gui_obj_t *bg = GUI_BASE(obj);
    gui_obj_t *text = gui_list_entry(bg->child_list.next, gui_obj_t, brother_list);
    text->w = 240;
    gui_text_content_set((void *)text, (void *)audio_source_scan_str[1],
                         strlen(audio_source_scan_str[1]));
    bg->x = 40;
    gui_img_a8_recolor((void *)bg,
                       theme_bg_white ? BG_THEME1_MID_LIGHT.color.argb_full : BG_THEME1_MID_DARK.color.argb_full);
    gui_img_set_image_data((void *)bg, BUTTON_BG_ELLIPSE_240_80_BIN);
    gui_img_refresh_size((void *)bg);

    gui_obj_create_timer(obj, 2000, true, scan_timer_page_audio_source);
    gui_obj_start_timer(obj);
}

static void click_button_connect_page_audio_source(void *obj, gui_event_t *e)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);

    if (audio_source.wait) { return; }
    // to do : connect device

    audio_source.wait = true;
    gui_obj_t *win = gui_list_entry(GUI_BASE(obj)->child_list.prev, gui_obj_t, brother_list);
    gui_obj_t *icon = gui_list_entry(win->child_list.next, gui_obj_t, brother_list);
    gui_img_set_image_data((void *)icon, ICON_CONNECT_BIN);
    gui_img_a8_mix_alpha((void *)icon, 75);
    gui_obj_create_timer(obj, 50, true, connect_timer_page_audio_source);
    gui_obj_start_timer(obj);
}

static void click_button_connect_auracast_page_audio_source(void *obj, gui_event_t *e)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);

    if (audio_source.wait) { return; }
    // to do : connect auracast device

    audio_source.wait = true;
    gui_obj_create_timer(obj, 2000, true, connect_auracast_timer_page_audio_source);
    gui_obj_start_timer(obj);
}

static void click_button_delete_page_audio_source(void *obj, gui_event_t *e)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);

    if (audio_source.wait) { return; }
    // to do : delete paired device

    audio_source.wait = true;
    gui_obj_create_timer(obj, 50, true, delete_timer_page_audio_source);
    gui_obj_start_timer(obj);
}

static void click_button_pair_page_tx_management(void *obj, gui_event_t *e)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);


    gui_obj_t *bg = GUI_BASE(obj);
    gui_obj_t *text = gui_list_entry(bg->child_list.next, gui_obj_t, brother_list);
    if (tx_management.pairing)
    {
        tx_management.pairing = false;
        gui_obj_delete_timer(obj);
        // to do : stop pair device
        gui_text_content_set((void *)text, (void *)tx_management_pair_str[0],
                             strlen(tx_management_pair_str[0]));
        gui_img_a8_recolor((void *)bg,
                           theme_bg_white ? BG_THEME3_BRIGHT_LIGHT.color.argb_full : BG_THEME3_BRIGHT_DARK.color.argb_full);
    }
    else
    {
        tx_management.pairing = true;
        // to do : pair device
        gui_text_content_set((void *)text, (void *)tx_management_pair_str[1],
                             strlen(tx_management_pair_str[1]));
        gui_img_a8_recolor((void *)bg,
                           theme_bg_white ? BG_THEME3_DARK_LIGHT.color.argb_full : BG_THEME3_DARK_DARK.color.argb_full);

        gui_obj_create_timer(obj, 2000, true, pair_timer_page_tx_management);
        gui_obj_start_timer(obj);
    }
}

static void click_button_connect_page_tx_management(void *obj, gui_event_t *e)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);

    if (tx_management.wait) { return; }
    // to do : connect device

    tx_management.wait = true;
    gui_obj_t *win = gui_list_entry(GUI_BASE(obj)->child_list.prev, gui_obj_t, brother_list);
    gui_obj_t *icon = gui_list_entry(win->child_list.next, gui_obj_t, brother_list);
    gui_img_set_image_data((void *)icon, ICON_CONNECT_BIN);
    gui_img_a8_mix_alpha((void *)icon, 75);
    gui_obj_create_timer(obj, 50, true, connect_timer_page_tx_management);
    gui_obj_start_timer(obj);
}

static void click_button_delete_page_tx_management(void *obj, gui_event_t *e)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);

    if (tx_management.wait) { return; }
    // to do : delete paired device

    tx_management.wait = true;
    gui_obj_create_timer(obj, 50, true, delete_timer_page_tx_management);
    gui_obj_start_timer(obj);
}

static void click_button_disconnect_page_tx_management(void *obj, gui_event_t *e)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);

    if (tx_management.wait) { return; }
    // to do : disconnect headphone
    tx_management.wait = true;

    gui_obj_create_timer(obj, 50, true, disconnect_timer_page_tx_management);
    gui_obj_start_timer(obj);
}

static void click_button_discovered_page_tx_management(void *obj, gui_event_t *e)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);

    if (tx_management.wait) { return; }
    // to do : connect auracast device

    tx_management.wait = true;
    gui_obj_create_timer(obj, 2000, true, connect_discovered_timer_page_tx_management);
    gui_obj_start_timer(obj);
}

static void click_toggle_cb(void *obj, gui_event_t *e)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);

    gui_obj_t *o = GUI_BASE(obj);
    bool *status = e->user_data;
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

static void click_button_page_support(void *obj, gui_event_t *e)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);

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
    gui_view_switch_direct(gui_view_get_current(), SUPPORT_VIEW,
                           SWITCH_OUT_ANIMATION_MOVE_TO_LEFT,
                           SWITCH_IN_ANIMATION_MOVE_FROM_RIGHT);
}

static void click_button_no_action(void *obj, gui_event_t *e)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);

}

static void timer_page_silentnow(void *obj)
{
    if (silent_time_val == 0)
    {
        gui_obj_t *parent = GUI_BASE(obj)->parent;
        gui_img_t *icon = (gui_img_t *)gui_list_entry(parent->child_list.next, gui_obj_t, brother_list);
        if (icon->data == ICON_SILENT_BIN)
        {
            // end of silent
            f_status.silentnow = 0;
            msg_2_regenerate_current_view();
        }
        else
        {
            gui_img_set_image_data(icon, ICON_SILENT_BIN);
            gui_img_refresh_size(icon);
            silent_time_val = 900;
            sprintf(page_str, "00: 15 :00");
            gui_text_content_set((void *)obj, page_str, 10);

            gui_text_t *t = (gui_text_t *)gui_list_entry(icon->base.brother_list.next, gui_obj_t, brother_list);
            gui_text_content_set(t, "Duration", 8);
        }
    }
    else
    {
        silent_time_val--;
        sprintf(page_str, "00: %02d :%02d", silent_time_val / 60, silent_time_val % 60);
        gui_text_content_set((void *)obj, page_str, 10);
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

static void win_timer_page_volume_unit_meter(void *obj)
{
    gui_obj_t *win_l = obj;
    gui_obj_t *win_r = gui_list_entry(win_l->brother_list.next, gui_obj_t, brother_list);

    vu_strength_l *= 3; // test
    vu_strength_l %= 14;
    vu_strength_r += 1;
    vu_strength_r %= 14;

    update_strength_bar(win_l, vu_strength_l);
    update_strength_bar(win_r, vu_strength_r);
}

static void disconnect_timer_page_audio_source(void *obj)
{
    gui_obj_t *icon = gui_list_entry(GUI_BASE(obj)->child_list.prev, gui_obj_t, brother_list);
    uint8_t alpha = ((gui_img_t *)icon)->opacity_value;
    alpha = alpha < 75 ? 255 : alpha - 10;
    gui_img_a8_mix_alpha((void *)icon, alpha);

    if (alpha != 255) { return; }
    audio_source.wait = false;//test
    if (audio_source.wait) { return; }
    uint8_t index = 0;
    uint8_t i = 0;
    gui_text_t *text = (gui_text_t *)gui_list_entry(GUI_BASE(obj)->child_list.next, gui_obj_t,
                                                    brother_list);

    if (text->content != audio_source.connected_name[0])
    {
        index = 1;
    }
    if (audio_source.connected_source_is_cast[index])
    {
        if (audio_source.auracast_num == 2)
        {
            i = 1;
        }
        else
        {
            i = audio_source.auracast_num;
            audio_source.auracast_num++;
        }
        while (i > 0)
        {
            sprintf(audio_source.auracast_name[i], "%s", audio_source.auracast_name[i - 1]);
            audio_source.connected_auracast_hq[i] = audio_source.connected_auracast_hq[i - 1];
            i--;
        }
        sprintf(audio_source.auracast_name[0], "%s", audio_source.connected_name[index]);
        audio_source.auracast_hq[0] = audio_source.connected_auracast_hq[index];
    }
    else
    {
        if (audio_source.paired_num == 4)
        {
            i = 3;
        }
        else
        {
            i = audio_source.paired_num;
            audio_source.paired_num++;
        }
        while (i > 0)
        {
            sprintf(audio_source.paired_name[i], "%s", audio_source.paired_name[i - 1]);
            i--;
        }
        sprintf(audio_source.paired_name[0], "%s", audio_source.connected_name[index]);
    }

    if (audio_source.streaming_index == index + 1)
    {
        audio_source.streaming_index = 0;
    }
    else if (audio_source.streaming_index == 2)
    {
        audio_source.streaming_index = 1;
    }
    if (index == 0 && audio_source.connected_num == 2)
    {
        sprintf(audio_source.connected_name[0], "%s", audio_source.connected_name[1]);
        audio_source.connected_source_is_cast[0] = audio_source.connected_source_is_cast[1];
    }
    audio_source.connected_num--;
    gui_obj_stop_timer(obj);
    msg_2_regenerate_current_view();
}

static void connect_timer_page_audio_source(void *obj)
{
    gui_obj_t *win = gui_list_entry(GUI_BASE(obj)->child_list.prev, gui_obj_t, brother_list);
    gui_obj_t *icon = gui_list_entry(win->child_list.next, gui_obj_t, brother_list);
    uint8_t alpha = ((gui_img_t *)icon)->opacity_value;
    alpha = alpha >= 255 ? 75 : alpha + 10;
    gui_img_a8_mix_alpha((void *)icon, alpha);

    if (alpha != 255) { return; }
    audio_source.wait = false;//test

    if (audio_source.wait) { return; }
    uint8_t index = 0;
    gui_text_t *text = (gui_text_t *)gui_list_entry(GUI_BASE(obj)->child_list.next, gui_obj_t,
                                                    brother_list);

    for (uint8_t i = 0; i < audio_source.paired_num; i++)
    {
        if (text->content == audio_source.paired_name[i])
        {
            index = i;
            break;
        }
    }

    char temp[30];
    sprintf(temp, "%s", audio_source.paired_name[index]);
    if (audio_source.connected_num == 2)
    {
        for (uint8_t i = index; i > 0; i--)
        {
            sprintf(audio_source.paired_name[i], "%s", audio_source.paired_name[i - 1]);
        }
        sprintf(audio_source.paired_name[0], "%s", audio_source.connected_name[1]);
    }
    else
    {
        for (uint8_t i = index; i < audio_source.paired_num - 1; i++)
        {
            sprintf(audio_source.paired_name[i], "%s", audio_source.paired_name[i + 1]);
        }
        audio_source.paired_num--;
        audio_source.connected_num++;
    }
    sprintf(audio_source.connected_name[1], "%s", audio_source.connected_name[0]);
    sprintf(audio_source.connected_name[0], "%s", temp);
    audio_source.connected_source_is_cast[1] = audio_source.connected_source_is_cast[0];
    audio_source.connected_source_is_cast[0] = false;

    gui_obj_stop_timer(obj);
    msg_2_regenerate_current_view();
}

static void connect_auracast_timer_page_audio_source(void *obj)
{
    audio_source.wait = false; //test
    if (audio_source.wait) { return; }
    uint8_t index = 0;
    gui_text_t *text = (gui_text_t *)gui_list_entry(GUI_BASE(obj)->child_list.next, gui_obj_t,
                                                    brother_list);

    for (uint8_t i = 0; i < audio_source.auracast_num; i++)
    {
        if (text->content == audio_source.auracast_name[i])
        {
            index = i;
            break;
        }
    }

    char temp[30];
    sprintf(temp, "%s", audio_source.auracast_name[index]);
    audio_source.connected_auracast_hq[0] = audio_source.connected_auracast_hq[index];
    if (audio_source.connected_num == 2)
    {
        if (audio_source.connected_source_is_cast[1])
        {
            for (uint8_t i = index; i > 0; i--)
            {
                sprintf(audio_source.auracast_name[i], "%s", audio_source.auracast_name[i - 1]);
            }
            sprintf(audio_source.auracast_name[0], "%s", audio_source.connected_name[1]);
            audio_source.auracast_hq[0] = audio_source.connected_auracast_hq[1];
        }
        else
        {
            for (uint8_t i = index; i > 0; i--)
            {
                sprintf(audio_source.paired_name[i], "%s", audio_source.paired_name[i - 1]);
            }
            sprintf(audio_source.paired_name[0], "%s", audio_source.connected_name[1]);
        }
    }
    else
    {
        for (uint8_t i = index; i < audio_source.auracast_num - 1; i++)
        {
            sprintf(audio_source.auracast_name[i], "%s", audio_source.auracast_name[i + 1]);
            audio_source.auracast_hq[i] = audio_source.auracast_hq[i + 1];
        }
        audio_source.auracast_num--;
        audio_source.connected_num++;
    }
    sprintf(audio_source.connected_name[1], "%s", audio_source.connected_name[0]);
    sprintf(audio_source.connected_name[0], "%s", temp);
    audio_source.connected_source_is_cast[1] = audio_source.connected_source_is_cast[0];
    audio_source.connected_source_is_cast[0] = true;
    audio_source.streaming_index = 1;

    gui_obj_stop_timer(obj);
    msg_2_regenerate_current_view();
}

static void pair_timer_page_audio_source(void *obj)
{
    audio_source.wait = false; //test
    if (audio_source.wait) { return; }

    if (audio_source.connected_num == 2)
    {
        uint8_t i = audio_source.paired_num == 4 ? 3 : audio_source.paired_num;
        while (i > 0)
        {
            sprintf(audio_source.paired_name[i], "%s", audio_source.paired_name[i - 1]);
            i--;
        }
        sprintf(audio_source.paired_name[0], "%s", audio_source.connected_name[1]);
        if (audio_source.paired_num < 4) { audio_source.paired_num++; }
    }
    else
    {
        audio_source.connected_num++;
    }
    sprintf(audio_source.connected_name[1], "%s", audio_source.connected_name[0]); //test
    sprintf(audio_source.connected_name[0], "%s", "device new");
    audio_source.connected_source_is_cast[1] = audio_source.connected_source_is_cast[0];
    audio_source.connected_source_is_cast[0] = false;
    audio_source.streaming_index = 1;

    gui_obj_stop_timer(obj);
    page_audio_source_list_offset_rec = 0;
    msg_2_regenerate_current_view();
}

static void scan_timer_page_audio_source(void *obj)
{
    audio_source.wait = false; //test
    if (audio_source.wait) { return; }

    sprintf(audio_source.auracast_name[0], "%s", "Broadcast 1"); //test
    sprintf(audio_source.auracast_name[1], "%s", "Broadcast 2");
    sprintf(audio_source.auracast_sub_channel_name[0], "%s", "Sub Channel 1"); //test
    sprintf(audio_source.auracast_sub_channel_name[1], "%s", "Sub Channel 2");
    audio_source.auracast_num = 2;
    audio_source.auracast_sub_channel_num = 2;
    audio_source.auracast_streaming_index = 1;
    audio_source.auracast_hq[0] = true;
    audio_source.auracast_hq[1] = false;

    gui_obj_stop_timer(obj);
    msg_2_regenerate_current_view();
}

static void delete_timer_page_audio_source(void *obj)
{
    gui_obj_t *icon = gui_list_entry(GUI_BASE(obj)->child_list.prev, gui_obj_t, brother_list);
    uint8_t alpha = ((gui_img_t *)icon)->opacity_value;
    alpha = alpha <= 75 ? 255 : alpha - 10;
    gui_img_a8_mix_alpha((void *)icon, alpha);

    if (alpha != 255) { return; } //test
    audio_source.wait = false;

    if (audio_source.wait) { return; }
    uint8_t index = 0;
    gui_text_t *text = (gui_text_t *)gui_list_entry(GUI_BASE(obj)->child_list.next, gui_obj_t,
                                                    brother_list);

    for (uint8_t i = 0; i < audio_source.paired_num; i++)
    {
        if (text->content == audio_source.connected_name[i])
        {
            index = i;
            break;
        }
    }

    for (uint8_t i = index; i < audio_source.paired_num - 1; i++)
    {
        sprintf(audio_source.paired_name[i], "%s", audio_source.paired_name[i + 1]);
    }
    audio_source.paired_num--;

    gui_obj_stop_timer(obj);
    msg_2_regenerate_current_view();
}

static void delete_timer_page_tx_management(void *obj)
{
    gui_obj_t *icon = gui_list_entry(GUI_BASE(obj)->child_list.prev, gui_obj_t, brother_list);
    uint8_t alpha = ((gui_img_t *)icon)->opacity_value;
    alpha = alpha <= 75 ? 255 : alpha - 10;
    gui_img_a8_mix_alpha((void *)icon, alpha);

    if (alpha != 255) { return; } //test
    tx_management.wait = false;

    if (tx_management.wait) { return; }
    uint8_t index = 0;
    gui_text_t *text = (gui_text_t *)gui_list_entry(GUI_BASE(obj)->child_list.next, gui_obj_t,
                                                    brother_list);

    for (uint8_t i = 0; i < tx_management.paired_num; i++)
    {
        if (text->content == tx_management.connected_name)
        {
            index = i;
            break;
        }
    }

    for (uint8_t i = index; i < tx_management.paired_num - 1; i++)
    {
        sprintf(tx_management.paired_name[i], "%s", tx_management.paired_name[i + 1]);
    }
    tx_management.paired_num--;

    gui_obj_stop_timer(obj);
    msg_2_regenerate_current_view();
}

static void disconnect_timer_page_tx_management(void *obj)
{
    gui_obj_t *icon = gui_list_entry(GUI_BASE(obj)->child_list.prev, gui_obj_t, brother_list);
    uint8_t alpha = ((gui_img_t *)icon)->opacity_value;
    alpha = alpha < 75 ? 255 : alpha - 10;
    gui_img_a8_mix_alpha((void *)icon, alpha);

    if (alpha != 255) { return; }
    tx_management.wait = false;//test
    if (tx_management.wait) { return; }
    uint8_t i = 0;
    if (tx_management.paired_num == 4)
    {
        i = 3;
    }
    else
    {
        i = tx_management.paired_num;
        tx_management.paired_num++;
    }
    while (i > 0)
    {
        sprintf(tx_management.paired_name[i], "%s", tx_management.paired_name[i - 1]);
        i--;
    }
    sprintf(tx_management.paired_name[0], "%s", tx_management.connected_name);

    tx_management.connected = false;
    gui_obj_stop_timer(obj);
    msg_2_regenerate_current_view();
}

static void connect_timer_page_tx_management(void *obj)
{
    gui_obj_t *win = gui_list_entry(GUI_BASE(obj)->child_list.prev, gui_obj_t, brother_list);
    gui_obj_t *icon = gui_list_entry(win->child_list.next, gui_obj_t, brother_list);
    uint8_t alpha = ((gui_img_t *)icon)->opacity_value;
    alpha = alpha >= 255 ? 75 : alpha + 10;
    gui_img_a8_mix_alpha((void *)icon, alpha);

    if (alpha != 255) { return; }
    tx_management.wait = false;//test

    if (tx_management.wait) { return; }
    uint8_t index = 0;
    gui_text_t *text = (gui_text_t *)gui_list_entry(GUI_BASE(obj)->child_list.next, gui_obj_t,
                                                    brother_list);

    for (uint8_t i = 0; i < tx_management.paired_num; i++)
    {
        if (text->content == tx_management.paired_name[i])
        {
            index = i;
            break;
        }
    }

    char temp[30];
    sprintf(temp, "%s", tx_management.paired_name[index]);
    if (tx_management.connected)
    {
        for (uint8_t i = index; i > 0; i--)
        {
            sprintf(tx_management.paired_name[i], "%s", tx_management.paired_name[i - 1]);
        }
        sprintf(tx_management.paired_name[0], "%s", tx_management.connected_name);
    }
    else
    {
        for (uint8_t i = index; i < tx_management.paired_num - 1; i++)
        {
            sprintf(tx_management.paired_name[i], "%s", tx_management.paired_name[i + 1]);
        }
        tx_management.paired_num--;
        tx_management.connected = true;
    }
    sprintf(tx_management.connected_name, "%s", temp);

    gui_obj_stop_timer(obj);
    msg_2_regenerate_current_view();
}

static void connect_discovered_timer_page_tx_management(void *obj)
{
    tx_management.wait = false; //test
    if (tx_management.wait) { return; }
    uint8_t index = 0;
    gui_text_t *text = (gui_text_t *)gui_list_entry(GUI_BASE(obj)->child_list.next, gui_obj_t,
                                                    brother_list);

    for (uint8_t i = 0; i < tx_management.discovered_num; i++)
    {
        if (text->content == tx_management.discovered_name[i])
        {
            index = i;
            break;
        }
    }

    char temp[30];
    sprintf(temp, "%s", tx_management.discovered_name[index]);
    if (tx_management.connected)
    {
        uint8_t i = 0;
        if (tx_management.paired_num == 4)
        {
            i = 3;
        }
        else
        {
            i = tx_management.paired_num;
            tx_management.paired_num++;
        }
        for (; i > 0; i--)
        {
            sprintf(tx_management.paired_name[i], "%s", tx_management.paired_name[i - 1]);
        }
        sprintf(tx_management.paired_name[0], "%s", tx_management.connected_name);
    }
    for (uint8_t i = index; i < tx_management.discovered_num - 1; i++)
    {
        sprintf(tx_management.discovered_name[i], "%s", tx_management.discovered_name[i + 1]);
    }
    tx_management.discovered_num--;
    tx_management.connected = true;
    sprintf(tx_management.connected_name, "%s", temp);

    gui_obj_stop_timer(obj);
    msg_2_regenerate_current_view();
}

static void pair_timer_page_tx_management(void *obj)
{
    GUI_UNUSED(obj);
    if (tx_management.discovered_num == 0)//test
    {
        tx_management.discovered_num = 3;
        sprintf(tx_management.discovered_name[0], "headphones 5");
        sprintf(tx_management.discovered_name[1], "headphones 6");
        sprintf(tx_management.discovered_name[2], "headphones 7");
        msg_2_regenerate_current_view();
    }
}

static void list_timer_page_audio_source(void *obj)
{
    gui_list_t *list = (gui_list_t *)obj;
    int16_t list_offset = list->offset;
    page_audio_source_list_offset_rec = list_offset;
    gui_obj_t *scrollbar = gui_list_entry(GUI_BASE(obj)->parent->child_list.prev, gui_obj_t,
                                          brother_list);

    int range = list->base.h - list->total_length;
    int t_y = list_offset * (SCROLL_BAR_BG_H - SCROLL_BAR_S_H) / range;
    gui_img_translate((void *)scrollbar, 0, t_y);
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
        msg_2_regenerate_current_view();
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
        msg_2_regenerate_current_view();
    }
}

static void note_design_page_audio_source(gui_obj_t *obj, void *p)
{
    GUI_UNUSED(p);
    gui_color_t font_color;
    gui_color_t font_color_warning;
    gui_color_t icon_color;
    gui_color_t bg_color;
    gui_color_t screen_color;
    gui_color_t toggle_bg_color;
    if (theme_bg_white)
    {
        font_color = FG_1_LIGHT;
        font_color_warning.color.argb_full = GUI_COLOR_ARGB8888(0xFF, 0x93, 0x93, 0x93);
        icon_color = BG_THEME1_BRIGHT_LIGHT;
        bg_color = BG_1_LIGHT;
        toggle_bg_color = BG_2_LIGHT;
        screen_color = SCREEN_BG_LIGHT;
    }
    else
    {
        font_color = FG_1_DARK;
        font_color_warning.color.argb_full = GUI_COLOR_ARGB8888(0xFF, 122, 122, 122);
        icon_color = BG_THEME1_BRIGHT_DARK;
        bg_color = BG_1_DARK;
        toggle_bg_color = BG_2_DARK;
        screen_color = SCREEN_BG_DARK;
    }
    int16_t y = 0;
    int16_t font_size = 20;
    {
        gui_text_t *text = gui_text_create(obj, 0, 16, y, 200, 40);
        gui_text_set(text, "Connected", GUI_FONT_SRC_BMP, font_color, 10, font_size);
        gui_text_type_set(text, HEADING_1_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(text, MID_LEFT);
    }

    y += 44;
    for (uint8_t i = 0; i < audio_source.connected_num; i++)
    {
        void *bg_data[2] = {BUTTON_BG_ELLIPSE_312_56_TOP_BIN, BUTTON_BG_ELLIPSE_312_56_BOT_BIN};
        if (audio_source.connected_num == 1)
        {
            bg_data[0] = BUTTON_BG_ELLIPSE_312_56_BIN;
        }
        gui_img_t *bg = gui_img_create_from_mem(obj, audio_source.connected_name[i], bg_data[i], 4, y, 0,
                                                0);
        gui_img_a8_recolor(bg, bg_color.color.argb_full);
        gui_img_a8_fix_bg(bg, screen_color.color.argb_full);
        gui_img_a8_mix_alpha(bg, bg->fg_color_set >> 24);
        gui_obj_add_event_cb(bg, click_button_disconnect_page_audio_source, GUI_EVENT_TOUCH_CLICKED, NULL);

        uint16_t length = strlen(audio_source.connected_name[i]);
        gui_text_t *t = gui_text_create(bg, 0, 12, 0, 244, 56);
        gui_text_set(t, audio_source.connected_name[i], GUI_FONT_SRC_BMP,
                     audio_source.streaming_index == (i + 1) ? icon_color : font_color, length, font_size);
        gui_text_type_set(t, HEADING_1_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(t, MID_LEFT);

        y += 56;

        if (audio_source.connected_source_is_cast[i])
        {
            gui_img_t *icon = gui_img_create_from_mem(bg, 0, ICON_AURACAST_SMALL_BIN, 12, 16, 0, 0);
            gui_img_a8_recolor(icon, JBL_ORANGE.color.argb_full);
            if (audio_source.auracast_hq[i])
            {
                gui_img_t *icon = gui_img_create_from_mem(bg, 0, ICON_HQ_BIN, 44, 14, 0, 0);
                gui_img_a8_recolor(icon, JBL_ORANGE.color.argb_full);
                t->base.x = 80;
                t->base.w = 176;
            }
            else
            {
                t->base.x = 44;
                t->base.w = 212;
            }
            if (audio_source.streaming_index == i + 1 && audio_source.auracast_sub_channel_num != 0)
            {
                void *bg_data_new[2];
                if (bg_data[0] == BUTTON_BG_ELLIPSE_312_56_BIN)
                {
                    gui_img_set_image_data(bg, BUTTON_BG_ELLIPSE_312_56_TOP_BIN);
                    bg_data_new[0] = BUTTON_BG_ELLIPSE_312_56_MID_BIN;
                    bg_data_new[1] = BUTTON_BG_ELLIPSE_312_56_BOT_BIN;
                }
                else
                {
                    bg_data_new[0] = BUTTON_BG_ELLIPSE_312_56_MID_BIN;
                    bg_data_new[1] = BUTTON_BG_ELLIPSE_312_56_MID_BIN;
                }
                for (uint8_t j = 0; j < audio_source.auracast_sub_channel_num; j++)
                {
                    gui_img_t *bg = gui_img_create_from_mem(obj, audio_source.auracast_sub_channel_name[j],
                                                            bg_data_new[j], 4, y, 0, 0);
                    gui_img_a8_recolor(bg, bg_color.color.argb_full);
                    gui_img_a8_fix_bg(bg, screen_color.color.argb_full);
                    gui_img_a8_mix_alpha(bg, bg->fg_color_set >> 24);

                    uint16_t length = strlen(audio_source.auracast_sub_channel_name[j]);
                    gui_text_t *t = gui_text_create(bg, 0, 48, 0, 244, 56);
                    gui_text_set(t, audio_source.auracast_sub_channel_name[j], GUI_FONT_SRC_BMP, icon_color, length,
                                 16);
                    gui_text_type_set(t, HEADING_2_BIN, FONT_SRC_MEMADDR);
                    gui_text_mode_set(t, MID_LEFT);

                    if (audio_source.auracast_streaming_index == j + 1)
                    {
                        gui_img_t *icon = gui_img_create_from_mem(bg, 0, ICON_SELECTED_BIN, 12, 12, 0, 0);
                        gui_img_a8_recolor(icon, icon_color.color.argb_full);
                    }
                    else
                    {
                        gui_obj_add_event_cb(bg, click_button_sub_channel_connect_page_audio_source,
                                             GUI_EVENT_TOUCH_CLICKED, NULL);
                    }
                    y += 56;
                }
            }
        }
        gui_img_t *icon = gui_img_create_from_mem(bg, 0, ICON_DISCONNECT_BIN, 268, 14, 0, 0);
        gui_img_a8_recolor(icon, font_color.color.argb_full);
    }

    y += 12;
    {
        gui_text_t *text = gui_text_create(obj, 0, 12, y, 288, 40);
        gui_text_set(text, (void *)audio_source_warning_str[0], GUI_FONT_SRC_BMP, font_color_warning,
                     strlen(audio_source_warning_str[0]), 16);
        gui_text_type_set(text, HEADING_2_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(text, MULTI_LEFT);
    }

    y += 55;
    {
        gui_img_t *bg = gui_img_create_from_mem(obj, 0, BUTTON_BG_ELLIPSE_240_80_BIN, 40, y, 0, 0);
        gui_img_a8_recolor(bg, icon_color.color.argb_full);
        gui_img_a8_fix_bg(bg, screen_color.color.argb_full);
        gui_obj_add_event_cb(bg, click_button_pair_page_audio_source, GUI_EVENT_TOUCH_CLICKED, NULL);
        gui_text_t *t = gui_text_create(bg, 0, 0, 0, 240, 80);
        gui_text_set(t, (void *)audio_source_pair_str[0], GUI_FONT_SRC_BMP, font_color,
                     strlen(audio_source_pair_str[0]), 32);
        gui_text_type_set(t, CAPTION_1_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(t, MID_CENTER);
    }

    y += 92;
    {
        gui_text_t *text = gui_text_create(obj, 0, 16, y, 200, 40);
        gui_text_set(text, "Paired", GUI_FONT_SRC_BMP, font_color, 6, font_size);
        gui_text_type_set(text, HEADING_1_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(text, MID_LEFT);
    }

    y += 44;
    for (uint8_t i = 0; i < audio_source.paired_num; i++)
    {
        void *bg_data[4] =
        {
            BUTTON_BG_ELLIPSE_312_56_TOP_BIN,
            BUTTON_BG_ELLIPSE_312_56_MID_BIN,
            BUTTON_BG_ELLIPSE_312_56_MID_BIN,
            BUTTON_BG_ELLIPSE_312_56_BOT_BIN
        };
        if (audio_source.paired_num == 1)
        {
            bg_data[0] = BUTTON_BG_ELLIPSE_312_56_BIN;
        }
        else if (audio_source.paired_num == 2)
        {
            bg_data[1] = BUTTON_BG_ELLIPSE_312_56_BOT_BIN;
        }
        else if (audio_source.paired_num == 3)
        {
            bg_data[2] = BUTTON_BG_ELLIPSE_312_56_BOT_BIN;
        }

        gui_img_t *bg = gui_img_create_from_mem(obj, audio_source.paired_name[i], bg_data[i], 4, y, 0, 0);
        gui_img_a8_recolor(bg, bg_color.color.argb_full);
        gui_img_a8_fix_bg(bg, screen_color.color.argb_full);
        gui_img_a8_mix_alpha(bg, bg->fg_color_set >> 24);
        gui_obj_add_event_cb(bg, click_button_connect_page_audio_source, GUI_EVENT_TOUCH_CLICKED, NULL);

        gui_text_t *t = gui_text_create(bg, 0, 12, 0, 256, 56);
        gui_text_set(t, audio_source.paired_name[i], GUI_FONT_SRC_BMP, font_color,
                     strlen(audio_source.paired_name[i]), font_size);
        gui_text_type_set(t, HEADING_1_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(t, MID_LEFT);
        gui_win_t *win = gui_win_create(bg, 0, 268, 0, 56, 56);
        gui_obj_add_event_cb(win, click_button_delete_page_audio_source, GUI_EVENT_TOUCH_CLICKED, NULL);
        gui_img_t *icon = gui_img_create_from_mem(win, 0, ICON_DELETE_BIN, 0, 14, 0, 0);
        gui_img_a8_recolor(icon, font_color.color.argb_full);

        y += 56;
    }

    y += 24;
    gui_img_t *toggle_bg = gui_img_create_from_mem(obj, 0, TOGGLE_BG_BIN, 244, y, 0, 0);
    gui_img_t *toggle = gui_img_create_from_mem(toggle_bg,
                                                (void *)page_name_array[language_index][AUDIO_SOURCE],
                                                TOGGLE_BIN, TOGGLE_OFF_X,
                                                TOGGLE_Y, 0, 0);
    gui_img_a8_recolor(toggle_bg, toggle_bg_color.color.argb_full);
    gui_img_a8_recolor(toggle, FG_1_DARK.color.argb_full);
    gui_obj_add_event_cb(toggle_bg, click_toggle_cb, GUI_EVENT_TOUCH_CLICKED,
                         &(audio_source.auracast_receiver));
    toggle_on_bg_color = icon_color.color.argb_full;
    {
        gui_text_t *text = gui_text_create(obj, 0, 16, y, 300, 40);
        gui_text_set(text, "Aurocast Receiver", GUI_FONT_SRC_BMP, font_color, 18, font_size);
        gui_text_type_set(text, HEADING_1_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(text, MID_LEFT);
    }
    if (audio_source.auracast_receiver)
    {
        gui_obj_move(GUI_BASE(toggle), TOGGLE_ON_X, TOGGLE_Y);
        gui_img_a8_recolor(toggle_bg, icon_color.color.argb_full);

        y += 52;
        if (audio_source.auracast_num)
        {
            for (uint8_t i = 0; i < audio_source.auracast_num; i++)
            {
                void *bg_data[2] = {BUTTON_BG_ELLIPSE_312_56_TOP_BIN, BUTTON_BG_ELLIPSE_312_56_BOT_BIN};
                if (audio_source.auracast_num == 1)
                {
                    bg_data[0] = BUTTON_BG_ELLIPSE_312_56_BIN;
                }
                gui_img_t *bg = gui_img_create_from_mem(obj, audio_source.auracast_name[i], bg_data[i], 4, y, 0, 0);
                gui_img_a8_recolor(bg, bg_color.color.argb_full);
                gui_img_a8_fix_bg(bg, screen_color.color.argb_full);
                gui_img_a8_mix_alpha(bg, bg->fg_color_set >> 24);
                gui_obj_add_event_cb(bg, click_button_connect_auracast_page_audio_source, GUI_EVENT_TOUCH_CLICKED,
                                     NULL);

                gui_text_t *t = gui_text_create(bg, 0, 44, 0, 252, 56);
                gui_text_set(t, audio_source.auracast_name[i], GUI_FONT_SRC_BMP, font_color,
                             strlen(audio_source.auracast_name[i]), font_size);
                gui_text_type_set(t, HEADING_1_BIN, FONT_SRC_MEMADDR);
                gui_text_mode_set(t, MID_LEFT);
                gui_img_t *icon = gui_img_create_from_mem(bg, 0, ICON_AURACAST_SMALL_BIN, 12, 16, 0, 0);
                gui_img_a8_recolor(icon, JBL_ORANGE.color.argb_full);
                if (audio_source.auracast_hq[i])
                {
                    gui_img_t *icon = gui_img_create_from_mem(bg, 0, ICON_HQ_BIN, 44, 14, 0, 0);
                    gui_img_a8_recolor(icon, JBL_ORANGE.color.argb_full);
                    t->base.x = 80;
                    t->base.w = 216;
                }

                y += 56;
            }
            y += 24;
        }
        else
        {
            gui_text_t *text = gui_text_create(obj, 0, 12, y, 288, 16);
            gui_text_set(text, (void *)audio_source_warning_str[1], GUI_FONT_SRC_BMP, font_color_warning,
                         strlen(audio_source_warning_str[1]), 16);
            gui_text_type_set(text, HEADING_2_BIN, FONT_SRC_MEMADDR);
            gui_text_mode_set(text, LEFT);
            y += 20;
        }

        {
            gui_img_t *bg = gui_img_create_from_mem(obj, 0, BUTTON_BG_ELLIPSE_201_80_BIN, 60, y, 0, 0);
            gui_img_a8_recolor(bg, icon_color.color.argb_full);
            gui_img_a8_fix_bg(bg, screen_color.color.argb_full);
            gui_obj_add_event_cb(bg, click_button_scan_page_audio_source, GUI_EVENT_TOUCH_CLICKED, NULL);
            gui_text_t *t = gui_text_create(bg, 0, 0, 0, 200, 80);
            gui_text_set(t, (void *)audio_source_scan_str[0], GUI_FONT_SRC_BMP, font_color,
                         strlen(audio_source_scan_str[0]), 32);
            gui_text_type_set(t, CAPTION_1_BIN, FONT_SRC_MEMADDR);
            gui_text_mode_set(t, MID_CENTER);
        }
    }
    gui_img_a8_mix_alpha(toggle_bg, toggle_bg->fg_color_set >> 24);
}

static void note_design_page_tx_management(gui_obj_t *obj, void *p)
{
    GUI_UNUSED(p);
    gui_color_t font_color;
    gui_color_t font_color_warning;
    gui_color_t bg_theme3_bright_color;
    gui_color_t bg_theme3_dark_color;
    gui_color_t bg_color;
    gui_color_t screen_color;
    if (theme_bg_white)
    {
        font_color = FG_1_LIGHT;
        font_color_warning.color.argb_full = GUI_COLOR_ARGB8888(0xFF, 0x93, 0x93, 0x93);
        bg_theme3_bright_color = BG_THEME3_BRIGHT_LIGHT;
        bg_theme3_dark_color = BG_THEME3_DARK_LIGHT;
        bg_color = BG_1_LIGHT;
        screen_color = SCREEN_BG_LIGHT;
    }
    else
    {
        font_color = FG_1_DARK;
        font_color_warning.color.argb_full = GUI_COLOR_ARGB8888(0xFF, 122, 122, 122);
        bg_theme3_bright_color = BG_THEME3_BRIGHT_DARK;
        bg_theme3_dark_color = BG_THEME3_DARK_DARK;
        bg_color = BG_1_DARK;
        screen_color = SCREEN_BG_DARK;
    }
    int16_t y = 0;
    int16_t font_size = 20;
    {
        gui_text_t *text = gui_text_create(obj, 0, 16, y, 200, 40);
        gui_text_set(text, "Connected", GUI_FONT_SRC_BMP, font_color, 9, font_size);
        gui_text_type_set(text, HEADING_1_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(text, MID_LEFT);
    }

    y += 40;
    if (tx_management.connected)
    {
        y += 4;
        gui_img_t *bg = gui_img_create_from_mem(obj, tx_management.connected_name,
                                                BUTTON_BG_ELLIPSE_312_56_BIN, 4, y, 0,
                                                0);
        gui_img_a8_recolor(bg, bg_color.color.argb_full);
        gui_img_a8_fix_bg(bg, screen_color.color.argb_full);
        gui_img_a8_mix_alpha(bg, bg->fg_color_set >> 24);
        gui_obj_add_event_cb(bg, click_button_disconnect_page_tx_management, GUI_EVENT_TOUCH_CLICKED, NULL);

        uint16_t length = strlen(tx_management.connected_name);
        gui_text_t *t = gui_text_create(bg, 0, 12, 0, 244, 56);
        gui_text_set(t, tx_management.connected_name, GUI_FONT_SRC_BMP, font_color, length, font_size);
        gui_text_type_set(t, HEADING_1_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(t, MID_LEFT);

        gui_img_t *icon = gui_img_create_from_mem(bg, 0, ICON_DISCONNECT_BIN, 268, 14, 0, 0);
        gui_img_a8_recolor(icon, font_color.color.argb_full);
        y += 68;
    }
    else
    {
        gui_text_t *text = gui_text_create(obj, 0, 12, y, 288, 27);
        gui_text_set(text, "No headphones connected", GUI_FONT_SRC_BMP, font_color_warning, 23, 16);
        gui_text_type_set(text, HEADING_2_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(text, LEFT);
        y += 27;
    }

    {
        gui_text_t *text = gui_text_create(obj, 0, 16, y, 200, 40);
        gui_text_set(text, "Paired", GUI_FONT_SRC_BMP, font_color, 6, font_size);
        gui_text_type_set(text, HEADING_1_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(text, MID_LEFT);
    }

    y += 44;
    for (uint8_t i = 0; i < tx_management.paired_num; i++)
    {
        void *bg_data[4] =
        {
            BUTTON_BG_ELLIPSE_312_56_TOP_BIN,
            BUTTON_BG_ELLIPSE_312_56_MID_BIN,
            BUTTON_BG_ELLIPSE_312_56_MID_BIN,
            BUTTON_BG_ELLIPSE_312_56_BOT_BIN
        };
        if (tx_management.paired_num == 1)
        {
            bg_data[0] = BUTTON_BG_ELLIPSE_312_56_BIN;
        }
        else if (tx_management.paired_num == 2)
        {
            bg_data[1] = BUTTON_BG_ELLIPSE_312_56_BOT_BIN;
        }
        else if (tx_management.paired_num == 3)
        {
            bg_data[2] = BUTTON_BG_ELLIPSE_312_56_BOT_BIN;
        }

        gui_img_t *bg = gui_img_create_from_mem(obj, tx_management.paired_name[i], bg_data[i], 4, y, 0, 0);
        gui_img_a8_recolor(bg, bg_color.color.argb_full);
        gui_img_a8_fix_bg(bg, screen_color.color.argb_full);
        gui_img_a8_mix_alpha(bg, bg->fg_color_set >> 24);
        gui_obj_add_event_cb(bg, click_button_connect_page_tx_management, GUI_EVENT_TOUCH_CLICKED, NULL);

        gui_text_t *t = gui_text_create(bg, 0, 12, 0, 256, 56);
        gui_text_set(t, tx_management.paired_name[i], GUI_FONT_SRC_BMP, font_color,
                     strlen(tx_management.paired_name[i]), font_size);
        gui_text_type_set(t, HEADING_1_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(t, MID_LEFT);
        gui_win_t *win = gui_win_create(bg, 0, 268, 0, 56, 56);
        gui_obj_add_event_cb(win, click_button_delete_page_tx_management, GUI_EVENT_TOUCH_CLICKED, NULL);
        gui_img_t *icon = gui_img_create_from_mem(win, 0, ICON_DELETE_BIN, 0, 14, 0, 0);
        gui_img_a8_recolor(icon, font_color.color.argb_full);

        y += 56;
    }

    y += 24;
    {
        gui_text_t *text = gui_text_create(obj, 0, 16, y, 300, 40);
        gui_text_set(text, "Discovered", GUI_FONT_SRC_BMP, font_color, 10, font_size);
        gui_text_type_set(text, HEADING_1_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(text, MID_LEFT);
    }

    y += 44;
    if (tx_management.discovered_num)
    {
        for (uint8_t i = 0; i < tx_management.discovered_num; i++)
        {
            void *bg_data[4] =
            {
                BUTTON_BG_ELLIPSE_312_56_TOP_BIN,
                BUTTON_BG_ELLIPSE_312_56_MID_BIN,
                BUTTON_BG_ELLIPSE_312_56_MID_BIN,
                BUTTON_BG_ELLIPSE_312_56_BOT_BIN
            };
            if (tx_management.discovered_num == 1)
            {
                bg_data[0] = BUTTON_BG_ELLIPSE_312_56_BIN;
            }
            else if (tx_management.discovered_num == 2)
            {
                bg_data[1] = BUTTON_BG_ELLIPSE_312_56_BOT_BIN;
            }
            else if (tx_management.discovered_num == 3)
            {
                bg_data[2] = BUTTON_BG_ELLIPSE_312_56_BOT_BIN;
            }

            gui_img_t *bg = gui_img_create_from_mem(obj, tx_management.discovered_name[i], bg_data[i], 4, y, 0,
                                                    0);
            gui_img_a8_recolor(bg, bg_color.color.argb_full);
            gui_img_a8_fix_bg(bg, screen_color.color.argb_full);
            gui_img_a8_mix_alpha(bg, bg->fg_color_set >> 24);
            gui_obj_add_event_cb(bg, click_button_discovered_page_tx_management, GUI_EVENT_TOUCH_CLICKED, NULL);

            gui_text_t *t = gui_text_create(bg, 0, 12, 0, 256, 56);
            gui_text_set(t, tx_management.discovered_name[i], GUI_FONT_SRC_BMP, font_color,
                         strlen(tx_management.discovered_name[i]), font_size);
            gui_text_type_set(t, HEADING_1_BIN, FONT_SRC_MEMADDR);
            gui_text_mode_set(t, MID_LEFT);

            y += 56;
        }
        y += 24;
    }

    {
        gui_img_t *bg = gui_img_create_from_mem(obj, 0, BUTTON_BG_ELLIPSE_240_80_BIN, 40, y, 0, 0);
        gui_obj_add_event_cb(bg, click_button_pair_page_tx_management, GUI_EVENT_TOUCH_CLICKED, NULL);
        gui_img_a8_fix_bg(bg, screen_color.color.argb_full);

        gui_text_t *t = gui_text_create(bg, 0, 0, 0, 240, 80);
        gui_text_type_set(t, CAPTION_1_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(t, MID_CENTER);
        if (tx_management.pairing)
        {
            gui_obj_create_timer((void *)bg, 2000, true, pair_timer_page_tx_management); //test
            gui_obj_start_timer((void *)bg);
            gui_img_a8_recolor(bg, bg_theme3_dark_color.color.argb_full);
            gui_text_set(t, (void *)tx_management_pair_str[1], GUI_FONT_SRC_BMP, font_color,
                         strlen(tx_management_pair_str[1]), 32);
        }
        else
        {
            gui_img_a8_recolor(bg, bg_theme3_bright_color.color.argb_full);
            gui_text_set(t, (void *)tx_management_pair_str[0], GUI_FONT_SRC_BMP, font_color,
                         strlen(tx_management_pair_str[0]), 32);
        }
    }
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

static void note_design_page_auracast(gui_obj_t *obj, void *p)
{
    GUI_UNUSED(p);
    gui_obj_t *parent = obj;

    gui_color_t font_color;
    gui_color_t toggle_bg_on_color;
    gui_color_t toggle_bg_off_color;
    gui_color_t password_bg_color;
    if (theme_bg_white)
    {
        font_color = FG_1_LIGHT;
        toggle_bg_on_color = BG_THEME1_BRIGHT_LIGHT;
        toggle_bg_off_color = FG_1_LIGHT;
        password_bg_color = FG_2_LIGHT;
    }
    else
    {
        font_color = FG_1_DARK;
        toggle_bg_on_color = BG_THEME1_BRIGHT_DARK;
        toggle_bg_off_color = BG_2_DARK;
        password_bg_color = FG_2_DARK;
    }

    gui_text_t *text = gui_text_create(parent, 0, 12, 5, 200, 15);
    gui_text_set(text, (void *)page_name_array[language_index][AURACAST_BROADCAST], GUI_FONT_SRC_BMP,
                 font_color,
                 strlen((void *)page_name_array[language_index][AURACAST_BROADCAST]), 30);
    gui_text_type_set(text, CAPTION_3_30_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, LEFT);

    gui_img_t *toggle_auracast_bg = gui_img_create_from_mem(parent, 0, TOGGLE_BG_BIN, 244, 0, 0,
                                                            0);
    gui_img_t *toggle_auracast = gui_img_create_from_mem(toggle_auracast_bg,
                                                         (void *)page_name_array[language_index][AURACAST_BROADCAST],
                                                         TOGGLE_BIN, TOGGLE_OFF_X,
                                                         TOGGLE_Y, 0, 0);
    gui_img_a8_recolor(toggle_auracast, FG_1_DARK.color.argb_full);
    gui_obj_add_event_cb(toggle_auracast_bg, click_toggle_cb, GUI_EVENT_TOUCH_CLICKED,
                         &(f_status.auracast));

    text = gui_text_create(parent, 0, 12, 45, 300, 15);
    gui_text_set(text, (void *)page_name_array[language_index][PASSWORD], GUI_FONT_SRC_BMP, font_color,
                 strlen((void *)page_name_array[language_index][PASSWORD]), 30);
    gui_text_type_set(text, CAPTION_3_30_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, LEFT);
    gui_img_t *toggle_password_bg = gui_img_create_from_mem(parent, 0, TOGGLE_BG_BIN, 244, 50, 0, 0);
    gui_img_t *toggle_password = gui_img_create_from_mem(toggle_password_bg,
                                                         (void *)page_name_array[language_index][PASSWORD],
                                                         TOGGLE_BIN,
                                                         TOGGLE_OFF_X,
                                                         TOGGLE_Y, 0, 0);
    gui_img_a8_recolor(toggle_password, FG_1_DARK.color.argb_full);

    gui_obj_add_event_cb(toggle_password_bg, click_toggle_cb, GUI_EVENT_TOUCH_CLICKED,
                         &(f_status.password));

    {
        if (f_status.auracast)
        {
            gui_obj_move(GUI_BASE(toggle_auracast), TOGGLE_ON_X, TOGGLE_Y);
            gui_img_a8_recolor(toggle_auracast_bg, toggle_bg_on_color.color.argb_full);
        }
        else
        {
            gui_img_a8_recolor(toggle_auracast_bg, toggle_bg_off_color.color.argb_full);
        }
        if (f_status.password)
        {
            gui_obj_move(GUI_BASE(toggle_password), TOGGLE_ON_X, TOGGLE_Y);
            gui_img_a8_recolor(toggle_password_bg, toggle_bg_on_color.color.argb_full);
            int16_t x = 16;
            for (uint8_t i = 0; i < 4; i++)
            {
                gui_img_t *bg = gui_img_create_from_mem(parent, 0, BUTTON_BG_RECT_BIN, x, 116, 0, 0);
                gui_img_a8_recolor(bg, password_bg_color.color.argb_full);
                gui_img_a8_mix_alpha(bg, password_bg_color.color.argb_full >> 24);
                gui_text_t *text = gui_text_create(bg, 0, 0, 0, 68, 68);
                gui_text_set(text, auracast_password + i, GUI_FONT_SRC_BMP, font_color, 1, 20);
                gui_text_type_set(text, HEADING_1_BIN, FONT_SRC_MEMADDR);
                gui_text_mode_set(text, MID_CENTER);
                x += 76;
            }
        }
        else
        {
            gui_img_a8_recolor(toggle_password_bg, toggle_bg_off_color.color.argb_full);
        }
        toggle_on_bg_color = BG_THEME1_BRIGHT_LIGHT.color.argb_full;
    }
    gui_img_a8_mix_alpha(toggle_auracast_bg, toggle_auracast_bg->fg_color_set >> 24);
    gui_img_a8_mix_alpha(toggle_password_bg, toggle_password_bg->fg_color_set >> 24);
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
        // bg_l->need_clip = true;
        // bg_m->need_clip = true;
        // bg_r->need_clip = true;
        // icon_l->need_clip = true;
        // icon_m->need_clip = true;
        // icon_r->need_clip = true;
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
            // icon_done->need_clip = true;
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
            // icon_done->need_clip = true;
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
            // icon_done->need_clip = true;
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

/*============================================================================*
 *                           Public Functions
 *============================================================================*/
void msg_2_regenerate_current_view(void)
{
    gui_msg_t msg =
    {
        .event = GUI_EVENT_USER_DEFINE,
        .cb = regenerate_current_view,
    };
    gui_send_msg_to_server(&msg);
}

void update_icon_color(gui_obj_t *obj, uint8_t status)
{
    gui_obj_t *right = gui_list_entry(obj->child_list.prev, gui_obj_t, brother_list);
    gui_obj_t *mid = gui_list_entry(right->brother_list.prev, gui_obj_t, brother_list);
    gui_obj_t *left = gui_list_entry(mid->brother_list.prev, gui_obj_t, brother_list);

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
    gui_scroll_text_t *music_name = gui_scroll_text_create(parent,
                                                           (void *)page_name_array[language_index][PLAYBACK],
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
        else if (music_index == MUSIC_MAX_NUM - 1 || strlen(music_array[music_index + 1]) == 0)
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
        else if (music_index == MUSIC_MAX_NUM - 1 || strlen(music_array[music_index + 1]) == 0)
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
        gui_img_a8_mix_alpha(control_bg, INVALID_OPACITY);
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
        img_data_array[10] = PROGRESSBAR_11_BIN;
        img_data_array[11] = PROGRESSBAR_12_BIN;
        img_data_array[12] = PROGRESSBAR_13_BIN;
        img_data_array[13] = PROGRESSBAR_14_BIN;
        img_data_array[14] = PROGRESSBAR_15_BIN;
        img_data_array[15] = PROGRESSBAR_16_BIN;
    }

    gui_text_t *text = gui_text_create(parent, 0, 12, 45, 200, 15);
    gui_text_set(text, (void *)page_name_array[language_index][VOLUME], GUI_FONT_SRC_BMP, font_color,
                 strlen((void *)page_name_array[language_index][VOLUME]), 30);
    gui_text_type_set(text, CAPTION_3_30_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, LEFT);

    gui_img_t *control_bg = gui_img_create_from_mem(parent, 0, CONTROL_BG_BIN, 12, 92, 0, 0);
    gui_img_t *button_sub_bg = gui_img_create_from_mem(control_bg, "l", BUTTON_BG_CIRCLE_BIN, 6, 6, 0,
                                                       0);
    gui_img_t *sub = gui_img_create_from_mem(control_bg, 0, SUB_BIN, 31, 42, 0, 0);
    gui_img_t *button_add_bg = gui_img_create_from_mem(control_bg, "r", BUTTON_BG_CIRCLE_BIN, 212, 6,
                                                       0, 0);
    gui_img_t *add = gui_img_create_from_mem(control_bg, 0, ADD_BIN, 237, 31, 0, 0);
    gui_img_t *progressbar_bg = gui_img_create_from_mem(control_bg, 0, PROGRESSBAR_16_BIN, 90, 27, 0,
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
        gui_img_refresh_size(progressbar);
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
        else if (volume_val == 16)
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
        else if (volume_val == 16)
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
    gui_text_set(text, (void *)page_name_array[language_index][AMBIENT_SOUND], GUI_FONT_SRC_BMP,
                 font_color,
                 strlen((void *)page_name_array[language_index][AMBIENT_SOUND]), 30);
    gui_text_type_set(text, CAPTION_3_30_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, LEFT);

    gui_img_t *toggle_bg = gui_img_create_from_mem(parent, 0, TOGGLE_BG_BIN, 244, 40, 0, 0);
    gui_img_t *toggle = gui_img_create_from_mem(toggle_bg,
                                                (void *)page_name_array[language_index][AMBIENT_SOUND],
                                                TOGGLE_BIN, TOGGLE_OFF_X,
                                                TOGGLE_Y, 0, 0);
    gui_img_a8_recolor(toggle, FG_1_DARK.color.argb_full);
    gui_obj_add_event_cb(toggle_bg, click_toggle_cb, GUI_EVENT_TOUCH_CLICKED,
                         &(f_status.ambient_sound));

    gui_img_t *control_bg = gui_img_create_from_mem(parent, 0, CONTROL_BG_BIN, 12, 92, 0, 0);
    gui_img_t *button_bg = gui_img_create_from_mem(control_bg,
                                                   (void *)page_name_array[language_index][AMBIENT_SOUND],
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
            gui_img_a8_mix_alpha(control_bg, INVALID_OPACITY);
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
            gui_img_a8_mix_alpha(control_bg, INVALID_OPACITY);
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
    gui_text_set(text, (void *)page_name_array[language_index][EQUALIZER], GUI_FONT_SRC_BMP, font_color,
                 strlen((void *)page_name_array[language_index][EQUALIZER]), 30);
    gui_text_type_set(text, CAPTION_3_30_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, LEFT);

    gui_img_t *toggle_bg = gui_img_create_from_mem(parent, 0, TOGGLE_BG_BIN, 244, 40, 0, 0);
    gui_img_t *toggle = gui_img_create_from_mem(toggle_bg,
                                                (void *)page_name_array[language_index][EQUALIZER],
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
            gui_img_a8_mix_alpha((void *)control_bg, INVALID_OPACITY);
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
            gui_img_a8_mix_alpha((void *)control_bg, INVALID_OPACITY);
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
    gui_text_set(text, (void *)page_name_array[language_index][SMART_TALK], GUI_FONT_SRC_BMP,
                 font_color,
                 strlen((void *)page_name_array[language_index][SMART_TALK]), 30);
    gui_text_type_set(text, CAPTION_3_30_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, LEFT);

    gui_img_t *toggle_bg = gui_img_create_from_mem(parent, 0, TOGGLE_BG_BIN, 244, 40, 0, 0);
    gui_img_t *toggle = gui_img_create_from_mem(toggle_bg,
                                                (void *)page_name_array[language_index][SMART_TALK],
                                                TOGGLE_BIN, TOGGLE_OFF_X,
                                                TOGGLE_Y, 0, 0);
    gui_img_a8_recolor(toggle, FG_1_DARK.color.argb_full);
    gui_obj_add_event_cb(toggle_bg, click_toggle_cb, GUI_EVENT_TOUCH_CLICKED, &(f_status.smart_talk));

    gui_img_t *control_bg = gui_img_create_from_mem(parent, 0, CONTROL_BG_BIN, 12, 92, 0, 0);
    gui_img_t *button_bg = gui_img_create_from_mem(control_bg,
                                                   (void *)page_name_array[language_index][SMART_TALK],
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
            gui_img_a8_mix_alpha((void *)control_bg, INVALID_OPACITY);
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
            gui_img_a8_mix_alpha((void *)control_bg, INVALID_OPACITY);
        }
        toggle_on_bg_color = BG_THEME1_BRIGHT_DARK.color.argb_full;
    }
    gui_img_a8_mix_alpha(toggle_bg, toggle_bg->fg_color_set >> 24);
    control_invalid = !f_status.ambient_sound;
}

#if SS_WITH_HEAD_TRACKING
void page_spatial_sound_with_head_tracking_design(gui_obj_t *parent)
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
    gui_text_set(text, (void *)page_name_array[language_index][SPATIAL_SOUND], GUI_FONT_SRC_BMP,
                 font_color,
                 strlen((void *)page_name_array[language_index][SPATIAL_SOUND]), 30);
    gui_text_type_set(text, CAPTION_3_30_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, LEFT);

    gui_img_t *settings_bg = gui_img_create_from_mem(parent, 0, TOGGLE_BG_BIN, 244, 40, 0, 0);
    gui_img_t *settings = gui_img_create_from_mem(settings_bg, 0, ICON_SETTINGS_BIN, 20, 8, 0, 0);
    gui_img_a8_recolor(settings, FG_1_DARK.color.argb_full);
    gui_obj_add_event_cb(settings_bg, click_button_page_spatial_sound_with_head_tracking,
                         GUI_EVENT_TOUCH_CLICKED, NULL);

    gui_img_t *control_bg = gui_img_create_from_mem(parent, 0, CONTROL_BG_BIN, 12, 92, 0, 0);
    gui_img_t *button_bg = gui_img_create_from_mem(control_bg,
                                                   (void *)page_name_array[language_index][SPATIAL_SOUND],
                                                   BUTTON_BG_ELLIPSE_BIN,
                                                   spatial_sound_status * 99, 0, 0, 0);
    gui_img_t *icon_l = gui_img_create_from_mem(control_bg, "l", ICON_SS_OFF_BIN, 30, 26, 0,
                                                0);
    gui_img_t *icon_m = gui_img_create_from_mem(control_bg, "m", ICON_SS_ON_BIN, 129, 26, 0,
                                                0);
    gui_img_t *icon_r = gui_img_create_from_mem(control_bg, "r", ICON_HEAD_TRACKING_BIN, 227, 26, 0,
                                                0);
    update_icon_color((void *)control_bg, spatial_sound_status);
    gui_img_a8_recolor(icon_l, FG_WHITE.color.argb_full);
    gui_img_a8_recolor(icon_m, FG_WHITE.color.argb_full);
    gui_img_a8_recolor(icon_r, FG_WHITE.color.argb_full);
    gui_obj_add_event_cb(icon_l, click_button_page_spatial_sound_with_head_tracking,
                         GUI_EVENT_TOUCH_CLICKED, NULL);
    gui_obj_add_event_cb(icon_m, click_button_page_spatial_sound_with_head_tracking,
                         GUI_EVENT_TOUCH_CLICKED, NULL);
    gui_obj_add_event_cb(icon_r, click_button_page_spatial_sound_with_head_tracking,
                         GUI_EVENT_TOUCH_CLICKED, NULL);

    if (theme_bg_white)
    {
        gui_img_a8_recolor(control_bg, BG_THEME1_DARK_LIGHT.color.argb_full);
        gui_img_a8_recolor(button_bg, BG_THEME1_BRIGHT_LIGHT.color.argb_full);
        gui_img_a8_recolor(settings_bg, BG_THEME1_BRIGHT_LIGHT.color.argb_full);
    }
    else
    {
        gui_img_a8_recolor(control_bg, BG_THEME1_DARK_DARK.color.argb_full);
        gui_img_a8_recolor(button_bg, BG_THEME1_BRIGHT_DARK.color.argb_full);
        gui_img_a8_recolor(settings_bg, BG_THEME1_BRIGHT_DARK.color.argb_full);
    }
}
#else
void page_spatial_sound_without_head_tracking_design(gui_obj_t *parent)
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
    gui_text_set(text, (void *)page_name_array[language_index][SPATIAL_SOUND], GUI_FONT_SRC_BMP,
                 font_color,
                 strlen((void *)page_name_array[language_index][SPATIAL_SOUND]), 30);
    gui_text_type_set(text, CAPTION_3_30_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, LEFT);

    gui_img_t *toggle_bg = gui_img_create_from_mem(parent, 0, TOGGLE_BG_BIN, 244, 40, 0, 0);
    gui_img_t *toggle = gui_img_create_from_mem(toggle_bg,
                                                (void *)page_name_array[language_index][SPATIAL_SOUND],
                                                TOGGLE_BIN, TOGGLE_OFF_X,
                                                TOGGLE_Y, 0, 0);
    gui_img_a8_recolor(toggle, FG_1_DARK.color.argb_full);
    gui_obj_add_event_cb(toggle_bg, click_toggle_cb, GUI_EVENT_TOUCH_CLICKED,
                         &(f_status.spatial_sound));

    gui_img_t *control_bg = gui_img_create_from_mem(parent, 0, CONTROL_BG_BIN, 12, 92, 0, 0);
    gui_img_t *button_bg = gui_img_create_from_mem(control_bg,
                                                   (void *)page_name_array[language_index][SPATIAL_SOUND],
                                                   BUTTON_BG_ELLIPSE_BIN,
                                                   spatial_sound_type * 99, 0, 0, 0);
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
    gui_obj_add_event_cb(movie, click_button_page_spatial_sound_without_head_tracking,
                         GUI_EVENT_TOUCH_CLICKED, NULL);
    gui_obj_add_event_cb(music, click_button_page_spatial_sound_without_head_tracking,
                         GUI_EVENT_TOUCH_CLICKED, NULL);
    gui_obj_add_event_cb(game, click_button_page_spatial_sound_without_head_tracking,
                         GUI_EVENT_TOUCH_CLICKED, NULL);

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
            gui_img_a8_mix_alpha((void *)control_bg, INVALID_OPACITY);
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
            gui_img_a8_mix_alpha((void *)control_bg, INVALID_OPACITY);
        }
        toggle_on_bg_color = BG_THEME1_BRIGHT_DARK.color.argb_full;
    }
    gui_img_a8_mix_alpha(toggle_bg, toggle_bg->fg_color_set >> 24);
    control_invalid = !f_status.spatial_sound;
}
#endif

void page_hr_audio_design(gui_obj_t *parent)
{
    quick_page_name_index = HR_AUDIO;
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
    gui_text_set(text, (void *)page_name_array[language_index][HR_AUDIO], GUI_FONT_SRC_BMP, font_color,
                 strlen((void *)page_name_array[language_index][HR_AUDIO]), 30);
    gui_text_type_set(text, CAPTION_3_30_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, MULTI_LEFT);

    gui_img_t *toggle_bg = gui_img_create_from_mem(parent, 0, TOGGLE_BG_BIN, 244, 40, 0,
                                                   0);
    gui_img_t *toggle = gui_img_create_from_mem(toggle_bg,
                                                (void *)page_name_array[language_index][HR_AUDIO],
                                                TOGGLE_BIN, TOGGLE_OFF_X,
                                                TOGGLE_Y, 0, 0);
    gui_img_a8_recolor(toggle, FG_1_DARK.color.argb_full);
    gui_obj_add_event_cb(toggle_bg, click_toggle_cb, GUI_EVENT_TOUCH_CLICKED,
                         &(f_status.hr_audio));

    if (theme_bg_white)
    {
        if (f_status.hr_audio)
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
        if (f_status.hr_audio)
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
    gui_text_set(text, (void *)page_name_array[language_index][VOICE_AWARE], GUI_FONT_SRC_BMP,
                 font_color,
                 strlen((void *)page_name_array[language_index][VOICE_AWARE]), 30);
    gui_text_type_set(text, CAPTION_3_30_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, LEFT);

    gui_img_t *toggle_bg = gui_img_create_from_mem(parent, 0, TOGGLE_BG_BIN, 244, 40, 0, 0);
    gui_img_t *toggle = gui_img_create_from_mem(toggle_bg,
                                                (void *)page_name_array[language_index][VOICE_AWARE],
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
            gui_img_a8_mix_alpha((void *)control_bg, INVALID_OPACITY);
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
            gui_img_a8_mix_alpha((void *)control_bg, INVALID_OPACITY);
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
    gui_text_set(text, (void *)page_name_array[language_index][AUTO_PLAY_PAUSE], GUI_FONT_SRC_BMP,
                 font_color,
                 strlen((void *)page_name_array[language_index][AUTO_PLAY_PAUSE]), 30);
    gui_text_type_set(text, CAPTION_3_30_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, LEFT);

    gui_img_t *toggle_bg = gui_img_create_from_mem(parent, 0, TOGGLE_BG_BIN, 244, 40, 0,
                                                   0);
    gui_img_t *toggle = gui_img_create_from_mem(toggle_bg,
                                                (void *)page_name_array[language_index][AUTO_PLAY_PAUSE],
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

void page_audio_source_design(gui_obj_t *parent)
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
    if (gui_view_get_current())
    {
        page_audio_source_list_offset_rec = 0;
    }
    uint16_t length_1 = (audio_source.connected_num + audio_source.paired_num) * 56;
    uint16_t length_2 = audio_source.auracast_receiver ? (128 + audio_source.auracast_num * 56) : 0;
    uint16_t length = 327 + length_1 + length_2;
    gui_list_t *list = gui_list_create(parent, 0, 0, 60, 0, 0, length, 0,
                                       VERTICAL, note_design_page_audio_source, NULL, 0);
    gui_list_set_style(list, LIST_CLASSIC);
    gui_list_set_note_num(list, 1);
    gui_list_set_out_scope(list, 20);
    gui_list_set_offset(list, page_audio_source_list_offset_rec);
    gui_obj_create_timer(GUI_BASE(list), 10, true, list_timer_page_audio_source);

    gui_img_t *mask = gui_img_create_from_mem(parent, 0, MASK_BIN, 0, 0, 0, 0);
    gui_text_t *text = gui_text_create(parent, 0, 0, 0, 320, 60);
    gui_text_set(text, (void *)page_name_array[language_index][AUDIO_SOURCE], GUI_FONT_SRC_BMP,
                 font_color,
                 strlen((void *)page_name_array[language_index][AUDIO_SOURCE]), 30);
    gui_text_type_set(text, CAPTION_3_30_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, MID_CENTER);

    gui_img_t *scrollbar_bg = gui_img_create_from_mem(parent, 0, SCROLLBAR_BG_BIN, 310, 60, 0, 0);
    gui_img_t *scrollbar = gui_img_create_from_mem(parent, 0, SCROLLBAR_S_BIN, 310, 60, 0, 0);
    if (theme_bg_white)
    {
        gui_img_a8_recolor(mask, SCREEN_BG_LIGHT.color.argb_full);
        gui_img_a8_fix_bg(mask, SCREEN_BG_LIGHT.color.argb_full);
        gui_img_a8_recolor(scrollbar_bg, FG_BLACK.color.argb_full);
        gui_img_a8_recolor(scrollbar, FG_BLACK.color.argb_full);
        toggle_on_bg_color = BG_THEME3_BRIGHT_LIGHT.color.argb_full;
    }
    else
    {
        gui_img_a8_recolor(mask, 0);
        gui_img_a8_fix_bg(mask, SCREEN_BG_LIGHT.color.argb_full);
        gui_img_a8_recolor(scrollbar_bg, FG_WHITE.color.argb_full);
        gui_img_a8_recolor(scrollbar, FG_WHITE.color.argb_full);
        toggle_on_bg_color = BG_THEME3_BRIGHT_DARK.color.argb_full;
    }
}

void page_auracast_design(gui_obj_t *parent)
{
    quick_page_name_index = AURACAST_BROADCAST;

    gui_win_t *win = gui_win_create(gui_obj_get_fake_root(), 0, 0, 0, 0, 0);
    win->base.parent = parent;
    gui_list_remove(&win->base.brother_list);
    gui_list_insert(&(parent->child_list), &(win->base.brother_list));

    uint16_t length = f_status.password ? 194 : 160;
    gui_list_t *list = gui_list_create(win, 0, 0, 40, 0, 0, length, 0,
                                       VERTICAL, note_design_page_auracast, NULL, 0);
    gui_list_set_style(list, LIST_CLASSIC);
    gui_list_set_note_num(list, 1);
    gui_list_set_inertia(list, false);

    gui_img_t *mask = gui_img_create_from_mem(win, 0, MASK_320_40_BIN, 0, 0, 0, 0);
    gui_img_a8_recolor(mask, theme_bg_white ? SCREEN_BG_LIGHT.color.argb_full : 0);
}

/* Case settings page */
void page_tx_management_design(gui_obj_t *parent)
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
    if (gui_view_get_current())
    {
        page_audio_source_list_offset_rec = 0;
        tx_management.discovered_num = 0;
        tx_management.wait = false;
    }
    uint16_t length_1 = tx_management.paired_num * 56;
    uint16_t length_2 = tx_management.discovered_num * 56 + (tx_management.discovered_num ? 24 : 0);
    uint16_t length = 244 + (tx_management.connected ? 76 : 27) + length_1 + length_2;
    gui_list_t *list = gui_list_create(parent, 0, 0, 60, 0, 0, length, 0,
                                       VERTICAL, note_design_page_tx_management, NULL, 0);
    gui_list_set_style(list, LIST_CLASSIC);
    gui_list_set_note_num(list, 1);
    gui_list_set_out_scope(list, 20);
    gui_list_set_offset(list, page_audio_source_list_offset_rec);
    gui_obj_create_timer(GUI_BASE(list), 10, true, list_timer_page_audio_source);

    gui_img_t *mask = gui_img_create_from_mem(parent, 0, MASK_BIN, 0, 0, 0, 0);
    gui_text_t *text = gui_text_create(parent, 0, 0, 0, 320, 60);
    gui_text_set(text, (void *)page_name_array[language_index][SMART_TX_MANAGEMENT], GUI_FONT_SRC_BMP,
                 font_color,
                 strlen((void *)page_name_array[language_index][SMART_TX_MANAGEMENT]), 30);
    gui_text_type_set(text, CAPTION_3_30_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, MID_CENTER);

    gui_img_t *scrollbar_bg = gui_img_create_from_mem(parent, 0, SCROLLBAR_BG_BIN, 310, 60, 0, 0);
    gui_img_t *scrollbar = gui_img_create_from_mem(parent, 0, SCROLLBAR_S_BIN, 310, 60, 0, 0);
    if (theme_bg_white)
    {
        gui_img_a8_recolor(mask, SCREEN_BG_LIGHT.color.argb_full);
        gui_img_a8_fix_bg(mask, SCREEN_BG_LIGHT.color.argb_full);
        gui_img_a8_recolor(scrollbar_bg, FG_BLACK.color.argb_full);
        gui_img_a8_recolor(scrollbar, FG_BLACK.color.argb_full);
    }
    else
    {
        gui_img_a8_recolor(mask, 0);
        gui_img_a8_fix_bg(mask, SCREEN_BG_LIGHT.color.argb_full);
        gui_img_a8_recolor(scrollbar_bg, FG_WHITE.color.argb_full);
        gui_img_a8_recolor(scrollbar, FG_WHITE.color.argb_full);
    }
}

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
    gui_text_set(text, (void *)page_name_array[language_index][SCREEN_BRIGHTNESS], GUI_FONT_SRC_BMP,
                 font_color,
                 strlen((void *)page_name_array[language_index][SCREEN_BRIGHTNESS]), 30);
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
    gui_text_set(text, (void *)page_name_array[language_index][DARK_LIGHT_MODE], GUI_FONT_SRC_BMP,
                 font_color,
                 strlen((void *)page_name_array[language_index][DARK_LIGHT_MODE]), 30);
    gui_text_type_set(text, CAPTION_3_30_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, LEFT);

    gui_img_t *control_bg = gui_img_create_from_mem(parent, 0, CONTROL_BG_BIN, 12, 92, 0, 0);
    gui_img_t *button_bg = gui_img_create_from_mem(control_bg,
                                                   (void *)page_name_array[language_index][DARK_LIGHT_MODE],
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

    gui_color_t font_color;

    if (theme_bg_white)
    {
        font_color = FG_1_LIGHT;
        img_data_array[0] = WALLPAPER_1_LIGHT_BIN;
        img_data_array[1] = WALLPAPER_2_LIGHT_BIN;
        img_data_array[2] = WALLPAPER_3_LIGHT_BIN;
        img_data_array[3] = WALLPAPER_4_LIGHT_BIN;
        img_data_array[4] = WALLPAPER_5_LIGHT_BIN;
    }
    else
    {
        font_color = FG_1_DARK;
        img_data_array[0] = WALLPAPER_1_DARK_BIN;
        img_data_array[1] = WALLPAPER_2_DARK_BIN;
        img_data_array[2] = WALLPAPER_3_DARK_BIN;
        img_data_array[3] = WALLPAPER_4_DARK_BIN;
        img_data_array[4] = WALLPAPER_5_DARK_BIN;
    }

    gui_text_t *text = gui_text_create(parent, 0, 12, 45, 200, 15);
    gui_text_set(text, (void *)page_name_array[language_index][LOCK_SCREEN], GUI_FONT_SRC_BMP,
                 font_color,
                 strlen((void *)page_name_array[language_index][LOCK_SCREEN]), 30);
    gui_text_type_set(text, CAPTION_3_30_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, LEFT);
    gui_win_t *win_l = gui_win_create(parent, "l", 18, 98, 78, 78);
    gui_win_t *win_r = gui_win_create(parent, "r", 224, 98, 78, 78);
    gui_img_t *switch_l = gui_img_create_from_mem(win_l, 0, SWITCH_L_BIN, 29, 26, 0, 0);
    gui_img_t *switch_r = gui_img_create_from_mem(win_r, 0, SWITCH_R_BIN, 29, 26, 0, 0);
    gui_obj_add_event_cb(win_l, click_button_page_lock_screen, GUI_EVENT_TOUCH_CLICKED, NULL);
    gui_obj_add_event_cb(win_r, click_button_page_lock_screen, GUI_EVENT_TOUCH_CLICKED, NULL);

    gui_img_t *wallpaper = gui_img_create_from_mem(parent, 0, img_data_array[wallpaper_index], 96, 97,
                                                   0, 0);
    gui_img_set_mode(wallpaper, IMG_BYPASS_MODE);
    gui_obj_add_event_cb(wallpaper, click_wallpaper_page_lock_screen, GUI_EVENT_TOUCH_CLICKED, NULL);

    if (theme_bg_white)
    {
        gui_img_a8_recolor(switch_l, FG_1_LIGHT.color.argb_full);
        gui_img_a8_recolor(switch_r, FG_1_LIGHT.color.argb_full);
        if (wallpaper_index == 0)
        {
            gui_img_a8_recolor(switch_l, FG_2_LIGHT.color.argb_full);
        }
        else if (wallpaper_index == 4)
        {
            gui_img_a8_recolor(switch_r, FG_2_LIGHT.color.argb_full);
        }
    }
    else
    {
        gui_img_a8_recolor(switch_l, FG_1_DARK.color.argb_full);
        gui_img_a8_recolor(switch_r, FG_1_DARK.color.argb_full);
        if (wallpaper_index == 0)
        {
            gui_img_a8_recolor(switch_l, FG_2_DARK.color.argb_full);
        }
        else if (wallpaper_index == 4)
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
    gui_text_set(text, (void *)page_name_array[language_index][AUTO_DIM_OFF_SCREEN], GUI_FONT_SRC_BMP,
                 font_color,
                 strlen((void *)page_name_array[language_index][AUTO_DIM_OFF_SCREEN]), 30);
    gui_text_type_set(text, CAPTION_3_30_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, LEFT);

    gui_img_t *toggle_bg = gui_img_create_from_mem(parent, 0, TOGGLE_BG_BIN, 244, 40, 0, 0);
    gui_img_t *toggle = gui_img_create_from_mem(toggle_bg,
                                                (void *)page_name_array[language_index][AUTO_DIM_OFF_SCREEN],
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
            gui_img_a8_mix_alpha(control_bg, INVALID_OPACITY);
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
            gui_img_a8_mix_alpha(control_bg, INVALID_OPACITY);
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
    gui_text_set(text, (void *)page_name_array[language_index][QUICK_WAKE_UP_SCREEN], GUI_FONT_SRC_BMP,
                 font_color,
                 strlen((void *)page_name_array[language_index][QUICK_WAKE_UP_SCREEN]), 30);
    gui_text_type_set(text, CAPTION_3_30_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, MULTI_LEFT);

    gui_img_t *toggle_bg = gui_img_create_from_mem(parent, 0, TOGGLE_BG_BIN, 244, 40, 0, 0);
    gui_img_t *toggle = gui_img_create_from_mem(toggle_bg,
                                                (void *)page_name_array[language_index][QUICK_WAKE_UP_SCREEN], TOGGLE_BIN, TOGGLE_OFF_X,
                                                TOGGLE_Y, 0, 0);
    gui_img_a8_recolor(toggle, FG_1_DARK.color.argb_full);
    gui_obj_add_event_cb(toggle_bg, click_toggle_cb, GUI_EVENT_TOUCH_CLICKED,
                         &(f_status.quick_wake_up_screen));

    if (f_status.quick_wake_up_screen)
    {
        gui_img_t *control_bg = gui_img_create_from_mem(parent, 0, CONTROL_BG_BIN, 12, 92, 0, 0);
        gui_img_t *button_bg = gui_img_create_from_mem(control_bg,
                                                       (void *)page_name_array[language_index][QUICK_WAKE_UP_SCREEN], BUTTON_BG_ELLIPSE_BIN,
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
        gui_list_t *list = gui_list_create(win, 0, 0, 96, 0, 0, 170, 0,
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
            gui_img_a8_recolor(scrollbar_bg, FG_BLACK.color.argb_full);
            gui_img_a8_recolor(scrollbar, FG_BLACK.color.argb_full);
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

    gui_img_t *bg_note = gui_img_create_from_mem(parent, 0, MENU_LISTNOTE_BG_BIN, 0, 0, 0, 0);
    gui_obj_hidden((void *)bg_note, true);

    gui_list_t *list = gui_list_create(parent, 0, 0, 60, 0, 0, 56, 0,
                                       VERTICAL, note_design_page_case_button_customize, NULL, 0);
    gui_list_set_style(list, LIST_CLASSIC);
    gui_list_set_note_num(list, 5);
    gui_list_set_out_scope(list, 20);
    gui_obj_create_timer(GUI_BASE(list), 10, true, list_timer_page_case_button_customize);

    gui_img_t *mask = gui_img_create_from_mem(parent, 0, MASK_BIN, 0, 0, 0, 0);
    gui_scroll_text_t *text = gui_scroll_text_create(parent, 0, 44, 16, 260, 30);
    gui_scroll_text_type_set(text, CAPTION_3_30_BIN, FONT_SRC_MEMADDR);
    gui_scroll_text_set(text, (void *)page_name_array[language_index][CASE_BUTTON_CUSTOMIZE],
                        GUI_FONT_SRC_BMP,
                        font_color, strlen((void *)page_name_array[language_index][CASE_BUTTON_CUSTOMIZE]), 30);
    gui_scroll_text_scroll_set(text, SCROLL_X, 20, 260, 5000, 0);

    gui_img_t *scrollbar_bg = gui_img_create_from_mem(parent, 0, SCROLLBAR_BG_BIN, 310, 72, 0, 0);
    gui_img_t *scrollbar = gui_img_create_from_mem(parent, 0, SCROLLBAR_M_BIN, 310, 72, 0, 0);
    if (theme_bg_white)
    {
        gui_img_a8_recolor(mask, SCREEN_BG_LIGHT.color.argb_full);
        gui_img_a8_fix_bg(mask, SCREEN_BG_LIGHT.color.argb_full);
        gui_img_a8_recolor(scrollbar_bg, FG_BLACK.color.argb_full);
        gui_img_a8_recolor(scrollbar, FG_BLACK.color.argb_full);
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
        scroll_color = FG_BLACK;
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

    gui_list_t *list = gui_list_create(parent, 0, 0, 60, 0, 0, 110, 0,
                                       VERTICAL, note_design_page_information_center_customize, NULL, 0);
    gui_list_set_style(list, LIST_CLASSIC);
    gui_list_set_note_num(list, 2);
    gui_list_set_out_scope(list, 10);
    gui_list_set_offset(list, page_information_center_customize_list_offset_rec);
    gui_obj_create_timer(GUI_BASE(list), 10, true, list_timer_page_information_center_customize);

    gui_img_t *mask = gui_img_create_from_mem(parent, 0, MASK_BIN, 0, 0, 0, 0);
    gui_scroll_text_t *text = gui_scroll_text_create(parent, 0, 44, 16, 260, 30);
    gui_scroll_text_type_set(text, CAPTION_3_30_BIN, FONT_SRC_MEMADDR);
    gui_scroll_text_set(text, (void *)page_name_array[language_index][INFORMATION_CENTER_CUSTOMIZE],
                        GUI_FONT_SRC_BMP,
                        font_color, strlen((void *)page_name_array[language_index][INFORMATION_CENTER_CUSTOMIZE]), 30);
    gui_scroll_text_scroll_set(text, SCROLL_X, 20, 260, 5000, 0);

    gui_img_t *scrollbar_bg = gui_img_create_from_mem(parent, 0, SCROLLBAR_BG_BIN, 310, 72, 0, 0);
    gui_img_t *scrollbar = gui_img_create_from_mem(parent, 0, SCROLLBAR_L_BIN, 310, 72, 0, 0);
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
    gui_text_set(text, (void *)page_name_array[language_index][UNLOCK_SLIDER], GUI_FONT_SRC_BMP,
                 font_color,
                 strlen((void *)page_name_array[language_index][UNLOCK_SLIDER]), 30);
    gui_text_type_set(text, CAPTION_3_30_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, LEFT);

    gui_img_t *toggle_bg = gui_img_create_from_mem(parent, 0, TOGGLE_BG_BIN, 244, 40, 0, 0);
    gui_img_t *toggle = gui_img_create_from_mem(toggle_bg,
                                                (void *)page_name_array[language_index][UNLOCK_SLIDER],
                                                TOGGLE_BIN, TOGGLE_OFF_X,
                                                TOGGLE_Y, 0, 0);
    gui_img_a8_recolor(toggle, FG_1_DARK.color.argb_full);
    gui_obj_add_event_cb(toggle_bg, click_toggle_cb, GUI_EVENT_TOUCH_CLICKED,
                         &(f_status.unlock_slider));

    text = gui_text_create(parent, 0, 12, 96, 296, 170);
    gui_text_set(text, (void *)unlock_slider_warning_str[language_index], GUI_FONT_SRC_BMP,
                 font_color_warning,
                 strlen(unlock_slider_warning_str[language_index]), 30);
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
    gui_text_set(text, (void *)page_name_array[language_index][NOTIFICATION], GUI_FONT_SRC_BMP,
                 font_color,
                 strlen((void *)page_name_array[language_index][NOTIFICATION]), 30);
    gui_text_type_set(text, CAPTION_3_30_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, LEFT);

    gui_img_t *toggle_notification_bg = gui_img_create_from_mem(parent, 0, TOGGLE_BG_BIN, 244, 40, 0,
                                                                0);
    gui_img_t *toggle_notification = gui_img_create_from_mem(toggle_notification_bg,
                                                             (void *)page_name_array[language_index][NOTIFICATION],
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
        gui_text_set(text, (void *)page_name_array[language_index][MESSAGE_PREVIEW], GUI_FONT_SRC_BMP,
                     font_color,
                     strlen((void *)page_name_array[language_index][MESSAGE_PREVIEW]), 30);
        gui_text_type_set(text, CAPTION_3_30_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(text, LEFT);
        toggle_message_bg = gui_img_create_from_mem(parent, 0, TOGGLE_BG_BIN, 244, 90, 0, 0);
        toggle_message = gui_img_create_from_mem(toggle_message_bg,
                                                 (void *)page_name_array[language_index][MESSAGE_PREVIEW],
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
            gui_img_a8_recolor(toggle_notification_bg, BG_THEME3_BRIGHT_LIGHT.color.argb_full);
            if (f_status.message_preview)
            {
                gui_obj_move(GUI_BASE(toggle_message), TOGGLE_ON_X, TOGGLE_Y);
                gui_img_a8_recolor(toggle_message_bg, BG_THEME3_BRIGHT_LIGHT.color.argb_full);
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
        toggle_on_bg_color = BG_THEME3_BRIGHT_LIGHT.color.argb_full;
    }
    else
    {
        if (f_status.notification)
        {
            gui_obj_move(GUI_BASE(toggle_notification), TOGGLE_ON_X, TOGGLE_Y);
            gui_img_a8_recolor(toggle_notification_bg, BG_THEME3_BRIGHT_DARK.color.argb_full);
            if (f_status.message_preview)
            {
                gui_obj_move(GUI_BASE(toggle_message), TOGGLE_ON_X, TOGGLE_Y);
                gui_img_a8_recolor(toggle_message_bg, BG_THEME3_BRIGHT_DARK.color.argb_full);
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
        toggle_on_bg_color = BG_THEME3_BRIGHT_DARK.color.argb_full;
    }
    gui_img_a8_mix_alpha(toggle_notification_bg, toggle_notification_bg->fg_color_set >> 24);
}

void page_clock_settings_design(gui_obj_t *parent)
{
    quick_page_name_index = CLOCK_SETTINGS;
    gui_color_t font_color;
    gui_color_t bg_theme3_bright_color;
    gui_color_t bg_theme3_mid_color;
    gui_color_t bg_theme3_dark_color;
    gui_color_t fg_2_color;
    gui_color_t screen_bg_color;
    if (theme_bg_white)
    {
        font_color = FG_1_LIGHT;
        bg_theme3_bright_color = BG_THEME3_BRIGHT_LIGHT;
        bg_theme3_mid_color = BG_THEME3_MID_LIGHT;
        bg_theme3_dark_color = BG_THEME3_DARK_LIGHT;
        fg_2_color = FG_2_LIGHT;
        screen_bg_color = SCREEN_BG_LIGHT;
    }
    else
    {
        font_color = FG_1_DARK;
        bg_theme3_bright_color = BG_THEME3_BRIGHT_DARK;
        bg_theme3_mid_color = BG_THEME3_MID_DARK;
        bg_theme3_dark_color = BG_THEME3_DARK_DARK;
        fg_2_color = FG_2_DARK;
        screen_bg_color = SCREEN_BG_DARK;
    }

    gui_text_t *text = gui_text_create(parent, 0, 12, 45, 200, 15);
    gui_text_set(text, (void *)page_name_array[language_index][CLOCK_SETTINGS], GUI_FONT_SRC_BMP,
                 font_color,
                 strlen((void *)page_name_array[language_index][CLOCK_SETTINGS]), 30);
    gui_text_type_set(text, CAPTION_3_30_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, LEFT);

    gui_img_t *settings_bg = gui_img_create_from_mem(parent, 0, BUTTON_BG_ELLIPSE_64_40_BIN, 244, 40, 0,
                                                     0);
    gui_img_t *settings = gui_img_create_from_mem(settings_bg, 0, ICON_SETTINGS_BIN, 20, 8, 0, 0);
    gui_img_a8_recolor(settings, FG_WHITE.color.argb_full);
    gui_obj_add_event_cb(settings_bg, click_button_settings_page_clock_settings,
                         GUI_EVENT_TOUCH_CLICKED, NULL);

    gui_img_t *control_bg = gui_img_create_from_mem(parent, 0, CONTROL_BG_BIN, 12, 92, 0, 0);
    gui_img_t *button_sub_bg = gui_img_create_from_mem(control_bg, "l", BUTTON_BG_CIRCLE_BIN, 6, 6, 0,
                                                       0);
    gui_img_t *sub = gui_img_create_from_mem(control_bg, "sub", SUB_BIN, 31, 42, 0, 0);
    gui_img_t *button_add_bg = gui_img_create_from_mem(control_bg, "r", BUTTON_BG_CIRCLE_BIN, 212, 6,
                                                       0, 0);
    gui_img_t *add = gui_img_create_from_mem(control_bg, "add", ADD_BIN, 237, 31, 0, 0);
    gui_obj_hidden((void *)button_sub_bg, 1);
    gui_obj_hidden((void *)button_add_bg, 1);
    gui_obj_create_timer(GUI_BASE(button_sub_bg), 10, true, press_button_page_clock_settings);
    gui_obj_create_timer(GUI_BASE(button_add_bg), 10, true, press_button_page_clock_settings);
    gui_obj_add_event_cb(button_sub_bg, click_button_no_action, GUI_EVENT_TOUCH_LONG, NULL);
    gui_obj_add_event_cb(button_add_bg, click_button_no_action, GUI_EVENT_TOUCH_LONG, NULL);

    sprintf(page_str, ":");
    text = gui_text_create(control_bg, 0, 140, 0, 16, 90);
    gui_text_set(text, page_str, GUI_FONT_SRC_BMP, font_color, strlen(page_str), 32);
    gui_text_type_set(text, CAPTION_1_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, MID_CENTER);

    gui_img_t *hour_bg = gui_img_create_from_mem(control_bg, "l", BUTTON_BG_ELLIPSE_60_64_BIN, 80, 13,
                                                 0, 0);
    gui_img_t *hour_bg_inner = gui_img_create_from_mem(hour_bg, 0, BUTTON_BG_ELLIPSE_56_60_BIN, 2, 2, 0,
                                                       0);
    gui_obj_hidden((void *)hour_bg_inner, 1);
    gui_obj_add_event_cb(hour_bg, click_button_page_clock_settings, GUI_EVENT_TOUCH_CLICKED, NULL);
    hour_val = timeinfo->tm_hour;
    sprintf(hour_str, "%02d", hour_val);
    text = gui_text_create(hour_bg, 0, 0, 0, 60, 64);
    gui_text_set(text, hour_str, GUI_FONT_SRC_BMP, font_color, 2, 30);
    gui_text_type_set(text, CAPTION_3_30_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, MID_CENTER);

    gui_img_t *min_bg = gui_img_create_from_mem(control_bg, "r", BUTTON_BG_ELLIPSE_60_64_BIN, 156, 13,
                                                0, 0);
    gui_img_t *min_bg_inner = gui_img_create_from_mem(min_bg, 0, BUTTON_BG_ELLIPSE_56_60_BIN, 2, 2, 0,
                                                      0);
    gui_obj_hidden((void *)min_bg_inner, 1);
    gui_obj_add_event_cb(min_bg, click_button_page_clock_settings, GUI_EVENT_TOUCH_CLICKED, NULL);
    minutes_val = timeinfo->tm_min;
    sprintf(minutes_str, "%02d", minutes_val);
    text = gui_text_create(min_bg, 0, 0, 0, 60, 64);
    gui_text_set(text, minutes_str, GUI_FONT_SRC_BMP, font_color, 2, 30);
    gui_text_type_set(text, CAPTION_3_30_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, MID_CENTER);

    gui_img_a8_recolor(settings_bg, bg_theme3_bright_color.color.argb_full);
    gui_img_a8_recolor(control_bg, bg_theme3_dark_color.color.argb_full);
    gui_img_a8_fix_bg(control_bg, screen_bg_color.color.argb_full);
    gui_img_a8_recolor(button_sub_bg, bg_theme3_mid_color.color.argb_full);
    gui_img_a8_recolor(button_add_bg, bg_theme3_mid_color.color.argb_full);
    gui_img_a8_recolor(sub, fg_2_color.color.argb_full);
    gui_img_a8_recolor(add, fg_2_color.color.argb_full);
    gui_img_a8_recolor(hour_bg, bg_theme3_mid_color.color.argb_full);
    gui_img_a8_recolor(hour_bg_inner, bg_theme3_bright_color.color.argb_full);
    gui_img_a8_recolor(min_bg, bg_theme3_mid_color.color.argb_full);
    gui_img_a8_recolor(min_bg_inner, bg_theme3_bright_color.color.argb_full);
    gui_img_a8_mix_alpha(sub, sub->fg_color_set >> 24);
    gui_img_a8_mix_alpha(add, add->fg_color_set >> 24);
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
    gui_text_set(text, (void *)page_name_array[language_index][TIME_FORMAT], GUI_FONT_SRC_BMP,
                 font_color,
                 strlen((void *)page_name_array[language_index][TIME_FORMAT]), 30);
    gui_text_type_set(text, CAPTION_3_30_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, LEFT);

    gui_img_t *control_bg = gui_img_create_from_mem(parent, 0, CONTROL_BG_BIN, 12, 92, 0, 0);
    gui_img_t *button_bg = gui_img_create_from_mem(control_bg,
                                                   (void *)page_name_array[language_index][DARK_LIGHT_MODE],
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
    gui_text_set(text, (void *)page_name_array[language_index][LANGUAGE], GUI_FONT_SRC_BMP, font_color,
                 strlen((void *)page_name_array[language_index][LANGUAGE]), 30);
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
    gui_text_set(text, (void *)language_type[language_index], GUI_FONT_SRC_BMP, font_color,
                 strlen(language_type[language_index]), 30);
    gui_text_type_set(text, CAPTION_3_30_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, MID_CENTER);

    if (theme_bg_white)
    {
        gui_img_a8_recolor(control_bg, BG_THEME3_DARK_LIGHT.color.argb_full);
        gui_img_a8_recolor(button_switch_l_bg, BG_THEME3_MID_LIGHT.color.argb_full);
        gui_img_a8_recolor(button_switch_r_bg, BG_THEME3_MID_LIGHT.color.argb_full);
        gui_img_a8_recolor(switch_l, FG_1_LIGHT.color.argb_full);
        gui_img_a8_recolor(switch_r, FG_1_LIGHT.color.argb_full);
        if (language_index == 0)
        {
            gui_img_a8_recolor(switch_l, FG_2_LIGHT.color.argb_full);
        }
        else if (language_index == 12)
        {
            gui_img_a8_recolor(switch_r, FG_2_LIGHT.color.argb_full);
        }
    }
    else
    {
        gui_img_a8_recolor(control_bg, BG_THEME3_DARK_DARK.color.argb_full);
        gui_img_a8_recolor(button_switch_l_bg, BG_THEME3_MID_DARK.color.argb_full);
        gui_img_a8_recolor(button_switch_r_bg, BG_THEME3_MID_DARK.color.argb_full);
        gui_img_a8_recolor(switch_l, FG_1_DARK.color.argb_full);
        gui_img_a8_recolor(switch_r, FG_1_DARK.color.argb_full);
        if (language_index == 0)
        {
            gui_img_a8_recolor(switch_l, FG_2_DARK.color.argb_full);
        }
        else if (language_index == 12)
        {
            gui_img_a8_recolor(switch_r, FG_2_DARK.color.argb_full);
        }
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
        scroll_color = FG_BLACK;
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

    gui_list_t *list = gui_list_create(parent, 0, 0, 60, 0, 0, 56, 0,
                                       VERTICAL, note_design_page_reorder_quick_access, NULL, 0);
    gui_list_set_style(list, LIST_CLASSIC);
    gui_list_set_note_num(list, quick_page_num);
    gui_list_set_out_scope(list, 10);
    gui_list_set_offset(list, page_reorder_quick_access_list_offset_rec);
    gui_obj_create_timer(GUI_BASE(list), 10, true, list_timer_page_reorder_quick_access);

    gui_img_t *mask = gui_img_create_from_mem(parent, 0, MASK_BIN, 0, 0, 0, 0);
    gui_scroll_text_t *text = gui_scroll_text_create(parent, 0, 44, 16, 260, 30);
    gui_scroll_text_type_set(text, CAPTION_3_30_BIN, FONT_SRC_MEMADDR);
    gui_scroll_text_set(text, (void *)page_name_array[language_index][REORDER_QUICK_ACCESS],
                        GUI_FONT_SRC_BMP,
                        font_color, strlen((void *)page_name_array[language_index][REORDER_QUICK_ACCESS]), 30);
    gui_scroll_text_scroll_set(text, SCROLL_X, 20, 260, 5000, 0);

    if (quick_page_num > 2)
    {
        gui_img_t *scrollbar_bg = gui_img_create_from_mem(parent, 0, SCROLLBAR_BG_BIN, 310, 72, 0, 0);
        gui_img_t *scrollbar = gui_img_create_from_mem(parent, 0, SCROLLBAR_L_BIN, 310, 72, 0, 0);
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
    gui_text_set(text, (void *)page_name_array[language_index][SUPPORT], GUI_FONT_SRC_BMP, font_color,
                 strlen((void *)page_name_array[language_index][SUPPORT]), 30);
    gui_text_type_set(text, CAPTION_3_30_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, MID_CENTER);
    text = gui_text_create(win_tips, 0, 56, 8, 248, 40);
    gui_text_set(text, (void *)page_name_array[language_index][PRODUCT_TIPS], GUI_FONT_SRC_BMP,
                 font_color,
                 strlen((void *)page_name_array[language_index][PRODUCT_TIPS]), 30);
    gui_text_type_set(text, CAPTION_3_30_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, MID_LEFT);
    text = gui_text_create(win_reset, 0, 56, 8, 248, 40);
    gui_text_set(text, (void *)page_name_array[language_index][FACTORY_RESET], GUI_FONT_SRC_BMP,
                 font_color,
                 strlen((void *)page_name_array[language_index][FACTORY_RESET]), 30);
    gui_text_type_set(text, CAPTION_3_30_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, MID_LEFT);
}

/* Tools page */
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
    gui_text_set(text, (void *)page_name_array[language_index][TIMER], GUI_FONT_SRC_BMP, font_color,
                 strlen((void *)page_name_array[language_index][TIMER]), 30);
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
    gui_text_set(text, (void *)min_str[language_index], GUI_FONT_SRC_BMP, font_color,
                 strlen(min_str[language_index]), 20);
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
    gui_text_set(text, (void *)page_name_array[language_index][FLASHLIGHT], GUI_FONT_SRC_BMP,
                 font_color,
                 strlen((void *)page_name_array[language_index][FLASHLIGHT]), 30);
    gui_text_type_set(text, CAPTION_3_30_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, LEFT);

    gui_img_t *control_bg = gui_img_create_from_mem(parent, 0, CONTROL_BG_BIN, 12, 92, 0, 0);
    gui_img_t *icon = gui_img_create_from_mem(control_bg, 0, ICON_FLASHLIGHT_ON_BIN, 138, 28, 0, 0);
    gui_img_a8_recolor(control_bg,
                       theme_bg_white ? BG_THEME2_BRIGHT_LIGHT.color.argb_full : BG_THEME2_BRIGHT_DARK.color.argb_full);
    gui_img_a8_recolor(icon, FG_WHITE.color.argb_full);
    gui_obj_add_event_cb(control_bg, click_button_page_flashlight, GUI_EVENT_TOUCH_CLICKED, NULL);
}

void page_silentnow_design(gui_obj_t *parent)
{
    quick_page_name_index = SILENTNOW;
    gui_color_t font_color;
    gui_color_t bg_theme2_bright_color;
    gui_color_t bg_theme2_dark_color;
    gui_color_t bg_2_color;
    gui_color_t fg_3_color;
    if (theme_bg_white)
    {
        font_color = FG_1_LIGHT;
        bg_theme2_bright_color = BG_THEME2_BRIGHT_LIGHT;
        bg_theme2_dark_color = BG_THEME2_DARK_LIGHT;
        bg_2_color = BG_2_LIGHT;
        fg_3_color = FG_3_LIGHT;
    }
    else
    {
        font_color = FG_1_DARK;
        bg_theme2_bright_color = BG_THEME2_BRIGHT_DARK;
        bg_theme2_dark_color = BG_THEME2_DARK_DARK;
        bg_2_color = BG_2_DARK;
        fg_3_color = FG_3_DARK;
    }
    silent_time_val = 900;

    gui_text_t *text = gui_text_create(parent, 0, 12, 45, 200, 15);
    gui_text_set(text, (void *)page_name_array[language_index][SILENTNOW], GUI_FONT_SRC_BMP, font_color,
                 strlen((void *)page_name_array[language_index][SILENTNOW]), 30);
    gui_text_type_set(text, CAPTION_3_30_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, LEFT);

    gui_img_t *play_bg = gui_img_create_from_mem(parent, 0, BUTTON_BG_ELLIPSE_64_40_BIN, 244, 40, 0, 0);
    gui_img_t *play = gui_img_create_from_mem(play_bg, 0, TIMER_PLAY_BIN, 24, 10, 0, 0);
    gui_img_a8_recolor(play, FG_WHITE.color.argb_full);
    gui_obj_add_event_cb(play_bg, click_button_page_silentnow, GUI_EVENT_TOUCH_CLICKED, NULL);

    gui_img_t *control_bg = gui_img_create_from_mem(parent, 0, CONTROL_BG_BIN, 12, 92, 0, 0);
    gui_img_a8_recolor(control_bg, bg_theme2_dark_color.color.argb_full);
    if (f_status.silentnow)
    {
        gui_img_a8_recolor(play_bg, bg_2_color.color.argb_full);
        gui_img_a8_mix_alpha(play_bg, play_bg->fg_color_set >> 24);
        gui_img_set_image_data(play, ICON_CANCEL_BIN);
        gui_img_refresh_size(play);

        gui_img_t *icon = gui_img_create_from_mem(control_bg, 0, ICON_SILENT_CIRCLE_BIN, 20, 29, 0, 0);
        gui_img_a8_recolor(icon, font_color.color.argb_full);

        gui_text_t *text = gui_text_create(control_bg, 0, 60, 0, 90, 90);
        gui_text_set(text, "Starts in", GUI_FONT_SRC_BMP, font_color, 9, 20);
        gui_text_type_set(text, HEADING_1_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(text, MID_LEFT);

        sprintf(page_str, "00: %02d :%02d", silent_time_val / 60, silent_time_val % 60);
        text = gui_text_create(control_bg, 0, 166, 0, 150, 90);
        gui_text_set(text, page_str, GUI_FONT_SRC_BMP, font_color, 10, 30);
        gui_text_type_set(text, CAPTION_3_30_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(text, MID_LEFT);
        gui_obj_create_timer(GUI_BASE(text), 1000, true, timer_page_silentnow);
    }
    else
    {
        gui_img_a8_recolor(play_bg, bg_theme2_bright_color.color.argb_full);

        gui_img_t *icon = gui_img_create_from_mem(control_bg, 0, ICON_SILENT_CIRCLE_SMALL_BIN, 19, 32, 0,
                                                  0);
        gui_img_a8_recolor(icon, font_color.color.argb_full);
        icon = gui_img_create_from_mem(control_bg, 0, ICON_SILENT_SMALL_BIN, 138, 35, 0, 0);
        gui_img_a8_recolor(icon, font_color.color.argb_full);
        icon = gui_img_create_from_mem(control_bg, 0, ICON_BELL_BOT_BIN, 263, 44, 0, 0);
        gui_img_a8_recolor(icon, fg_3_color.color.argb_full);
        gui_img_a8_mix_alpha(icon, icon->fg_color_set >> 24);
        icon = gui_img_create_from_mem(control_bg, 0, ICON_BELL_TOP_BIN, 257, 33, 0, 0);
        gui_img_a8_recolor(icon, font_color.color.argb_full);

        sprintf(page_str, "15 %s", min_str[language_index]);
        gui_text_t *text = gui_text_create(control_bg, 0, 52, 0, 70, 90);
        gui_text_set(text, page_str, GUI_FONT_SRC_BMP, font_color, 6, 20);
        gui_text_type_set(text, HEADING_1_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(text, MID_LEFT);
        sprintf(page_str + 7, "30 %s", min_str[language_index]);
        text = gui_text_create(control_bg, 0, 168, 0, 70, 90);
        gui_text_set(text, page_str + 7, GUI_FONT_SRC_BMP, font_color, 6, 20);
        gui_text_type_set(text, HEADING_1_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(text, MID_LEFT);
    }
}

void page_volume_unit_meter_design(gui_obj_t *parent)
{
    quick_page_name_index = VOLUME_UNIT_METER;
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
    gui_text_set(text, (void *)page_name_array[language_index][VOLUME_UNIT_METER], GUI_FONT_SRC_BMP,
                 font_color,
                 strlen((void *)page_name_array[language_index][VOLUME_UNIT_METER]), 30);
    gui_text_type_set(text, CAPTION_3_30_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, LEFT);

    gui_img_t *icon_l = gui_img_create_from_mem(parent, 0, ICON_L_BIN, 12, 92, 0, 0);
    gui_img_t *icon_r = gui_img_create_from_mem(parent, 0, ICON_R_BIN, 12, 142, 0, 0);
    gui_img_a8_recolor(icon_l, font_color.color.argb_full);
    gui_img_a8_recolor(icon_r, font_color.color.argb_full);

    gui_win_t *win_l = gui_win_create(parent, 0, 72, 95, 207, 34);
    gui_win_t *win_r = gui_win_create(parent, 0, 72, 145, 207, 34);
    int16_t x = 0;
    for (uint8_t i = 0; i < 13; i++)
    {
        gui_img_create_from_mem(win_l, 0, STRENGTH_BAR_BIN, x, 0, 0, 0);
        gui_img_create_from_mem(win_r, 0, STRENGTH_BAR_BIN, x, 0, 0, 0);
        x += 16;
    }
    update_strength_bar((void *)win_l, vu_strength_l);
    update_strength_bar((void *)win_r, vu_strength_r);
    gui_obj_create_timer(GUI_BASE(win_l), 500, true, win_timer_page_volume_unit_meter);
    gui_obj_start_timer(GUI_BASE(win_l));
}

void page_qrcode_design(gui_obj_t *parent)
{
    quick_page_name_index = JBL_HEADPHONES_APP;
    gui_img_t *img = gui_img_create_from_mem(parent, 0, QRCODE_BIN, 104, 40, 0, 0);
    gui_img_set_mode(img, IMG_2D_SW_SRC_OVER_MODE);
}