/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
   * @file      ancs.c
   * @brief     This file handles ANCS Client routines.
   * @author    jane
   * @date      2017-06-06
   * @version   v1.0
   **************************************************************************************
   * @attention
   * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
   **************************************************************************************
  */

/*============================================================================*
 *                              Header Files
 *============================================================================*/

#include <trace.h>
#include <string.h>
#include <os_msg.h>
#include <os_mem.h>
#include <ancs.h>
#include <ancs_client.h>
#ifdef RTK_MODULE_USING_DATABASE
#include "dp_notification.h"
#endif // RTK_MODULE_USING_DATABASE

//#include <ams_client.h>
/** @defgroup  PERIPH_ANCS Peripheral ANCS
    * @brief Apple ANCS service modulization
    * @{
    */
/*============================================================================*
 *                         Types
 *============================================================================*/
typedef struct
{
    uint8_t                m_parse_state;
    uint8_t                app_type;
    uint16_t               current_len;
    void                  *ancs_queue_handle;
    uint8_t               *ptr;
    T_DS_NOTIFICATION_ATTR notification_attr;
#if F_BT_ANCS_GET_APP_ATTR
    T_DS_APP_ATTR          app_attr;
#endif
} T_APP_ANCS_LINK;

typedef struct
{
    uint16_t   total_len;
    uint8_t    total_data[ANCS_MAX_APP_IDENTIFIER_LEN + ANCS_MAX_SUB_TITLE_LEN + ANCS_MAX_ATTR_LEN];
} T_DS_NOTIFICATION_ATTR_TOTAL;

/*============================================================================*
 *                              Variables
 *============================================================================*/
/** @defgroup PERIPH_ANCS_Exported_Variables ANCS Exported Variables
   * @brief app register ancs client to upperstack, and return ancs client id
   * @{
   */
T_CLIENT_ID      ancs_client; /**< ancs client id*/

T_APP_ANCS_LINK *ancs_link_table;
uint8_t          ancs_link_number;
uint8_t  call_conn_id;
uint32_t call_notification_uid;
T_DS_NOTIFICATION_ATTR_TOTAL ds_notification_attr_total;

/** End of PERIPH_ANCS_Exported_Variables
    * @}
    */
extern void *evt_queue_handle;  //!< Event queue handle
extern void *io_queue_handle;   //!< IO queue handle
/*============================================================================*
 *                              Functions
 *============================================================================*/
/** @defgroup PERIPH_ANCS_Exported_Functions ANCS Exported Functions
   * @{
   */
void ancs_send_msg_to_app(uint8_t conn_id)
{
    T_IO_MSG io_msg;
    uint8_t event = EVENT_IO_TO_APP;
    io_msg.type = IO_MSG_TYPE_ANCS;
    io_msg.subtype = 0;
    io_msg.u.param = conn_id;

    if (os_msg_send(io_queue_handle, &io_msg, 0) == false)
    {
        GAP_PRINT_ERROR0("ancs_send_msg_to_app fail1");
    }
    else if (os_msg_send(evt_queue_handle, &event, 0) == false)
    {
        GAP_PRINT_ERROR0("ancs_send_msg_to_app fail2");
    }
}

