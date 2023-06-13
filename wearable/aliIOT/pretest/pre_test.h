#ifndef _PRE_TEST_H_
#define _PRE_TEST_H_

#if defined(__cplusplus)||defined(c_plusplus)
extern "C" {
#endif

void pre_test_show_transport_qrcode(unsigned char *transport_code,
                                    unsigned int *len_transport_code);

int pretest_sm2_encrypt(void);

int pretest_sm2_sign(void);

#if defined(__cplusplus)||defined(c_plusplus)
}
#endif

#endif
