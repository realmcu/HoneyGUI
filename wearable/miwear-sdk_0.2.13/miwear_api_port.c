#include <stdio.h>


#include "miwear_api.h"
#include "miwear_type.h"
#include "gap_adv.h"
#include "os_timer.h"
#include "md5.h"
//#include <symbol_include.h>
#include "aes_api.h"
#include "gap_conn_le.h"
#include "profile_server.h"
#include "sha1.h"
#include "sha256.h"
#include "hmac.h"
#include "ccm.h"
#include "ecc.h"
#include "stdarg.h"
//#include "trace.h"
#include "rtthread.h"
/* clang-format off */

miwear_status_t miwear_att_mtu_get(uint16_t conn_handle, uint16_t *mtu)
{
    rt_kprintf("[MIWEAR_PORT] line is %d \n", __LINE__);
    uint8_t mtu_size = 0;
    le_get_conn_param(GAP_PARAM_CONN_MTU_SIZE, &mtu_size, 0); //connectid is default
    if (mtu_size >= 23)
    {
        rt_kprintf("[MIWEAR_PORT] mtu size is %d \n", mtu_size);
        *mtu = mtu_size;
        //rt("[periph_handle_gap_msg] update mtu size to %d", mtu_size);
    }

    return MIWEAR_SUCCESS;

}

miwear_status_t miwear_gap_address_get(miwear_addr_t mac)
{
    rt_kprintf("[MIWEAR_PORT] miwear_gap_address_get");
    if (GAP_CAUSE_SUCCESS == gap_get_param(GAP_PARAM_BD_ADDR, mac))
    {
        return MIWEAR_SUCCESS;
    }
    else
    {
        return MIWEAR_ERR_INTERNAL;
    }
}

miwear_status_t miwear_gap_update_conn_params(uint16_t conn_handle,
                                              miwear_gap_conn_param_t conn_params)
{
    rt_kprintf("[MIWEAR_PORT] miwear_gap_update_conn_params line is %d\n", __LINE__);

    le_update_conn_param(0, conn_params.min_conn_interval, conn_params.max_conn_interval,
                         conn_params.slave_latency,
                         conn_params.conn_sup_timeout, conn_params.min_conn_interval * 2 - 2,
                         conn_params.slave_latency * 2 - 2);
    rt_kprintf("[MIWEAR_PORT]miwear_gap_update_conn_params min_conn_interval =%d,max_conn_interval = %d,slave_latency = %d,conn_sup_timeout =%d\n\
                                                        ", conn_params.min_conn_interval,
               conn_params.max_conn_interval,
               conn_params.slave_latency, conn_params.conn_sup_timeout);
    return MIWEAR_SUCCESS;


}

miwear_status_t miwear_gatts_notify(uint16_t conn_handle, uint16_t srv_handle,
                                    uint16_t char_value_handle, uint16_t offset,
                                    uint8_t *p_value, uint16_t len)
{
    rt_kprintf("[MIWEAR_PORT] line is %d \n", __LINE__);
    rt_kprintf("[MIWEAR_PORT] miwear_gatts_notify srv_handle = %d; char_value_handle = %d; offset = %d; len = %d \n",
               srv_handle, char_value_handle, offset, len);
    T_SERVER_ID service_id = 0;

    for (uint8_t i = 0; i < 5; i++)
    {
        if (server_get_start_handle(i) == srv_handle)
        {
            service_id = i;
            break;
        }
    }

    if (server_send_data(conn_handle, service_id, char_value_handle - srv_handle, p_value + offset, len,
                         GATT_PDU_TYPE_NOTIFICATION))
    {
        rt_kprintf("[MIWEAR_PORT] miwear_gatts_notify service_id = %d \n", service_id);
        return MIWEAR_SUCCESS;
    }
    else
    {
        return MIWEAR_ERR_INTERNAL;
    }
}

miwear_status_t miwear_timer_create(void **p_timer_id,
                                    miwear_timer_handler timeout_handler,
                                    miwear_timer_mode mode)
{
    rt_kprintf("[MIWEAR_PORT] line is %d \n", __LINE__);

    uint32_t timeout_value_ms = 1000;
    bool reload = (mode == MIWEAR_TIMER_SINGLE_SHOT) ? false : true;

    if (os_timer_create(p_timer_id, "tuya_ble_sdk_timer", 1, timeout_value_ms, reload, timeout_handler))
    {
        return MIWEAR_SUCCESS;
    }
    else
    {
        return MIWEAR_ERR_UNKNOWN;
    }

}

