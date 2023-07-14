

#include "arm_mve.h"
#include "mve_add_test.h"
#include "rtl_rtc.h"
#include "trace.h"


void arm_add_s8(int8_t *pSrcA, int8_t *pSrcB, int8_t *pDst, uint32_t blockSize)
{
    uint32_t blkCnt;           /* loop counters */
    int8x16_t vecA;
    int8x16_t vecB;

    /* Compute 16 outputs at a time */
    blkCnt = blockSize >> 4;
    while (blkCnt > 0U)
    {
        /*
         * C = A + B
         * Add and then store the results in the destination buffer.
         */
        vecA = vld1q(pSrcA);
        vecB = vld1q(pSrcB);
        vst1q(pDst, vqaddq(vecA, vecB));
        /*
         * Decrement the blockSize loop counter
         */
        blkCnt--;
        /*
         * advance vector source and destination pointers
         */
        pSrcA  += 16;
        pSrcB  += 16;
        pDst   += 16;
    }
    /*
     * tail
     */
    blkCnt = blockSize & 0xF;
    if (blkCnt > 0U)
    {
        mve_pred16_t p0 = vctp8q(blkCnt);
        vecA = vld1q(pSrcA);
        vecB = vld1q(pSrcB);
        vstrbq_p(pDst, vqaddq(vecA, vecB), p0);
    }

}

void add_s8_test(void)
{
    int8_t a[16] = {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7,
                    0x8, 0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf
                   };
    int8_t b[16] = {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7,
                    0x8, 0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf
                   };
    int8_t ret[16] = {0};

    arm_add_s8(a, b, ret, 16);

    DBG_DIRECT("ret 0x%x 0x%x 0x%x 0x%x", ret[0], ret[1], ret[2], ret[3]);
}

void mve_add_test()
{
    add_s8_test();

}

