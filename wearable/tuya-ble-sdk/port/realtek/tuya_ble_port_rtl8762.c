#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "tuya_ble_port.h"
#include "tuya_ble_type.h"
#include "os_sched.h"
#include "os_sync.h"
#include "gap_adv.h"
#include "os_mem.h"
#include "os_msg.h"
#include "os_task.h"
#include "aes_api.h"
#include "md5.h"
#include "hmac.h"
#include "trace.h"
#include "ftl.h"
#include "os_timer.h"
#include "tuya_ble_internal_config.h"
//#include "uart.h"
//#include "peripheral_app.h"
//#include "app_task.h"
//#include "rtl876x_lib_platform.h"
#include "gap_conn_le.h"
#include "tuya_ble_service_rtl8762c.h"
#include "watch_msg.h"

#if defined RTL8762G
#include "rtl_wdg.h"
#elif defined RTL8762D || defined RTL8772F
#include "rtl876x_wdg.h"
#endif

uint8_t m_conn_id = 0;



tuya_ble_status_t tuya_ble_gap_advertising_adv_data_update(uint8_t const *p_ad_data, uint8_t ad_len)
{
    le_adv_set_param(GAP_PARAM_ADV_DATA, ad_len, (uint8_t *)p_ad_data);
    le_adv_update_param();
    return TUYA_BLE_SUCCESS;
}

tuya_ble_status_t tuya_ble_gap_advertising_scan_rsp_data_update(uint8_t const *p_sr_data,
                                                                uint8_t sr_len)
{
    le_adv_set_param(GAP_PARAM_SCAN_RSP_DATA, sr_len, (uint8_t *)p_sr_data);
    le_adv_update_param();
    return TUYA_BLE_SUCCESS;
}

tuya_ble_status_t tuya_ble_gap_disconnect(void)
{
    if (le_disconnect(m_conn_id) == GAP_CAUSE_SUCCESS)
    {
        return TUYA_BLE_SUCCESS;
    }
    else
    {
        return TUYA_BLE_ERR_BUSY;
    }
}


tuya_ble_status_t tuya_ble_gap_addr_get(tuya_ble_gap_addr_t *p_addr)
{
    if (GAP_CAUSE_SUCCESS == gap_get_param(GAP_PARAM_BD_ADDR, p_addr->addr))
    {
        p_addr->addr_type = TUYA_BLE_ADDRESS_TYPE_PUBLIC;
        return TUYA_BLE_SUCCESS;
    }
    else
    {
        return TUYA_BLE_ERR_INTERNAL;
    }
}


tuya_ble_status_t tuya_ble_gap_addr_set(tuya_ble_gap_addr_t *p_addr)
{
    //todo
//    if (UpdateMAC(p_addr->addr))
//    {
//        return TUYA_BLE_SUCCESS;
//    }
//    else
//    {
//        return TUYA_BLE_ERR_INTERNAL;
//    }
    gap_set_bd_addr(p_addr->addr);
    return TUYA_BLE_SUCCESS;
}

static bool ble_value_notify(uint8_t *data, uint8_t len)
{
    if (len > PARA_VALUE_LEN_MAX)
    {
        APP_PRINT_ERROR0("ble notify value len exceed the limit");
        return false;
    }
    else
    {
        return server_send_data(m_conn_id, get_tuya_srv_id(), KNS_KEY_VALUE_INDEX, data, len,
                                GATT_PDU_TYPE_NOTIFICATION);
    }
}

tuya_ble_status_t tuya_ble_gatt_send_data(const uint8_t *p_data, uint16_t len)
{
    uint8_t data_len = len;
    if (data_len > TUYA_BLE_DATA_MTU_MAX)
    {
        data_len = TUYA_BLE_DATA_MTU_MAX;
    }
    if (ble_value_notify((uint8_t *)p_data, len))
    {
        return TUYA_BLE_SUCCESS;
    }
    else
    {
        return TUYA_BLE_ERR_BUSY;
    }
}


