/*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     communicate_protocol.c
* @brief
* @details
* @author
* @date
* @version  v0.1
*********************************************************************************************************
*/

#include "board.h"
#include "string.h"
#include "trace.h"
#include "crc16btx.h"
#include "os_msg.h"
#include "os_timer.h"
#include "communicate_protocol.h"
#include "communicate_parse.h"
//#include "ble_task.h"
#include "stdlib.h"
#include "app_msg.h"
#include "watch_msg.h"
#if USE_HRS_MASTER
#include "gcs_client.h"
#endif
/******************* Private variables **********************************/

#define MAX_RESEND_COUNT        3
extern void *l1recv_queue_handle;

/* Packet receive parameter */
static uint32_t     package_len = 0;
static uint32_t     package_offset = 0;

/* L1 layer message parameter */
static uint8_t      received_buffer[GLOBAL_RECEIVE_BUFFER_SIZE];
static uint16_t     L1_sequence_id = 0;


extern void *l1send_task_handle;



/**
* @brief  CRC check L1 reveived packet
* @param   crc_value: crc_value
* @param   pData: data pointer to be checked
* @param   length:  data length
* @retval  error code
*/
static bool l1_crc_check(uint16_t crc_value, uint8_t *pData, uint16_t length)
{
    uint16_t crc = btxfcs(0x0000, pData, length);
    if (crc == crc_value)
    {
        return true;
    }
    return false;

}

static bool communicate_send_internal(uint8_t *p_data, uint16_t data_len)
{
    //todo check link status

    T_IO_MSG msg;
    uint8_t *buf = NULL;
    buf = malloc((data_len + 1) * sizeof(uint8_t));
    if (buf == NULL)
    {
        return false;
    }

    msg.type = IO_MSG_TYPE_WRISTBNAD;
    msg.subtype = IO_MSG_PROTOCOL_SEND;
    msg.u.buf = buf;
    buf[0] = data_len;
    memcpy(buf + 1, p_data, data_len);

    if (app_send_msg_to_bt_task(&msg) == false)
    {
        free(buf);
        return false;
    }
    return true;
}

void package_prepare_send(struct protocol_pack *package)
{
    uint8_t buf[32];
    /*fill header*/
    package->l1_length = package->l2_lenght + 4;
    buf[L1_HEADER_MAGIC_POS]                = L1_HEADER_MAGIC;                               /* Magic */
    buf[L1_HEADER_PROTOCOL_VERSION_POS]     = L1_HEADER_VERSION;                  /* protocol version */
    buf[L1_PAYLOAD_LENGTH_HIGH_BYTE_POS]    = (package->l1_length) >> 8;    /* length high byte */
    buf[L1_PAYLOAD_LENGTH_LOW_BYTE_POS]     = package->l1_length;            /* length low byte */

    L1_sequence_id ++;

    /* sequence id */
    buf[L1_HEADER_SEQ_ID_HIGH_BYTE_POS]     = L1_sequence_id >> 8;
    buf[L1_HEADER_SEQ_ID_LOW_BYTE_POS]      = L1_sequence_id;

    APP_PRINT_INFO1("sequence id:%d", L1_sequence_id);

    /* next prepare L2 payload */
    buf[8] = package->l2_cmd_id;
    buf[9] = L2_HEADER_VERSION;
    buf[10] = package->l2_key;
    buf[11] = 0;
#if USE_HRS_MASTER
    buf[12] = package->l2_lenght;
#else
    buf[12] = 0;
#endif
    if (package->l2_lenght != 0)
    {
        memcpy(buf + 13, package->l2_payload, package->l2_lenght);
    }

    /*cal crc*/
    uint16_t crc16_ret = btxfcs(0, buf + L1_HEADER_SIZE, package->l1_length);
    buf[L1_HEADER_CRC16_HIGH_BYTE_POS]      = crc16_ret >> 8;
    buf[L1_HEADER_CRC16_LOW_BYTE_POS]       = crc16_ret;

    if (communicate_send_internal(buf, package->l1_length + L1_HEADER_SIZE) == false)
    {
        return;
    }

    uint8_t event;
    static uint32_t retry_count = 0;
    if (os_msg_recv(l1recv_queue_handle, &event, 3000) == true)
    {
        APP_PRINT_INFO0("receive L1 send ACK success!");
        retry_count = 0;
        return;
    }
    else
    {
        retry_count++;
        APP_PRINT_INFO1("receive L1 send ACK time out! do retry count = %d", retry_count);
        if (retry_count < MAX_RESEND_COUNT)
        {
            package_prepare_send(package);
        }
        else
        {
            APP_PRINT_ERROR1("retry fail!!! retry count= %d", retry_count);
            retry_count = 0;
        }
        return;
    }
}


/**
* @brief  Do action to received ACK packet
*         if check fail should start to resend procedure
* @param  sequence_id: L1 layer sequence id
* @param  crc_check: ACK packet crc check result ,CRC_SUCCESS or CRC_FAIL
* @retval void
*/
static void ack_package_handle(uint16_t sequence_id, bool err_flag)
{
    if (err_flag == false)
    {
        uint8_t event = sequence_id;
        if (os_msg_send(l1recv_queue_handle, &event, 0) == false)
        {
            APP_PRINT_ERROR0("send_msg_to_l1send_task_fail");
        }
    }
    else
    {
        /*do nothing only operation like ACK timeout*/
        APP_PRINT_INFO0("get the wrong ack of host");
    }
}


