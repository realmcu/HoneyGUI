/**
*********************************************************************************************************
*               Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     simple_ble_service.c
* @brief    simple BLE profile source file.
* @details  Demonstration of how to implement a self-definition profile.
* @author
* @date     2016-02-18
* @version  v0.1
*********************************************************************************************************
*/

#include <string.h>
#include "trace.h"
#include "datatrans_profile.h"
#include "gap.h"


/********************************************************************************************************
* local static variables defined here, only used in this source file.
********************************************************************************************************/
T_SERVER_ID DatatransServiceId;
extern T_SERVER_ID datatrans_srv_id;
uint8_t feature[2] = {0x22, 0x02};
_T_Feature_Info feature_info;
/**<  128bit UUID of simple BLE service. */
const uint8_t GATT_UUID128_DATATRANS_PROFILE[16] = { 0x12, 0xA2, 0x4D, 0x2E, 0xFE, 0x14, 0x48, 0x8e, 0x93, 0xD2, 0x17, 0x3C, 0xFF, 0xE0, 0x00, 0x00};

static P_FUN_SERVER_GENERAL_CB pfn_dts_cb = NULL;

/**< @brief  profile/service definition.  */
T_ATTRIB_APPL datatrans_service_tbl[] =
{
    /* <<Primary Service>>, Index 0 */
    {
        (ATTRIB_FLAG_VOID | ATTRIB_FLAG_LE),        /* flags     */
        {
            LO_WORD(GATT_UUID_PRIMARY_SERVICE),
            HI_WORD(GATT_UUID_PRIMARY_SERVICE),     /* type_value */
        },
        UUID_128BIT_SIZE,                           /* bValueLen     */
        (void *)GATT_UUID128_DATATRANS_PROFILE,        /* p_value_context */
        GATT_PERM_READ                              /* permissions  */
    },

    /* <<Characteristic>> write/notification  Index 1*/
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            GATT_CHAR_PROP_WRITE_NO_RSP | GATT_CHAR_PROP_WRITE, /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },
    /* <<Characteristic>> Data write  Index 2*/
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHAR_DATA),
            HI_WORD(GATT_UUID_CHAR_DATA)
        },
        0,                                          /* bValueLen */
        NULL,
        GATT_PERM_WRITE                             /* permissions */
    },
    /* <<Characteristic>> Index 3  */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            GATT_CHAR_PROP_NOTIFY,  /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },
    /* <<Characteristic>> Data Notification Index 4  */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHAR_DATA_NOTIFY),
            HI_WORD(GATT_UUID_CHAR_DATA_NOTIFY)
        },
        0,                                          /* bValueLen */
        NULL,
        GATT_PERM_NOTIF_IND                        /* permissions */
    },

    /* <<Characteristic>> CCCD Index 5*/
    {
        ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_CCCD_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            HI_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            LO_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT), /* client char. config. bit field */
            HI_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT)
        },
        2,                                          /* bValueLen */
        NULL,
        (GATT_PERM_READ | GATT_PERM_WRITE)          /* permissions */
    },

    /* <<Characteristic>> Index 6 */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            GATT_CHAR_PROP_NOTIFY | GATT_CHAR_PROP_WRITE_NO_RSP | GATT_CHAR_PROP_WRITE\
            | GATT_CHAR_PROP_READ,  /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },
    /* <<Characteristic>> Flow Control Index 7 */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_FLOW_CTRL_NOTIFY),
            HI_WORD(GATT_UUID_FLOW_CTRL_NOTIFY)
        },
        0,                                          /* bValueLen */
        NULL,
        (GATT_PERM_NOTIF_IND | GATT_PERM_READ | GATT_PERM_WRITE)     /* permissions */
    },

    /* <<Characteristic>> CCCD Index 8*/
    {
        ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_CCCD_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            HI_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            LO_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT), /* client char. config. bit field */
            HI_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT)
        },
        2,                                          /* bValueLen */
        NULL,
        (GATT_PERM_READ | GATT_PERM_WRITE)          /* permissions */
    }
};