tuya_ble_status_t tuya_ble_common_uart_init(void)
{
    return TUYA_BLE_SUCCESS;
}


tuya_ble_status_t tuya_ble_common_uart_send_data(const uint8_t *p_data, uint16_t len)
{
    //uart_senddata_continuous(UART,p_data,len);
    return TUYA_BLE_SUCCESS;
}


tuya_ble_status_t tuya_ble_timer_create(void **p_timer_id, uint32_t timeout_value_ms,
                                        tuya_ble_timer_mode mode, tuya_ble_timer_handler_t timeout_handler)
{
    bool reload = (mode == TUYA_BLE_TIMER_SINGLE_SHOT) ? false : true;

    if (os_timer_create(p_timer_id, "tuya_ble_sdk_timer", 1, timeout_value_ms, reload, timeout_handler))
    {
        return TUYA_BLE_SUCCESS;
    }
    else
    {
        return TUYA_BLE_ERR_COMMON;
    }
}

tuya_ble_status_t tuya_ble_timer_delete(void *timer_id)
{
    if (os_timer_delete(&timer_id))
    {
        return TUYA_BLE_SUCCESS;
    }
    else
    {
        return TUYA_BLE_ERR_COMMON;
    }
}

tuya_ble_status_t tuya_ble_timer_start(void *timer_id)
{

    if (os_timer_start(&timer_id))
    {
        return TUYA_BLE_SUCCESS;
    }
    else
    {
        return TUYA_BLE_ERR_COMMON;
    }
}

tuya_ble_status_t tuya_ble_timer_stop(void *timer_id)
{
    if (os_timer_stop(&timer_id))
    {
        return TUYA_BLE_SUCCESS;
    }
    else
    {
        return TUYA_BLE_ERR_COMMON;
    }
}


void tuya_ble_device_delay_ms(uint32_t ms)
{
    os_delay(ms);
}


tuya_ble_status_t tuya_ble_rand_generator(uint8_t *p_buf, uint8_t len)
{
    uint32_t cnt = len / 4;
    uint8_t  remain = len % 4;
    int32_t temp;
    for (uint32_t i = 0; i < cnt; i++)
    {
        temp = rand();
        memcpy(p_buf, (uint8_t *)&temp, 4);
        p_buf += 4;
    }
    temp = rand();
    memcpy(p_buf, (uint8_t *)&temp, remain);

    return TUYA_BLE_SUCCESS;
}

/*
 *@brief
 *@param
 *
 *@note
 *
 * */
tuya_ble_status_t tuya_ble_device_reset(void)
{
    WDG_SystemReset(RESET_ALL, SW_RESET_APP_START);
    return TUYA_BLE_SUCCESS;
}


tuya_ble_status_t tuya_ble_rtc_get_timestamp(uint32_t *timestamp, int32_t *timezone)
{
    *timestamp = 0;
    *timezone = 0;
    return TUYA_BLE_SUCCESS;
}

tuya_ble_status_t tuya_ble_rtc_set_timestamp(uint32_t timestamp, int32_t timezone)
{
    return TUYA_BLE_SUCCESS;
}


tuya_ble_status_t tuya_ble_nv_init(void)
{
    return TUYA_BLE_SUCCESS;
}

tuya_ble_status_t tuya_ble_nv_erase(uint32_t addr, uint32_t size)
{
    return TUYA_BLE_SUCCESS;
}

tuya_ble_status_t tuya_ble_nv_write(uint32_t addr, const uint8_t *p_data, uint32_t size)
{
    if (ftl_save((uint8_t *)p_data, addr, size) == 0)
    {
        return TUYA_BLE_SUCCESS;
    }
    else
    {
        return TUYA_BLE_ERR_BUSY;
    }
}


tuya_ble_status_t tuya_ble_nv_read(uint32_t addr, uint8_t *p_data, uint32_t size)
{
    if (ftl_load((uint8_t *)p_data, addr, size) == 0)
    {
        return TUYA_BLE_SUCCESS;
    }
    else
    {
        return TUYA_BLE_ERR_BUSY;
    }
}


