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
#include "communicate_task.h"
#include "communicate_parse_health.h"
#include "string.h"
#include "trace.h"




/**
* @brief   resolve health data command received from remote APP
* @param   key: L2 key
* @param   pValue: received value pointer
* @param   length: value length
* @retval  error code
*/
void resolve_HealthData_command(uint8_t key, const uint8_t *pValue, uint16_t length)
{
    switch (key)
    {
    case KEY_REQUEST_DATA:
        {
        }
        break;
    case KEY_SET_STEPS_NOTIFY:
        {
        }
        break;

    case KEY_DAILY_DATA_SYNC:
        {
        }
        break;

    case KEY_LATEST_DATA_SYNC:
        {
        }
        break;

    case KEY_REQUEST_HEART_DATA:
        {
        }
        break;
    case KEY_HEART_DATA_SAMPLE_SETTING:
        {
        }
        break;
    case KEY_REQUEST_HEART_SAMPLE_SETTING:
        {
        }
        break;
    default:
        break;
    }
}