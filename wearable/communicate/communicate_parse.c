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

#include "communicate_parse.h"
#include "communicate_protocol.h"
#include "communicate_parse_navigation.h"
#include "string.h"
#include "trace.h"
//#include "rtthread.h"

/**
* @brief   resolve revceived data from remote APP
* @param   pValue: received value pointer
* @param   length: value length
* @retval  void
*/
#define GSENSOR_START    2
#define GSENSOR_AXIS_X   0
#define GSENSOR_AXIS_Y   1
#define GSENSOR_AXIS_Z   2
bool L2_frame_resolve(uint8_t *pData, uint16_t length)
{
    if ((pData == NULL) || (length == 0))
    {
        return false;
    }

    WRISTBAND_COMMUNICATE_COMMAND command_id = (WRISTBAND_COMMUNICATE_COMMAND)pData[0];
    uint8_t first_key = pData[2];
    uint16_t first_value_length = (((pData[3] << 8) | pData[4]) & 0x1FF);
    APP_PRINT_INFO1("L2_frame_resolve length is %d", length);
    switch (command_id)
    {
    case BOND_COMMAND_ID:
        {
            APP_PRINT_INFO1("BOND_COMMAND, KEY = 0x%x", first_key);
            resolve_private_bond_command(first_key, pData + L2_FIRST_VALUE_POS, first_value_length);
        }
        break;

    case SET_CONFIG_COMMAND_ID:
        {
            APP_PRINT_INFO1("SET_CONFIG_COMMAND, KEY = 0x%x", first_key);
            resolve_settings_config_command(first_key, pData + L2_FIRST_VALUE_POS, first_value_length);
        }
        break;

    case HEALTH_DATA_COMMAND_ID:
        {
            APP_PRINT_INFO1("HEALTH_DATA_COMMAND, KEY = 0x%x", first_key);
            resolve_HealthData_command(first_key, pData + L2_FIRST_VALUE_POS, first_value_length);
        }
        break;
    case NAVIGATION_INFORMATION_ID:
        {
            APP_PRINT_INFO1("NAVIGATION_INFORMATION_ID, KEY = 0x%x", first_key);
            APP_PRINT_INFO1("NAVIGATION_INFORMATION_ID,length is %d", length);
            APP_PRINT_INFO1("RECEIVE navi info is  %b", TRACE_BINARY(length,
                                                                     (uint8_t *)pData + L2_FIRST_VALUE_POS));
            resolve_Navigation_command(first_key, pData + L2_FIRST_VALUE_POS, first_value_length);
        }
        break;
    case NOTIFY_COMMAND_ID:
        {
            APP_PRINT_INFO2("NOTIFY_COMMAND_ID, KEY = 0x%x, info= 0x%x", first_key, pData[5]);
            resolve_Notify_command(first_key, pData + L2_FIRST_VALUE_POS, first_value_length, pData[5]);
        }
        break;

    case CONTROL_COMMAND_ID:
        {
            APP_PRINT_INFO1("CONTROL_COMMAND, KEY = 0x%x", first_key);
            resolve_Control_command(first_key, pData + L2_FIRST_VALUE_POS, first_value_length);
        }
        break;
    case SEND_HRS_RAW_DATA:
        {
            uint16_t gsensor_axis[3] = {0};
            APP_PRINT_INFO2("hrs act data is %d, hrs reliability is %d", *(pData + L2_FIRST_VALUE_POS),
                            *(pData + L2_FIRST_VALUE_POS + 1));
            for (uint8_t i = 0; i < (length - 2) / 6; i++)
            {
                gsensor_axis[0] = pData[GSENSOR_START + GSENSOR_AXIS_X + i * 3] * 256 + pData[GSENSOR_START +
                                                                                              GSENSOR_AXIS_X + i * 3 + 1];
                gsensor_axis[1] = pData[GSENSOR_START + GSENSOR_AXIS_X + i * 3] * 256 + pData[GSENSOR_START +
                                                                                              GSENSOR_AXIS_X + i * 3 + 1];
                gsensor_axis[2] = pData[GSENSOR_START + GSENSOR_AXIS_X + i * 3] * 256 + pData[GSENSOR_START +
                                                                                              GSENSOR_AXIS_X + i * 3 + 1];
                APP_PRINT_INFO3("Gsensor data X = 0x%x, y = 0x%x, z = 0x%x", \
                                gsensor_axis[0], \
                                gsensor_axis[1], \
                                gsensor_axis[2]);
                //rt_kprintf("Gsensor data X = 0x%x, y = 0x%x, z = 0x%x\n", \
                //          gsensor_axis[0], \
                //          gsensor_axis[1], \
                //          gsensor_axis[2]);
            }
            APP_PRINT_INFO2("hrs act data is %d, hrs reliability is %d\n", *(pData + L2_FIRST_VALUE_POS),
                            *(pData + L2_FIRST_VALUE_POS + 1));
        }
        break;
    default:
        break;
    }
    return true;
}
