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
#include "tuya_ble_sdk_test.h"

#if ( TUYA_BLE_SDK_TEST_ENABLE != 0 )
#include "tuya_ble_port.h"
#include "tuya_ble_api.h"
#include "tuya_ble_log.h"
#include "tuya_ble_main.h"
#include "tuya_ble_mem.h"
#include "tuya_ble_utils.h"
#include "tuya_ble_storage.h"
#include "tuya_ble_unix_time.h"
#include "tuya_ble_data_handler.h"
//#include "tuya_ble_bulk_data_demo.h"

#if ( TUYA_BLE_FEATURE_WEATHER_ENABLE != 0)
#include "tuya_ble_feature_weather.h"
#endif


/*********************************************************************
 * LOCAL CONSTANT
 */

/*********************************************************************
 * LOCAL STRUCT
 */
#pragma pack(1)
typedef struct
{
    uint8_t  type;
    uint8_t  len;
    uint16_t value[16];
} test_pid_t;

typedef struct
{
    uint32_t firmware;
    uint32_t hardware;
} test_version_t;

typedef struct
{
    uint32_t min;
    uint32_t max;
} test_adv_interval_t;

typedef struct
{
    uint32_t min;
    uint32_t max;
} test_conn_interval_t;

typedef struct
{
    int32_t cycle;
    uint16_t len;
    uint8_t  buf[TUYA_BLE_SEND_MAX_DP_DATA_LEN];
} test_dp_report_t;

typedef struct
{
    bool    flag;
    uint8_t mode;
} test_enter_sleep_t;

typedef struct
{
    test_pid_t pid;
    test_version_t version;
    test_adv_interval_t adv_interval;
    test_conn_interval_t conn_interval;
    test_dp_report_t dp_report;
    test_enter_sleep_t enter_sleep;
    uint16_t timezone;
} test_param_t;
#pragma pack()

/*********************************************************************
 * LOCAL VARIABLE
 */
static tuya_ble_timer_t enter_sleep_timer;

static test_param_t test_param;
static uint32_t test_sn = 0;
//static ty_adc_t test_adc;
//static ty_pwm_t test_pwm;

/*********************************************************************
 * VARIABLE
 */

/*********************************************************************
 * LOCAL FUNCTION
 */
static void ty_uart_virtual_enter_sleep_timer_cb(tuya_ble_timer_t timer);


static void tuya_ble_prod_asciitohex(uint8_t *ascbuf, uint8_t len, uint8_t *hexbuf)
{
    uint8_t i = 0, j = 0;

    for (j = 0; j < (len / 2); j++)
    {
        if ((ascbuf[i] >= 0x30) && (ascbuf[i] <= 0x39))
        {
            hexbuf[j] = ((ascbuf[i] - 0x30) << 4);
        }
        else if ((ascbuf[i] >= 65) && (ascbuf[i] <= 70))
        {
            hexbuf[j] = ((ascbuf[i] - 55) << 4);
        }
        else if ((ascbuf[i] >= 97) && (ascbuf[i] <= 102))
        {
            hexbuf[j] = ((ascbuf[i] - 87) << 4);
        }
        i++;
        if ((ascbuf[i] >= 0x30) && (ascbuf[i] <= 0x39))
        {
            hexbuf[j] |= (ascbuf[i] - 0x30);
        }
        else if ((ascbuf[i] >= 65) && (ascbuf[i] <= 70))
        {
            hexbuf[j] |= (ascbuf[i] - 55);
        }
        else if ((ascbuf[i] >= 97) && (ascbuf[i] <= 102))
        {
            hexbuf[j] |= (ascbuf[i] - 87);
        }
        i++;

    }

}

