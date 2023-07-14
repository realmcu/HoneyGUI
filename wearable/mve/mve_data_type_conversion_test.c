

#include "arm_mve.h"
#include "mve_data_type_conversion_test.h"
#include "rtl_rtc.h"
#include "trace.h"


void vcvtaq_test(void)
{
    float16x8_t a;

    float16_t a_array[8] = {1.1f, 0, 0};
    int16_t ret_array[8] = {0};

    a = vld1q_f16(a_array);

    int16x8_t ret = vcvtaq_s16_f16(a);
    vst1q_s16(ret_array, ret);
    DBG_DIRECT("vcvtaq_s16_f16 test ret 0x%x a_array 0x%x %4f", ret_array[0], a_array[0], a_array[0]);
}

void vcvtnq_test(void)
{
    float16x8_t a;

    float16_t a_array[8] = {1.1f, 0, 0};
    int16_t ret_array[8] = {0};

    a = vld1q_f16(a_array);

    int16x8_t ret = vcvtnq_s16_f16(a);
    vst1q_s16(ret_array, ret);
    DBG_DIRECT("vcvtnq_s16_f16 test ret 0x%x a_array 0x%x %f4", ret_array[0], a_array[0], a_array[0]);
}

void mve_data_type_conversion_test()
{
    vcvtaq_test();

    vcvtnq_test();
}

