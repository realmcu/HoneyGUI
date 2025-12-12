/*
 * File      : app_message_adapter.c
 * Brief     : Message notification adapter implementation
 */
#include "app_message_adapter.h"
#include "dashboard_adapt.h"
#include "gui_img.h"
#include "gui_text.h"
#include "gui_obj.h"
#include "root_image_800_480/ui_resource.h"
#include <string.h>

#ifndef _WIN32
#include "trace.h"
#include "os_timer.h"
#endif

static gui_message_info_t g_message_info = {0};
static gui_win_t *message_popup_win = NULL;
static gui_text_t *message_text = NULL;

#ifndef _WIN32
static void *message_timer = NULL;

static void message_timer_cb(void *p_handle)
{
    APP_PRINT_INFO0("message_timer_cb: auto dismiss message");
    gui_message_dismiss();
    os_timer_stop(&p_handle);
}
#endif

void gui_message_notify_update(const uint8_t *pValue, uint16_t length, uint8_t message_type)
{
    if (pValue == NULL || length == 0)
    {
        return;
    }

#ifndef _WIN32
    APP_PRINT_INFO1("gui_message_notify_update: length=%d", length);
    APP_PRINT_INFO1("gui_message_notify_update: type=%d", message_type);
#endif

    // Clear previous message
    memset(g_message_info.content, 0, sizeof(g_message_info.content));

    // Copy new message content
    if (length > MESSAGE_CONTENT_MAX_LEN)
    {
        length = MESSAGE_CONTENT_MAX_LEN;
    }

    memcpy(g_message_info.content, pValue, length);
    g_message_info.content_len = length;
    g_message_info.message_type = message_type;
    g_message_info.is_visible = true;

    // Show popup directly
    gui_message_popup_show(g_message_info.content, g_message_info.content_len,
                           g_message_info.message_type);

#ifndef _WIN32
    // Auto dismiss after 3 seconds
    if (message_timer == NULL)
    {
        os_timer_create(&message_timer, "msg-timer", 0, 3000, false, message_timer_cb);
    }
    os_timer_restart(&message_timer, 3000);
#endif
}

void gui_message_get_info(gui_message_info_t *info)
{
    if (info)
    {
        memcpy(info, &g_message_info, sizeof(gui_message_info_t));
    }
}

void gui_message_dismiss(void)
{
    memset(g_message_info.content, 0, sizeof(g_message_info.content));
    g_message_info.content_len = 0;
    g_message_info.is_visible = false;

    // Hide popup directly
    gui_message_popup_hide();
}

void gui_message_popup_create(gui_obj_t *parent)
{
    // Create popup window
    message_popup_win = gui_win_create(parent, "message_popup", 0, 0, 800, 480);
    gui_obj_hidden((gui_obj_t *)message_popup_win, true);

    // Create background (semi-transparent overlay)
    gui_img_t *message_bg = gui_img_create_from_mem(message_popup_win, "msg_bg", MESSAGE_BG_BIN, 221, 0,
                                                    0, 0);

    // Create message text - adjust width to fit background
    message_text = gui_text_create(message_bg, "msg_text", 70, 20, 220, 100);
    gui_text_set(message_text, "", GUI_FONT_SRC_BMP, APP_COLOR_WHITE, 20, 32);
    gui_text_mode_set(message_text, MULTI_LEFT);

    return;
}
void gui_message_popup_show(const uint8_t *message, uint8_t length, uint8_t message_type)
{
    (void)message_type; // Unused parameter

    if (message_popup_win == NULL || message_text == NULL)
    {
        return;
    }

    // Update message text
    char display_text[MESSAGE_CONTENT_MAX_LEN + 1] = {0};
    if (length > 0 && length <= MESSAGE_CONTENT_MAX_LEN)
    {
        memcpy(display_text, message, length);
        display_text[length] = '\0';
    }

    // Use same font size as creation (32)
    gui_text_set(message_text, display_text, GUI_FONT_SRC_BMP, APP_COLOR_WHITE, 20, 32);

    // Show popup
    gui_obj_hidden((gui_obj_t *)message_popup_win, false);
}

void gui_message_popup_hide(void)
{
    if (message_popup_win)
    {
        gui_obj_hidden((gui_obj_t *)message_popup_win, true);
    }
}
