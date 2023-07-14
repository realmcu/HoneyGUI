

#include "arm_mve.h"
#include "mve_vector_arithmetic_test.h"
#include "rtl_rtc.h"
#include "trace.h"


void vaddq_test(void)
{
    uint8x16_t inactive;
    uint8x16_t a, b;

    mve_pred16_t p = 0x02;

    uint8_t a_array[16] = {0xff, 0xff, 0xff};
    uint8_t b_array[16] = {0x4, 0x5, 0x6};
    uint8_t inactive_array[16] = {0xa, 0xa, 0xa};
    uint8_t ret_array[16] = {0};

    a = vld1q_u8(a_array);
    b = vld1q_u8(b_array);
    //inactive = vld1q_u8(inactive_array);
    //vld1q_u8
    uint8x16_t ret = vaddq_u8(a, b);
    vstrbq_u8(ret_array, ret);
    DBG_DIRECT("vaddq_u8 test ret_array 0x%x 0x%x 0x%x 0x%x", ret_array[0], ret_array[1], ret_array[2],
               ret_array[3]);
    //vaddq_m_u8
    ret = vaddq_m_u8(inactive, a, b, p);
    vstrbq_u8(ret_array, ret);
    DBG_DIRECT("vaddq_m_u8 test ret_array 0x%x 0x%x 0x%x 0x%x", ret_array[0], ret_array[1],
               ret_array[2], ret_array[3]);

    p = 0x02;// change the p value
    //vaddq_x_u8
    ret = vaddq_x_u8(a, b, p);
    vstrbq_u8(ret_array, ret);
    DBG_DIRECT("vaddq_x_u8 test ret_array 0x%x 0x%x 0x%x 0x%x", ret_array[0], ret_array[1],
               ret_array[2], ret_array[3]);

    //vaddq_n_u8
    uint8_t n_b = 0x1;
    ret = vaddq_n_u8(a, n_b);
    vstrbq_u8(ret_array, ret);
    DBG_DIRECT("vaddq_n_u8 test ret_array 0x%x 0x%x 0x%x 0x%x", ret_array[0], ret_array[1],
               ret_array[2], ret_array[3]);

    ret = vaddq_m_n_u8(inactive, a, n_b, p);
    vstrbq_u8(ret_array, ret);
    DBG_DIRECT("vaddq_m_n_u8 test ret_array 0x%x 0x%x 0x%x 0x%x", ret_array[0], ret_array[1],
               ret_array[2], ret_array[3]);
}

void vqaddq_test(void)
{
    uint8x16_t inactive;
    uint8x16_t a, b;

    mve_pred16_t p = 0x02;

    uint8_t a_array[16] = {0xff, 0xff, 0xff};
    uint8_t b_array[16] = {0x4, 0xf, 0xff};
    uint8_t inactive_array[16] = {0xa, 0xa, 0xa};
    uint8_t ret_array[16] = {0x0};

    a = vld1q_u8(a_array);
    b = vld1q_u8(b_array);
    //inactive = vld1q_u8(inactive_array);

    uint8x16_t ret = vqaddq_u8(a, b);
    vstrbq_u8(ret_array, ret);
    DBG_DIRECT("vqaddq_u8 test ret_array 0x%x 0x%x 0x%x 0x%x", ret_array[0], ret_array[1], ret_array[2],
               ret_array[3]);
}

void vaddvq_test(void)
{
    uint8x16_t a;

    mve_pred16_t p = 0x05;

    uint8_t a_array[16] = {1, 2, 3};

    a = vld1q_u8(a_array);

    uint32_t ret = vaddvq_u8(a);
    DBG_DIRECT("vaddvq_u8 test ret_array 0x%x", ret);

    ret = vaddvq_p_u8(a, p);
    DBG_DIRECT("vaddvq_p_u8 test ret_array 0x%x", ret);
}

void mve_vector_arithmetic_test()
{
    vaddq_test();

    vqaddq_test();

    vaddvq_test();
}