T_APP_RESULT   DataTransAttrReadCb(uint8_t conn_id, T_SERVER_ID service_id,
                                   uint16_t attrib_index, uint16_t offset, uint16_t *p_length, uint8_t **pp_value)
{

    T_APP_RESULT  wCause  = APP_RESULT_SUCCESS;

    switch (attrib_index)
    {
    default:
        APP_PRINT_ERROR1("DataTransAttrReadCb, Attr not found, index %d", attrib_index);
        wCause = APP_RESULT_ATTR_NOT_FOUND;
        break;
    case GATT_UUID_CHAR_FLOW_NOTIFY_INDEX:
        {
            T_DTS_CALLBACK_DATA callback_data;
            callback_data.msg_type = SERVICE_CALLBACK_TYPE_READ_CHAR_VALUE;
            callback_data.msg_index.read_index = DTS_FLOW_READ_PARA;
            if (pfn_dts_cb)
            {
                pfn_dts_cb(service_id, (void *)&callback_data);
            }
            //return the device information.
            *pp_value  = (uint8_t *)feature_info.info;
            *p_length = feature_info.lenth;
        }
        break;
    }

    return (wCause);
}

/**
 * @brief write characteristic data from service.
 *
 * @param ServiceID          ServiceID to be written.
 * @param iAttribIndex          Attribute index of characteristic.
 * @param wLength            length of value to be written.
 * @param pValue            value to be written.
 * @return Profile procedure result
*/
T_APP_RESULT DataTransAttrWriteCb(uint8_t conn_id, T_SERVER_ID ServiceId,
                                  uint16_t iAttribIndex, T_WRITE_TYPE write_type, uint16_t wLength, uint8_t *pValue,
                                  P_FUN_WRITE_IND_POST_PROC *pWriteIndPostProc)
{

    T_APP_RESULT  wCause = APP_RESULT_SUCCESS;
    T_DTS_CALLBACK_DATA callback_data;
    PROFILE_PRINT_INFO1("--> DataTransAttrWriteCb write_type = 0x%x",
                        write_type);
    callback_data.msg_type = SERVICE_CALLBACK_TYPE_WRITE_CHAR_VALUE;

    if (pValue == NULL)
    {
        wCause = APP_RESULT_INVALID_PDU;
        return wCause;
    }

    if (GATT_UUID_CHAR_DATA_INDEX == iAttribIndex)
    {
        callback_data.msg_index.write_index = DTS_WRITE_CHAR_DATA;
        callback_data.msg_data.data = pValue;
        callback_data.msg_data.len  = wLength;
//              if (dataTransInfo.device_mode.bt_flowctrl)
//              {
//                      transferConfigInfo.bt_buf_free++;
//              }

//              wCause = HandleBTReceiveData(conn_id, wLength, pValue);
    }
    else if (GATT_UUID_CHAR_FLOW_NOTIFY_INDEX == iAttribIndex)
    {
        callback_data.msg_index.write_index = DTS_WRITE_FLOW_PARA;
        callback_data.msg_data.data = pValue;
        callback_data.msg_data.len  = wLength;
        //dataTransInfo.device_mode.bt_flowctrl = pValue[0];
    }
    else
    {
        PROFILE_PRINT_ERROR2("--> DataTransAttrWriteCb Error  iAttribIndex = 0x%x wLength=%d",
                             iAttribIndex,
                             wLength);
        wCause = APP_RESULT_ATTR_NOT_FOUND;
    }
    if (pfn_dts_cb && (wCause == APP_RESULT_SUCCESS))
    {
        wCause = pfn_dts_cb(ServiceId, (void *)&callback_data);
    }
    return wCause;
}

/**
 * @brief update CCCD bits from stack.
 *
 * @param ServiceId          Service ID.
 * @param Index          Attribute index of characteristic data.
 * @param wCCCBits         CCCD bits from stack.
 * @return None
*/
void DataTransCccdUpdateCb(uint8_t conn_id, T_SERVER_ID service_id, uint16_t Index,
                           uint16_t wCCCBits)
{

    PROFILE_PRINT_INFO2("DataTransCccdUpdateCb  Index = %d wCCCDBits %x", Index, wCCCBits);
    T_APP_RESULT  cause = APP_RESULT_SUCCESS;
    T_DTS_CALLBACK_DATA callback_data;
    callback_data.msg_type = SERVICE_CALLBACK_TYPE_INDIFICATION_NOTIFICATION;
    switch (Index)
    {
    case GATT_UUID_CHAR_DATA_CCCD_INDEX:
        {
            if (wCCCBits & GATT_CLIENT_CHAR_CONFIG_NOTIFY)
            {
                callback_data.msg_index.notification_indification_index = DTS_DATA_NOTIFY_ENABLE;
            }
            else
            {
                callback_data.msg_index.notification_indification_index = DTS_DATA_NOTIFY_DISABLE;
            }
        }
        break;
    case GATT_UUID_CHAR_FLOW_CCCD_INDEX:
        {
            if (wCCCBits & GATT_CLIENT_CHAR_CONFIG_NOTIFY)
            {
                callback_data.msg_index.notification_indification_index = DTS_FLOW_NOTIFY_ENABLE;
            }
            else
            {
                callback_data.msg_index.notification_indification_index = DTS_FLOW_NOTIFY_DISABLE;
                // Disable Notification
            }
        }
        break;
    default:
        cause = APP_RESULT_ATTR_NOT_FOUND;
        break;
    }

    if (pfn_dts_cb && (cause == APP_RESULT_SUCCESS))
    {
        pfn_dts_cb(service_id, (void *)&callback_data);
    }
}

