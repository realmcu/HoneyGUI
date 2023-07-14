

#include "arm_mve.h"
#include "mve_mac_test.h"
#include "rtl_rtc.h"
#include "trace.h"


void arm_dot_prod_s32(int32_t *pSrcA, int32_t *pSrcB, uint32_t blockSize, int64_t *result)
{
    uint32_t blkCnt;           /* loop counters */
    int32x4_t vecA;
    int32x4_t vecB;
    int64_t sum = 0LL;

    /* Compute 4 outputs at a time */
    blkCnt = blockSize >> 2;
    while (blkCnt > 0U)
    {
        /*
         * C = A[0]* B[0] + A[1]* B[1] + A[2]* B[2] + .....+ A[blockSize-1]* B[blockSize-1]
         * Calculate dot product and then store the result in a temporary buffer.
         */
        vecA = vld1q_s32(pSrcA);
        vecB = vld1q_s32(pSrcB);
        sum = vmladavaq_s32(sum, vecA, vecB);
        /*
         * Decrement the blockSize loop counter
         */
        blkCnt--;
        /*
         * advance vector source and destination pointers
         */
        pSrcA += 4;
        pSrcB += 4;
    }
    /*
     * tail
     */
    blkCnt = blockSize & 3;
    if (blkCnt > 0U)
    {
        mve_pred16_t p0 = vctp8q(blkCnt);//vctp32q(blkCnt);
        vecA = vld1q_s32(pSrcA);
        vecB = vld1q_s32(pSrcB);
        sum = vmladavaq_p_s32(sum, vecA, vecB, p0);
    }

    *result = sum;
}

void mac_s32_test(void)
{
    int32_t a[16] = {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7,
                     0x8, 0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf
                    };
    int32_t b[16] = {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7,
                     0x8, 0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf
                    };
    int64_t ret = 2;

    arm_dot_prod_s32(a, b, 5, &ret);

    DBG_DIRECT("ret 0x%x", ret);
}

void mve_mac_test()
{
    mac_s32_test();

}