#if TUYA_BLE_USE_OS
bool tuya_ble_os_task_create(void **pp_handle, const char *p_name, void (*p_routine)(void *),
                             void *p_param, uint16_t stack_size, uint16_t priority)
{
    return os_task_create(pp_handle, p_name, p_routine, p_param, stack_size, priority);
}

bool tuya_ble_os_task_delete(void *p_handle)
{
    return os_task_delete(p_handle);
}

bool tuya_ble_os_task_suspend(void *p_handle)
{
    return os_task_suspend(p_handle);
}

bool tuya_ble_os_task_resume(void *p_handle)
{
    return os_task_resume(p_handle);
}

bool tuya_ble_os_msg_queue_create(void **pp_handle, uint32_t msg_num, uint32_t msg_size)
{
    return os_msg_queue_create(pp_handle, "tuya port", msg_num, msg_size);
}

bool tuya_ble_os_msg_queue_delete(void *p_handle)
{
    return os_msg_queue_delete(p_handle);
}

bool tuya_ble_os_msg_queue_peek(void *p_handle, uint32_t *p_msg_num)
{
    return os_msg_queue_peek(p_handle, p_msg_num);
}

bool tuya_ble_os_msg_queue_send(void *p_handle, void *p_msg, uint32_t wait_ms)
{
    return os_msg_send(p_handle, p_msg, wait_ms);
}

bool tuya_ble_os_msg_queue_recv(void *p_handle, void *p_msg, uint32_t wait_ms)
{
    return os_msg_recv(p_handle, p_msg, wait_ms);
}

#include "app_msg.h"
#include "bt_task.h"
#include "tuya_ble_secure.h"


#if !TUYA_BLE_SELF_BUILT_TASK

static void *tuya_queue_handle = NULL;


void tuya_ble_event_handler_port(void)
{
    tuya_ble_evt_param_t tuya_evt;
    if (os_msg_recv(tuya_queue_handle, &tuya_evt, 100) == true)
    {
        tuya_ble_event_process(&tuya_evt);
    }
}


bool tuya_ble_event_queue_send_port(tuya_ble_evt_param_t *evt, uint32_t wait_ms)
{
    if (tuya_queue_handle == NULL)
    {
        os_msg_queue_create(&tuya_queue_handle, "tuya event port", 0x20, sizeof(tuya_ble_evt_param_t));
    }
    T_IO_MSG msg;
    msg.type = IO_MSG_TYPE_WRISTBNAD;
    msg.subtype = IO_MSG_TUYA_BLE_EVENT;
    if (app_send_msg_to_bt_task(&msg) == false)
    {
        return false;
    }

    if (os_msg_send(tuya_queue_handle, evt, wait_ms))
    {
        return true;
    }
    else
    {
        return false;
    }
}
#endif

#endif


bool tuya_ble_aes128_ecb_encrypt(uint8_t *key, uint8_t *input, uint16_t input_len, uint8_t *output)
{
    uint16_t length;
    if (input_len % 16)
    {
        return false;
    }
    length = input_len;
    while (length > 0)
    {
        aes128_ecb_encrypt(input, key, output);
        input  += 16;
        output += 16;
        length -= 16;
    }

    return true;
}

bool tuya_ble_aes128_ecb_decrypt(uint8_t *key, uint8_t *input, uint16_t input_len, uint8_t *output)
{
    uint16_t length;
    if (input_len % 16)
    {
        return false;
    }
    length = input_len;
    while (length > 0)
    {
        aes128_ecb_decrypt(input, key, output);
        input  += 16;
        output += 16;
        length -= 16;
    }
    return true;
}


bool tuya_ble_aes128_cbc_encrypt(uint8_t *key, uint8_t *iv, uint8_t *input, uint16_t input_len,
                                 uint8_t *output)
{
    uint32_t len;
    if (input_len % 16)
    {
        return false;
    }

    len = input_len / 4;

    //todo
    AES_CBC_CTX ctx;
    ctx.input = input;
    ctx.iv = iv;
    ctx.word_len = len;
    aes128_cbc_encrypt(&ctx, key, output);

    return true;
}

