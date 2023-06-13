/**
*********************************************************************************************************
*               Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     miwear_service.c
* @brief    MI wear service profile source file.
* @details  for mi wear.
* @author   wh
* @date
* @version
*********************************************************************************************************
*/

#include <string.h>
#include <trace.h>
#include <miwear_service.h>
#include <gap.h>
#include <gap_conn_le.h>

#include "rtthread.h"
#ifdef RTK_MI_WEAR
#include "miwear_api.h"
#include "miwear_type.h"
#endif




//T_SERVER_ID miwear_service_id;


static P_FUN_SERVER_GENERAL_CB pfn_miwear_ble_service_cb = NULL;

char user_descr0050[] = "Version";
char user_descr0051[] = "TX Characteristic";
char user_descr0052[] = "RX Characteristic";
/**< @brief  profile/service definition.  */
const T_ATTRIB_APPL miwear_ble_service_tbl[] =
{
    /* <<Primary Service>>, .. */
    {
        (ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_LE),  /* flags     */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_PRIMARY_SERVICE),
            HI_WORD(GATT_UUID_PRIMARY_SERVICE),
            LO_WORD(MIWEAR_BLE_UUID_MI_SERVICE),      /* service UUID */
            HI_WORD(MIWEAR_BLE_UUID_MI_SERVICE)
        },
        UUID_16BIT_SIZE,                            /* bValueLen     */
        NULL,                                       /* p_value_context */
        GATT_PERM_READ                              /* permissions  */
    },
    /* <<Characteristic>> Proto Version */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            (GATT_CHAR_PROP_READ) /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* wFlags */
        {   /* bTypeValue */
            LO_WORD(MIWEAR_MI_SERVICE_BLE_UUID_VERSION),
            HI_WORD(MIWEAR_MI_SERVICE_BLE_UUID_VERSION),
        },
        0,                                          /* variable size */
        NULL,
        GATT_PERM_READ                         /* wPermissions */
    },
    {
        ATTRIB_FLAG_VOID | ATTRIB_FLAG_ASCII_Z,     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHAR_USER_DESCR),
            HI_WORD(GATT_UUID_CHAR_USER_DESCR),
        },
        (sizeof(user_descr0050) - 1),                                           /* bValueLen */
        (void *)user_descr0050,
        GATT_PERM_READ           /* permissions */
    },
    /* <<Characteristic>> Proto TX Characteristic */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            (GATT_CHAR_PROP_NOTIFY | GATT_CHAR_PROP_WRITE_NO_RSP) /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* wFlags */
        {   /* bTypeValue */
            LO_WORD(MIWEAR_MI_SERVICE_BLE_UUID_PROTO_TX),
            HI_WORD(MIWEAR_MI_SERVICE_BLE_UUID_PROTO_TX),
        },
        0,                                          /* variable size */
        NULL,
        GATT_PERM_WRITE                         /* wPermissions */
    },

    /* client characteristic configuration for Proto TX*/
    {
        (ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_CCCD_APPL),    /* wFlags */
        {
            LO_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            HI_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            LO_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT), /* client char. config. bit field */
            HI_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT)
        },
        2,                                          /* bValueLen */
        NULL,
        (GATT_PERM_READ | GATT_PERM_WRITE)          /* wPermissions */
    },
    {
        ATTRIB_FLAG_VOID | ATTRIB_FLAG_ASCII_Z,     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHAR_USER_DESCR),
            HI_WORD(GATT_UUID_CHAR_USER_DESCR),
        },
        (sizeof(user_descr0051) - 1),                                           /* bValueLen */
        (void *)user_descr0051,
        GATT_PERM_READ           /* permissions */
    },
    /* <<Characteristic>> Proto RX Characteristic */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            (GATT_CHAR_PROP_NOTIFY | GATT_CHAR_PROP_WRITE | GATT_CHAR_PROP_WRITE_NO_RSP) /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                          /* type_value */
            LO_WORD(MIWEAR_MI_SERVICE_BLE_UUID_PROTO_RX),
            HI_WORD(MIWEAR_MI_SERVICE_BLE_UUID_PROTO_RX)
        },
        0,                                          /* bValueLen */
        NULL,
        GATT_PERM_WRITE                             /* permissions */
    },
    /* client characteristic configuration for Proto RX*/
    {
        (ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_CCCD_APPL),    /* wFlags */
        {
            LO_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            HI_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            LO_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT), /* client char. config. bit field */
            HI_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT)
        },
        2,                                          /* bValueLen */
        NULL,
        (GATT_PERM_READ | GATT_PERM_WRITE)          /* wPermissions */
    },
    {
        ATTRIB_FLAG_VOID | ATTRIB_FLAG_ASCII_Z,     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHAR_USER_DESCR),
            HI_WORD(GATT_UUID_CHAR_USER_DESCR),
        },
        (sizeof(user_descr0052) - 1),                                           /* bValueLen */
        (void *)user_descr0052,
        GATT_PERM_READ           /* permissions */
    },
};


