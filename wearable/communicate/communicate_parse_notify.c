/*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     communicate_parse_notify.c
* @brief
* @details
* @author
* @date
* @version  v0.1
*********************************************************************************************************
*/

#include "board.h"
#include "communicate_parse.h"
#include "communicate_parse_notify.h"
#include "communicate_protocol.h"
#include "communicate_task.h"
#include "string.h"
#include "trace.h"
#include "app_cluster_data.h"

app_phone_data current_phone_status;
static P_FUNC_MES_CB notify_mes_cb = NULL;
static P_FUNC_NOTIFY_CB notify_call_cb = NULL;
//Incomming_Message_Notify_Bit_t;
uint8_t call_status = 0;

void Notify_register_message_cb(P_FUNC_MES_CB cb)
{
    notify_mes_cb = cb;
}
void Notify_register_call_cb(P_FUNC_NOTIFY_CB cb)
{
    notify_call_cb = cb;
}
void accept_call()
{
    APP_PRINT_INFO2("call status is %d,line is %d", call_status, __LINE__);
    if (call_status == KEY_INCOMMING_CALL)
    {
        struct protocol_pack p = {0};
        p.l2_cmd_id = NOTIFY_COMMAND_ID;
        p.l2_key = 0x07;
        p.l2_lenght = 1;
        p.l2_payload[0] = 0;
        package_prepare_send(&p);
    }
}
void reject_call()
{

    APP_PRINT_INFO2("call status is %d,line is %d", call_status, __LINE__);
    if ((call_status == KEY_INCOMMING_CALL_ACCEPT) || (call_status == KEY_INCOMMING_CALL))
    {
        APP_PRINT_INFO0("reject_call");
        struct protocol_pack p = {0};
        p.l2_cmd_id = NOTIFY_COMMAND_ID;
        p.l2_key = 0x05;
        p.l2_lenght = 1;
        p.l2_payload[0] = 0;
        package_prepare_send(&p);
    }
}
void send_call_handle(uint8_t handle_type)
{
    if (handle_type == 0x05)
    {
        cb_send_to_queue(reject_call);
    }
    else if (handle_type == 0x07)
    {
        cb_send_to_queue(accept_call);
    }
}
/**
* @brief   resolve notify command received from remote APP
* @param   key: L2 key
* @param   pValue: received value pointer
* @param   length: value length
* @retval  error code
*/
void resolve_Notify_command(uint8_t key, uint8_t *pValue, uint16_t length, uint8_t messaye_type)
{
    APP_PRINT_INFO1("resolve_Notify_command KEY is %d", key);
    APP_PRINT_INFO1("resolve_Notify_command pValue is %s", TRACE_STRING(pValue));
    APP_PRINT_INFO1("resolve_Notify_command pValue is %b", TRACE_BINARY(length, pValue));
    call_status = key;
    switch (key)
    {
    case KEY_INCOMMING_CALL:

    case KEY_INCOMMING_CALL_ACCEPT:

    case KEY_INCOMMING_CALL_REFUSE:
        {
            app_cluster_data_update_phone_status(key, pValue, length);
//            if (notify_call_cb != NULL)
//            {
//                notify_call_cb(key, pValue, length);
//            }
            break;
        }
    case KEY_INCOMMING_MESSAGE:
        {
            app_cluster_data_update_message_status(pValue + 1, length, messaye_type);
//            if (notify_mes_cb != NULL)
//            {
//                notify_mes_cb(length, pValue + 1);
//            }
            break;
        }
    default:
        break;
    }
}
