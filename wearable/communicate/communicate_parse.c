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
#include "string.h"
#include "trace.h"


/**
* @brief   resolve revceived data from remote APP
* @param   pValue: received value pointer
* @param   length: value length
* @retval  void
*/
bool L2_frame_resolve(uint8_t *pData, uint16_t length)
{
    if ((pData == NULL) || (length == 0))
    {
        return false;
    }

    WRISTBAND_COMMUNICATE_COMMAND command_id = (WRISTBAND_COMMUNICATE_COMMAND)pData[0];
    uint8_t first_key = pData[2];
    uint16_t first_value_length = (((pData[3] << 8) | pData[4]) & 0x1FF);

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

    case NOTIFY_COMMAND_ID:
        {
            APP_PRINT_INFO1("NOTIFY_COMMAND_ID, KEY = 0x%x", first_key);
            resolve_Notify_command(first_key, pData + L2_FIRST_VALUE_POS, first_value_length);
        }
        break;

    case CONTROL_COMMAND_ID:
        {
            APP_PRINT_INFO1("CONTROL_COMMAND, KEY = 0x%x", first_key);
            resolve_Control_command(first_key, pData + L2_FIRST_VALUE_POS, first_value_length);
        }
        break;
    default:
        break;
    }
    return true;
}

