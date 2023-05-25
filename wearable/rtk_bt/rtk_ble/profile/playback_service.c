/**
*****************************************************************************************
*     Copyright(c) 2020, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
   * @file      playback_service.c
   * @brief     Source file for using playback service
   * @author    lemon
   * @date      2020-08-11
   * @version   v1.0
   **************************************************************************************
   * @attention
   * <h2><center>&copy; COPYRIGHT 2020 Realtek Semiconductor Corporation</center></h2>
   **************************************************************************************
  */
/*============================================================================*
 *                              Header Files
 *============================================================================*/

#include <gatt.h>
#include <bt_types.h>
#include "trace.h"
#include "app_main.h"
#include "app_cfg.h"
#include "playback_service.h"
#include "app_playback_update_file.h"
/** @defgroup  PLAYBACK_SERVICE PLAYBACK Service
    * @brief LE Service to implement PLAYBACK feature
    * @{
    */

/*============================================================================*
 *                              Constants
 *============================================================================*/
/** @defgroup PLAYBACK_SERVICE_Exported_Constants PLAYBACK service Exported Constants
    * @{
    */

/** @brief  PLAYBACK service UUID */
static const uint8_t GATT_UUID_PLAYBACK_SERVICE[16] = {GATT_UUID128_PLAYBACK_SERVICE_ADV};
/** @brief  PLAYBACK BLE Service Callbacks */
static const T_FUN_GATT_SERVICE_CBS playback_service_cbs =
{
    NULL,                           /**< Read callback function pointer */
    playback_service_attr_write_cb, /**< Write callback function pointer */
    playback_cccd_update_cb         /**< CCCD update callback function pointer */
};

/** @brief  PLAYBACK profile/service definition
*   @note   Here is an example of PLAYBACK service table including Write
*/
static const T_ATTRIB_APPL gatt_extended_service_table[] =
{
    /*--------------------------PLAYBACK Service ---------------------------*/
    /* <<Primary Service>>, .. 0 */
    {
        (ATTRIB_FLAG_VOID | ATTRIB_FLAG_LE),        /* flags */
        {
            LO_WORD(GATT_UUID_PRIMARY_SERVICE),
            HI_WORD(GATT_UUID_PRIMARY_SERVICE),     /* type_value */
        },
        UUID_128BIT_SIZE,                           /* bValueLen */
        (void *)GATT_UUID_PLAYBACK_SERVICE,              /* p_value_context */
        GATT_PERM_READ                              /* permissions */
    },

    /* <<Characteristic1>>, .. 1 write*/
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* flags */
        {   /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            GATT_CHAR_PROP_WRITE | GATT_CHAR_PROP_WRITE_NO_RSP,            /* characteristic properties */
            //XXXXMJMJ GATT_CHAR_PROP_INDICATE,     /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },
    /*  PLAYBACK characteristic value 2 */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {   /* type_value */
            LO_WORD(GATT_UUID_CHAR_PLAYBACK_WRITE),
            HI_WORD(GATT_UUID_CHAR_PLAYBACK_WRITE),
        },
        0,                                          /* variable size */ //2
        (void *)NULL,
        GATT_PERM_WRITE            /* permissions */ //GATT_PERM_WRITE GATT_PERM_READ | GATT_PERM_WRITE
    },

    /* <<Characteristic2>>, .. 3, notifiy */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* flags */
        {   /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            (GATT_CHAR_PROP_NOTIFY),                    /* characteristic properties */
            //XXXXMJMJ GATT_CHAR_PROP_INDICATE,     /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },
    /*  PLAYBACK characteristic value 4 */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {   /* type_value */
            LO_WORD(GATT_UUID_CHAR_PLAYBACK_NOTIFY),
            HI_WORD(GATT_UUID_CHAR_PLAYBACK_NOTIFY),
        },
        0,                                          /* variable size */
        (void *)NULL,
        GATT_PERM_NOTIF_IND                               /* permissions */ //GATT_PERM_NONE GATT_PERM_NOTIF_IND GATT_PERM_NOTIF_IND_ENCRYPTED_REQ
    },

    /* client characteristic configuration */
    {
        ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_CCCD_APPL,                   /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            HI_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            /* NOTE: this value has an instantiation for each client, a write to */
            /* this attribute does not modify this default value:                */
            LO_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT), /* client char. config. bit field */
            HI_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT)
        },
        2,                                          /* bValueLen */
        NULL,
        (GATT_PERM_READ | GATT_PERM_WRITE)          /* permissions */
    }

};

/** End of PLAYBACK_SERVICE_Exported_Constants
    * @}
    */

/*============================================================================*
 *                              Variables
 *============================================================================*/
/** @defgroup PLAYBACK_SERVICE_Exported_Variables PLAYBACK service Exported Variables
    * @brief
    * @{
    */

/** @brief  Service ID only used in this file */
static T_SERVER_ID srv_id_local;

/** @brief  Function pointer used to send event to application from PLAYBACK service
*   @note   It is initiated in playback_add_service()
*/
static P_FUN_SERVER_GENERAL_CB p_playback_extended_cb = NULL;