void app_handle_notification_attribute_data(T_APP_ANCS_LINK *p_ancs_link)
{
    if (p_ancs_link->notification_attr.attribute_id != DS_NOTIFICATION_ATTR_ID_NEGATIVE_ACTION_LABEL)
    {
        p_ancs_link->m_parse_state = DS_PARSE_ATTRIBUTE_ID;
        uint8_t *p_value = p_ancs_link->notification_attr.data;

#if F_BT_ANCS_APP_FILTER
        //filter QQ , wechat , short message and incomming call
        if (p_ancs_link->notification_attr.attribute_id == DS_NOTIFICATION_ATTR_ID_APP_IDENTIFIER)
        {
#if F_BT_ANCS_CLIENT_DEBUG
            APP_PRINT_INFO1("parse notify attr: app identifiter %s", TRACE_STRING(p_value));
#endif
            //wechat
            if (0 == memcmp(p_value, "com.tencent.xin", 16))
            {
                p_ancs_link->app_type = 1;
                ds_notification_attr_total.total_data[0] = 'w'; //sync tuya
                ds_notification_attr_total.total_len = 1;
            }
            else if (0 == memcmp(p_value, "com.apple.MobileSMS", 20))
            {
                p_ancs_link->app_type = 2;
                ds_notification_attr_total.total_data[0] = 's'; //sync tuya
                ds_notification_attr_total.total_len = 1;
            }
            else if (0 == memcmp(p_value, "com.apple.mobilephone", 22))
            {
                p_ancs_link->app_type = 3;
                ds_notification_attr_total.total_data[0] = 'p'; //sync tuya
                ds_notification_attr_total.total_len = 1;
            }
            else if (0 == memcmp(p_value, "com.tencent.mqq", 16))
            {
                p_ancs_link->app_type = 4;
                ds_notification_attr_total.total_data[0] = 'q'; //sync tuya
                ds_notification_attr_total.total_len = 1;
            }
            else if (0 == memcmp(p_value, "com.tencent.qq", 15))
            {
                p_ancs_link->app_type = 4;
                ds_notification_attr_total.total_data[0] = 'q'; //sync tuya
                ds_notification_attr_total.total_len = 1;
            }
        }
        if (p_ancs_link->notification_attr.attribute_id == DS_NOTIFICATION_ATTR_ID_TITLE)
        {
            if ((p_ancs_link->app_type == 1))
            {
                APP_PRINT_INFO1("Wechat: message %s", TRACE_STRING(p_value));
                memcpy(ds_notification_attr_total.total_data + ds_notification_attr_total.total_len, p_value,
                       p_ancs_link->current_len);
                ds_notification_attr_total.total_len += p_ancs_link->current_len;
                ds_notification_attr_total.total_data[ds_notification_attr_total.total_len] = ':';
                ds_notification_attr_total.total_len += 1;
            }
            else if ((p_ancs_link->app_type == 2))
            {
                APP_PRINT_INFO1("MobileSMS: message %s", TRACE_STRING(p_value));
                memcpy(ds_notification_attr_total.total_data + ds_notification_attr_total.total_len, p_value,
                       p_ancs_link->current_len);
                ds_notification_attr_total.total_len += p_ancs_link->current_len;
                ds_notification_attr_total.total_data[ds_notification_attr_total.total_len] = ':';
                ds_notification_attr_total.total_len += 1;
            }
            else if ((p_ancs_link->app_type == 3))
            {
                APP_PRINT_INFO1("MobileCall: Name or Phone number %s", TRACE_STRING(p_value));
                memcpy(ds_notification_attr_total.total_data + ds_notification_attr_total.total_len, p_value,
                       p_ancs_link->current_len);
                ds_notification_attr_total.total_len += p_ancs_link->current_len;
                ds_notification_attr_total.total_data[ds_notification_attr_total.total_len] = ':';
                ds_notification_attr_total.total_len += 1;
            }
            else if ((p_ancs_link->app_type == 4))
            {
                APP_PRINT_INFO1("QQ: message %s", TRACE_STRING(p_value));
            }
        }
        if (p_ancs_link->notification_attr.attribute_id == DS_NOTIFICATION_ATTR_ID_SUB_TITLE)
        {
            if ((p_ancs_link->app_type == 1))
            {
                APP_PRINT_INFO1("Wechat: message %s", TRACE_STRING(p_value));
            }
            else if ((p_ancs_link->app_type == 2))
            {
                APP_PRINT_INFO1("MobileSMS: message %s", TRACE_STRING(p_value));
            }
            else if ((p_ancs_link->app_type == 3))
            {
                APP_PRINT_INFO1("MobileCall: Name or Phone number %s", TRACE_STRING(p_value));
            }
            else if ((p_ancs_link->app_type == 4))
            {
                APP_PRINT_INFO1("QQ: message %s", TRACE_STRING(p_value));
                memcpy(ds_notification_attr_total.total_data + ds_notification_attr_total.total_len, p_value,
                       p_ancs_link->current_len);
                ds_notification_attr_total.total_len += p_ancs_link->current_len;
            }
        }
        if (p_ancs_link->notification_attr.attribute_id == DS_NOTIFICATION_ATTR_ID_MESSAGE)
        {
            memcpy(ds_notification_attr_total.total_data + ds_notification_attr_total.total_len, p_value,
                   p_ancs_link->current_len);
            ds_notification_attr_total.total_len += p_ancs_link->current_len;
            ds_notification_attr_total.total_data[ds_notification_attr_total.total_len] = '\0';
            set_notification(ds_notification_attr_total.total_len, ds_notification_attr_total.total_data);
            APP_PRINT_INFO1("%s", TRACE_STRING(ds_notification_attr_total.total_data));

            ds_notification_attr_total.total_data[ds_notification_attr_total.total_len] = '\0';
#if 1// test code
            uint8_t msg_num = notification_current_num_get();

            for (uint8_t i = msg_num; i > 0; i--)
            {
                get_notification(ds_notification_attr_total.total_len, ds_notification_attr_total.total_data, i);
                APP_PRINT_INFO2("msg_num %d %s", i, TRACE_STRING(ds_notification_attr_total.total_data));
            }
#endif
            ds_notification_attr_total.total_len = 0;
        }
        if (p_ancs_link->notification_attr.attribute_id == DS_NOTIFICATION_ATTR_ID_DATE)
        {
#if F_BT_ANCS_CLIENT_DEBUG
            APP_PRINT_INFO1("parse notify attr: date %s", TRACE_STRING(p_value));
#endif
        }
#endif
    }
    else/* All attributes has been parased*/
    {
#if F_BT_ANCS_CLIENT_DEBUG
        APP_PRINT_INFO0("parse notify attr: parse done");
#endif
        p_ancs_link->m_parse_state = DS_PARSE_NOT_START;
        memset(&p_ancs_link->notification_attr, 0, sizeof(T_DS_NOTIFICATION_ATTR));
    }
}

