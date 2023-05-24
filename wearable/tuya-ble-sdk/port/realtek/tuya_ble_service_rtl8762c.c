/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      pxp_ext.c
* @brief
* @details
* @author
* @date
* @version
* *********************************************************************************************************
*/
#include "stdint.h"
#include "stddef.h"
#include "string.h"
#include "trace.h"
#include "profile_server.h"
#include "tuya_ble_service_rtl8762c.h"



//const uint8_t   GATT_UUID128_KNS_SERVICE[16] = {0x70, 0x75, 0x6F, 0x72, 0x47, 0x74, 0x62, 0x2E, 0x61, 0x79, 0x75, 0x74, 0x00, 0xA3, 0x01, 0x56};
const uint8_t   GATT_UUID16_KNS_SERVICE[2] = {0x50, 0xFD};

uint8_t gParaValue_data[PARA_VALUE_LEN_MAX] = {0};

/**<  Function pointer used to send event to application from pxp extended profile. */

static P_FUN_SERVER_GENERAL_CB pfn_kns_cb = NULL;

static const T_ATTRIB_APPL kns_attr_tbl[] =
{
    /*----------------- simple key Service -------------------*/
    /* <<Primary Service>>, .. */
    {
        (ATTRIB_FLAG_VOID | ATTRIB_FLAG_LE),    /* wFlags     */
        {   /* bTypeValue */
            LO_WORD(GATT_UUID_PRIMARY_SERVICE),
            HI_WORD(GATT_UUID_PRIMARY_SERVICE),
        },
        UUID_16BIT_SIZE,                           /* bValueLen     */
        (void *)GATT_UUID16_KNS_SERVICE,           /* pValueContext */
        GATT_PERM_READ                              /* wPermissions  */
    },

    /* Set para Characteristic */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* wFlags */
        {   /* bTypeValue */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            GATT_CHAR_PROP_WRITE | GATT_CHAR_PROP_WRITE_NO_RSP,            /* characteristic properties */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* wPermissions */
    },

    /* Set para Characteristic value  */
    {
        ATTRIB_FLAG_VALUE_APPL | ATTRIB_FLAG_UUID_128BIT,                   /* wFlags */
        {   /* bTypeValue */
            GATT_UUID16_CHAR_PARAM
        },
        0,                                          /* variable size */
        NULL,
        GATT_PERM_READ | GATT_PERM_WRITE                           /* wPermissions */
    },

    /* Key  <<Characteristic>>, .. */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* wFlags */
        {   /* bTypeValue */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            (                /* characteristic properties */
                GATT_CHAR_PROP_NOTIFY)
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* wPermissions */
    },
    /* simple key value */
    {
        ATTRIB_FLAG_VALUE_APPL | ATTRIB_FLAG_UUID_128BIT,                   /* wFlags */
        {   /* bTypeValue */
            GATT_UUID16_CHAR_KEY
        },
        0,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* wPermissions */
    },
    /* client characteristic configuration */
    {
        ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_CCCD_APPL,                   /* wFlags */
        {   /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            HI_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            /* NOTE: this value has an instantiation for each client, a write to */
            /* this attribute does not modify this default value:                */
            LO_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT), /* client char. config. bit field */
            HI_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT)
        },
        2,                                          /* bValueLen */
        NULL,
        (GATT_PERM_READ | GATT_PERM_WRITE)          /* wPermissions */
    }
};



/**< @brief  PXP key notification service service size definition.  */
const uint16_t kns_attr_tbl_size = sizeof(kns_attr_tbl);

//uint32_t gParaValue = 30;

bool kns_set_parameter(T_KNS_PARAM_TYPE param_type, uint8_t length, uint8_t *p_value)
{
    bool ret = true;

    switch (param_type)
    {
    default:
        {
            ret = false;
            PROFILE_PRINT_ERROR0("bas_set_parameter failed");
        }
        break;

    case KNS_PARAM_VALUE:
        {
            if (length > PARA_VALUE_LEN_MAX)
            {
                ret = false;
            }
            else
            {
                memset(gParaValue_data, 0, PARA_VALUE_LEN_MAX);
                memcpy(gParaValue_data, p_value, length);
                // gParaValue = *p_value;
            }
        }
        break;
    }

    return ret;
}
/**
 * @brief read characteristic data from stack.
 *
 * @param ServiceId          ServiceId generated when register to upper stack.
 * @param attrib_index          Attribute index of getting characteristic data.
 * @param offset                Used for Blob Read.
 * @param length_ptr            length of getting characteristic data.
 * @param ppValue            pointer of getting characteristic data.
 * @return TProfileResult   profile procedure results.
*/