/*********************************************************
FN:
*/
void tuya_ble_sdk_test_init(void)
{
//    /* adc */
//    test_adc.channal = 0;
//    ty_hal_adc_init();
//
//    /* spi */
//    ty_hal_spi_init();
//
//    /* pwm */
//    test_pwm.pin = 11;
//    test_pwm.polarity = 0;
//    test_pwm.freq = 2700;
//    test_pwm.duty = 50;
//    ty_hal_pwm_init(&test_pwm);
//
//    /* i2c*/
//    ty_pdl_lighting_drv_init();

    test_param.pid.type = TUYA_BLE_PRODUCT_ID_TYPE_PID;
    test_param.pid.len = 8;
    memcpy(test_param.pid.value, APP_PRODUCT_ID, test_param.pid.len);

    test_param.version.firmware = TY_APP_VER_NUM;
    test_param.version.hardware = TY_HARD_VER_NUM;

    test_param.adv_interval.min = TUYA_BLE_APP_DEFAULT_ADV_INTERVAL;
    test_param.adv_interval.max = TUYA_BLE_APP_DEFAULT_ADV_INTERVAL;
    test_param.conn_interval.min = TUYA_BLE_APP_CONN_INTERVAL_MIN;
    test_param.conn_interval.max = TUYA_BLE_APP_CONN_INTERVAL_MAX;

    test_param.enter_sleep.flag = false;
    test_param.enter_sleep.mode = 0;

    tuya_ble_timer_create(&enter_sleep_timer, 200, TUYA_BLE_TIMER_SINGLE_SHOT,
                          ty_uart_virtual_enter_sleep_timer_cb);
}

