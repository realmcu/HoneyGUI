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
#define CURRENT_VIEW_NAME CALL_VIEW


/*============================================================================*
 *                           Function Declaration
 *============================================================================*/
static void call_view_design(gui_view_t *view);

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
    .on_switch_in = call_view_design,
};

static const char *descriptor_rec = NULL;

static const char *connecting_str[13] =
{
    "Connecting...",        // English
    "Connexion...",         // French
    "Verbinden...",         // German
    "Conectando...",        // Spanish
    "Connessione...",       // Italian
    "Verbinden...",         // Dutch
    "A estabelecer...",     // Portuguese
    "Подключение…",         // Russian
    "正在连接……",           // Chinese Simplified
    "接続中…",              // Japanese
    "연결 중...",           // Korean
    "กำลังเชื่อมต่อ...",        // Thai
    "Đang kết nối…"         // Vietnamese
};
static const char *calling_str[13] =
{
    "Calling...",        // English
    "Appel...",          // French
    "...wird angerufen", // German
    "Llamando...",       // Spanish
    "Chiamo...",         // Italian
    "Bellen...",         // Dutch
    "A chamar...",       // Portuguese
    "Вызов…",           // Russian
    "正在呼叫……",       // Chinese Simplified
    "通話中…",          // Japanese
    "통화 중...",        // Korean
    "กำลังโทร...",      // Thai
    "Đang gọi…"         // Vietnamese
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

static void switch_call_view(void *msg)
{
    GUI_UNUSED(msg);

    gui_view_t *view_c = gui_view_get_current();
    if (view_c)
    {
        gui_view_set_animate_step(view_c, 400);
        gui_view_switch_direct(view_c, descriptor.name, SWITCH_OUT_NONE_ANIMATION,
                               SWITCH_IN_NONE_ANIMATION);
    }
}

static void click_button_back(void *obj, gui_event_t e, void *param)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);
    GUI_UNUSED(param);
    gui_view_switch_direct(current_view, QUICK_VIEW, SWITCH_OUT_ANIMATION_MOVE_TO_RIGHT,
                           SWITCH_IN_ANIMATION_MOVE_FROM_LEFT);
}

static void click_button_cancel(void *obj, gui_event_t e, void *param)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);
    GUI_UNUSED(param);
    // to do: cancel call
    if (gui_call && gui_call->call_end)
    {
        gui_call->call_end();
    }
    f_status.call = CALL_COMING; //reset status
    if (!descriptor_rec)
    {
        descriptor_rec = QUICK_VIEW;
    }
    gui_view_set_animate_step(current_view, 400);
    gui_view_switch_direct(current_view, descriptor_rec, SWITCH_OUT_NONE_ANIMATION,
                           SWITCH_IN_NONE_ANIMATION);
}

static void click_button_take(void *obj, gui_event_t e, void *param)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);
    GUI_UNUSED(param);
    // to do: take call
    if (gui_call && gui_call->call_start)
    {
        gui_call->call_start();
    }
    f_status.call = CALL_ING;
    msg_2_regenerate_current_view();
}

static void click_button_mute(void *obj, gui_event_t e, void *param)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);
    GUI_UNUSED(param);
    // to do: mute
    f_status.mute = !f_status.mute;
    if (gui_call && gui_call->call_set_speaker)
    {
        gui_call->call_set_speaker(!f_status.mute);
    }
    msg_2_regenerate_current_view();
}

static void calling_timer(void *p)
{
    GUI_UNUSED(p);
    sprintf(calltime_str, "%02d:%02d", calltime_val / 60, calltime_val % 60);
    gui_text_content_set((gui_text_t *)p, calltime_str, strlen(calltime_str));
}

static void call_wait_timer(void *p)
{
    GUI_UNUSED(p);
    // wait for calling
    if (f_status.call == CALL_ING)
    {
        msg_2_regenerate_current_view();
    }
}