void app_parse_notification_attribute(T_APP_ANCS_LINK *p_ancs_link, uint8_t *p_data, uint8_t len)
{
    int i;

    for (i = 0; i < len; i++)
    {
        switch (p_ancs_link->m_parse_state)
        {
        case DS_PARSE_GET_NOTIFICATION_COMMAND_ID:
            p_ancs_link->notification_attr.command_id = p_data[i];
            p_ancs_link->m_parse_state = DS_PARSE_UID1;
            break;

        case DS_PARSE_UID1:
            p_ancs_link->notification_attr.notification_uid[0] = p_data[i];
            p_ancs_link->m_parse_state = DS_PARSE_UID2;
            break;

        case DS_PARSE_UID2:
            p_ancs_link->notification_attr.notification_uid[1] = p_data[i];
            p_ancs_link->m_parse_state = DS_PARSE_UID3;
            break;

        case DS_PARSE_UID3:
            p_ancs_link->notification_attr.notification_uid[2] = p_data[i];
            p_ancs_link->m_parse_state = DS_PARSE_UID4;
            break;

        case DS_PARSE_UID4:
            p_ancs_link->notification_attr.notification_uid[3] = p_data[i];
            p_ancs_link->m_parse_state = DS_PARSE_ATTRIBUTE_ID;
            break;

        case DS_PARSE_ATTRIBUTE_ID:
            p_ancs_link->notification_attr.attribute_id = p_data[i];
            p_ancs_link->m_parse_state = DS_PARSE_ATTRIBUTE_LEN1;
            break;

        case DS_PARSE_ATTRIBUTE_LEN1:
            p_ancs_link->notification_attr.attribute_len = p_data[i];
            p_ancs_link->m_parse_state = DS_PARSE_ATTRIBUTE_LEN2;
            break;

        case DS_PARSE_ATTRIBUTE_LEN2:
            p_ancs_link->notification_attr.attribute_len |= (p_data[i] << 8);
            p_ancs_link->m_parse_state = DS_PARSE_ATTRIBUTE_READY;
            p_ancs_link->ptr = p_ancs_link->notification_attr.data;
            p_ancs_link->current_len = 0;
#if F_BT_ANCS_CLIENT_DEBUG
            APP_PRINT_INFO2("parse notify attr: attribute_id %d, attribute_len %d",
                            p_ancs_link->notification_attr.attribute_id,
                            p_ancs_link->notification_attr.attribute_len
                           );
#endif
            if (p_ancs_link->notification_attr.attribute_len == 0)
            {
                p_ancs_link->m_parse_state = DS_PARSE_ATTRIBUTE_ID;
            }
            if (p_ancs_link->notification_attr.attribute_len > ANCS_MAX_ATTR_LEN)
            {
#if F_BT_ANCS_CLIENT_DEBUG
                APP_PRINT_ERROR2("parse notify attr: error, attribute_len %d > max length %d",
                                 p_ancs_link->notification_attr.attribute_len,
                                 ANCS_MAX_ATTR_LEN
                                );
#endif
                p_ancs_link->m_parse_state = DS_PARSE_NOT_START;
                memset(&p_ancs_link->notification_attr, 0, sizeof(T_DS_NOTIFICATION_ATTR));
            }
            break;

        case DS_PARSE_ATTRIBUTE_READY:
            *p_ancs_link->ptr++ = p_data[i];
            p_ancs_link->current_len++;

            if (p_ancs_link->current_len == p_ancs_link->notification_attr.attribute_len)
            {
                /*An attribute is always a string whose length in bytes is provided in the tuple but that is not NULL-terminated.*/
                *p_ancs_link->ptr++ = 0;
#if F_BT_ANCS_CLIENT_DEBUG
                APP_PRINT_INFO1("parse notify attr: data %b",
                                TRACE_BINARY(p_ancs_link->notification_attr.attribute_len,
                                             p_ancs_link->notification_attr.data));
#endif
                app_handle_notification_attribute_data(p_ancs_link);
            }
            break;
        }
    }
}

