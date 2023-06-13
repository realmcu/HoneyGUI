#ifndef _TRANSPORT_SELFTEST_H_
#define _TRANSPORT_SELFTEST_H_

#include "alipay_common.h"


EXTERNC void test_show_transport_qrcode(uint8_t *transport_code, uint32_t *len_transport_code);

EXTERNC int test_peformence(void);

EXTERNC int test_native_generate_code_func_and_performance(void);

EXTERNC int test_script_generate_code_func_and_performance(void);

EXTERNC retval_t test_get_precheck_send_data(char *out, uint32_t *len_out);

EXTERNC retval_t test_get_cardlist_send_data(char *out, uint32_t *len_out);

EXTERNC retval_t test_get_carddata_send_data(char *cardNo, char *cardType, char *out,
                                             uint32_t *len_out);

EXTERNC retval_t test_get_script_send_data(char *cardNo, char *cardType, char *scriptName,
                                           char *scriptType, char *out, uint32_t *len_out);

EXTERNC retval_t test_parse_rsp_data(char *data, uint32_t len_data, char *out, uint32_t *len_out);

#endif