/**
 * @brief Simple ble Service Callbacks.
*/
const T_FUN_GATT_SERVICE_CBS DataTransServiceCBs =
{
    DataTransAttrReadCb,  // Read callback function pointer
    DataTransAttrWriteCb, // Write callback function pointer
    DataTransCccdUpdateCb  // CCCD update callback function pointer
};

/**
  * @brief add Simple BLE service to application.
  *
  * @param[in] pFunc          pointer of app callback function called by profile.
  * @return service ID auto generated by profile layer.
  * @retval ServiceId
  */
T_SERVER_ID Datatrans_AddService(void *pFunc)
{
    if (false == server_add_service(&DatatransServiceId,
                                    (uint8_t *)datatrans_service_tbl,
                                    sizeof(datatrans_service_tbl),
                                    DataTransServiceCBs))
    {
        PROFILE_PRINT_ERROR1("Datatrans_AddService: ServiceId %d", DatatransServiceId);
        DatatransServiceId = 0xff;
        return DatatransServiceId;
    }

    pfn_dts_cb = (P_FUN_SERVER_GENERAL_CB)pFunc;
    return DatatransServiceId;
}

#include "rtthread.h"
#include "os_sync.h"
static uint8_t testBuf[256] = {0xAA};
static void task_entry(void *p_param)
{
    uint32_t s;
    while (1)
    {
        uint8_t credits;
        s = os_lock();
        le_get_gap_param(GAP_PARAM_LE_REMAIN_CREDITS, &credits);
        os_unlock(s);
        if (credits > 0)
        {
            s = os_lock();
            server_send_data(0, DatatransServiceId, 4,
                             testBuf, 244, GATT_PDU_TYPE_NOTIFICATION);
            os_unlock(s);
            rt_kprintf("current thread name = %s \n", rt_thread_self()->name);
            rt_thread_mdelay(1000);
        }
        else
        {
            rt_thread_mdelay(10);
        }
    }
}
#include "gap_conn_le.h"
#include "stdlib.h"
static void set_interval(uint8_t argc, char **argv)
{
    uint16_t desired_interval_min = atoi(argv[1]);;
    uint16_t desired_interval_max = atoi(argv[1]);;
    uint16_t desired_latency = 0;
    uint16_t desired_superv_tout = 5000;
    le_update_conn_param(0, desired_interval_min, desired_interval_max,
                         desired_latency,
                         desired_superv_tout / 10, desired_interval_min * 2 - 2, desired_interval_max * 2 - 2);
}
MSH_CMD_EXPORT(set_interval, test test test);
static void set_2m(void)
{
    uint8_t tx_phy;
    uint8_t rx_phy;
    le_get_conn_param(GAP_PARAM_CONN_RX_PHY_TYPE, &rx_phy, 0);
    le_get_conn_param(GAP_PARAM_CONN_TX_PHY_TYPE, &tx_phy, 0);
    rt_kprintf("GAP_CONN_STATE_CONNECTED: tx_phy %d, rx_phy %d", tx_phy, rx_phy);

    uint8_t all_phys;
    uint8_t tx_phys;
    uint8_t rx_phys;
    T_GAP_PHYS_OPTIONS phy_options = GAP_PHYS_OPTIONS_CODED_PREFER_S8;

    all_phys = GAP_PHYS_PREFER_ALL;
    tx_phys = GAP_PHYS_PREFER_2M_BIT;
    rx_phys = GAP_PHYS_PREFER_2M_BIT;
    le_set_phy(0, all_phys, tx_phys, rx_phys, phy_options);
}
MSH_CMD_EXPORT(set_2m, test test test);
static void set_data_len(void)
{
    uint8_t conn_id = 0;
    uint16_t tx_octets = 251;
    uint16_t tx_time = 2120;
    le_set_data_len(conn_id, tx_octets, tx_time);
}
MSH_CMD_EXPORT(set_data_len, test test test);

static void throughput_test(void)
{
    rt_thread_startup(rt_thread_create("throughput_task", task_entry, NULL, 1024,
                                       25, 2));
}

MSH_CMD_EXPORT(throughput_test, test test test);

