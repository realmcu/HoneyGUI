/*
 * File      : app_dashboard_data.h
 */
#ifndef __APP_DASHBOARD_DATA_H__
#define __APP_DASHBOARD_DATA_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <gui_img.h>
#include <gui_win.h>
#include <gui_text.h>
#include <gui_obj.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include "gui_server.h"
#include "root_image_800_480/ui_resource.h"
#include <stdlib.h>
#ifndef _WIN32
#include "communicate_parse_navigation.h"
#endif

#define UPDATE_DASHBOARD_GRANULARITY 2

typedef enum
{
    T_TURN_INFO_NONE  = 0x0,
    T_TURN_INFO_LEFT  = 0x1,
    T_TURN_INFO_RIGHT = 0x2,
} T_TURN_INFO;

typedef enum
{
    T_BLUETOOTH_INFO_OFF  = 0x0,
    T_BLUETOOTH_INFO_ON   = 0x1,
} T_BLUETOOTH_INFO;

typedef enum
{
    T_TENSE_AM_DISPLAY    = 0x0,
    T_TENSE_PM_DISPLAY    = 0x1,

} T_TENSE_APM_INFO;

typedef enum
{
    T_PHONE_STATUS_NONE,
    T_PHONE_STATUS_ONGOING,
    T_PHONE_STATUS_ACCEPT,
} T_PHONE_STATUS;

typedef enum
{
    T_NAVI_INFO_1,
    T_NAVI_INFO_2,
    T_NAVI_INFO_3,
    T_NAVI_INFO_4,
    T_NAVI_INFO_5,
} T_NAVI_INFO;

typedef struct app_phone_data_t
{
    uint8_t current_phone_number[11];
    uint8_t current_phone_number_len;
    T_PHONE_STATUS phone_status;
} app_phone_data;

typedef struct app_message_data_t
{
    uint8_t wechat_msg[40];
    uint8_t wechat_msg_len;
    bool wechat_notify_message;
} app_message_data;

typedef struct app_navi_data_t
{
    uint8_t navigation_msg[40];
    uint8_t navigation_num_len;
    uint8_t navigation_unit[40];
    uint8_t navigation_unit_len;
    uint8_t road_names[40];
    uint8_t road_num_len;
} app_navi_data;

typedef struct app_dashboard_data_t
{
    bool show_main_display;
    uint8_t battery_level;
    uint8_t current_speed;
    T_TURN_INFO current_turn_information;
    T_BLUETOOTH_INFO current_bluetooth_status;
    T_NAVI_INFO current_navi_info;
    app_navi_data current_navi_data;
    app_phone_data current_phone_status;
    app_message_data current_message_status;
    uint32_t refresh_timer_value;
    T_TENSE_APM_INFO tense_apm_display;
} app_dashboard_data;

static app_dashboard_data app_current_dashboard_data;

typedef enum
{
    BP_UPDATE_VALUE_EVENT   = 0x00,
    BP_LE_CONN_EVENT        = 0x01,
    BP_LE_DISC_EVENT        = 0x02,
} T_LE_EVENT;

/**
 * \brief  Refresh the dashboard application information
 * \param[in] None
 * \return None.
 */
void app_dashboard_auto_refresh_data_demo(void);

/**
 * \brief  Set current battery level
 * \param[in] uint8_t battery_level: Current battery level
 * \return None.
 */
void app_dashboard_data_set_battery_level(uint8_t battery_level);

/**
 * \brief  Get current battery level
 * \param[in] None
 * \return uint8_t: Current battery level
 */
uint8_t app_dashboard_data_get_battery_level(void);

/**
 * \brief  Set current car speed
 * \param[in] uint8_t current_car_speed: Current car speed
 * \return None.
 */
void app_dashboard_data_set_car_speed(uint8_t current_car_speed);

/**
 * \brief  Get current car speed
 * \param[in] None
 * \return uint8_t: Current car speed
 */