#if F_BT_ANCS_GET_APP_ATTR
void app_parse_app_attribute(T_APP_ANCS_LINK *p_ancs_link, uint8_t *p_data, uint8_t len)
{
    int i;
    for (i = 0; i < len; i++)
    {
        switch (p_ancs_link->m_parse_state)
        {
        case DS_PARSE_GET_APP_COMMAND_ID:
            p_ancs_link->app_attr.command_id = p_data[i];
            p_ancs_link->m_parse_state = DS_PARSE_APP_IDENTIFIER_START;
            break;

        case DS_PARSE_APP_IDENTIFIER_START:
            if (p_data[i] == 0x00)
            {
                p_ancs_link->m_parse_state = DS_PARSE_APP_IDENTIFIER_END;

                if (i + 1 == len)
                {
                    p_ancs_link->m_parse_state = DS_PARSE_NOT_START;
                }
                else
                {
                    p_ancs_link->m_parse_state = DS_PARSE_APP_ATTRIBUTE_ID;
                }

            }
            break;

        case DS_PARSE_APP_ATTRIBUTE_ID:
            p_ancs_link->app_attr.attribute_id = p_data[i];
            p_ancs_link->m_parse_state = DS_PARSE_APP_ATTRIBUTE_LEN1;
            break;

        case DS_PARSE_APP_ATTRIBUTE_LEN1:
            p_ancs_link->app_attr.attribute_len = p_data[i];
            p_ancs_link->m_parse_state = DS_PARSE_APP_ATTRIBUTE_LEN2;
            break;

        case DS_PARSE_APP_ATTRIBUTE_LEN2:
            p_ancs_link->app_attr.attribute_len |= (p_data[i] << 8);
            p_ancs_link->m_parse_state = DS_PARSE_APP_ATTRIBUTE_READY;
            p_ancs_link->ptr = p_ancs_link->app_attr.data;
            p_ancs_link->current_len = 0;
#if F_BT_ANCS_CLIENT_DEBUG
            APP_PRINT_INFO2("parse app attr: attribute_id %d, attribute_len %d",
                            p_ancs_link->app_attr.attribute_id,
                            p_ancs_link->app_attr.attribute_len
                           );
#endif
            if (p_ancs_link->app_attr.attribute_len == 0)
            {
                p_ancs_link->m_parse_state = DS_PARSE_APP_ATTRIBUTE_ID;

            }
            if (p_ancs_link->app_attr.attribute_len > ANCS_MAX_ATTR_LEN)
            {
#if F_BT_ANCS_CLIENT_DEBUG
                APP_PRINT_ERROR2("parse app attr: error, attribute_len %d > max length %d",
                                 p_ancs_link->app_attr.attribute_len,
                                 ANCS_MAX_ATTR_LEN
                                );
#endif
                p_ancs_link->m_parse_state = DS_PARSE_NOT_START;
            }
            break;

        case DS_PARSE_APP_ATTRIBUTE_READY:
            *p_ancs_link->ptr++ = p_data[i];
            p_ancs_link->current_len++;

            if (p_ancs_link->current_len == p_ancs_link->app_attr.attribute_len)
            {
                *p_ancs_link->ptr++ = 0;
#if F_BT_ANCS_CLIENT_DEBUG
                APP_PRINT_INFO4("parse app attr: command_id 0x%x, attribute_id 0x%x, attribute_len %d, data %s",
                                p_ancs_link->app_attr.command_id,
                                p_ancs_link->app_attr.attribute_id,
                                p_ancs_link->app_attr.attribute_len,
                                TRACE_STRING(p_ancs_link->app_attr.data));
#endif
                p_ancs_link->m_parse_state = DS_PARSE_NOT_START;

            }
            break;
        }
    }
}
#endif

