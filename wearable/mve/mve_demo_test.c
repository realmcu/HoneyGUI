

#include "arm_mve.h"
#include "mve_demo_test.h"
#include "rtl_rtc.h"
#include "trace.h"
#include "drv_gpio.h"
#define MVE_TEST_PIN                                     P3_0


void grayscale(unsigned char *out, unsigned char *rgb, int pixelCnt)
{
    int Index = 0;
    int outPtr = 0;
    int aIn, rIn, gIn, bIn, yOut;
    while (Index < pixelCnt)
    {
        // aIn = rgb[ Index + 3];
        rIn = rgb[ Index + 2];
        gIn = rgb[ Index + 1];
        bIn = rgb[ Index];
        Index += 4;
        yOut = ((77 * rIn) + (151 * gIn) + (28 * bIn)) >> 8;
        out[outPtr++] = yOut;
    }
}

void grayscale_mve(unsigned char *out, unsigned char *rgb, int PixelCnt)
{
    uint16x8_t tmp1, tmp2, sum1, sum2;
    uint8x16_t result;
    uint8x16x4_t pixels;
    PixelCnt = PixelCnt >> 4;
    while (PixelCnt > 0)
    {
        pixels = vld4q_u8(rgb);
        rgb += 64;
        tmp1 = vmovltq_u8(pixels.val[2]);
        tmp2 = vmovlbq_u8(pixels.val[2]);
        sum1 = vmulq_n_u16(tmp1, 77);
        sum2 = vmulq_n_u16(tmp2, 77);
        tmp1 = vmovltq_u8(pixels.val[1]);
        tmp2 = vmovlbq_u8(pixels.val[1]);
        sum1 = vmlaq_n_u16(sum1, tmp1, 151);
        sum2 = vmlaq_n_u16(sum2, tmp2, 151);
        tmp1 = vmovltq_u8(pixels.val[0]);
        tmp2 = vmovlbq_u8(pixels.val[0]);
        sum1 = vmlaq_n_u16(sum1, tmp1, 28);
        sum2 = vmlaq_n_u16(sum2, tmp2, 28);
        result = vshrntq(result, sum1, 8);
        result = vshrnbq(result, sum2, 8);
        vst1q_u8(out, result);
        out += 16;
        PixelCnt--;
    }
}

void rtc_init()
{
    RTC_SetPrescaler(0);

    RTC_NvCmd(ENABLE);
    RTC_ResetCounter();
    RTC_Cmd(ENABLE);
}

void mve_demo_test()
{
//    rtc_init();
    Pad_Config(MVE_TEST_PIN, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
    uint32_t start_time = 0, end_time = 0;

    start_time = RTC_GetCounter();
    drv_pin_mode(MVE_TEST_PIN, PIN_MODE_OUTPUT);
    drv_pin_write(MVE_TEST_PIN, 1);
    drv_pin_write(MVE_TEST_PIN, 0);
    grayscale(0x00520000, 0x00620000, 454 * 454);
    end_time = RTC_GetCounter();
    drv_pin_write(MVE_TEST_PIN, 1);
    DBG_DIRECT("grayscale rtc time %d\r\n", end_time - start_time);

    start_time = RTC_GetCounter();
    drv_pin_write(MVE_TEST_PIN, 1);
    drv_pin_write(MVE_TEST_PIN, 0);
    grayscale_mve(0x00520000, 0x00620000, 454 * 454);
    end_time = RTC_GetCounter();
    drv_pin_write(MVE_TEST_PIN, 1);
    DBG_DIRECT("grayscale_mve rtc time %d\r\n", end_time - start_time);
}

void mve_demo_test_loop(void)
{
    uint8_t n = 10;
    while (n > 0)
    {
        mve_demo_test();
        n--;
    }
}
