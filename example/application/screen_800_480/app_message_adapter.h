/*
 * File      : app_message_adapter.h
 * Brief     : Message notification adapter for dashboard application
 */
#ifndef __APP_MESSAGE_ADAPTER_H__
#define __APP_MESSAGE_ADAPTER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "gui_win.h"

#define MESSAGE_CONTENT_MAX_LEN 40

typedef struct
{
    uint8_t content[MESSAGE_CONTENT_MAX_LEN];
    uint8_t content_len;
    uint8_t message_type;
    bool is_visible;
} gui_message_info_t;

/**
 * @brief Update message notification status
 * @param pValue Message content pointer
 * @param length Message content length
 * @param message_type Message type (e.g., WeChat, SMS, etc.)
 */
void gui_message_notify_update(const uint8_t *pValue, uint16_t length, uint8_t message_type);

/**
 * @brief Get current message info
 * @param info Pointer to store message info
 */
void gui_message_get_info(gui_message_info_t *info);

/**
 * @brief Dismiss message notification
 */
void gui_message_dismiss(void);

/**
 * @brief Create message notification popup window
 * @param parent Parent widget
 * @return None
 */
void gui_message_popup_create(gui_obj_t *parent);

/**
 * @brief Show message notification popup
 * @param message Message content
 * @param length Message length
 * @param message_type Message type
 */
void gui_message_popup_show(const uint8_t *message, uint8_t length, uint8_t message_type);

/**
 * @brief Hide message notification popup
 */
void gui_message_popup_hide(void);

#ifdef __cplusplus
}
#endif

#endif /* __APP_MESSAGE_ADAPTER_H__ */