/**
 * @brief  Parse ancs data source notification
 * @param[in]  conn_id connection identifier
 * @param[in]  *p_data  point to data buffer
 * @param[in]  len  data length
 * @return void
 */
void app_parse_data_soucre_notifications(uint8_t conn_id, uint8_t *p_data, uint8_t len)
{
    APP_PRINT_INFO2("ANCS_FROM_DATA_SOURCE: conn_id %d, len =%d", conn_id, len);
#if F_BT_ANCS_CLIENT_DEBUG
    APP_PRINT_INFO1("data = %b", TRACE_BINARY(len, p_data));
#endif
    T_APP_ANCS_LINK *p_ancs_link = &ancs_link_table[conn_id];

#if F_BT_ANCS_CLIENT_DEBUG
    APP_PRINT_INFO1("m_parse_state %d", p_ancs_link->m_parse_state);
#endif
    if (p_ancs_link->m_parse_state == DS_PARSE_NOT_START)
    {
        if (len >= 1 && p_data[0] == CP_CMD_ID_GET_NOTIFICATION_ATTR)
        {
            p_ancs_link->m_parse_state = DS_PARSE_GET_NOTIFICATION_COMMAND_ID;
        }
#if F_BT_ANCS_GET_APP_ATTR
        else if (len >= 1 && p_data[0] == CP_CMD_ID_GET_APP_ATTR)
        {
            p_ancs_link->m_parse_state = DS_PARSE_GET_APP_COMMAND_ID;
        }
#endif
    }

    if (p_ancs_link->m_parse_state < DS_PARSE_GET_APP_COMMAND_ID)
    {
        app_parse_notification_attribute(p_ancs_link, p_data, len);
    }
#if F_BT_ANCS_GET_APP_ATTR
    else
    {
        app_parse_app_attribute(p_ancs_link, p_data, len);
    }
#endif
}

