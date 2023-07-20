/*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     communicate_parse.c
* @brief
* @details
* @author
* @date
* @version  v0.1
*********************************************************************************************************
*/

#include "board.h"
#include "communicate_parse.h"
#include "communicate_parse_setting.h"
#include "string.h"
#include "trace.h"
#include "gap_bond_le.h"




void resolve_settings_config_command(uint8_t key, const uint8_t *pValue,
                                     uint16_t length)
{
    switch (key)
    {
    case KEY_PHONE_OS_VERSION:
        {
        }
        break;
    case KEY_TIME_SETTINGS:
        {
        }
        break;
    case KEY_ALARM_SETTINGS:
        {
        }
        break;
    case KEY_REQUEST_ALARM_SETTINGS: //request alarm clock list
        {
        }
        break;
    case KEY_STEP_TARGET_SETTINGS:
        {
        }
        break;
    case KEY_SLEEP_TARGET_SETTINGS:
        {
        }
        break;
    case KEY_PROFILE_SETTINGS:
        {
        }
        break;
    case KEY_LONG_TIME_SIT_ALERT:
        {

        }
        break;
    case KEY_LIFT_SWITCH_SETTING:
        {

        }
        break;
    case KEY_LIFT_SWITCH_REQUEST:
        {

        }
        break;
    case KEY_TWIST_SWITCH_SETTING:
        {

        }
        break;
    case KEY_TWIST_SWITCH_REQUEST:
        {
        }
        break;
    case KEY_INCOMMING_MESSAGE_SETTINGS:
        {
        }
        break;
    case KEY_INCOMMING_MESSAGE_ALL_SETTINGS:
        {
        }
        break;
    case KEY_LONG_TIME_SIT_SETTING_REQUEST:
        {
        }
        break;
    case KEY_INCOMMING_MESSAGE_SETTINGS_REQUEST:
        {
        }
        break;
    case KEY_FUNCTIONS_REQUEST:
        {
        }
        break;
    case KEY_EXERCISEMODE_REQUEST:
        {
        }
        break;
    case KEY_DIAL_SETTING:
        {
        }
        break;
    case KEY_DIAL_REQUEST:
        {
        }
        break;
    case KEY_HR_SAMPLE_REQUEST:
        {
        }
        break;
    case KEY_HOUR_FORMAT_SETTING:
        {
        }
        break;
    case KEY_HOUR_FORMAT_REQUEST:
        {
        }
        break;
    case KEY_DISTANCE_UNIT_SETTING:
        {
        }
        break;
    case KEY_DISTANCE_UNIT_REQUEST:
        {
        }
        break;
    case KEY_DNDM_SETTING:
        {
        }
        break;
    case KEY_DNDM_REQUEST:
        {
        }
        break;
    case KEY_OLED_DISPLAY_TIME_SETTING:
        {
        }
        break;
    case KEY_OLED_DISPLAY_TIME_REQUEST:
        {
        }
        break;
    case KEY_LANGUAGE_SETTING:
        {
        }
        break;
    case KEY_LANGUAGE_REQUEST:
        {
        }
        break;
    case KEY_DEVICEINFO_REQUEST:
        {
        }
        break;
    case KEY_BACKLIGHT_SETTING:
        {
        }
        break;
    case KEY_BACKLIGHT_REQUEST:
        {
        }
        break;
    case KEY_HIDDEN_FUNC_SETTING:
        {
        }
        break;
    case KEY_HIDDEN_FUNC_REQUEST:
        {
        }
        break;
    case KEY_BBPRO_MAC_REQUEST:
        {
        }
        break;
    case KEY_BBPRO_STATE_REQUEST:
        {
        }
        break;
    case KEY_BBPRO_CONNECTED_STATE_REQUEST:
        {
        }
        break;
    case KEY_BBPRO_CREATE_CONNECTION_REQUEST:
        {
        }
        break;
    default:
        break;
    }
}
