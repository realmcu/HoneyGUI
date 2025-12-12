/*============================================================================*
 *                        Phone Module Header
 *============================================================================*/
#ifndef __APP_PHONE_ADAPTER_H__
#define __APP_PHONE_ADAPTER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "gui_obj.h"

/*============================================================================*
 *                           Function Declarations
 *============================================================================*/

/* Phone Control Functions */

/**
 * @brief Accept incoming call
 */
void app_dashboard_data_set_accept_call(void);

/**
 * @brief Reject or end call
 */
void app_dashboard_data_set_reject_end_call(void);

/**
 * @brief Update phone status from communication protocol
 * @param key Event key (incoming call, accept, refuse)
 * @param phone_number Phone number data
 * @param length Length of phone number
 */
void app_dashboard_data_update_phone_status(uint8_t key, const uint8_t *phone_number,
                                            uint16_t length);

/* Phone Popup UI Functions */

/**
 * @brief Create phone popup UI
 * @param parent Parent widget
 */
void app_phone_popup_create(gui_obj_t *parent);

/**
 * @brief Update phone popup display based on dashboard state
 */
void app_phone_popup_update(void);

/**
 * @brief Show phone popup with phone number
 * @param phone_number Phone number string
 */
void app_phone_popup_show(const char *phone_number);

/**
 * @brief Hide phone popup
 */
void app_phone_popup_hide(void);

/**
 * @brief Set calling mode
 * @param is_calling True if in call, false if incoming
 */
void app_phone_popup_set_calling_mode(bool is_calling);

#ifdef __cplusplus
}
#endif

#endif /* __APP_PHONE_ADAPTER_H__ */
