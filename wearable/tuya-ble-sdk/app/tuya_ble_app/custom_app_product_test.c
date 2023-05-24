/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include "custom_app_product_test.h"
#include "tuya_ble_main.h"
#include "tuya_ble_mem.h"
#include "tuya_ble_log.h"
#include "tuya_ble_api.h"
#include "tuya_ble_utils.h"
#include "tuya_ble_internal_config.h"

static void tuya_ble_product_test_response(uint8_t channel, uint16_t sub_cmd, uint8_t *data,
                                           uint16_t size);

void tuya_ble_custom_app_production_test_process(uint8_t channel, uint8_t *p_in_data,
                                                 uint16_t in_len)
{
    uint16_t cmd = 0;
    uint8_t tmp_true_buf[] = "{\"ret\":true}";

    uint8_t *data_buffer = NULL;
    uint16_t data_len = ((p_in_data[4] << 8) + p_in_data[5]);

    if ((p_in_data[6] != 3) || (data_len < 3))
    {
        return;
    }

    cmd = (p_in_data[7] << 8) + p_in_data[8];
    data_len -= 3;
    if (data_len > 0)
    {
        data_buffer = p_in_data + 9;
    }

    // TODO .. YOUR JOBS.
    switch (cmd)
    {
    case TY_FTM_PCBA_CMD_ENTER:
        tuya_ble_product_test_response(channel, cmd, tmp_true_buf, strlen((void *)tmp_true_buf));
        break;

    case TY_FTM_PCBA_CMD_EXIT:
        tuya_ble_product_test_response(channel, cmd, tmp_true_buf, strlen((void *)tmp_true_buf));
        break;

    default:
        break;
    }
}


static void tuya_ble_product_test_response(uint8_t channel, uint16_t sub_cmd, uint8_t *data,
                                           uint16_t size)
{
    uint8_t *rsp;
    uint16_t rsp_len;
    uint16_t data_len = size + 3;

    rsp = (uint8_t *)tuya_ble_malloc(16 + size);
    if (rsp == NULL)
    {
        return;
    }

    rsp[0] = TY_FTM_PROTOCOL_FIRST_HEAD;
    rsp[1] = TY_FTM_PROTOCOL_SECOND_HEAD;
    rsp[2] = TY_FTM_PROTOCOL_VERSION;
    rsp[3] = TY_FTM_PCBA_CMD_SET;
    rsp[4] = (uint8_t)(data_len >> 8);
    rsp[5] = (uint8_t)(data_len);
    rsp[6] = PROTOCOL_TYPE_BLE;
    rsp[7] = (uint8_t)(sub_cmd >> 8);
    rsp[8] = (uint8_t)(sub_cmd);
    memcpy(&rsp[9], data, size);
    rsp_len = data_len + 6;

    rsp[rsp_len] = tuya_ble_check_sum(rsp, rsp_len);
    rsp_len += 1;
    tuya_ble_production_test_asynchronous_response(channel, rsp, rsp_len);

    tuya_ble_free(rsp);
}

/* [] END OF FILE */