/*********************************************************
FN:
*/
void tuya_ble_app_sdk_test_process(uint8_t channel, uint8_t *p_in_data, uint16_t in_len)
{
    (void)(channel);
    TUYA_APP_LOG_HEXDUMP_INFO("tuya ble sdk api test cmd", p_in_data, in_len);

    tuya_ble_sdk_test_cmd_t *cmd = (tuya_ble_sdk_test_cmd_t *)p_in_data;
    cmd->len = BSWAP_16(cmd->len);
    cmd->sub_id = BSWAP_16(cmd->sub_id);

    if ((cmd->type != 3) || (cmd->len < 3))
    {
        return;
    }

    uint16_t data_len = cmd->len - 3;
    uint8_t  rsp_data[256] = {0};
    uint32_t rsp_len = 0;

    switch (cmd->sub_id)
    {
    case TY_UARTV_CMD_SHAKE_HAND:
        break;

    case TY_UARTV_CMD_SET_PID:
        if (tuya_ble_device_update_product_id(cmd->value[0], cmd->value[1],
                                              &cmd->value[2]) == TUYA_BLE_SUCCESS)
        {
            test_param.pid.type = cmd->value[0];
            test_param.pid.len = cmd->value[1];
            memcpy(test_param.pid.value, &cmd->value[2], test_param.pid.len);

            rsp_data[rsp_len++] = 0;
        }
        else
        {
            rsp_data[rsp_len++] = 1;
        }
        break;

    case TY_UARTV_CMD_GET_PID:
        rsp_data[rsp_len++] = test_param.pid.type;
        rsp_data[rsp_len++] = test_param.pid.len;

        memcpy(&rsp_data[rsp_len], test_param.pid.value, test_param.pid.len);
        rsp_len += test_param.pid.len;
        break;

    case TY_UARTV_CMD_SET_AUTH_INFO:
        {
            tuya_ble_gap_addr_t addr;
            addr.addr_type = TUYA_BLE_ADDRESS_TYPE_PUBLIC;

            tuya_ble_prod_asciitohex(&cmd->value[0], 12, addr.addr);

            if (tuya_ble_storage_write_auth_key_device_id_mac(&cmd->value[12 + DEVICE_ID_LEN], AUTH_KEY_LEN, \
                                                              &cmd->value[12], DEVICE_ID_LEN, \
                                                              addr.addr, 6, \
                                                              &cmd->value[0], 12, NULL, 0) == TUYA_BLE_SUCCESS)
            {
                tuya_ble_gap_addr_set(&addr);
                rsp_data[rsp_len++] = 0;
            }
            else
            {
                rsp_data[rsp_len++] = 1;
            }
        }
        break;

    case TY_UARTV_CMD_GET_AUTH_INFO:
        memcpy(&rsp_data[rsp_len], tuya_ble_current_para.auth_settings.mac_string, MAC_STRING_LEN);
        rsp_len += MAC_STRING_LEN;
        memcpy(&rsp_data[rsp_len], tuya_ble_current_para.auth_settings.device_id, DEVICE_ID_LEN);
        rsp_len += DEVICE_ID_LEN;
        memcpy(&rsp_data[rsp_len], tuya_ble_current_para.auth_settings.auth_key, AUTH_KEY_LEN);
        rsp_len += AUTH_KEY_LEN;
        break;

    case TY_UARTV_CMD_SET_VERSION:
        test_param.version.firmware = (cmd->value[0] << 24) + (cmd->value[1] << 16) +
                                      (cmd->value[2] << 8) + cmd->value[3];
        test_param.version.hardware = (cmd->value[4] << 24) + (cmd->value[5] << 16) +
                                      (cmd->value[6] << 8) + cmd->value[7];
        tuya_ble_set_device_version(test_param.version.firmware, test_param.version.hardware);

        rsp_data[rsp_len++] = 0;
        break;

    case TY_UARTV_CMD_GET_VERSION:
        rsp_data[rsp_len++] = (uint8_t)(test_param.version.firmware >> 24);
        rsp_data[rsp_len++] = (uint8_t)(test_param.version.firmware >> 16);
        rsp_data[rsp_len++] = (uint8_t)(test_param.version.firmware >> 8);
        rsp_data[rsp_len++] = (uint8_t)(test_param.version.firmware);
        rsp_data[rsp_len++] = (uint8_t)(test_param.version.hardware >> 24);
        rsp_data[rsp_len++] = (uint8_t)(test_param.version.hardware >> 16);
        rsp_data[rsp_len++] = (uint8_t)(test_param.version.hardware >> 8);
        rsp_data[rsp_len++] = (uint8_t)(test_param.version.hardware);
        break;

#if 0
    case TY_UARTV_CMD_SET_ADV_INTERVAL:
        {
            uint16_t adv_interval_min, adv_interval_max;
            adv_interval_min = (cmd->value[0] << 8) + cmd->value[1];
            adv_interval_max = (cmd->value[2] << 8) + cmd->value[3];

            if (ty_bal_set_adv_interval(adv_interval_max) != TUYA_BLE_SUCCESS)
            {
                rsp_data[rsp_len++] = 1;
            }
            else
            {
                test_param.adv_interval.min = adv_interval_min;
                test_param.adv_interval.max = adv_interval_max;

                rsp_data[rsp_len++] = 0;
            }
        }
        break;

    case TY_UARTV_CMD_GET_ADV_INTERVAL:
        rsp_data[rsp_len++] = (uint8_t)(test_param.adv_interval.min >> 8);
        rsp_data[rsp_len++] = (uint8_t)(test_param.adv_interval.min);
        rsp_data[rsp_len++] = (uint8_t)(test_param.adv_interval.max >> 8);
        rsp_data[rsp_len++] = (uint8_t)(test_param.adv_interval.max);
        break;

    case TY_UARTV_CMD_SET_CONN_INTERVAL:
        {
            uint16_t conn_interval_min, conn_interval_max;
            conn_interval_min = (cmd->value[0] << 8) + cmd->value[1];
            conn_interval_max = (cmd->value[2] << 8) + cmd->value[3];

            if (ty_bal_conn_param_update(conn_interval_min, conn_interval_max, 0, 5000) != TUYA_BLE_SUCCESS)
            {
                rsp_data[rsp_len++] = 1;
            }
            else
            {
                test_param.conn_interval.min = conn_interval_min;
                test_param.conn_interval.max = conn_interval_max;

                rsp_data[rsp_len++] = 0;
            }
        }
        break;

    case TY_UARTV_CMD_GET_CONN_INTERVAL:
        rsp_data[rsp_len++] = (uint8_t)(test_param.conn_interval.min >> 8);
        rsp_data[rsp_len++] = (uint8_t)(test_param.conn_interval.min);
        rsp_data[rsp_len++] = (uint8_t)(test_param.conn_interval.max >> 8);
        rsp_data[rsp_len++] = (uint8_t)(test_param.conn_interval.max);
        break;
#endif

    case TY_UARTV_CMD_PASSTHROUGH_REPORT:
        rsp_data[rsp_len++] = tuya_ble_data_passthrough(cmd->value, data_len);
        break;

    case TY_UARTV_CMD_TIME_SYNC:
        rsp_data[rsp_len++] = tuya_ble_time_req(cmd->value[0]);
        break;

    case TY_UARTV_CMD_DP_REPORT:
        if (cmd->value[0] == 0)
        {
            rsp_data[rsp_len++] = tuya_ble_dp_data_send(test_sn++, cmd->value[1], cmd->value[2], cmd->value[3],
                                                        &cmd->value[4], data_len - 4);
        }
        else
        {
            if (cmd->value[4] == 0)
            {
                rsp_data[rsp_len++] = tuya_ble_dp_data_with_time_send(test_sn++, cmd->value[2], cmd->value[4],
                                                                      &cmd->value[5], &cmd->value[18], data_len - 18);
            }
            else
            {
                rsp_data[rsp_len++] = tuya_ble_dp_data_with_time_send(test_sn++, cmd->value[2], cmd->value[4],
                                                                      &cmd->value[5], &cmd->value[9], data_len - 9);
            }
        }
        break;

    case TY_UARTV_CMD_DP_REPORT_PERIOD:
        {
            uint32_t cycle = (cmd->value[0] << 24) + (cmd->value[1] << 16) + (cmd->value[2] << 8) +
                             cmd->value[3];

            test_param.dp_report.cycle = cycle;
            test_param.dp_report.len = data_len - 4;
            memcpy(test_param.dp_report.buf, &cmd->value[4], test_param.dp_report.len);

            uint8_t *buf = test_param.dp_report.buf;
            uint32_t len = test_param.dp_report.len;
            if (buf[0] == 0)
            {
                tuya_ble_dp_data_send(test_sn++, buf[1], buf[2], buf[3], &buf[4], len - 4);
            }
            else
            {
                if (buf[4] == 0)
                {
                    tuya_ble_dp_data_with_time_send(test_sn++, buf[2], buf[4], &buf[5], &buf[18], len - 18);
                }
                else
                {
                    tuya_ble_dp_data_with_time_send(test_sn++, buf[2], buf[4], &buf[5], &buf[9], len - 9);
                }
            }

            rsp_data[rsp_len++] = 0;
        }
        break;

    case TY_UARTV_CMD_SET_BULKDATA:
        {
            uint32_t cycle = (cmd->value[0] << 24) + (cmd->value[1] << 16) + (cmd->value[2] << 8) +
                             cmd->value[3];
            uint32_t timestep = (cmd->value[4] << 24) + (cmd->value[5] << 16) + (cmd->value[6] << 8) +
                                cmd->value[7];

            //tuya_ble_bulk_data_generation(timestep, &cmd->value[8], data_len-8);
            rsp_data[rsp_len++] = 0;
        }
        break;

    case TY_UARTV_CMD_SET_CONN_REQUEST:
        rsp_data[rsp_len++] = tuya_ble_adv_data_connecting_request_set(cmd->value[0]);
        break;

    case TY_UARTV_CMD_SET_DEVICE_RESET:
        if (cmd->value[0] == 0)
        {
            rsp_data[rsp_len++] = tuya_ble_device_unbind();
        }
        else
        {
            rsp_data[rsp_len++] = tuya_ble_device_factory_reset();
        }
        break;

    case TY_UARTV_CMD_SET_SLEEP_MODE:
        test_param.enter_sleep.mode = cmd->value[0];

        tuya_ble_timer_start(enter_sleep_timer);

        rsp_data[rsp_len++] = 0;
        break;

    case TY_UARTV_CMD_GET_DEVICE_STATE:
        rsp_data[rsp_len++] = tuya_ble_connect_status_get();
        break;

    case TY_UARTV_CMD_GET_TIME:
        {
            uint32_t timestamp = 0;
            tuya_ble_time_struct_data_t data;

            tuya_ble_rtc_get_timestamp(&timestamp, NULL);
            tuya_ble_utc_sec_2_mytime(timestamp, &data, false);

            rsp_data[rsp_len++] = data.nYear - 2000;
            rsp_data[rsp_len++] = data.nMonth;
            rsp_data[rsp_len++] = data.nDay;
            rsp_data[rsp_len++] = data.nHour;
            rsp_data[rsp_len++] = data.nMin;
            rsp_data[rsp_len++] = data.nSec;
            rsp_data[rsp_len++] = data.DayIndex;
            rsp_data[rsp_len++] = (uint8_t)(test_param.timezone >> 8);
            rsp_data[rsp_len++] = (uint8_t)(test_param.timezone);
            TUYA_APP_LOG_INFO("[%d-%d-%d %d:%d:%d weak=%d  timezone=%d]\r\n", rsp_data[0], rsp_data[1],
                              rsp_data[2], rsp_data[3], rsp_data[4], rsp_data[5], rsp_data[6], test_param.timezone);
        }
        break;

#if ( TUYA_BLE_FEATURE_WEATHER_ENABLE != 0 )
    case TY_UARTV_CMD_GET_WEATHER:
        {
            uint32_t combine_type = (cmd->value[1] << 24) + (cmd->value[2] << 16) +
                                    (cmd->value[3] << 8) + cmd->value[4];
            rsp_data[rsp_len++] = 0;
            rsp_data[rsp_len++] = tuya_ble_feature_weather_data_request_with_location(cmd->value[0],
                                                                                      combine_type, cmd->value[5]);
        }
        break;
#endif

    case TY_UARTV_CMD_GET_RAND:
        {
            uint8_t rand_buf[32] = {0};
            uint8_t len = cmd->value[0];
            tuya_ble_rand_generator(rand_buf, len);

            memcpy(rsp_data, rand_buf, len);
            rsp_len += len;
        }
        break;

    case TY_UARTV_CMD_DP_WRITE:
        break;

    case TY_UARTV_CMD_DP_RSP:
        break;

    case TY_UARTV_CMD_UNBIND_MODE:
        break;

#if 0
    case TY_UARTV_CMD_PWM:
        {
            uint32_t freq = (cmd->value[0] << 24) + (cmd->value[1] << 16) + (cmd->value[2] << 8) +
                            cmd->value[3];
            uint32_t duty = cmd->value[4];

            test_pwm.freq = freq;
            test_pwm.duty = duty;
            ty_hal_pwm_start(&test_pwm);

            rsp_data[rsp_len++] = 0;
        }
        break;

    case TY_UARTV_CMD_ADC:
        ty_hal_get_battery_vol(&test_adc);

        rsp_data[rsp_len++] = (uint8_t)(test_adc.value >> 24);
        rsp_data[rsp_len++] = (uint8_t)(test_adc.value >> 16);
        rsp_data[rsp_len++] = (uint8_t)(test_adc.value >> 8);
        rsp_data[rsp_len++] = (uint8_t)(test_adc.value);
        break;

    case TY_UARTV_CMD_SPI:
        ty_hal_spi_echo_example(&cmd->value[0], rsp_data, data_len);

        rsp_len += data_len;
        break;

    case TY_UARTV_CMD_IIC:
        if (cmd->value[0] == 0)
        {
            ty_pdl_backlight_turn_on();
        }
        else
        {
            ty_pdl_backlight_turn_off();
        }
        break;
#endif

    default:
        break;
    }

    tuya_ble_sdk_test_send(cmd->sub_id, rsp_data, rsp_len);
}