void app_parse_notification_source_data(uint8_t conn_id, uint8_t *p_data, uint8_t len)
{
    if (8 == len)
    {
        T_NS_DATA ns_data;

        memcpy(&ns_data, p_data, len);
#if F_BT_ANCS_CLIENT_DEBUG
        APP_PRINT_INFO5("app_parse_notification_source_data: event_id %d, event_flags 0x%02x, category_id %d, category_count %d, notification_uid 0x%08x",
                        ns_data.event_id,
                        ns_data.event_flags,
                        ns_data.category_id,
                        ns_data.category_count,
                        ns_data.notification_uid
                       );
        APP_PRINT_INFO5("event_flags: slient %d, important %d, pre existing %d, positive action %d, negative action %d ",
                        ns_data.event_flags & NS_EVENT_FLAG_SILENT,
                        ns_data.event_flags & NS_EVENT_FLAG_IMPORTANT,
                        ns_data.event_flags & NS_EVENT_FLAG_PRE_EXISTING,
                        ns_data.event_flags & NS_EVENT_FLAG_POSITIVE_ACTION,
                        ns_data.event_flags & NS_EVENT_FLAG_NEGATIVE_ACTION
                       );
#endif
        //you can filter by category_id here, for demo purpose, we didn't filter any CategoryID here.
#if F_BT_ANCS_APP_FILTER
        //filter social and other category & phone category & email category
        if ((ns_data.category_id == NS_CATEGORY_ID_SOCIAL ||
             ns_data.category_id == NS_CATEGORY_ID_OTHER ||
             ns_data.category_id == NS_CATEGORY_ID_INCOMING_CALL ||
             ns_data.category_id == NS_CATEGORY_ID_EMAIL) &&
            ((ns_data.event_flags & NS_EVENT_FLAG_PRE_EXISTING) != 0x04))
        {
#endif
            if (ns_data.category_id == NS_CATEGORY_ID_INCOMING_CALL)
            {
                call_conn_id = conn_id;
                call_notification_uid = ns_data.notification_uid;
                //refuse or accept incomming call
                if (ns_data.event_id == NS_EVENT_ID_NOTIFICATION_REMOVED)
                {
                    //wristband_system_interact(INTERACT_CALLING_REFUSE, 0, NULL);//accept or refuse same action
                }
            }
            if (ns_data.event_id != NS_EVENT_ID_NOTIFICATION_REMOVED)
            {
                uint32_t msg_num;
                T_ANCS_MSG ancs_msg;
                uint8_t attr_id_list[14];
                uint8_t cur_index = 0;

                attr_id_list[cur_index++] = DS_NOTIFICATION_ATTR_ID_APP_IDENTIFIER;
                attr_id_list[cur_index++] = DS_NOTIFICATION_ATTR_ID_TITLE;
                attr_id_list[cur_index++] = LO_WORD(ANCS_MAX_APP_IDENTIFIER_LEN);
                attr_id_list[cur_index++] = HI_WORD(ANCS_MAX_APP_IDENTIFIER_LEN);

                attr_id_list[cur_index++] = DS_NOTIFICATION_ATTR_ID_SUB_TITLE;
                attr_id_list[cur_index++] = LO_WORD(ANCS_MAX_SUB_TITLE_LEN);
                attr_id_list[cur_index++] = HI_WORD(ANCS_MAX_SUB_TITLE_LEN);

                attr_id_list[cur_index++] = DS_NOTIFICATION_ATTR_ID_MESSAGE;
                attr_id_list[cur_index++] = LO_WORD(ANCS_MAX_ATTR_LEN);
                attr_id_list[cur_index++] = HI_WORD(ANCS_MAX_ATTR_LEN);

                attr_id_list[cur_index++] = DS_NOTIFICATION_ATTR_ID_MESSAGE_SIZE;
                attr_id_list[cur_index++] = DS_NOTIFICATION_ATTR_ID_DATE;
                attr_id_list[cur_index++] = DS_NOTIFICATION_ATTR_ID_POSITIVE_ACTION_LABEL;
                attr_id_list[cur_index++] = DS_NOTIFICATION_ATTR_ID_NEGATIVE_ACTION_LABEL;

                os_msg_queue_peek(ancs_link_table[conn_id].ancs_queue_handle, &msg_num);
#if F_BT_ANCS_CLIENT_DEBUG
                APP_PRINT_INFO1("app_parse_notification_source_data: msg_num %d", msg_num);
#endif
                if (msg_num == 0)
                {
                    if (ancs_get_notification_attr(conn_id, ns_data.notification_uid, attr_id_list,
                                                   cur_index) == true)
                    {
                        return;
                    }
                }

                ancs_msg.type = ANCS_MSG_TYPE_GET_NOTIFI_ATTR;
                ancs_msg.data.notifi_attr.conn_id = conn_id;
                ancs_msg.data.notifi_attr.notification_uid = ns_data.notification_uid;
                ancs_msg.data.notifi_attr.attribute_ids_len = cur_index;
                memcpy(ancs_msg.data.notifi_attr.attribute_ids, attr_id_list, cur_index);
                if (os_msg_send(ancs_link_table[conn_id].ancs_queue_handle, &ancs_msg, 0) == false)
                {
                    APP_PRINT_ERROR0("app_parse_notification_source_data: discard, msg queue is full");
                }
            }
#if F_BT_ANCS_APP_FILTER
        }
#endif
    }
}

/**
 * @brief  Ancs clinet callback handle message from upperstack
 * @param[in]  client_id client identifier
 * @param[in]  conn_id connection identifier
 * @param[in]  *p_data  point to data buffer
 * @return @ref T_APP_RESULT
 */
