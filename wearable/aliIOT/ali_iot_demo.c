/*
 * Change Logs:
 * Date           Author        Notes
 * 2022-11-17     wanghao
 */

#include <rtthread.h>
#include <rtdevice.h>
#include "rtconfig.h"
#include "rthw.h"
#include "stdlib.h"
#include "string.h"
#include "trace.h"
#include "pre_test.h"
#include "transport_selftest.h"


#if 0
int sm2_sign(const uint8_t *hex_data, const uint32_t data_len,
             const uint8_t *private_key, const uint32_t key_len,
             uint8_t *signature, uint32_t *sign_len)
{
    return 0;
}

int sm2_verify(const uint8_t *hex_data, const uint32_t data_len,
               const uint8_t *public_key, const uint32_t key_len,
               const uint8_t *signature, const uint32_t sign_len)
{
    return 0;
}

int sm2_sign_with_id(const uint8_t *hex_data, const uint32_t data_len,
                     const uint8_t *private_key, const uint32_t key_len,
                     const uint8_t *id, const uint32_t id_len,
                     uint8_t *signature, uint32_t *sign_len)
{
    return 0;
}

int sm2_verify_with_id(const uint8_t *hex_data, const uint32_t data_len,
                       const uint8_t *public_key, const uint32_t key_len,
                       const uint8_t *id, const uint32_t id_len,
                       const uint8_t *signature, const uint32_t sign_len)
{
    return 0;
}

int ecdsa_sign(const uint8_t *hash_data, const uint32_t data_len,
               const uint8_t *private_key, const uint32_t key_len,
               const uint8_t *ecc_name, uint8_t *signature, uint32_t *sign_len)
{
    return 0;
}

int ecdsa_verify(const uint8_t *hash_data, const uint32_t data_len,
                 const uint8_t *public_key, const uint32_t key_len,
                 const uint8_t *ecc_name, const char *signature, const uint32_t sign_len)
{
    return 0;
}
#endif



static void ali_iot_xxx(uint8_t argc, char **argv)
{
//    rt_kprintf("start pretest_sm2_encrypt\n");
    pretest_sm2_encrypt();
    pretest_sm2_sign();
//    rt_kprintf("stop pretest_sm2_sign\n");
    sm2_sign_test();


    //test_peformence();
}
MSH_CMD_EXPORT(ali_iot_xxx, lcd display test);



