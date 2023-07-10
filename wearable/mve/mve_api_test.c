

#include "arm_mve.h"
#include "mve_api_test.h"
#include "rtl_rtc.h"
#include "trace.h"


void vaddq_m_test()
{
    uint8x16_t inactive;
    uint8x16_t a, b;

    mve_pred16_t p = 0x02;

    uint8_t a_array[16] = {1, 2, 3};
    uint8_t b_array[16] = {4, 5, 6};
    uint8_t inactive_array[16] = {0xa, 0xa, 0xa};
    uint8_t ret_array[16] = {0};

    a = vld1q(a_array);
    b = vld1q(b_array);
    inactive = vld1q(inactive_array);

    uint8x16_t ret = vaddq_u8(a, b);
    vstrbq(ret_array, ret);
    DBG_DIRECT("vaddq_u8 test ret_array 0x%x 0x%x 0x%x 0x%x", ret_array[0], ret_array[1], ret_array[2],
               ret_array[3]);

    ret = vaddq_m_u8(inactive, a, b, p);
    vstrbq(ret_array, ret);
    DBG_DIRECT("vaddq_m_u8 test ret_array 0x%x 0x%x 0x%x 0x%x", ret_array[0], ret_array[1],
               ret_array[2], ret_array[3]);
}

void mve_api_test()
{
    vaddq_m_test();

}

