/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "dashboard_adapt.h"
#include "gui_api_os.h"
#include "gui_win.h"
#include "gui_text.h"
#include "gui_img.h"
#include "root_image_800_480/ui_resource.h"
#include <string.h>
#ifndef _HONEYGUI_SIMULATOR_
#if CONFIG_REALTEK_APP_DASHBOARD_DEMO_SUPPORT
#include "communicate_protocol.h"
#include "communicate_parse_notify.h"
#include "communicate_parse.h"
#endif
#endif
/*============================================================================*
 *                           Types
 *============================================================================*/
typedef enum
{
    PHONE_STATUS_NONE = 0,
    PHONE_STATUS_ONGOING,
    PHONE_STATUS_ACCEPT,
} phone_status_t;

typedef struct
{
    uint8_t phone_number[11];
    uint8_t phone_number_len;
    phone_status_t status;
} phone_data_t;

/*============================================================================*
 *                           Variables
 *============================================================================*/
static phone_data_t current_phone_data = {0};

/* Phone popup UI components */
static gui_win_t *phone_popup = NULL;
static gui_text_t *phone_number_text = NULL;
static gui_img_t *accept_btn = NULL;
static gui_img_t *reject_btn = NULL;
static bool popup_created = false;

/*============================================================================*
 *                           Phone Call Adapter Functions
 *============================================================================*/

/**
 * @brief Get current phone status
 */
static phone_data_t *get_phone_data(void)
{
    return &current_phone_data;
}

/**
 * @brief Accept incoming call
 */
void app_dashboard_data_set_accept_call(void)
{
#ifndef _HONEYGUI_SIMULATOR_
    phone_data_t *phone = get_phone_data();

#if CONFIG_REALTEK_APP_DASHBOARD_DEMO_SUPPORT
    if (phone->status == PHONE_STATUS_ONGOING)
    {
        struct protocol_pack p = {0};
        p.l2_cmd_id = NOTIFY_COMMAND_ID;
        p.l2_key = NOTIFY_CALL_EVENT_ACCEPT;
        p.l2_length = 1;
        p.l2_payload[0] = 0;
        package_prepare_send(&p);

//        gui_log("Dashboard: Accept call\n");
    }
#endif
#endif
}

/**
 * @brief Reject or end call
 */
void app_dashboard_data_set_reject_end_call(void)
{
    phone_data_t *phone = get_phone_data();
    GUI_UNUSED(phone);
#ifndef _HONEYGUI_SIMULATOR_
#if CONFIG_REALTEK_APP_DASHBOARD_DEMO_SUPPORT
    if ((phone->status == PHONE_STATUS_ACCEPT) ||
        (phone->status == PHONE_STATUS_ONGOING))
    {
        struct protocol_pack p = {0};
        p.l2_cmd_id = NOTIFY_COMMAND_ID;
        p.l2_key = NOTIFY_CALL_EVENT_REJECT_END;
        p.l2_length = 1;
        p.l2_payload[0] = 0;
        package_prepare_send(&p);

//        gui_log("Dashboard: Reject/End call\n");
    }
#endif
#endif
}

/**
 * @brief Update phone status from communication protocol
 * @param key Event key (incoming call, accept, refuse)
 * @param phone_number Phone number data
 * @param length Length of phone number
 */
void app_dashboard_data_update_phone_status(uint8_t key, const uint8_t *phone_number,
                                            uint16_t length)
{
    if (length > 11)
    {
        gui_log("Dashboard: Phone number too long\n");
        return;
    }
    GUI_UNUSED(key);
    GUI_UNUSED(phone_number);
    GUI_UNUSED(length);
#ifndef _HONEYGUI_SIMULATOR_
    phone_data_t *phone = get_phone_data();
    gui_dashboard_t *info = get_dashboard_info();

    memset(phone->phone_number, 0, sizeof(phone->phone_number));

    if (key == KEY_INCOMMING_CALL)
    {
        phone->status = PHONE_STATUS_ONGOING;

        // Update dashboard info
        if (info != NULL)
        {
            info->phone_active = 1;
            info->phone_calling = 0;
            memcpy(info->phone_number, phone_number, length);
            info->phone_number[length] = '\0';
            info->phone_number_len = length;
        }
    }
    else if (key == KEY_INCOMMING_CALL_ACCEPT)
    {
        phone->status = PHONE_STATUS_ACCEPT;

        // Update dashboard info
        if (info != NULL)
        {
            info->phone_calling = 1;
        }
    }
    else if (key == KEY_INCOMMING_CALL_REFUSE)
    {
        phone->status = PHONE_STATUS_NONE;

        // Update dashboard info
        if (info != NULL)
        {
            info->phone_active = 0;
            info->phone_calling = 0;
            memset(info->phone_number, 0, sizeof(info->phone_number));
            info->phone_number_len = 0;
        }
    }

    phone->phone_number_len = length;
    memcpy(phone->phone_number, phone_number, length);
#endif
//    gui_log("Dashboard: Phone status updated, key=%d, number_len=%d, active=%d, calling=%d\n",
//            key, length, info ? info->phone_active : 0, info ? info->phone_calling : 0);
}