uint8_t app_dashboard_data_get_car_speed(void);

/**
 * \brief  Set current car turn information
 * \param[in] T_TURN_INFO current_car_turn_info: Current car turn information
 * \return None.
 */
void app_dashboard_data_set_car_turn_info(T_TURN_INFO current_car_turn_info);

/**
 * \brief  Get current car speed
 * \param[in] None
 * \return T_TURN_INFO: Current car turn information
 */
T_TURN_INFO app_dashboard_data_get_car_turn_info(void);

/**
 * \brief  Set current bluetooth status
 * \param[in] T_BLUETOOTH_INFO current_bluetooth_status: Current bluetooth status
 * \return None.
 */
void app_dashboard_data_set_bluetooth_status(T_BLUETOOTH_INFO current_bluetooth_status);

/**
 * \brief  Get current bluetooth status
 * \param[in] None
 * \return T_BLUETOOTH_INFO: Current bluetooth status
 */
T_BLUETOOTH_INFO app_dashboard_data_get_bluetooth_status(void);

/**
 * \brief  Set current navi status
 * \param[in] T_NAVI_INFO current_navi_status: Current navigation status
 * \return None.
 */
void app_dashboard_data_set_navi_status(T_NAVI_INFO current_navi_status);

/**
 * \brief  Get current navi status
 * \param[in] None
 * \return T_NAVI_INFO: Current navigation status
 */
T_NAVI_INFO app_dashboard_data_get_navi_status(void);

/**
 * \brief  Set current phone status
 * \param[in] app_phone_data current_phone_status: Current phone status
 * \return None.
 */
void app_dashboard_data_set_phone_status(app_phone_data current_phone_status);

/**
 * \brief  Get current navi status
 * \param[in] app_phone_data* rtn_phone_status: Replay current phone status
 * \return None
 */
void app_dashboard_data_get_phone_status(app_phone_data *rtn_phone_status);

/**
 * \brief  Get the dashboard application information
 * \param[in] None
 * \return app_dashboard_data*: Return the dashboard application data
 */
app_dashboard_data *app_dashboard_get_data(void);

/**
 * \brief  Refresh the widget information
 * \param[in] gui_img_t *target_image_obj: Target image object
 * \param[in] uint8_t target_value: Update image index
 * \param[in] void* target_resource_file[]: Target image list
 * \param[in] uint8_t max_index_of_resource: Size of the image list
 * \return None.
 */
void app_update_gui_widget(gui_img_t *target_image_obj,
                           uint8_t target_value,
                           void *target_resource_file[],
                           uint8_t max_index_of_resource);

void app_dashboard_initialize_data(void);
void app_dashboard_data_set_show_main_display(uint8_t Value, uint8_t le_event);
uint8_t app_dashboard_data_get_show_main_display(void);
void app_dashboard_data_set_navi_data_update(app_navi_data current_navi_data);
void app_dashboard_data_get_navi_data_update(app_navi_data *rtn_phone_status);
void app_dashboard_data_update_navi_status(const uint8_t *pValue, uint16_t length);
void app_dashboard_data_set_message_data_update(app_message_data current_message_status);
void app_dashboard_data_get_message_data_update(app_message_data *rtn_phone_status);
void app_dashboard_data_update_phone_status(uint8_t key, const uint8_t *pValue, uint16_t length);
void app_dashboard_data_update_message_status(const uint8_t *pValue, uint16_t length,
                                              uint8_t messaye_type);
void app_dashboard_data_set_current_timer(void);
uint32_t app_dashboard_data_get_current_timer(void);
void app_dashboard_data_set_tense_timer_info(T_TENSE_APM_INFO current_tense_apm_info);
T_TENSE_APM_INFO app_dashboard_data_get_tense_timer_info(void);

void app_dashboard_data_set_reject_end_call(void);
void app_dashboard_data_set_accept_call(void);
#ifdef __cplusplus
}
#endif

#endif