T_APP_RESULT miwear_ble_service_attr_read_cb(uint8_t conn_id, T_SERVER_ID service_id,
                                             uint16_t attrib_index,
                                             uint16_t offset, uint16_t *p_length, uint8_t **pp_value)
{
    T_APP_RESULT cause = APP_RESULT_SUCCESS;
    uint16_t conn_handle = conn_id;

    uint16_t attr_handle = 0;
    attr_handle = server_get_start_handle(service_id) + attrib_index;
    rt_kprintf("service_id = %d; server_get_start_handle(service_id) = %d \n", service_id,
               server_get_start_handle(service_id));

    rt_kprintf("%s, conn handle = %d, att handle = %d START\n", __func__,
               conn_handle, attr_handle);
    uint16_t value_len = 8;
    static uint8_t value[8];

    miwear_status_t status = miwear_gatt_chrc_value_get(attr_handle, value, &value_len);
    if (status)
    {
        rt_kprintf("%s, get attr(0x%02xd)value err(%d)\n", __func__, attr_handle,
                   status);
        return APP_RESULT_APP_ERR;
    }
    *pp_value = value;
    *p_length = value_len;

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
T_APP_RESULT miwear_ble_service_attr_write_cb(uint8_t conn_id, T_SERVER_ID service_id,
                                              uint16_t attrib_index, T_WRITE_TYPE write_type, uint16_t length, uint8_t *p_value,
                                              P_FUN_WRITE_IND_POST_PROC *p_write_ind_post_proc)
{
    T_APP_RESULT  cause = APP_RESULT_SUCCESS;
    uint16_t conn_handle = conn_id;
    uint16_t len = length;
    uint16_t offset = 0;
    uint8_t *buf = p_value;
    uint16_t attr_handle = 0;

    attr_handle = server_get_start_handle(service_id) + attrib_index;

    rt_kprintf(
        "%s, conn handle = %d, attr handle = %d, data len = %d, offset = %d\n",
        __func__, conn_handle, attr_handle, len, offset);

    miwear_gatts_evt_param_t gatts_param;
    gatts_param.conn_handle = conn_handle;

    uint8_t  remote_bd[6] = {0};
    T_GAP_REMOTE_ADDR_TYPE remote_bd_type;
    le_get_conn_addr(conn_id, remote_bd, (unsigned char *)&remote_bd_type);

    if (GAP_REMOTE_ADDR_LE_PUBLIC == remote_bd_type)
    {
        memcpy(gatts_param.peer_public_addr, remote_bd, 6);
        rt_kprintf("public addr = 0x%x - 0x%x - 0x%x - 0x%x - 0x%x - 0x%x \n", remote_bd[0], remote_bd[1],
                   remote_bd[3], remote_bd[3], remote_bd[4], remote_bd[5]);
    }
    else
    {
        rt_kprintf("random addr = 0x%x - 0x%x - 0x%x - 0x%x - 0x%x - 0x%x \n", remote_bd[0], remote_bd[1],
                   remote_bd[3], remote_bd[3], remote_bd[4], remote_bd[5]);
    }

    gatts_param.write.value_handle = attr_handle;
    gatts_param.write.offset = 0;
    gatts_param.write.len = len;

    uint8_t *data_buf;
    data_buf = miwear_malloc(len);
    if (!data_buf)
    {
        return APP_RESULT_APP_ERR;
    }
    memcpy(data_buf, buf + offset, len);
    gatts_param.write.data = data_buf;

    for (int index = 0; index < len; index ++)
    {
        rt_kprintf("%s, %d = %02X\n", __func__, index, data_buf[index]);
    }

    miwear_gatts_event_callback(MIWEAR_GATTS_EVT_WRITE, &gatts_param);
    miwear_free(data_buf);

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
void miwear_ble_service_cccd_update_cb(uint8_t conn_id, T_SERVER_ID service_id, uint16_t index,
                                       uint16_t cccbits)
{
    return;
}

/**
 * @brief Simple ble Service Callbacks.
*/
const T_FUN_GATT_SERVICE_CBS miwear_ble_service_cbs =
{
    miwear_ble_service_attr_read_cb,  // Read callback function pointer
    miwear_ble_service_attr_write_cb, // Write callback function pointer
    miwear_ble_service_cccd_update_cb // CCCD update callback function pointer
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
T_SERVER_ID miwear_ble_service_add_service(void *p_func)
{
    T_SERVER_ID service_id;
    if (false == server_add_service(&service_id,
                                    (uint8_t *)miwear_ble_service_tbl,
                                    sizeof(miwear_ble_service_tbl),
                                    miwear_ble_service_cbs))
    {
        APP_PRINT_ERROR0("simp_ble_service_add_service: fail");
        service_id = 0xff;
        return service_id;
    }

    pfn_miwear_ble_service_cb = (P_FUN_SERVER_GENERAL_CB)p_func;
    return service_id;
}