/*============================================================================*
 *                           Phone Popup UI Functions
 *============================================================================*/

/**
 * @brief Accept button callback
 */
static void on_accept_call(void *obj, gui_event_t e, void *param)
{
//    gui_log("Phone: Accept button clicked\n");
    GUI_UNUSED(obj);
    GUI_UNUSED(e);
    GUI_UNUSED(param);
    app_dashboard_data_set_accept_call();
}

/**
 * @brief Reject button callback
 */
static void on_reject_call(void *obj, gui_event_t e, void *param)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);
    GUI_UNUSED(param);
//    gui_log("Phone: Reject button clicked\n");
    app_dashboard_data_set_reject_end_call();
}

/**
 * @brief Create phone popup UI
 * @param parent Parent widget
 */
void app_phone_popup_create(gui_obj_t *parent)
{
    phone_popup = gui_win_create(parent, "phone_popup", 253, 360, 295, 49);
    gui_obj_hidden(GUI_BASE(phone_popup), true);

    gui_img_create_from_mem(phone_popup, "popup_bg", TEL_BG_BIN, 0, 0, 0, 0);

    // reject button
    reject_btn = gui_img_create_from_mem(phone_popup, "reject", TEL_REJECT_ICON_BIN, 10, 18, 36, 36);
    gui_img_set_mode(reject_btn, IMG_SRC_OVER_MODE);
    gui_obj_add_event_cb(GUI_BASE(reject_btn), (gui_event_cb_t)on_reject_call,
                         GUI_EVENT_TOUCH_CLICKED, NULL);

    // accept button
    accept_btn = gui_img_create_from_mem(phone_popup, "accept", TEL_ACCPET_ICON_BIN, 247, 18, 36, 36);
    gui_img_set_mode(accept_btn, IMG_SRC_OVER_MODE);
    gui_obj_add_event_cb(GUI_BASE(accept_btn), (gui_event_cb_t)on_accept_call,
                         GUI_EVENT_TOUCH_CLICKED, NULL);

    // phone number text
    phone_number_text = gui_text_create(phone_popup, "phone_num", 70, 25, 158, 30);
    gui_text_set(phone_number_text, "123456789123", GUI_FONT_SRC_TTF,
                 gui_rgb(255, 255, 255), strlen("123456789123"), 28);
    gui_text_type_set(phone_number_text, HARMONYOS_SANS_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(phone_number_text, CENTER);
}

/**
 * @brief Update phone popup display
 */
void app_phone_popup_update(void)
{
    if (!popup_created || phone_popup == NULL)
    {
        return;
    }

    gui_dashboard_t *info = get_dashboard_info();
    if (info == NULL)
    {
        return;
    }

    if (info->phone_active)
    {
        // Show popup
        gui_obj_hidden(GUI_BASE(phone_popup), false);

        if (info->phone_calling)
        {
            // In call mode - show "Calling..."
            gui_text_content_set(phone_number_text, "Calling...", 10);

            // Hide accept button, show only reject (hang up)
            gui_obj_hidden(GUI_BASE(accept_btn), true);
            gui_obj_hidden(GUI_BASE(reject_btn), false);
        }
        else
        {
            // Incoming call mode - show phone number
            if (info->phone_number_len > 0)
            {
                gui_text_content_set(phone_number_text, info->phone_number, info->phone_number_len);
            }
            else
            {
                gui_text_content_set(phone_number_text, "Unknown", 7);
            }

            // Show both buttons
            gui_obj_hidden(GUI_BASE(accept_btn), false);
            gui_obj_hidden(GUI_BASE(reject_btn), false);
        }
    }
    else
    {
        // Hide popup
        gui_obj_hidden(GUI_BASE(phone_popup), true);
    }
}

/**
 * @brief Show phone popup with phone number
 * @param phone_number Phone number string
 */
void app_phone_popup_show(const char *phone_number)
{
    if (!popup_created || phone_popup == NULL)
    {
        return;
    }

    gui_obj_hidden(GUI_BASE(phone_popup), false);

    if (phone_number != NULL && phone_number_text != NULL)
    {
        gui_text_content_set(phone_number_text, (char *)phone_number, strlen(phone_number));
    }
}

/**
 * @brief Hide phone popup
 */
void app_phone_popup_hide(void)
{
    if (!popup_created || phone_popup == NULL)
    {
        return;
    }

    gui_obj_hidden(GUI_BASE(phone_popup), true);
}

/**
 * @brief Set calling mode
 * @param is_calling True if in call, false if incoming
 */
void app_phone_popup_set_calling_mode(bool is_calling)
{
    if (!popup_created)
    {
        return;
    }

    if (is_calling)
    {
        gui_text_content_set(phone_number_text, "Calling...", 10);
        gui_obj_hidden(GUI_BASE(accept_btn), true);
    }
    else
    {
        gui_obj_hidden(GUI_BASE(accept_btn), false);
    }
}