/*********************************************************
FN:
*/
uint32_t tuya_ble_sdk_test_send(uint16_t sub_cmd, uint8_t *data, uint16_t size)
{
    uint8_t *rsp = NULL;
    uint16_t rsp_len;
    uint16_t data_len = size + 3;

    rsp = (uint8_t *)tuya_ble_malloc(16 + size);
    if (rsp == NULL)
    {
        return 0;
    }

    rsp[0] = 0x66;
    rsp[1] = 0xAA;
    rsp[2] = 0x00;
    rsp[3] = 0xF2;
    rsp[4] = (uint8_t)(data_len >> 8);
    rsp[5] = (uint8_t)(data_len);
    rsp[6] = 0x03;
    rsp[7] = (uint8_t)(sub_cmd >> 8);
    rsp[8] = (uint8_t)(sub_cmd);
    memcpy(&rsp[9], data, size);
    rsp_len = data_len + 6;

    rsp[rsp_len] = tuya_ble_check_sum(rsp, rsp_len);
    rsp_len += 1;

    tuya_ble_production_test_asynchronous_response(0, rsp, rsp_len);

    tuya_ble_free(rsp);
    return 1;
}

/*********************************************************
FN:
*/
static void ty_uart_virtual_enter_sleep_timer_cb(tuya_ble_timer_t timer)
{

}