/**
* @brief   response to L1 reveived packet
* @param   sequence_id:   L1 sequence_id
* @param   check_success: crc check result
* @retval  error code
*/
void l1_send_ack(uint16_t sequence_id, bool check_success)
{
    uint8_t ack_package_buffer[8] = {0};
    L1_version_value_t version_ack;

    version_ack.version_def.version = L2_HEADER_VERSION;
    version_ack.version_def.ack_flag = 1;
    version_ack.version_def.err_flag = (check_success ? 0 : 1);
    version_ack.version_def.reserve = 0;

    ack_package_buffer[0] = L1_HEADER_MAGIC;
    ack_package_buffer[1] = version_ack.value;
    ack_package_buffer[2] = 0;//length
    ack_package_buffer[3] = 0;//length
    ack_package_buffer[4] = 0;//crc16
    ack_package_buffer[5] = 0;//crc16
    ack_package_buffer[6] = (sequence_id >> 8) & 0xFF;
    ack_package_buffer[7] = sequence_id & 0xFF;
#if USE_HRS_MASTER
//    communicate_send_internal(ack_package_buffer, L1_HEADER_SIZE);
    //gcs_attr_write(0, GATT_WRITE_TYPE_REQ,19, 8,(uint8_t *)ack_package_buffer);
    gcs_attr_write(0, GATT_WRITE_TYPE_REQ, 16, 8, (uint8_t *)ack_package_buffer);
    //gcs_attr_write(0, GATT_WRITE_TYPE_REQ,21, 8,(uint8_t *)ack_package_buffer);
#else
    communicate_send_internal(ack_package_buffer, L1_HEADER_SIZE);
#endif
}



/**
* @brief   deliver L1 received data to upper layer
* @param   pData: pointer to L1 received data
* @param   length:  data length
* @retval  void
*/
void resolve_remote_data(uint8_t *data, uint16_t length)
{
    static bool receiving = false;
    L1_version_value_t inner_version;

    if ((data[0] == L1_HEADER_MAGIC) && (!receiving))
    {
        package_len = (((uint16_t)(data[2])) << 8) + data[3] + 8;
        receiving = true;
    }
    else if (!receiving)
    {
        //magic code error
        APP_PRINT_ERROR0("Magic code error!");
        return;
    }

    if (package_offset + length <= GLOBAL_RECEIVE_BUFFER_SIZE)
    {
        memcpy(received_buffer + package_offset, data, length);
        package_offset += length;
    }
    else
    {
        //CMD length error
        APP_PRINT_ERROR0("CMD length larger than buffer size!");
        package_offset = 0;
        package_len = 0;
        receiving = false;
        return;
    }

    if (package_offset == package_len)
    {
        package_offset = 0;
        package_len = 0;
        receiving = false;
        //data package
        inner_version.value = received_buffer[L1_HEADER_PROTOCOL_VERSION_POS];
        uint16_t crc16_value = (received_buffer[L1_HEADER_CRC16_HIGH_BYTE_POS] << 8
                                | received_buffer[L1_HEADER_CRC16_LOW_BYTE_POS]);
        uint16_t L2_frame_length = received_buffer[L1_PAYLOAD_LENGTH_LOW_BYTE_POS]
                                   | (received_buffer[L1_PAYLOAD_LENGTH_HIGH_BYTE_POS] << 8);
        uint16_t seq_id = received_buffer[L1_HEADER_SEQ_ID_LOW_BYTE_POS]
                          | (received_buffer[L1_HEADER_SEQ_ID_HIGH_BYTE_POS] << 8);

        APP_PRINT_INFO4("ack error flag:%d, ack or data flag:%d, version:%d, seq_id:%d\n",
                        inner_version.version_def.err_flag, inner_version.version_def.ack_flag,
                        inner_version.version_def.version, seq_id);

        if (inner_version.version_def.ack_flag == RESPONSE_PACKAGE)
        {
            APP_PRINT_INFO0("receive a ack package");
            //restart receive state machine
            ack_package_handle(seq_id, inner_version.version_def.err_flag);
            return;
        }

        if (l1_crc_check(crc16_value, received_buffer + L1_HEADER_SIZE, L2_frame_length) == true)
        {
            APP_PRINT_INFO0("receive data package & send response");
            /* send response */
            l1_send_ack(seq_id, true);
            /*throw data to upper layer*/
            L2_frame_resolve(received_buffer + L1_HEADER_SIZE, L2_frame_length);
        }
        else
        {
            //send response
            l1_send_ack(seq_id, false);
            //schedule error handler
            APP_PRINT_INFO0("received data crc check error");
        }
    }
    else if (package_offset > package_len)//Received data length larger than the expected length
    {
        APP_PRINT_INFO0("CMD length larger than the expected length");
        package_offset = 0;
        package_len = 0;
        receiving = false;
    }
    else if (package_offset < package_len)//Received data length less than the expected length
    {

    }

}