/** End of PLAYBACK_SERVICE_Exported_Variables
    * @}
    */

/*============================================================================*
 *                              Private Functions
 *============================================================================*/
/** @defgroup PLAYBACK_SERVICE_Exported_Functions PLAYBACK service Exported Functions
    * @brief
    * @{
    */
/**
    * @brief    Write characteristic data from service
    * @param    conn_id     ID to identify the connection
    * @param    service_id   Service ID to be written
    * @param    attr_index  Attribute index of characteristic
    * @param    write_type  Write type of data to be written
    * @param    length      Length of value to be written
    * @param    p_value     Value to be written
    * @param    p_write_ind_post_proc   Write indicate post procedure
    * @return   T_APP_RESULT
    * @retval   Profile procedure result
    */
static T_APP_RESULT playback_service_attr_write_cb(uint8_t conn_id, T_SERVER_ID service_id,
                                                   uint16_t attr_index, T_WRITE_TYPE write_type, uint16_t length,
                                                   uint8_t *p_value, P_FUN_WRITE_IND_POST_PROC *p_write_ind_post_proc)
{
    T_APP_RESULT  cause = APP_RESULT_SUCCESS;

    APP_PRINT_INFO2("playback_service_attr_write_cb: attr_index 0x%02x, length %d", attr_index, length);

    if (BLE_SERVICE_CHAR_PLAYBACK_WRITE_INDEX == attr_index)
    {
        cause = app_playback_ble_handle_cp_req(conn_id, length, p_value);
    }
    else
    {
        APP_PRINT_ERROR0("playback_service_attr_write_cb: unknown attr_index");
        cause = APP_RESULT_ATTR_NOT_FOUND;
    }
    return cause;

}

void playback_cccd_update_cb(uint8_t conn_id, T_SERVER_ID service_id, uint16_t attrib_idx,
                             uint16_t cccd_bits)
{
    T_PLAYBACK_CALLBACK_DATA callback_data;
    callback_data.msg_type = SERVICE_CALLBACK_TYPE_INDIFICATION_NOTIFICATION;
    callback_data.conn_id = conn_id;
    APP_PRINT_INFO2("playback_cccd_update_cb: attrib_idx %d, cccd_bits 0x%04x", attrib_idx, cccd_bits);
    switch (attrib_idx)
    {
    case BLE_SERVICE_CHAR_PLAYBACK_DATA_CCCD_INDEX:
        {
            APP_PRINT_INFO0("playback_cccd_update_cb:BLE_SERVICE_CHAR_PLAYBACK_DATA_CCCD_INDEX");

            if (cccd_bits & GATT_CLIENT_CHAR_CONFIG_NOTIFY)
            {
                // Enable Notification
                APP_PRINT_INFO0("playback_cccd_update_cb:TRANSMIT_SVC_TX_DATA_CCCD_ENABLE");
            }
            else
            {
                APP_PRINT_INFO0("playback_cccd_update_cb:TRANSMIT_SVC_TX_DATA_CCCD_DISABLE");
            }

            callback_data.msg_data.notification_indification_index = BLE_SERVICE_CHAR_PLAYBACK_NOTIFY_INDEX;

            if (p_playback_extended_cb)
            {
                p_playback_extended_cb(service_id, (void *)&callback_data);
            }
            break;
        }
    default:
        break;
    }
    return;
}

/**
    * @brief    Add PLAYBACK BLE service to application
    * @param    p_func  Pointer of APP callback function called by profile
    * @return   Service ID auto generated by profile layer
    * @retval   A T_SERVER_ID type value
    */
T_SERVER_ID playback_add_service(void *p_func)
{
    T_SERVER_ID service_id;
    if (false == server_add_service(&service_id,
                                    (uint8_t *)gatt_extended_service_table,
                                    sizeof(gatt_extended_service_table),
                                    playback_service_cbs))
    {
        APP_PRINT_ERROR1("playback_add_service: service_id %d", service_id);
        service_id = 0xff;
        return service_id;
    }
    p_playback_extended_cb = (P_FUN_SERVER_GENERAL_CB)p_func;
    srv_id_local = service_id;
    APP_PRINT_INFO1("playback_add_service success: service_id %d", service_id);
    return service_id;
}
/**
* @brief    Send notification to peer side
* @param    conn_id  PID to identify the connection
* @param    p_data  value to be send to peer
* @param    data_len  data length of the value to be send
* @return   void
*/
void playback_service_send_notification(uint8_t conn_id, uint8_t *p_data, uint16_t data_len)
{
    APP_PRINT_INFO2("playback_service_send_notification, conn_id:0x%x, len:%d", conn_id, data_len);
    server_send_data(conn_id, srv_id_local, BLE_SERVICE_CHAR_PLAYBACK_NOTIFY_INDEX, p_data, data_len,
                     GATT_PDU_TYPE_NOTIFICATION);
}
/** End of PLAYBACK_SERVICE_Exported_Functions
    * @}
    */

/** @} */ /* End of group PLAYBACK_SERVICE */