miwear_status_t miwear_timer_delete(void *timer_id)
{
    if (timer_id == NULL)
    {
        rt_kprintf("%s, invalid timer id addr\n", __func__);
        return MIWEAR_ERR_INVALID_ADDR;
    }
    rt_kprintf("[MIWEAR_PORT] line is %d \n", __LINE__);
    if (os_timer_delete(&timer_id))
    {
        return MIWEAR_SUCCESS;
    }
    else
    {
        return MIWEAR_ERR_UNKNOWN;
    }
}

miwear_status_t miwear_timer_start(void *timer_id, uint32_t timeout_value,
                                   void *p_context)
{
    if (timer_id == NULL)
    {
        rt_kprintf("%s, invalid timer id addr\n", __func__);
        return MIWEAR_ERR_INVALID_ADDR;
    }
    rt_kprintf("[MIWEAR_PORT] line is %d \n", __LINE__);
    if (os_timer_restart(&timer_id, timeout_value))
    {
        return MIWEAR_SUCCESS;
    }
    else
    {
        return MIWEAR_ERR_UNKNOWN;
    }

}

miwear_status_t miwear_timer_stop(void *timer_id)
{
    if (timer_id == NULL)
    {
        rt_kprintf("%s, invalid timer id addr\n", __func__);
        return MIWEAR_ERR_INVALID_ADDR;
    }
    rt_kprintf("%s, miwear_timer_stop\n", __func__);
    if (os_timer_stop(&timer_id))
    {
        return MIWEAR_SUCCESS;
    }
    else
    {
        return MIWEAR_ERR_UNKNOWN;
    }
}

miwear_status_t miwear_record_delete(const char *key)
{
    rt_kprintf("[MIWEAR_PORT] line is %d \n", __LINE__);
    extern bool flashDB_yunmai_delete_data(const char *key);
    if (flashDB_yunmai_delete_data(key))
    {
        return MIWEAR_SUCCESS;
    }
    else
    {
        return MIWEAR_ERR_BUSY;
    }
}

miwear_status_t miwear_record_read(const char *key, uint8_t *value,
                                   uint16_t *p_len)
{
    rt_kprintf("[MIWEAR_PORT] line is %d \n", __LINE__);
    bool flashDB_yunmai_get_data(const char *key, uint8_t *value,
                                 uint16_t *p_len);
    if (flashDB_yunmai_get_data(key, value, p_len))
    {

        rt_kprintf("[MIWEAR_PORT] flashDB_yunmai_get_data success is %d \n", __LINE__);
        return MIWEAR_SUCCESS;
    }
    else
    {
        rt_kprintf("[MIWEAR_PORT] flashDB_yunmai_get_data fail is %d \n", __LINE__);
        return MIWEAR_ERR_BUSY;
    }
}

miwear_status_t miwear_record_write(const char *key, const uint8_t *value,
                                    uint16_t len)
{
    rt_kprintf("[MIWEAR_PORT] line is %d \n", __LINE__);
    extern bool flashDB_yunmai_set_data(const char *key, const uint8_t *value,
                                        uint16_t p_len);
    if (flashDB_yunmai_set_data(key, value, len))
    {
        rt_kprintf("[MIWEAR_PORT] flashDB_yunmai_set_data success is %d \n", __LINE__);
        return MIWEAR_SUCCESS;
    }
    else
    {
        rt_kprintf("[MIWEAR_PORT] flashDB_yunmai_set_data fail is %d \n", __LINE__);
        return MIWEAR_ERR_BUSY;
    }

}

miwear_status_t miwear_rand_num_generator(uint8_t *p_buf, uint16_t len)
{
    rt_kprintf("[MIWEAR_PORT] line is %d \n", __LINE__);
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
    return MIWEAR_SUCCESS;
}
//static int ecc_rng(uint8_t *dest, unsigned size)
//{
//    rt_kprintf("[MIWEAR_PORT] line is %d \n", __LINE__);
//    miwear_rand_num_generator(dest, (uint32_t)size);
//    return 1;
//}

static void micro_ecc_init(void *p_ctx)
{
    rt_kprintf("[MIWEAR_PORT] line is %d \n", __LINE__);
}
miwear_status_t miwear_crypto_init(void)
{
    rt_kprintf("[MIWEAR_PORT] line is %d \n", __LINE__);
    micro_ecc_init(NULL);
    return MIWEAR_SUCCESS;
}

miwear_status_t miwear_aes128_encrypt(const uint8_t *key,
                                      const uint8_t *plaintext, uint16_t plen,
                                      uint8_t *ciphertext)
{
    rt_kprintf("[MIWEAR_PORT] line is %d \n", __LINE__);
    uint16_t length;
    if (plen % 16)
    {
        return MIWEAR_ERR_UNKNOWN;
    }
    length = plen;
    while (length > 0)
    {
        aes128_ecb_encrypt((uint8_t *)plaintext, key, ciphertext);
        plaintext  += 16;
        ciphertext += 16;
        length -= 16;
    }

    return MIWEAR_SUCCESS;
}