/*********************************************************
FN:
*/
void tuya_ble_sdk_test_wake_up_handler(void)
{

}

/*********************************************************
FN:
*/
void tuya_ble_sdk_test_dp_report_handler(void)
{
    test_param.dp_report.cycle--;

    if (test_param.dp_report.cycle > 0)
    {
        uint8_t *buf = test_param.dp_report.buf;
        uint32_t len = test_param.dp_report.len;

        if (buf[0] == 0)
        {
            tuya_ble_dp_data_send(test_sn++, buf[1], buf[2], buf[3], &buf[4], len - 4);
        }
        else
        {
            if (buf[4] == 0)
            {
                tuya_ble_dp_data_with_time_send(test_sn++, buf[2], buf[4], &buf[5], &buf[18], len - 18);
            }
            else
            {
                tuya_ble_dp_data_with_time_send(test_sn++, buf[2], buf[4], &buf[5], &buf[9], len - 9);
            }
        }
    }
}

/*********************************************************
FN:
*/
void tuya_ble_sdk_test_get_time_rsp(tuya_ble_time_noraml_data_t *data)
{
    uint8_t rsp_data[10] = {0};
    uint8_t rsp_len = 9;

    test_param.timezone = data->time_zone;

    rsp_data[0] = (data->nYear < 2000) ? (data->nYear) : (data->nYear - 2000);
    rsp_data[1] = data->nMonth;
    rsp_data[2] = data->nDay;
    rsp_data[3] = data->nHour;
    rsp_data[4] = data->nMin;
    rsp_data[5] = data->nSec;
    rsp_data[6] = data->DayIndex;
    rsp_data[7] = (uint8_t)(data->time_zone >> 8);
    rsp_data[8] = (uint8_t)(data->time_zone);
    TUYA_APP_LOG_INFO("[%d-%d-%d %d:%d:%d weak=%d timezone=%d]\r\n", rsp_data[0], rsp_data[1],
                      rsp_data[2], rsp_data[3], rsp_data[4], rsp_data[5], rsp_data[6], data->time_zone);

    tuya_ble_sdk_test_send(TY_UARTV_CMD_GET_TIME, rsp_data, rsp_len);
}

/*********************************************************
FN:
*/
void tuya_ble_sdk_test_unbind_mode_rsp(uint8_t mode)
{
    uint8_t tmp_mode = mode;
    tuya_ble_sdk_test_send(TY_UARTV_CMD_UNBIND_MODE, &tmp_mode, sizeof(uint8_t));
}

#endif

