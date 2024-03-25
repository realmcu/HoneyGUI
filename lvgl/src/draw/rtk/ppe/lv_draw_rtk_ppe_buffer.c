/**
 * @file lv_draw_rtk_ppe_buffer.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "rtl_rcc.h"
#if LV_USE_GPU_RTK_PPE
#include "lv_draw_rtk_ppe_buffer.h"
#include "rtl_gdma.h"

#define LV_PPE_INTERNAL_BUFFER_SIZE     (10 * 1024)

static uint8_t __attribute__((aligned(4))) ppe_buf[LV_PPE_INTERNAL_BUFFER_SIZE];

uint8_t *lv_draw_rtk_ppe_get_buffer(uint32_t *size)
{
    *size = LV_PPE_INTERNAL_BUFFER_SIZE;
    return ppe_buf;
}

void lv_draw_rtk_ppe_read_buffer(lv_draw_ctx_t *draw_ctx, const lv_area_t *area, uint8_t *dst)
{
    uint16_t w = area->x2 - area->x1 + 1;
    uint16_t h = area->y2 - area->y1 + 1;
    uint16_t bit_depth = LV_COLOR_DEPTH;
    uint8_t pixel_byte = bit_depth / 8;
    uint32_t base_address = (uint32_t)draw_ctx->buf + (area->x1 + area->y1 * MY_DISP_HOR_RES) *
                            pixel_byte;

    RCC_PeriphClockCmd(APBPeriph_GDMA, APBPeriph_GDMA_CLOCK, ENABLE);
    GDMA_InitTypeDef RX_GDMA_InitStruct;
    uint32_t rx_block_num = h;
    GDMA_LLIDef *LVGL_GDMA_LLIStruct = lv_mem_alloc(sizeof(GDMA_LLIDef) * h);
    /*--------------GDMA init-----------------------------*/
    GDMA_StructInit(&RX_GDMA_InitStruct);
    RX_GDMA_InitStruct.GDMA_ChannelNum          = 0;
    RX_GDMA_InitStruct.GDMA_BufferSize          = w * h * pixel_byte / 2;
    RX_GDMA_InitStruct.GDMA_DIR                 = GDMA_DIR_MemoryToMemory;
    RX_GDMA_InitStruct.GDMA_SourceInc           = DMA_SourceInc_Inc;
    RX_GDMA_InitStruct.GDMA_DestinationInc      = DMA_DestinationInc_Inc;
    RX_GDMA_InitStruct.GDMA_SourceMsize         =
        GDMA_Msize_32;                         // 8 msize for source msize
    RX_GDMA_InitStruct.GDMA_DestinationMsize    =
        GDMA_Msize_32;                         // 8 msize for destiantion msize
    RX_GDMA_InitStruct.GDMA_DestinationDataSize =
        GDMA_DataSize_HalfWord;                   // 32 bit width for destination transaction
    RX_GDMA_InitStruct.GDMA_SourceDataSize      =
        GDMA_DataSize_HalfWord;                   // 32 bit width for source transaction
    RX_GDMA_InitStruct.GDMA_SourceAddr          = (uint32_t)base_address;
    RX_GDMA_InitStruct.GDMA_DestinationAddr     = (uint32_t)dst;
#if defined RTL87x2G
    RX_GDMA_InitStruct.GDMA_Secure_En = 1;
#elif defined RTL8763EP
    RX_GDMA_InitStruct.GDMA_DestHandshake       = 43;
#endif
    if (rx_block_num > 1)
    {
        RX_GDMA_InitStruct.GDMA_Multi_Block_Mode = LLI_TRANSFER;
        RX_GDMA_InitStruct.GDMA_Multi_Block_En = 1;
        RX_GDMA_InitStruct.GDMA_Multi_Block_Struct = (uint32_t)LVGL_GDMA_LLIStruct;
    }
    GDMA_Init(GDMA_Channel0, &RX_GDMA_InitStruct);
    if (rx_block_num > 1)
    {
        for (int i = 0; i < rx_block_num; i++)
        {
            LVGL_GDMA_LLIStruct[i].SAR = (uint32_t)(base_address + i * MY_DISP_HOR_RES * pixel_byte);
            LVGL_GDMA_LLIStruct[i].DAR = RX_GDMA_InitStruct.GDMA_SourceAddr + w * i * pixel_byte;
            /* configure high 32 bit of CTL register */
            LVGL_GDMA_LLIStruct[i].CTL_HIGH = w * pixel_byte / 2;
            if (i == rx_block_num - 1)
            {
                LVGL_GDMA_LLIStruct[i].LLP = 0;
                LVGL_GDMA_LLIStruct[i].CTL_LOW = (BIT(0)
                                                  | (RX_GDMA_InitStruct.GDMA_DestinationDataSize << 1)
                                                  | (RX_GDMA_InitStruct.GDMA_SourceDataSize << 4)
                                                  | (RX_GDMA_InitStruct.GDMA_DestinationInc << 7)
                                                  | (RX_GDMA_InitStruct.GDMA_SourceInc << 9)
                                                  | (RX_GDMA_InitStruct.GDMA_DestinationMsize << 11)
                                                  | (RX_GDMA_InitStruct.GDMA_SourceMsize << 14)
                                                  | (RX_GDMA_InitStruct.GDMA_DIR << 20));
            }
            else
            {
                LVGL_GDMA_LLIStruct[i].LLP = (uint32_t)&LVGL_GDMA_LLIStruct[i + 1];
                /* configure low 32 bit of CTL register */
                LVGL_GDMA_LLIStruct[i].CTL_LOW = GDMA_Channel0->GDMA_CTL_LOWx;
            }
        }
    }
    GDMA_INTConfig(0, GDMA_INT_Transfer, ENABLE);
    GDMA_Cmd(0, ENABLE);
    while (GDMA_GetTransferINTStatus(0) != SET);
    GDMA_ClearINTPendingBit(0, GDMA_INT_Transfer);
    lv_mem_free(LVGL_GDMA_LLIStruct);
}