static int swap_endian(const uint8_t *in, uint8_t *out, uint16_t size)
{
    rt_kprintf("[MIWEAR_PORT] line is %d \n", __LINE__);
    if (out < in + size && in < out + size)
    {
        return -1;
    }

    for (int i = 0; i < size; i++)
    {
        out[i] = in[size - 1 - i];
    }

    return 0;
}
miwear_status_t miwear_crypto_ecdh_keypair_gen(miwear_ecc_curve_t curve,
                                               miwear_ecc256_sk_t sk,
                                               miwear_ecc256_pk_t pk)
{
    rt_kprintf("[MIWEAR_PORT] line is %d \n", __LINE__);
    miwear_ecc256_pk_t le_pk;
    miwear_ecc256_sk_t le_sk;
//    const struct uECC_Curve_t* p_curve;

//    p_curve = uECC_secp256r1();
    uint64_t random_data = rand();
    int ret = ecc_make_key(le_pk, le_sk, &random_data);
    if (!ret)
    {
        return MIWEAR_ERR_INTERNAL;
    }

    swap_endian(&le_sk[0], &sk[0], 32);
    swap_endian(&le_pk[0], &pk[0], 32);
    swap_endian(&le_pk[32], &pk[32], 32);
    return MIWEAR_SUCCESS;
}



miwear_status_t miwear_crypto_ecdh_shared_secret_compute(
    miwear_ecc_curve_t curve, const miwear_ecc256_sk_t sk,
    const miwear_ecc256_pk_t pk, miwear_ecc256_ss_t ss)
{
    rt_kprintf("[MIWEAR_PORT] line is %d \n", __LINE__);
    miwear_ecc256_sk_t le_sk;
    miwear_ecc256_pk_t le_pk;
    miwear_ecc256_ss_t le_ss;

//    p_curve = uECC_secp256r1();
    swap_endian(&sk[0], &le_sk[0], 32);
    swap_endian(&pk[0], &le_pk[0], 32);
    swap_endian(&pk[32], &le_pk[32], 32);
    if (!ecc_validate_key(le_pk))
    {
        return MIWEAR_ERR_INVALID_PARAM;
    }

    int ret = ecdh_shared_secret(le_pk, le_sk, le_ss);
    if (!ret)
    {
        return MIWEAR_ERR_INTERNAL;
    }

    swap_endian(&le_ss[0], &ss[0], 32);

    return MIWEAR_SUCCESS;
}

miwear_status_t miwear_crypto_hkdf_sha256(const uint8_t *key, uint16_t key_len,
                                          const uint8_t *salt,
                                          uint16_t salt_len,
                                          const uint8_t *info,
                                          uint16_t info_len, uint8_t *out,
                                          uint16_t out_len)
{
//      sha256_hkdf(key,key_len,salt,salt_len,info,info_len,out,out_len);
//
//    return MIWEAR_SUCCESS;
    int ret = 0;
    rt_kprintf("[MIWEAR_PORT] line is %d \n", __LINE__);
    ret = sha256_hkdf(key, key_len, salt, salt_len, info, info_len, out,
                      out_len);
    if (ret)
    {
        return MIWEAR_ERR_INTERNAL;
    }
    return MIWEAR_SUCCESS;
}

miwear_status_t miwear_crypto_hmac_sha256(const uint8_t *k, uint16_t key_len,
                                          const uint8_t *p_in, uint16_t in_len,
                                          uint8_t *p_out)
{
    rt_kprintf("[MIWEAR_PORT] line is %d \n", __LINE__);
    hmac_sha256_crypt(k, key_len, p_in, in_len, p_out);
    return MIWEAR_SUCCESS;
}

miwear_status_t miwear_hmac_sha256_with_psk(const uint8_t *plain_data, uint16_t plen,
                                            uint8_t *hash_data)
{
    rt_kprintf("[MIWEAR_PORT] line is %d \n", __LINE__);
    uint8_t psk[16];
    miwear_addr_t mac = {0};
    miwear_gap_address_get(mac);

    miwear_crypto_hmac_sha256(psk, sizeof(psk),   plain_data, plen, hash_data);
    return MIWEAR_SUCCESS;
}
#include "sha256.h"
miwear_status_t miwear_crypto_sha256(const uint8_t *input, uint16_t ilen,
                                     uint8_t *output)
{
    rt_kprintf("[MIWEAR_PORT] line is %d \n", __LINE__);
//      mbedtls_sha256_update((mbedtls_sha256_context *)output,input, ilen);
    mbedtls_sha256(input, ilen, output, 0);
    return MIWEAR_SUCCESS;
}

