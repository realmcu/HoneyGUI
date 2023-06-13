/**
*********************************************************************************************************
*               Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     simple_ble_service.c
* @brief    simple BLE profile source file.
* @details  Demonstration of how to implement a self-definition profile.
* @author
* @date
* @version
*********************************************************************************************************
*/

#include <string.h>
#include <trace.h>
#include <upay_service.h>
#include <gap.h>


static T_SERVER_ID upay_service_id;

/**<  Function pointer used to send event to application from simple profile. Initiated in simp_ble_service_add_service. */
static P_FUN_SERVER_GENERAL_CB pfn_upay_service_cb = NULL;

/**< @brief  profile/service definition.  */
const T_ATTRIB_APPL upay_service_tbl[] =
{
    /* <<Primary Service>>, .. */
    {
        (ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_LE),  /* flags     */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_PRIMARY_SERVICE),
            HI_WORD(GATT_UUID_PRIMARY_SERVICE),
            LO_WORD(GATT_UUID_UPAY_SERVICE),              /* service UUID */
            HI_WORD(GATT_UUID_UPAY_SERVICE)
        },
        UUID_16BIT_SIZE,                            /* bValueLen     */
        NULL,                                       /* p_value_context */
        GATT_PERM_READ                              /* permissions  */
    },
    /* <<Characteristic>>, .. */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            (GATT_CHAR_PROP_WRITE |                   /* characteristic properties */
             GATT_CHAR_PROP_NOTIFY | GATT_CHAR_PROP_READ)
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },

    {
        ATTRIB_FLAG_VALUE_APPL,                              /* flags */
        {                                                           /* type_value */
            LO_WORD(GATT_UPAY_CHARACTERISTIC),
            HI_WORD(GATT_UPAY_CHARACTERISTIC)
        },
        0,                                                 /* bValueLen */
        NULL,
        (GATT_PERM_READ | GATT_PERM_WRITE)           /* permissions */
    },
    /* client characteristic configuration */
    {
        (ATTRIB_FLAG_VALUE_INCL |                   /* flags */
         ATTRIB_FLAG_CCCD_APPL),
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

/**
 * @brief read characteristic data from service.
 *
 * @param service_id          ServiceID of characteristic data.
 * @param attrib_index        Attribute index of getting characteristic data.
 * @param offset              Used for Blob Read.
 * @param p_length            length of getting characteristic data.
 * @param pp_value            data got from service.
 * @return Profile procedure result
*/
T_APP_RESULT  upay_service_attr_read_cb(uint8_t conn_id, T_SERVER_ID service_id,
                                        uint16_t attrib_index, uint16_t offset, uint16_t *p_length, uint8_t **pp_value)
{
    T_APP_RESULT  cause  = APP_RESULT_SUCCESS;

    switch (attrib_index)
    {
    default:
        APP_PRINT_ERROR1("upay service read, Attr not found, index %d", attrib_index);
        cause = APP_RESULT_ATTR_NOT_FOUND;
        break;
    case INDEX_UPAY_READ:
        {
            TUPAYS_CALLBACK_DATA callback_data;
            callback_data.msg_type = SERVICE_CALLBACK_TYPE_READ_CHAR_VALUE;
            callback_data.msg_data.read_value_index = UPAY_READ;
            callback_data.conn_id = conn_id;
            if (pfn_upay_service_cb)
            {
                pfn_upay_service_cb(service_id, (void *)&callback_data);
            }
            uint8_t tmp_buf = 0xA5;
            *pp_value = &tmp_buf;
            *p_length = 0x01;
        }
        break;

    }

    return (cause);
}

/**
 * @brief write characteristic data from service.
 *
 * @param conn_id
 * @param service_id        ServiceID to be written.
 * @param attrib_index      Attribute index of characteristic.
 * @param length            length of value to be written.
 * @param p_value           value to be written.
 * @return Profile procedure result
*/
T_APP_RESULT upay_service_attr_write_cb(uint8_t conn_id, T_SERVER_ID service_id,
                                        uint16_t attrib_index, T_WRITE_TYPE write_type, uint16_t length, uint8_t *p_value,
                                        P_FUN_WRITE_IND_POST_PROC *p_write_ind_post_proc)
{
    TUPAYS_CALLBACK_DATA callback_data;
    T_APP_RESULT  cause = APP_RESULT_SUCCESS;
    APP_PRINT_INFO1("upay service write_type = 0x%x", write_type);
    if (INDEX_UPAY_WRITE == attrib_index)
    {
        /* Make sure written value size is valid. */
        if (p_value == NULL)
        {
            cause  = APP_RESULT_INVALID_VALUE_SIZE;
        }
        else
        {
            /* Notify Application. */
            callback_data.msg_type = SERVICE_CALLBACK_TYPE_WRITE_CHAR_VALUE;
            callback_data.conn_id  = conn_id;
            callback_data.msg_data.write.opcode = UPAY_WRITE;
            callback_data.msg_data.write.write_type = write_type;
            callback_data.msg_data.write.len = length;
            callback_data.msg_data.write.p_value = p_value;

            if (pfn_upay_service_cb)
            {
                pfn_upay_service_cb(service_id, (void *)&callback_data);
            }
        }
    }
    else
    {
        APP_PRINT_ERROR2("simp_ble_service_attr_write_cb Error: attrib_index 0x%x, length %d",
                         attrib_index,
                         length);
        cause = APP_RESULT_ATTR_NOT_FOUND;
    }
    return cause;
}

/**
 * @brief update CCCD bits from stack.
 *
 * @param conn_id           connection id.
 * @param service_id          Service ID.
 * @param index          Attribute index of characteristic data.
 * @param cccbits         CCCD bits from stack.
 * @return None
*/
void upay_service_cccd_update_cb(uint8_t conn_id, T_SERVER_ID service_id, uint16_t index,
                                 uint16_t cccbits)
{
    TUPAYS_CALLBACK_DATA callback_data;
    bool is_handled = false;
    callback_data.conn_id = conn_id;
    callback_data.msg_type = SERVICE_CALLBACK_TYPE_INDIFICATION_NOTIFICATION;
    APP_PRINT_INFO2("simp_ble_service_cccd_update_cb: index = %d, cccbits 0x%x", index, cccbits);
    switch (index)
    {
    case INDEX_UPAY_CHAR_CCCD:
        {
            if (cccbits & GATT_CLIENT_CHAR_CONFIG_NOTIFY)
            {
                // Enable Notification
                callback_data.msg_data.notification_indification_index = UPAY_NOTIFY_INDICATE_ENABLE;
            }
            else
            {
                // Disable Notification
                callback_data.msg_data.notification_indification_index = UPAY_NOTIFY_INDICATE_DISABLE;
            }
            is_handled =  true;
        }
        break;

    default:
        break;
    }
    /* Notify Application. */
    if (pfn_upay_service_cb && (is_handled == true))
    {
        pfn_upay_service_cb(service_id, (void *)&callback_data);
    }
}

/**
 * @brief Simple ble Service Callbacks.
*/
const T_FUN_GATT_SERVICE_CBS upay_service_cbs =
{
    upay_service_attr_read_cb,  // Read callback function pointer
    upay_service_attr_write_cb, // Write callback function pointer
    upay_service_cccd_update_cb // CCCD update callback function pointer
};

/**
  * @brief Add simple BLE service to the BLE stack database.
  *
  * @param[in]   p_func  Callback when service attribute was read, write or cccd update.
  * @return Service id generated by the BLE stack: @ref T_SERVER_ID.
  * @retval 0xFF Operation failure.
  * @retval others Service id assigned by stack.
  *
  */
T_SERVER_ID upays_add_service(void *p_func)
{
    if (false == server_add_service(&upay_service_id,
                                    (uint8_t *)upay_service_tbl,
                                    sizeof(upay_service_tbl),
                                    upay_service_cbs))
    {
        APP_PRINT_ERROR0("simp_ble_service_add_service: fail");
        upay_service_id = 0xff;
        return upay_service_id;
    }

    pfn_upay_service_cb = (P_FUN_SERVER_GENERAL_CB)p_func;
    return upay_service_id;
}

