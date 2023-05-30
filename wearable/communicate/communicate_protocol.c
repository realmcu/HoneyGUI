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
#include "ble_cb.h"
#include "crc16btx.h"
#include "gap_conn_le.h"
#include "os_msg.h"
#include "os_timer.h"
#include "ftl.h"
#include "ancs_client.h"
#include "ancs.h"
#include "wristband_private_service.h"
#include "communicate_protocol.h"
#include "communicate_parse.h"

/******************* Private variables **********************************/

#define MAX_RESEND_COUNT        3
extern void *l1recv_queue_handle;

/* Packet receive parameter */
static uint32_t     package_len = 0;
static uint32_t     package_offset = 0;

/* L1 layer message parameter */
static uint8_t      received_buffer[GLOBAL_RECEIVE_BUFFER_SIZE];
static uint16_t     L1_sequence_id = 0;
static uint8_t      wristband_credits = 10;



extern void *l1send_task_handle;



/**
* @brief  CRC check L1 reveived packet
* @param   crc_value: crc_value
* @param   pData: data pointer to be checked
* @param   length:  data length
* @retval  error code
*/
static bool L1_crc_check(uint16_t crc_value, uint8_t *pData, uint16_t length)
{
    uint16_t crc = btxfcs(0x0000, pData, length);
    if (crc == crc_value)
    {
        return true;
    }
    return false;

}




bool L1_send(uint8_t *buf, uint16_t length)
{
    uint16_t record_len = length;
    /*fill header*/
    buf[L1_HEADER_MAGIC_POS]                = L1_HEADER_MAGIC;                               /* Magic */
    buf[L1_HEADER_PROTOCOL_VERSION_POS]     = L1_HEADER_VERSION;                  /* protocol version */
    buf[L1_PAYLOAD_LENGTH_HIGH_BYTE_POS]    = (length - L1_HEADER_SIZE) >> 8;    /* length high byte */
    buf[L1_PAYLOAD_LENGTH_LOW_BYTE_POS]     = length - L1_HEADER_SIZE;            /* length low byte */
    /*cal crc*/
    uint16_t crc16_ret = btxfcs(0, buf + L1_HEADER_SIZE, length - L1_HEADER_SIZE);
    buf[L1_HEADER_CRC16_HIGH_BYTE_POS]      = crc16_ret >> 8;
    buf[L1_HEADER_CRC16_LOW_BYTE_POS]       = crc16_ret;

    L1_sequence_id ++;

    /* sequence id */
    buf[L1_HEADER_SEQ_ID_HIGH_BYTE_POS]     = L1_sequence_id >> 8;
    buf[L1_HEADER_SEQ_ID_LOW_BYTE_POS]      = L1_sequence_id;

    APP_PRINT_INFO1("sequence id:%d", L1_sequence_id);

    uint8_t conn_id = 0;
    uint8_t mtu_size = 0;
    le_get_gap_param(GAP_PARAM_LE_REMAIN_CREDITS, &wristband_credits);
    le_get_conn_param(GAP_PARAM_CONN_MTU_SIZE, &mtu_size, conn_id);
    uint8_t send_offset = 0;
    while (wristband_credits && length)
    {
        if (length >= mtu_size - 3)
        {
            server_send_data(conn_id, wristband_ser_id, GATT_SRV_BWPS_RX_INDEX,
                             buf + send_offset, mtu_size - 3, GATT_PDU_TYPE_NOTIFICATION);
            send_offset += mtu_size - 3;
            length = length - (mtu_size - 3);
        }
        else
        {
            server_send_data(conn_id, wristband_ser_id, GATT_SRV_BWPS_RX_INDEX,
                             buf + send_offset, length, GATT_PDU_TYPE_NOTIFICATION);
            length = 0;
            send_offset = 0;
        }

    }


    uint8_t event;
    static uint32_t retry_count = 0;
    if (os_msg_recv(l1recv_queue_handle, &event, 3000) == true)
    {
        APP_PRINT_INFO0("receive L1 send ACK success!");
        retry_count = 0;
        return true;
    }
    else
    {
        retry_count++;
        APP_PRINT_INFO1("receive L1 send ACK time out! do retry count = %d", retry_count);
        if (retry_count < MAX_RESEND_COUNT)
        {
            L1_send(buf, record_len);
        }
        else
        {
            APP_PRINT_ERROR1("retry fail!!! retry count= %d", retry_count);
            retry_count = 0;
        }
        return false;
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
void L1_send_ack(uint16_t sequence_id, bool check_success)
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

    uint8_t conn_id = 0;
    server_send_data(conn_id, wristband_ser_id, GATT_SRV_BWPS_RX_INDEX,
                     ack_package_buffer, L1_HEADER_SIZE, GATT_PDU_TYPE_NOTIFICATION);
}



/**
* @brief   deliver L1 received data to upper layer
* @param   pData: pointer to L1 received data
* @param   length:  data length
* @retval  void
*/
void L1_receive_data(uint8_t *data, uint16_t length)
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

        if (L1_crc_check(crc16_value, received_buffer + L1_HEADER_SIZE, L2_frame_length) == true)
        {
            APP_PRINT_INFO0("receive data package & send response");
            /* send response */
            L1_send_ack(seq_id, true);
            /*throw data to upper layer*/
            L2_frame_resolve(received_buffer + L1_HEADER_SIZE, L2_frame_length);
        }
        else
        {
            //send response
            L1_send_ack(seq_id, false);
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