T_APP_RESULT ancs_client_cb(T_CLIENT_ID client_id, uint8_t conn_id, void *p_data)
{
    T_APP_RESULT  result = APP_RESULT_SUCCESS;
    T_ANCS_CB_DATA *p_cb_data = (T_ANCS_CB_DATA *)p_data;

    switch (p_cb_data->cb_type)
    {
    case ANCS_CLIENT_CB_TYPE_DISC_STATE:
        switch (p_cb_data->cb_content.disc_state)
        {
        case DISC_ANCS_DONE:
            APP_PRINT_INFO0("ANCS BLE Client CB: discover procedure done.");
            ancs_set_data_source_notify(conn_id, true);
            break;
        case DISC_ANCS_FAILED:
            APP_PRINT_ERROR0("ANCS BLE Client CB: discover request failed.");
            break;
        default:
            break;
        }
        break;

    case ANCS_CLIENT_CB_TYPE_NOTIF_IND_RESULT:
        switch (p_cb_data->cb_content.notify_data.type)
        {
        case ANCS_FROM_DATA_SOURCE:
            ;
            APP_PRINT_INFO2("ANCS_FROM_DATA_SOURCE: conn_id %d, length  %d",
                            conn_id, p_cb_data->cb_content.notify_data.value_size);
            app_parse_data_soucre_notifications(conn_id, p_cb_data->cb_content.notify_data.p_value,
                                                p_cb_data->cb_content.notify_data.value_size);
            break;
        case ANCS_FROM_NOTIFICATION_SOURCE:
            APP_PRINT_INFO2("ANCS_FROM_NOTIFICATION_SOURCE: conn_id %d, length  %d",
                            conn_id, p_cb_data->cb_content.notify_data.value_size);
            app_parse_notification_source_data(conn_id, p_cb_data->cb_content.notify_data.p_value,
                                               p_cb_data->cb_content.notify_data.value_size);
            break;
        default:
            break;
        }
        break;

    case ANCS_CLIENT_CB_TYPE_WRITE_RESULT:
        {
            if (p_cb_data->cb_content.write_result.cause != ATT_SUCCESS)
            {
                APP_PRINT_ERROR1("ANCS_CLIENT_CB_TYPE_WRITE_RESULT: Failed, cause 0x%x",
                                 p_cb_data->cb_content.write_result.cause);
            }
            switch (p_cb_data->cb_content.write_result.type)
            {
            case ANCS_WRITE_NOTIFICATION_SOURCE_NOTIFY_ENABLE:
                APP_PRINT_INFO0("ANCS_WRITE_NOTIFICATION_SOURCE_NOTIFY_ENABLE");
                //ams_start_discovery(conn_id);
                break;

            case ANCS_WRITE_NOTIFICATION_SOURCE_NOTIFY_DISABLE:
                APP_PRINT_INFO0("ANCS_WRITE_NOTIFICATION_SOURCE_NOTIFY_DISABLE");
                break;

            case ANCS_WRITE_DATA_SOURCE_NOTIFY_ENABLE:
                APP_PRINT_INFO0("ANCS_WRITE_DATA_SOURCE_NOTIFY_ENABLE");
                ancs_set_notification_source_notify(conn_id, true);
                break;
            case ANCS_WRITE_DATA_SOURCE_NOTIFY_DISABLE:
                APP_PRINT_INFO0("ANCS_WRITE_DATA_SOURCE_NOTIFY_DISABLE");
                break;

            case ANCS_WRITE_CONTROL_POINT:
                APP_PRINT_INFO0("ANCS_WRITE_CONTROL_POINT");
#if F_BT_ANCS_CLIENT_DEBUG
                if (p_cb_data->cb_content.write_result.cause == 0x4A0)
                {
                    APP_PRINT_ERROR0("The commandID was not recognized by the NP.");
                }
                else if (p_cb_data->cb_content.write_result.cause == 0x4A1)
                {
                    APP_PRINT_ERROR0("The command was improperly formatted.");
                }
                else if (p_cb_data->cb_content.write_result.cause == 0x4A2)
                {
                    APP_PRINT_ERROR0("One of the parameters (for example, the NotificationUID) does not refer to an existing object on the NP.");
                }
                else if (p_cb_data->cb_content.write_result.cause == 0x4A3)
                {
                    APP_PRINT_ERROR0("The action was not performed.");
                }
#endif
                uint32_t msg_num;
                os_msg_queue_peek(ancs_link_table[conn_id].ancs_queue_handle, &msg_num);
                if (msg_num)
                {
                    ancs_send_msg_to_app(conn_id);
                }
                break;

            default:
                break;
            }
        }
        break;

    case ANCS_CLIENT_CB_TYPE_DISCONNECT_INFO:
        {
            T_ANCS_MSG ancs_msg;
            void *ancs_queue_handle = ancs_link_table[conn_id].ancs_queue_handle;
            APP_PRINT_INFO1("ANCS_CLIENT_CB_TYPE_DISCONNECT_INFO: conn_id = 0x%x", conn_id);
            memset(&ancs_link_table[conn_id], 0, sizeof(T_APP_ANCS_LINK));
            ancs_link_table[conn_id].ancs_queue_handle = ancs_queue_handle;
            /*release msg queue*/
            while (os_msg_recv(ancs_link_table[conn_id].ancs_queue_handle, &ancs_msg, 0));
        }
        break;

    default:
        break;
    }
    return result;
}