miwear_status_t miwear_crypto_ccm_encrypt(const uint8_t *key, const uint8_t *iv,
                                          size_t iv_len, const uint8_t *add,
                                          size_t add_len, const uint8_t *in,
                                          size_t in_len, uint8_t *out,
                                          uint8_t *tag, size_t tag_len)
{
    rt_kprintf("[MIWEAR_PORT] line is %d \n", __LINE__);
    int ret = 0;
    ret = aes_ccm_encrypt_and_tag(key, iv, iv_len, add, add_len, in, in_len,
                                  out, tag, tag_len);
    if (ret)
    {
        return MIWEAR_ERR_INTERNAL;
    }
    return MIWEAR_SUCCESS;

}

miwear_status_t miwear_crypto_ccm_decrypt(const uint8_t *key, const uint8_t *iv,
                                          size_t iv_len, const uint8_t *add,
                                          size_t add_len, const uint8_t *in,
                                          size_t in_len, uint8_t *out,
                                          const uint8_t *tag, size_t tag_len)
{
    rt_kprintf("[MIWEAR_PORT] line is %d \n", __LINE__);
    int ret = 0;
    ret = aes_ccm_auth_decrypt(key, iv, iv_len, add, add_len, in, in_len, out,
                               tag, tag_len);
    if (ret)
    {
        return MIWEAR_ERR_INTERNAL;
    }
    return MIWEAR_SUCCESS;
}

miwear_status_t miwear_did_get(char *did_buf, uint8_t *p_did_len)
{
    rt_kprintf("[MIWEAR_PORT] line is %d \n", __LINE__);

    if (did_buf == NULL || p_did_len == NULL)
    {
        return MIWEAR_ERR_INVALID_ADDR;
    }

    char *pdid = "435642719";

    *p_did_len = strlen(pdid);
    strcpy(did_buf, pdid);

    return MIWEAR_SUCCESS;
}

//
miwear_status_t miwear_device_model_get(char *model_buf, uint8_t *p_model_len)
{
    rt_kprintf("[MIWEAR_PORT] line is %d \n", __LINE__);
    if (model_buf == NULL || p_model_len == NULL)
    {
        return MIWEAR_ERR_INVALID_ADDR;
    }
    // #ifdef HOUYI_PDID_TEST
    char model[] = "yunmai.jmprope.sr803";
    // #else
    // char model[] = "xiaomi.watch.mars";
    // #endif

    int len = strlen(model);
    if (*p_model_len < len)
    {
        return MIWEAR_ERR_DATA_SIZE;
    }

    strcpy(model_buf, model);
    *p_model_len = len;

    return MIWEAR_SUCCESS;

}

miwear_status_t miwear_device_serial_number_get(char *sn_buf, uint8_t *p_sn_len)
{
    rt_kprintf("[MIWEAR_PORT] line is %d \n", __LINE__);
    if (sn_buf == NULL || p_sn_len == NULL)
    {
        return MIWEAR_ERR_INVALID_ADDR;
    }

    char sn[] = "4f35c793a7e7"; // mock
    int len = strlen(sn);
    if (*p_sn_len < len)
    {
        return MIWEAR_ERR_DATA_SIZE;
    }

    strcpy(sn_buf, sn);
    *p_sn_len = len;

    return MIWEAR_SUCCESS;
}

miwear_status_t miwear_device_oob_get(char *oob_buf, uint8_t *p_oob_len)
{
    rt_kprintf("[MIWEAR_PORT] line is %d \n", __LINE__);
    *p_oob_len = 0;
    return MIWEAR_SUCCESS;
}

void miwear_log_printf(const char *format, ...)
{
    rt_kprintf(format);
}

void miwear_hexdump_printf(const char *p_name, const uint8_t *p_data,
                           uint16_t len)
{
    rt_kprintf("[MIWEAR_PORT] line is %d \n", __LINE__);
    rt_kprintf("miwear Hex: %s ", p_name);
    for (uint32_t i = 0; i < (len - 1); i++)
    {
        rt_kprintf("0x%x-", p_data[i]);
    }
    rt_kprintf("0x%x\n", p_data[len - 1]);
}
#include "rtdef.h"
#include "rtthread.h"
void *miwear_mutex_create(void)
{
    rt_kprintf("[MIWEAR_PORT] line is %d \n", __LINE__);
    rt_mutex_t mutex;
    mutex = rt_mutex_create("protocol_mutex", RT_IPC_FLAG_FIFO);
    if (mutex != RT_NULL)
    {
        return mutex;
    }
    else
    {
        RT_ASSERT(RT_NULL != RT_NULL);
        return NULL;
    }
}