bool tuya_ble_aes128_cbc_decrypt(uint8_t *key, uint8_t *iv, uint8_t *input, uint16_t input_len,
                                 uint8_t *output)
{
    uint32_t len;

    if (input_len % 16)
    {
        return false;
    }

    len = input_len / 4;

    //todo
    AES_CBC_CTX ctx;
    ctx.input = input;
    ctx.iv = iv;
    ctx.word_len = len;
    aes128_cbc_decrypt(&ctx, key, output);

    return true;
}


bool tuya_ble_md5_crypt(uint8_t *input, uint16_t input_len, uint8_t *output)
{
    mbedtls_md5_context md5_ctx;
    mbedtls_md5_init(&md5_ctx);
    mbedtls_md5_starts(&md5_ctx);
    mbedtls_md5_update(&md5_ctx, input, input_len);
    mbedtls_md5_finish(&md5_ctx, output);
    mbedtls_md5_free(&md5_ctx);

    return true;
}


bool tuya_ble_hmac_sha1_crypt(const uint8_t *key, uint32_t key_len, const uint8_t *input,
                              uint32_t input_len, uint8_t *output)
{
    hmac_sha1_crypt(key, key_len, input, input_len, output);
    return true;
}


bool tuya_ble_hmac_sha256_crypt(const uint8_t *key, uint32_t key_len, const uint8_t *input,
                                uint32_t input_len, uint8_t *output)
{
    hmac_sha256_crypt(key, key_len, input, input_len, output);
    return true;
}


#if TUYA_BLE_LOG_ENABLE
uint8_t get_args(const char *format)
{
    uint8_t arg_no = 0;
    if (!format)
    {
        return 0;
    }
    for (; * (format + 1) != '\0'; format++)
    {
        if ((*format == '%') && (*(format + 1) != '%'))
        {
            arg_no++;
        }
    }
    if (arg_no > 8)
    {
        arg_no = 8;
    }
    return arg_no;
}


void log_hexdump(const char *name, uint8_t width, uint8_t *buf, uint16_t size)
{
#if 0
    uint16_t total_line;
    uint16_t line = size / width;
    uint16_t line_remain = size % width;
    if (line_remain > 0)
    {
        total_line = line + 1;
    }
    else
    {
        total_line = line;
    }

    if (line > 3)
    {
        line = 3;
    }
    APP_PRINT_INFO1("HEX:%s\n", TRACE_STRING(name));
    uint8_t i = 0;
    for (i = 0; i < line; i++)
    {
        APP_PRINT_INFO3("[%d-%d]:%b\n", total_line, i + 1, TRACE_BINARY(width, buf + i * width));
    }
    if (line_remain > 0)
    {
        APP_PRINT_INFO3("[%d-%d]:%b\n", total_line, total_line, TRACE_BINARY(line_remain, buf + i * width));
    }
#else
#if __RTTHREAD__
    rt_kprintf("Hex: ");

    for (uint32_t i = 0; i < (size - 1); i++)
    {
        rt_kprintf("0x%x-", buf[i]);
    }

    rt_kprintf("0x%x", buf[size - 1]);
#else
    DBG_DIRECT("Hex: ");

    for (uint32_t i = 0; i < (size - 1); i++)
    {
        DBG_DIRECT("0x%x-", buf[i]);
    }

    DBG_DIRECT("0x%x", buf[size - 1]);
#endif
#endif
}

#endif


#if (TUYA_BLE_USE_PLATFORM_MEMORY_HEAP == 1)

void *tuya_ble_port_malloc(uint32_t size)
{
    return os_mem_alloc(RAM_TYPE_BUFFER_ON, size);
}

void tuya_ble_port_free(void *pv)
{
    os_mem_free(pv);
}

#endif