T_APP_RESULT kns_attr_read_cb(uint8_t conn_id, T_SERVER_ID service_id, uint16_t attrib_index,
                              uint16_t offset, uint16_t *p_length, uint8_t **pp_value)
{
    T_APP_RESULT  cause  = APP_RESULT_SUCCESS;
    *p_length = 0;

    switch (attrib_index)
    {
    default:
        /* invalid attribute index. */
        PROFILE_PRINT_ERROR1("kns_attr_read_cb attrib_index = %d not found", attrib_index);
        cause  = APP_RESULT_ATTR_NOT_FOUND;
        break;
    case KNS_PARAM_VALUE_INDEX:
        {
            T_KNS_CALLBACK_DATA callback_data;

            callback_data.msg_type = SERVICE_CALLBACK_TYPE_READ_CHAR_VALUE;
            //callback_data.conn_id = conn_id;
            callback_data.msg_data.read_index = KNS_READ_PARA;
            if (pfn_kns_cb)
            {
                pfn_kns_cb(service_id, (void *)&callback_data);
            }

            *pp_value = (uint8_t *)gParaValue_data;
            *p_length = sizeof(gParaValue_data);
        }
        break;
    }

    PROFILE_PRINT_INFO2("<-- gatt get PxpExtended attribute value: AttribIndex--%d, length--%d",
                        attrib_index,
                        *p_length);
    return (cause);
}

/**
 * @brief write characteristic data from stack.
 *
 * @param ServiceId          ServiceId generated when register to upper stack.
 * @param iAttribIndex          Attribute index of getting characteristic data.
 * @param wLength                length of data to be written.
 * @param pValue            pointer of data to be written.
 * @return TProfileResult   profile procedure results.
*/

static T_APP_RESULT kns_attr_write_cb(uint8_t conn_id, T_SERVER_ID service_id,
                                      uint16_t attr_index, T_WRITE_TYPE write_type, uint16_t length,
                                      uint8_t *p_value, P_FUN_WRITE_IND_POST_PROC *p_write_ind_post_proc)
{
    T_APP_RESULT  cause = APP_RESULT_SUCCESS;
    T_KNS_CALLBACK_DATA callback_data;
    if (!p_value)
    {
        PROFILE_PRINT_ERROR2("kns_attr_write_cb: p_value %p, length 0x%x", p_value, length);
        cause = APP_RESULT_INVALID_PDU;
        return cause;
    }

    switch (attr_index)
    {
    default:
        cause = APP_RESULT_ATTR_NOT_FOUND;
        break;
    case KNS_PARAM_VALUE_INDEX:
        if (length > PARA_VALUE_LEN_MAX)
        {
            cause = APP_RESULT_INVALID_VALUE_SIZE;
            PROFILE_PRINT_ERROR2("kns_attr_write_cb: p_value %p, length 0x%x", p_value, length);
        }
        else
        {
            PROFILE_PRINT_ERROR2("kns_attr_write_cb: p_value %p, length 0x%x", p_value, length);
            //callback_data.conn_id = conn_id;
            callback_data.msg_type = SERVICE_CALLBACK_TYPE_WRITE_CHAR_VALUE;
            callback_data.msg_data.write_value.data_len = length;
            memcpy(callback_data.msg_data.write_value.data, p_value, length);
        }
        break;

    }
    if (pfn_kns_cb && (cause == APP_RESULT_SUCCESS))
    {
        pfn_kns_cb(service_id, (void *)&callback_data);
    }
    return cause;

}
/**
 * @brief update CCCD bits from stack.
 *
 * @param ServiceId          ServiceId generated when register to upper stack.
 * @param Index          Attribute index of getting characteristic data.
 * @param wCCCBits                CCCD bits to be updated.
 * @return none.
*/

void kns_cccd_update_cb(uint8_t conn_id, T_SERVER_ID service_id, uint16_t index, uint16_t ccc_bits)
{
    T_KNS_CALLBACK_DATA callback_data;
    callback_data.msg_type = SERVICE_CALLBACK_TYPE_INDIFICATION_NOTIFICATION;
    //callback_data.conn_id = conn_id;
    bool handle = true;
    PROFILE_PRINT_INFO2("kns_cccd_update_cb index = %d ccc_bits %x", index, ccc_bits);

    switch (index)
    {
    case KNS_KEY_VALUE_CCCD_INDEX:
        {
            if (ccc_bits & GATT_CLIENT_CHAR_CONFIG_NOTIFY)
            {
                callback_data.msg_data.notification_indification_index = KNS_NOTIFY_ENABLE;
            }
            else
            {
                callback_data.msg_data.notification_indification_index = KNS_NOTIFY_DISABLE;
            }
            break;
        }
    default:
        {
            handle = false;
            break;
        }

    }

    if (pfn_kns_cb && (handle == true))
    {
        pfn_kns_cb(service_id, (void *)&callback_data);
    }

    return;
}

/**
 * @brief KNS Service Callbacks.
*/
const T_FUN_GATT_SERVICE_CBS kns_cbs =
{
    NULL,//kns_attr_read_cb,  // Read callback function pointer
    kns_attr_write_cb, // Write callback function pointer
    kns_cccd_update_cb  // CCCD update callback function pointer
};

/**
 * @brief Add PXP extended service to stack/profile.
 *
 * @param pFunc          pointer of function to send data to application from PXP extended service.
 * @return ServiceId.   the service ID auto generated by stack when register this service to stack
*/
uint8_t kns_add_service(void *pFunc)
{
    T_SERVER_ID service_id;
    if (false == server_add_service(&service_id,
                                    (uint8_t *)kns_attr_tbl,
                                    kns_attr_tbl_size,
                                    kns_cbs))
    {
        PROFILE_PRINT_ERROR1("kns_add_service: service_id %d", service_id);
        service_id = 0xff;
    }

    pfn_kns_cb = (P_FUN_SERVER_GENERAL_CB)pFunc;
    return service_id;
}