void miwear_mutex_lock(void *mutex)
{
    rt_kprintf("[MIWEAR_PORT] line is %d \n", __LINE__);
    if (mutex == RT_NULL)
    {
        return;
    }
    rt_mutex_take(mutex, RT_WAITING_FOREVER);
}

void miwear_mutex_unlock(void *mutex)
{
    rt_kprintf("[MIWEAR_PORT] line is %d \n", __LINE__);
    if (mutex == RT_NULL)
    {
        return;
    }
    rt_mutex_release(mutex);
}

int miwear_get_trace_time_us(void)
{
    time_t now;
    rt_device_t device;
    device = rt_device_find("rtc");
    rt_device_control(device, RT_DEVICE_CTRL_RTC_GET_TIME, &now);
    return now * 1000000;
}

void *miwear_malloc(size_t size)
{
    return malloc(size);
}

void *miwear_calloc(size_t num, size_t size)
{
    return calloc(num, size);
}

void *miwear_realloc(void *ptr, size_t size)
{
    return realloc(ptr, size);
}

void miwear_free(void *ptr)
{
    free(ptr);
}

int miwear_file_open(miwear_file_type_t type, uint8_t *id, size_t id_len,
                     int index, file_open_cb_t open_cb)
{
    RT_ASSERT(RT_NULL != RT_NULL);
    return 0;
}

int miwear_file_read(int handle, uint8_t *buf, size_t len,
                     file_read_cb_t read_cb)
{
    RT_ASSERT(RT_NULL != RT_NULL);
    return 0;
}

int miwear_file_close(int handle, file_close_cb_t close_cb)
{
    RT_ASSERT(RT_NULL != RT_NULL);
    return 0;
}

miwear_status_t miwear_spp_service_send_data(uint16_t conn_handle,
                                             uint8_t *data, uint16_t data_len,
                                             miwear_spp_notify_complete_cb_t cb,
                                             void *cb_data)
{
    RT_ASSERT(RT_NULL != RT_NULL);
    return MIWEAR_SUCCESS;
}

int miwear_mass_prepare(miwear_mass_prepare_type_t prepare_type,
                        uint8_t mass_type, uint8_t *id,
                        size_t id_len, size_t size, int index,
                        void *md5_context, uint8_t support_compress_mode,
                        prepare_cb_t prepare_cb)
{

    RT_ASSERT(RT_NULL != RT_NULL);
    return 0;
}

int miwear_mass_write(int handle, uint8_t *buf, size_t buf_len,
                      size_t packet_len_written, size_t packet_len_total,
                      mass_write_cb_t write_cb)
{
    RT_ASSERT(RT_NULL != RT_NULL);
    return 0;
}

int miwear_mass_finish(int handle, int result)
{
    RT_ASSERT(RT_NULL != RT_NULL);
    return 0;
}

void miwear_md5_init(void **md5_context)
{
    RT_ASSERT(RT_NULL != RT_NULL);
    mbedtls_md5_context *md5_ctx = calloc(1, sizeof(mbedtls_md5_context));
    if (NULL == md5_ctx)
    {

        *md5_context = NULL;
        return;
    }

    mbedtls_md5_init(md5_ctx);
    *md5_context = md5_ctx;
}

void miwear_md5_update(void *md5_context, uint8_t *buf, uint32_t len)
{
    RT_ASSERT(RT_NULL != RT_NULL);
    mbedtls_md5_update(md5_context, buf, len);
}

void miwear_md5_final(void *md5_context, uint8_t result[16])
{
    RT_ASSERT(RT_NULL != RT_NULL);
    mbedtls_md5_finish(md5_context, result);
    mbedtls_md5_free(md5_context);
}

void miwear_ancs_control_point_write(uint8_t *data, uint16_t len)
{
    RT_ASSERT(RT_NULL != RT_NULL);
}

void miwear_media_ams_char_write(uint16_t conn_handle, uint16_t char_handle,
                                 uint8_t *data, uint16_t data_len,
                                 uint8_t count)
{
    RT_ASSERT(RT_NULL != RT_NULL);
}

void miwear_media_ams_char_read(uint16_t conn_handle, uint16_t char_handle,
                                uint16_t offset)
{
    RT_ASSERT(RT_NULL != RT_NULL);
}

bool miwear_bind_proc_allowed(void)
{
    return true;
}