static void call_view_design(gui_view_t *view)
{
    gui_view_set_animate_step(view, 10);
    if (gui_view_get_current())
    {
        descriptor_rec = gui_view_get_current()->descriptor->name;
    }
    gui_obj_t *parent = GUI_BASE(view);

    switch (f_status.call)
    {
    case CALL_COMING:
        {
            gui_text_t *text = gui_text_create(parent, 0, 0, 40, 320, 40);
            gui_text_set(text, call_source_str, GUI_FONT_SRC_BMP, FG_1_DARK, strlen(call_source_str), 20);
            gui_text_type_set(text, HEADING_1_BIN, FONT_SRC_MEMADDR);
            gui_text_mode_set(text, MID_CENTER);

            gui_img_t *button_l = gui_img_create_from_mem(parent, 0, BUTTON_BG_ELLIPSE_134_66_BIN, 12, 92,
                                                          0, 0);
            gui_img_t *button_r = gui_img_create_from_mem(parent, 0, BUTTON_BG_ELLIPSE_134_66_BIN, 174, 92,
                                                          0, 0);
            gui_img_t *icon_l = gui_img_create_from_mem(button_l, 0, ICON_CANCEL_BIG_BIN, 54, 20, 0, 0);
            gui_img_t *icon_r = gui_img_create_from_mem(button_r, 0, ICON_CALL_BIG_BIN, 47, 14, 0, 0);
            gui_obj_add_event_cb(button_l, click_button_cancel, GUI_EVENT_TOUCH_CLICKED, NULL);
            gui_obj_add_event_cb(button_r, click_button_take, GUI_EVENT_TOUCH_CLICKED, NULL);
            gui_img_a8_recolor(button_l, FG_WARNING.color.argb_full);
            gui_img_a8_recolor(button_r, FG_NORMAL.color.argb_full);
            gui_img_a8_recolor(icon_l, FG_WHITE.color.argb_full);
            gui_img_a8_recolor(icon_r, icon_l->fg_color_set);
            f_status.mute = 0;
        }
        break;
    case CALL_ING:
        {
            gui_color_t mute_color;
            gui_color_t mute_bg_color;
            if (f_status.mute)
            {
                mute_color = FG_WARNING;
                mute_bg_color = FG_WHITE;
            }
            else
            {
                mute_color = FG_WHITE;
                mute_bg_color = BG_2_DARK;
            }
            gui_win_t *win_icon_back = (gui_win_t *)gui_win_create(parent, 0, 0, 0, 52, 52);
            gui_img_t *icon_back = gui_img_create_from_mem(win_icon_back, 0, ICON_BACK_BIN, 0, 0, 0, 0);
            gui_img_a8_recolor(icon_back, FG_1_DARK.color.argb_full);
            gui_obj_add_event_cb(win_icon_back, click_button_back, GUI_EVENT_TOUCH_CLICKED, NULL);

            gui_text_t *text = gui_text_create(parent, 0, 12, 40, 320, 40);
            gui_text_set(text, call_source_str, GUI_FONT_SRC_BMP, FG_1_DARK, strlen(call_source_str), 20);
            gui_text_type_set(text, HEADING_1_BIN, FONT_SRC_MEMADDR);
            gui_text_mode_set(text, MID_LEFT);

            sprintf(calltime_str, "%02d:%02d", calltime_val / 60, calltime_val % 60);
            text = gui_text_create(parent, 0, 200, 40, 108, 40);
            gui_text_set(text, calltime_str, GUI_FONT_SRC_BMP, FG_NORMAL, strlen(calltime_str), 20);
            gui_text_type_set(text, HEADING_1_BIN, FONT_SRC_MEMADDR);
            gui_text_mode_set(text, MID_RIGHT);
            gui_obj_create_timer((void *)text, 1000, true, calling_timer);

            gui_img_t *icon_call = gui_img_create_from_mem(parent, 0, ICON_CALL_BIN, 250, 50, 0, 0);
            gui_img_a8_recolor(icon_call, FG_NORMAL.color.argb_full);

            gui_img_t *button_l = gui_img_create_from_mem(parent, 0, BUTTON_BG_ELLIPSE_134_66_BIN, 12, 92,
                                                          0, 0);
            gui_img_t *button_r = gui_img_create_from_mem(parent, 0, BUTTON_BG_ELLIPSE_134_66_BIN, 174, 92,
                                                          0, 0);
            gui_img_t *icon_l = gui_img_create_from_mem(button_l, 0, ICON_CANCEL_BIG_BIN, 54, 20, 0, 0);
            gui_img_t *icon_r = gui_img_create_from_mem(button_r, 0, ICON_MUTE_BIN, 47, 14, 0, 0);
            gui_obj_add_event_cb(button_l, click_button_cancel, GUI_EVENT_TOUCH_CLICKED, NULL);
            gui_obj_add_event_cb(button_r, click_button_mute, GUI_EVENT_TOUCH_CLICKED, NULL);
            gui_img_a8_recolor(button_l, FG_WARNING.color.argb_full);
            gui_img_a8_recolor(button_r, mute_bg_color.color.argb_full);
            gui_img_a8_recolor(icon_l, FG_WHITE.color.argb_full);
            gui_img_a8_recolor(icon_r, mute_color.color.argb_full);
            gui_img_a8_mix_alpha(button_r, button_r->fg_color_set >> 24);
        }
        break;
    case CALL_OUTGOING:
        {
            gui_obj_create_timer(parent, 1000, true, call_wait_timer);
            gui_text_t *text = gui_text_create(parent, 0, 0, 40, 320, 40);
            gui_text_set(text, (void *)calling_str[language_index], GUI_FONT_SRC_BMP, FG_1_DARK,
                         strlen(calling_str[language_index]), 20);
            gui_text_type_set(text, HEADING_1_BIN, FONT_SRC_MEMADDR);
            gui_text_mode_set(text, MID_CENTER);

            gui_img_t *button = gui_img_create_from_mem(parent, 0, BUTTON_BG_ELLIPSE_134_66_BIN, 93, 92,
                                                        0, 0);
            gui_img_t *icon = gui_img_create_from_mem(button, 0, ICON_CANCEL_BIG_BIN, 54, 20, 0, 0);
            gui_obj_add_event_cb(button, click_button_cancel, GUI_EVENT_TOUCH_CLICKED, NULL);
            gui_img_a8_recolor(button, FG_WARNING.color.argb_full);
            gui_img_a8_recolor(icon, FG_WHITE.color.argb_full);
            f_status.mute = 0;
        }
        break;
    case CALL_CONNECTING:
        {
            gui_obj_create_timer(parent, 1000, true, call_wait_timer);
            gui_text_t *text = gui_text_create(parent, 0, 0, 40, 320, 40);
            gui_text_set(text, (void *)connecting_str[language_index], GUI_FONT_SRC_BMP, FG_1_DARK,
                         strlen(connecting_str[language_index]), 20);
            gui_text_type_set(text, HEADING_1_BIN, FONT_SRC_MEMADDR);
            gui_text_mode_set(text, MID_CENTER);

            gui_img_t *button_l = gui_img_create_from_mem(parent, 0, BUTTON_BG_ELLIPSE_134_66_BIN, 12, 92,
                                                          0, 0);
            gui_img_t *button_r = gui_img_create_from_mem(parent, 0, BUTTON_BG_ELLIPSE_134_66_BIN, 174, 92,
                                                          0, 0);
            gui_img_t *icon_l = gui_img_create_from_mem(button_l, 0, ICON_CANCEL_BIG_BIN, 54, 20, 0, 0);
            gui_img_t *icon_r = gui_img_create_from_mem(button_r, 0, ICON_CALL_BIG_BIN, 47, 14, 0, 0);
            gui_img_a8_recolor(button_l, FG_WARNING.color.argb_full);
            gui_img_a8_recolor(button_r, FG_NORMAL.color.argb_full);
            gui_img_a8_recolor(icon_l, FG_WHITE.color.argb_full);
            gui_img_a8_recolor(icon_r, icon_l->fg_color_set);
            gui_img_a8_mix_alpha(button_l, 122);
            gui_img_a8_mix_alpha(button_r, 122);
            gui_img_a8_mix_alpha(icon_l, 122);
            gui_img_a8_mix_alpha(icon_r, 122);
            f_status.mute = 0;
        }
        break;
    default:
        break;
    }
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/
void msg_2_switch_call_view(void)
{
    gui_msg_t msg =
    {
        .event = GUI_EVENT_USER_DEFINE,
        .cb = switch_call_view,
    };
    gui_send_msg_to_server(&msg);
}