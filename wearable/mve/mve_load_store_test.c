

#include "arm_mve.h"
#include "mve_load_store_test.h"
#include "rtl_rtc.h"
#include "trace.h"


void stride_test(void)
{
    uint16_t base[8] = {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7};
    uint16_t ret_array[8] = {0x0};

    uint16x8_t ret = vld1q_u16(base);
    vst1q_u16(ret_array, ret);
    DBG_DIRECT("vld1q_u16 test ret_array 0x%x 0x%x 0x%x 0x%x", ret_array[0], ret_array[1],
               ret_array[2], ret_array[3]);
}

void consecutive_test(void)
{
    uint16_t base[8] = {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7};
    uint16_t ret_array[8] = {0x0};

    uint16x8_t ret = vldrhq_u16(base);
    vst1q_u16(ret_array, ret);
    DBG_DIRECT("vldrhq_u16 test ret_array 0x%x 0x%x 0x%x 0x%x", ret_array[0], ret_array[1],
               ret_array[2], ret_array[3]);
}

void gather_test(void)
{
    //s8
    uint8x16_t offset_u8;

    int8_t base_s8[16] = {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7,
                          0x8, 0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf
                         };
    uint8_t offset_array_u8[16] = {0x0, 0x0, 0x0, 0x3, 0x4, 0x5, 0x6, 0x7};
    int8_t ret_array_s8[16] = {0};

    offset_u8 = vld1q_u8(offset_array_u8);

    int8x16_t ret_s8 = vldrbq_gather_offset_s8(base_s8, offset_u8);
    vst1q_s8(ret_array_s8, ret_s8);
    DBG_DIRECT("vldrbq_gather_offset_s8 test ret_array 0x%x 0x%x 0x%x 0x%x", ret_array_s8[0],
               ret_array_s8[1],
               ret_array_s8[2], ret_array_s8[3]);
    //u8
    uint8_t base_u8[16] = {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7,
                           0x8, 0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf
                          };
    uint8_t ret_array_u8[16] = {0};

    uint8x16_t ret_u8 = vldrbq_gather_offset_u8(base_u8, offset_u8);
    vst1q_u8(ret_array_u8, ret_u8);
    DBG_DIRECT("vldrbq_gather_offset_u8 test ret_array 0x%x 0x%x 0x%x 0x%x", ret_array_u8[0],
               ret_array_u8[1],
               ret_array_u8[2], ret_array_u8[3]);

    //u16
    uint16x8_t offset_u16;

    uint16_t base_u16[8] = {0x123, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7};
    uint16_t offset_u16_array[8] = {0x2, 0x0, 0x4};
    uint16_t ret_array_u16[16] = {0};

    offset_u16 = vld1q_u16(offset_u16_array);

    uint16x8_t ret_u16 = vldrhq_gather_offset_u16(base_u16, offset_u16);
    vst1q_u16(ret_array_u16, ret_u16);
    DBG_DIRECT("vldrhq_gather_offset_u16 test ret_array 0x%x 0x%x 0x%x 0x%x", ret_array_u16[0],
               ret_array_u16[1],
               ret_array_u16[2], ret_array_u16[3]);
    //s16
    int16_t base_s16[8] = {0x22, 0x111, 0x23, 0x34, 0x45, 0x56, 0x67, 0x78};
    int16_t ret_array_s16[16] = {0};

    int16x8_t ret_s16 = vldrhq_gather_offset_s16(base_s16, offset_u16);
    vst1q_s16(ret_array_s16, ret_s16);
    DBG_DIRECT("vldrhq_gather_offset_s16 test ret_array 0x%x 0x%x 0x%x 0x%x", ret_array_s16[0],
               ret_array_s16[1],
               ret_array_s16[2], ret_array_s16[3]);
}

void mve_load_store_test()
{
    stride_test();

    consecutive_test();

    gather_test();
}