void ancs_handle_msg(T_IO_MSG *p_io_msg)
{
    uint8_t conn_id = p_io_msg->u.param;
    T_ANCS_MSG ancs_msg;
    if (os_msg_recv(ancs_link_table[conn_id].ancs_queue_handle, &ancs_msg, 0) == false)
    {
        APP_PRINT_INFO1("ancs_handle_msg: conn_id 0x%x os_msg_recv null(no msg in ancs queue)", conn_id);
        return;
    }
    if (ancs_msg.type == ANCS_MSG_TYPE_GET_NOTIFI_ATTR)
    {
        APP_PRINT_INFO1("ANCS_MSG_TYPE_GET_NOTIFI_ATTR: notification_uid 0x%x",
                        ancs_msg.data.notifi_attr.notification_uid);
        if (ancs_get_notification_attr(ancs_msg.data.notifi_attr.conn_id,
                                       ancs_msg.data.notifi_attr.notification_uid,
                                       ancs_msg.data.notifi_attr.attribute_ids, ancs_msg.data.notifi_attr.attribute_ids_len
                                      ) == false)
        {
            APP_PRINT_ERROR0("ANCS_MSG_TYPE_GET_NOTIFI_ATTR: Failed");
        }

    }
    else if (ancs_msg.type == ANCS_MSG_TYPE_PERFORM_NOTIFI_ACTION)
    {
        APP_PRINT_INFO1("ANCS_MSG_TYPE_PERFORM_NOTIFI_ACTION: notification_uid 0x%x",
                        ancs_msg.data.perform_action.notification_uid);
        if (ancs_perform_notification_action(ancs_msg.data.perform_action.conn_id,
                                             ancs_msg.data.perform_action.notification_uid,
                                             ancs_msg.data.perform_action.action_id) == false)
        {
            APP_PRINT_ERROR0("ANCS_MSG_TYPE_PERFORM_NOTIFI_ACTION: Failed");
        }

    }
#if F_BT_ANCS_GET_APP_ATTR
    else if (ancs_msg.type == ANCS_MSG_TYPE_GET_APP_ATTR)
    {
        APP_PRINT_INFO1("ANCS_MSG_TYPE_GET_APP_ATTR: app_identifier %s",
                        TRACE_STRING(ancs_msg.data.app_attr.app_identifier));
        if (ancs_get_app_attr(ancs_msg.data.app_attr.conn_id,
                              ancs_msg.data.app_attr.app_identifier,
                              ancs_msg.data.app_attr.attribute_ids,
                              ancs_msg.data.app_attr.attribute_ids_len) == false)
        {
            APP_PRINT_ERROR0("ANCS_MSG_TYPE_GET_APP_ATTR: Failed");
        }

    }
#endif
}

/**
 * @brief  App register ancs client to upperstack.
 *         This ancs_client_cb callback function will handle message.
 * @param[in] link_num Initialize link number
 * @return void
 */
void ancs_init(uint8_t link_num)
{
    uint8_t i;
    ancs_link_number = link_num;
    ancs_link_table = os_mem_zalloc(RAM_TYPE_DATA_ON, ancs_link_number * sizeof(T_APP_ANCS_LINK));
    if (ancs_link_table == NULL)
    {
        APP_PRINT_ERROR0("ancs_init: allocate buffer failed");
    }
    for (i = 0; i < ancs_link_number; i++)
    {
#if defined RTL8772F || defined RTL8762G
        if (os_msg_queue_create(&(ancs_link_table[i].ancs_queue_handle), "ancsQ", ANCS_MSG_QUEUE_NUM,
                                sizeof(T_ANCS_MSG)) == false)
        {
            APP_PRINT_ERROR2("ancs_init: link_num %d, i 0x%x create queue failed", link_num, i);
        }
#elif defined RTL8762D
        if (os_msg_queue_create(&(ancs_link_table[i].ancs_queue_handle), ANCS_MSG_QUEUE_NUM,
                                sizeof(T_ANCS_MSG)) == false)
        {
            APP_PRINT_ERROR2("ancs_init: link_num %d, i 0x%x create queue failed", link_num, i);
        }
#endif
    }
    ancs_client = ancs_add_client(ancs_client_cb, link_num);
}
/** @} */ /* End of group PERIPH_ANCS_Exported_Functions */
/** @} */ /* End of group PERIPH_ANCS */

