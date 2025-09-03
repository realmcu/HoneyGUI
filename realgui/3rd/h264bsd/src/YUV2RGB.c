#include <gui_img.h>
#ifdef  __ARM_FEATURE_MVE
#include "arm_mve.h"
#endif

#if defined(__ARMCLANG__)


#ifndef _WIN32
// uint8_t *ycbcrbuf = (uint8_t *)0x8000000 + 0x200000;
// uint8_t *rgbbuf =  (uint8_t *)(0x8000000 + 0x300000 + 8);
#else
uint8_t ycbcrbuf[640 * 368 * 2];
uint8_t rgbbuf[640 * 368 * 4];
#endif

/*
R = CLIP(Y + (91881 * CR >> 16) - 179)
G = CLIP(Y - ((22544 * CB + 46793 * CR) >> 16) + 135)
B = CLIP(Y + (116129 * CB >> 16) - 226)

R = CLIP(Y + (45940 * CR * 2 >> 16) - 179)
G = CLIP(Y - ((11272 * CB * 2 + 23396 * CR * 2) >> 16) + 135)
B = CLIP(Y + (58064 * CB * 2 >> 16) - 226)

4 2 0
640*368
planer
*/

//static const int ycbcr_h = 368;
//static const int ycbcr_w = 640;
static void mve_yuv420_to_rgb(const uint8_t *inputy, const uint8_t *inputcb, const uint8_t *inputcr,
                              uint8_t *outputrgb, GUI_FormatType colorformat,
                              uint32_t raw_w, uint32_t raw_h)
{

    uint16x8_t ycbcroffset = {0, 0, 1, 1, 2, 2, 3, 3};
    uint16x8_t n45940 = vdupq_n_u16(45940);
    uint16x8_t n11272 = vdupq_n_u16(11272);
    uint16x8_t n23396 = vdupq_n_u16(23396);
    uint16x8_t n58064 = vdupq_n_u16(58064);
    uint16x8_t resulta = vdupq_n_u16(0xff);

    if (colorformat == RGB565)
    {
        for (uint32_t i = 0; i < raw_h; i ++)
        {
            for (uint32_t j = 0; j < raw_w; j += 8)
            {
                uint16x8_t rawy = vldrbq_u16(&inputy[i * raw_w + j]);
                uint16x8_t rawcb = vldrbq_gather_offset_u16(&inputcb[i / 2 * raw_w / 2 + j / 2], ycbcroffset);
                uint16x8_t rawcr = vldrbq_gather_offset_u16(&inputcr[i / 2 * raw_w / 2 + j / 2], ycbcroffset);

                rawcb = vmulq_n_u16(rawcb, 2);
                rawcr = vmulq_n_u16(rawcr, 2);

                uint16x8_t resultr = vaddq_u16(rawy, vmulhq_u16(rawcr, n45940));
                resultr = vqsubq_n_u16(resultr, 179);

                uint16x8_t subg = vmulhq_u16(rawcb, n11272);
                subg = vaddq_u16(subg, vmulhq_u16(rawcr, n23396));
                uint16x8_t resultg = vaddq_n_u16(rawy, 135);
                resultg = vqsubq_u16(resultg, subg);

                uint16x8_t resultb = vaddq_u16(rawy, vmulhq_u16(rawcb, n58064));
                resultb = vqsubq_n_u16(resultb, 226);
                uint16x8_t rgb565;
                rgb565 = vsriq_n_u16(vshlq_n_u16(resultb, 8), rgb565, 5);
                rgb565 = vsriq_n_u16(vshlq_n_u16(resultg, 8), rgb565, 6);
                rgb565 = vsriq_n_u16(vshlq_n_u16(resultr, 8), rgb565, 5);
                vst1q_u16((uint16_t *)&outputrgb[(i * raw_w + j) * 2], rgb565);
            }
        }
    }
    else if (colorformat == ARGB8888)
    {
        for (uint32_t i = 0; i < raw_h; i ++)
        {
            for (uint32_t j = 0; j < raw_w; j += 8)
            {
                uint16x8_t rawy = vldrbq_u16(&inputy[i * raw_w + j]);
                uint16x8_t rawcb = vldrbq_gather_offset_u16(&inputcb[i / 2 * raw_w / 2 + j / 2], ycbcroffset);
                uint16x8_t rawcr = vldrbq_gather_offset_u16(&inputcr[i / 2 * raw_w / 2 + j / 2], ycbcroffset);

                rawcb = vmulq_n_u16(rawcb, 2);
                rawcr = vmulq_n_u16(rawcr, 2);

                uint16x8_t resultr = vaddq_u16(rawy, vmulhq_u16(rawcr, n45940));
                resultr = vqsubq_n_u16(resultr, 179);

                uint16x8_t subg = vmulhq_u16(rawcb, n11272);
                subg = vaddq_u16(subg, vmulhq_u16(rawcr, n23396));
                uint16x8_t resultg = vaddq_n_u16(rawy, 135);
                resultg = vqsubq_u16(resultg, subg);

                uint16x8_t resultb = vaddq_u16(rawy, vmulhq_u16(rawcb, n58064));
                resultb = vqsubq_n_u16(resultb, 226);

                uint16x8x2_t rgba;
                rgba.val[0] = vmlaq_n_u16(resultr, resultg, 0x100);
                rgba.val[1] = vmlaq_n_u16(resultb, resulta, 0x100);
                vst2q_u16((uint16_t *)&outputrgb[(i * raw_w + j) * 4], rgba);
            }
        }
    }
    else
    {
        gui_log("YUV2RGB color format not support !");
    }
}


void mve_yuv420_to_rgb565(uint8_t *data, uint8_t *pOutput,
                          uint32_t width, uint32_t height)
{
    GUI_FormatType colorformat;
    colorformat = RGB565;


    uint8_t *ybuf = data;
    uint8_t *cbbuf = data + width * height;
    uint8_t *crbuf = data + width * height + width * height / 4;

    mve_yuv420_to_rgb(ybuf, cbbuf, crbuf, pOutput, colorformat, width, height);

}
#endif
#if 0
static void my_own_test(void)
{
    GUI_FormatType colorformat;
    colorformat = ARGB8888;
    memcpy(ycbcrbuf, YUV420_640_360_BIN, ycbcr_w * ycbcr_h + ycbcr_w * ycbcr_h / 2);
    if (colorformat == RGB565)
    {
        memset(rgbbuf, 0, ycbcr_w * ycbcr_h * 2);
    }
    else if (colorformat == ARGB8888)
    {
        memset(rgbbuf, 0, ycbcr_w * ycbcr_h * 4);
    }
    else
    {
        return;
    }

    uint8_t *ybuf = ycbcrbuf;
    uint8_t *cbbuf = ycbcrbuf + ycbcr_w * ycbcr_h;
    uint8_t *crbuf = ycbcrbuf + ycbcr_w * ycbcr_h + ycbcr_w * ycbcr_h / 4;

    uint32_t ycbcr2rgbt1 = log_timestamp_get();
    mve_yuv420_to_rgb(ybuf, cbbuf, crbuf, rgbbuf, colorformat, ycbcr_w, ycbcr_h);
    uint32_t ycbcr2rgbt2 = log_timestamp_get();

    gui_log("ycbcr2rgb time is %dus \n", ycbcr2rgbt2 - ycbcr2rgbt1);

    gui_rgb_data_head_t head;
    memset(&head, 0x0, sizeof(head));
    head.w = ycbcr_w;
    head.h = ycbcr_h;
    head.type = colorformat;
    memcpy((uint8_t *)(0x8000000 + 0x300000), &head, sizeof(head));
}
#endif