void lv_draw_rtk_ppe_write_buffer(uint8_t *src, const lv_area_t *area, lv_draw_ctx_t *draw_ctx)
{
    uint16_t w = area->x2 - area->x1 + 1;
    uint16_t h = area->y2 - area->y1 + 1;
    uint16_t bit_depth = LV_COLOR_DEPTH;
    uint8_t pixel_byte = bit_depth / 8;
    uint32_t base_address = (uint32_t)draw_ctx->buf + (area->x1 + area->y1 * MY_DISP_HOR_RES) *
                            pixel_byte;

    RCC_PeriphClockCmd(APBPeriph_GDMA, APBPeriph_GDMA_CLOCK, ENABLE);
    GDMA_InitTypeDef RX_GDMA_InitStruct;
    uint32_t rx_block_num = h;
    GDMA_LLIDef *LVGL_GDMA_LLIStruct = lv_mem_alloc(sizeof(GDMA_LLIDef) * h);
    /*--------------GDMA init-----------------------------*/
    GDMA_StructInit(&RX_GDMA_InitStruct);
    RX_GDMA_InitStruct.GDMA_ChannelNum          = 0;
    RX_GDMA_InitStruct.GDMA_BufferSize          = w * h * pixel_byte / 2;
    RX_GDMA_InitStruct.GDMA_DIR                 = GDMA_DIR_MemoryToMemory;
    RX_GDMA_InitStruct.GDMA_SourceInc           = DMA_SourceInc_Inc;
    RX_GDMA_InitStruct.GDMA_DestinationInc      = DMA_DestinationInc_Inc;
    RX_GDMA_InitStruct.GDMA_SourceMsize         =
        GDMA_Msize_32;                         // 8 msize for source msize
    RX_GDMA_InitStruct.GDMA_DestinationMsize    =
        GDMA_Msize_32;                         // 8 msize for destiantion msize
    RX_GDMA_InitStruct.GDMA_DestinationDataSize =
        GDMA_DataSize_HalfWord;                   // 32 bit width for destination transaction
    RX_GDMA_InitStruct.GDMA_SourceDataSize      =
        GDMA_DataSize_HalfWord;                   // 32 bit width for source transaction
    RX_GDMA_InitStruct.GDMA_SourceAddr          = (uint32_t)src;
    RX_GDMA_InitStruct.GDMA_DestinationAddr     = (uint32_t)base_address;
#if defined RTL87x2G
    RX_GDMA_InitStruct.GDMA_Secure_En = 1;
#elif defined RTL8763EP
    RX_GDMA_InitStruct.GDMA_DestHandshake       = 43;
#endif
    if (rx_block_num > 1)
    {
        RX_GDMA_InitStruct.GDMA_Multi_Block_Mode = LLI_TRANSFER;
        RX_GDMA_InitStruct.GDMA_Multi_Block_En = 1;
        RX_GDMA_InitStruct.GDMA_Multi_Block_Struct = (uint32_t)LVGL_GDMA_LLIStruct;
    }
    GDMA_Init(GDMA_Channel0, &RX_GDMA_InitStruct);
    if (rx_block_num > 1)
    {
        for (int i = 0; i < rx_block_num; i++)
        {
            LVGL_GDMA_LLIStruct[i].SAR = RX_GDMA_InitStruct.GDMA_SourceAddr + w * i * pixel_byte;
            LVGL_GDMA_LLIStruct[i].DAR = (uint32_t)(base_address + i * MY_DISP_HOR_RES * pixel_byte);
            /* configure high 32 bit of CTL register */
            LVGL_GDMA_LLIStruct[i].CTL_HIGH = w * pixel_byte / 2;
            if (i == rx_block_num - 1)
            {
                LVGL_GDMA_LLIStruct[i].LLP = 0;
                LVGL_GDMA_LLIStruct[i].CTL_LOW = (BIT(0)
                                                  | (RX_GDMA_InitStruct.GDMA_DestinationDataSize << 1)
                                                  | (RX_GDMA_InitStruct.GDMA_SourceDataSize << 4)
                                                  | (RX_GDMA_InitStruct.GDMA_DestinationInc << 7)
                                                  | (RX_GDMA_InitStruct.GDMA_SourceInc << 9)
                                                  | (RX_GDMA_InitStruct.GDMA_DestinationMsize << 11)
                                                  | (RX_GDMA_InitStruct.GDMA_SourceMsize << 14)
                                                  | (RX_GDMA_InitStruct.GDMA_DIR << 20));
            }
            else
            {
                LVGL_GDMA_LLIStruct[i].LLP = (uint32_t)&LVGL_GDMA_LLIStruct[i + 1];
                /* configure low 32 bit of CTL register */
                LVGL_GDMA_LLIStruct[i].CTL_LOW = GDMA_Channel0->GDMA_CTL_LOWx;
            }
        }
    }
    GDMA_INTConfig(0, GDMA_INT_Transfer, ENABLE);
    GDMA_Cmd(0, ENABLE);
    while (GDMA_GetTransferINTStatus(0) != SET);
    GDMA_ClearINTPendingBit(0, GDMA_INT_Transfer);
    lv_mem_free(LVGL_GDMA_LLIStruct);
}
#endif