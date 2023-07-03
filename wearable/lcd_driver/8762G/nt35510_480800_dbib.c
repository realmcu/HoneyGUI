#include "rtl_lcdc.h"
#include "rtl_rcc.h"
#include "rtl_lcdc_dbib.h"
#include "nt35510_480800_dbib.h"
#include "utils.h"


#define LCDC_DMA_CHANNEL_NUM              0
#define LCDC_DMA_CHANNEL_INDEX            LCDC_DMA_Channel0

#define DMA_MULTIBLOCK 0


void nt35510_write_cmd(uint16_t command)
{
    DBIB_SetCS();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    DBIB_ResetCS();
    uint8_t cmd_low, cmd_high;
    cmd_low = command;
    cmd_high = command >> 8;
    DBIB_SendCmd(cmd_high);
    DBIB_SendCmd(cmd_low);
}



void nt35510_write_data16(uint16_t data)
{
    uint8_t data_buf[2];
    data_buf[0] = data >> 8;
    data_buf[1] = data;
    DBIB_SendData(data_buf, 2);
}

void rtl_lcd_hal_power_on(void)
{
//    nt35510_write_cmd(0x1100);
////    rt_thread_mdelay(160);
//    nt35510_write_cmd(0x2900);
    //lcd_set_backlight(100);
}

void rtl_lcd_hal_power_off(void)
{
    nt35510_write_cmd(0x2800);
    nt35510_write_cmd(0x1000);
    //lcd_set_backlight(0);
}

static void nt35510_seq_init(void)
{
#include "nt35510_seq_init.txt"
}

void rtk_lcd_hal_set_window(uint16_t xStart, uint16_t yStart, uint16_t w, uint16_t h)
{
    LCDC_Cmd(DISABLE);
    LCDC_SwitchMode(LCDC_MANUAL_MODE);
    LCDC_SwitchDirect(LCDC_TX_MODE);
    LCDC_Cmd(ENABLE);
    nt35510_write_cmd(0x2a00);
    nt35510_write_data16(xStart >> 8);
    nt35510_write_cmd(0x2a01);
    nt35510_write_data16(xStart & 0xff);
    nt35510_write_cmd(0x2a02);
    nt35510_write_data16((w - 1) >> 8);
    nt35510_write_cmd(0x2a03);
    nt35510_write_data16((w - 1) & 0xff);

    nt35510_write_cmd(0x2b00);
    nt35510_write_data16(yStart >> 8);
    nt35510_write_cmd(0x2b01);
    nt35510_write_data16(yStart & 0xff);
    nt35510_write_cmd(0x2b02);
    nt35510_write_data16((h - 1) >> 8);
    nt35510_write_cmd(0x2b03);
    nt35510_write_data16((h - 1) & 0xff);
    DBIB_SetCS();
}

void rtk_lcd_hal_update_framebuffer(uint8_t *buf, uint32_t len)
{
#if !DMA_MULTIBLOCK
    LCDC_DMA_InitTypeDef LCDC_DMA_InitStruct = {0};
    LCDC_DMA_StructInit(&LCDC_DMA_InitStruct);
    LCDC_DMA_InitStruct.LCDC_DMA_ChannelNum          = LCDC_DMA_CHANNEL_NUM;
    LCDC_DMA_InitStruct.LCDC_DMA_DIR                 = DMA_DIR_PeripheralToMemory;
    LCDC_DMA_InitStruct.LCDC_DMA_SourceInc           = DMA_SourceInc_Inc;
    LCDC_DMA_InitStruct.LCDC_DMA_DestinationInc      = DMA_DestinationInc_Fix;
    LCDC_DMA_InitStruct.LCDC_DMA_SourceDataSize      = DMA_DataSize_Word;
    LCDC_DMA_InitStruct.LCDC_DMA_DestinationDataSize = DMA_DataSize_Word;
    LCDC_DMA_InitStruct.LCDC_DMA_SourceMsize         = DMA_Msize_8;
    LCDC_DMA_InitStruct.LCDC_DMA_DestinationMsize    = DMA_Msize_8;
    LCDC_DMA_InitStruct.LCDC_DMA_SourceAddr          = 0x00500000;
    LCDC_DMA_InitStruct.LCDC_DMA_Multi_Block_En      = 0;
    LCDC_DMA_Init(LCDC_DMA_CHANNEL_INDEX, &LCDC_DMA_InitStruct);
#else
    LCDC_DMA_InitTypeDef LCDC_DMA_InitStruct = {0};
    LCDC_DMA_StructInit(&LCDC_DMA_InitStruct);
    LCDC_DMA_InitStruct.LCDC_DMA_ChannelNum          = LCDC_DMA_CHANNEL_NUM;
    LCDC_DMA_InitStruct.LCDC_DMA_DIR                 = DMA_DIR_PeripheralToMemory;
    LCDC_DMA_InitStruct.LCDC_DMA_SourceInc           = DMA_SourceInc_Inc;
    LCDC_DMA_InitStruct.LCDC_DMA_DestinationInc      = DMA_DestinationInc_Fix;
    LCDC_DMA_InitStruct.LCDC_DMA_SourceDataSize      = DMA_DataSize_Word;
    LCDC_DMA_InitStruct.LCDC_DMA_DestinationDataSize = DMA_DataSize_Word;
    LCDC_DMA_InitStruct.LCDC_DMA_SourceMsize         = DMA_Msize_8;
    LCDC_DMA_InitStruct.LCDC_DMA_DestinationMsize    = DMA_Msize_8;
    LCDC_DMA_InitStruct.LCDC_DMA_SourceAddr          = 0;

    LCDC_DMA_InitStruct.LCDC_DMA_Multi_Block_Mode   =
        LLI_TRANSFER;//LLI_TRANSFER or LLI_WITH_CONTIGUOUS_SAR
    LCDC_DMA_InitStruct.LCDC_DMA_Multi_Block_En     = 1;
    LCDC_DMA_InitStruct.LCDC_DMA_Multi_Block_Struct  = LCDC_DMA_LINKLIST_REG_BASE + 0x50;
    LCDC_DMA_Init(LCDC_DMA_CHANNEL_INDEX, &LCDC_DMA_InitStruct);

    LCDC_SET_GROUP1_BLOCKSIZE(NT35510_LCD_WIDTH * NT35510_DRV_PIXEL_BYTES);
    LCDC_SET_GROUP2_BLOCKSIZE(NT35510_LCD_WIDTH * NT35510_DRV_PIXEL_BYTES);

    LCDC_DMALLI_InitTypeDef LCDC_DMA_LLI_Init = {0};
    LCDC_DMA_LLI_Init.g1_source_addr = (uint32_t)buf;
    LCDC_DMA_LLI_Init.g1_sar_offset = NT35510_LCD_WIDTH * NT35510_DRV_PIXEL_BYTES * 2;

    LCDC_DMA_LLI_Init.g2_source_addr = (uint32_t)(buf + NT35510_LCD_WIDTH * NT35510_DRV_PIXEL_BYTES);
    LCDC_DMA_LLI_Init.g2_sar_offset = NT35510_LCD_WIDTH * NT35510_DRV_PIXEL_BYTES * 2;
    LCDC_DMA_LinkList_Init(&LCDC_DMA_LLI_Init,
                           &LCDC_DMA_InitStruct);//LLI_TRANSFER or LLI_WITH_CONTIGUOUS_SAR
#endif

    LCDC_ClearDmaFifo();
    LCDC_ClearTxPixelCnt();

    LCDC_SwitchMode(LCDC_AUTO_MODE);
    LCDC_SwitchDirect(LCDC_TX_MODE);
    DBIB_BypassCmdByteCmd(DISABLE);

    uint8_t cmd[2] = {0x2C, 0x00};
    LCDC_DBIB_SetCmdSequence(cmd, 2);

    LCDC_SetTxPixelLen(len);

    LCDC_Cmd(ENABLE);


    LCDC_DMA_SetSourceAddress(LCDC_DMA_CHANNEL_INDEX, (uint32_t)buf);
#if DMA_MULTIBLOCK
    LCDC_DMA_MultiBlockCmd(ENABLE);
#endif
    LCDC_DMAChannelCmd(LCDC_DMA_CHANNEL_NUM, ENABLE);
    LCDC_DmaCmd(ENABLE);
    LCDC_AutoWriteCmd(ENABLE);

    LCDC_HANDLER_DMA_FIFO_CTRL_t handler_reg_0x18;
    do
    {
        handler_reg_0x18.d32 = LCDC_HANDLER->DMA_FIFO_CTRL;
    }
    while (handler_reg_0x18.b.dma_enable != RESET);
    LCDC_HANDLER_OPERATE_CTR_t handler_reg_0x14;
    do
    {
        handler_reg_0x14.d32 = LCDC_HANDLER->OPERATE_CTR;
    }
    while (handler_reg_0x14.b.auto_write_start != RESET);


    LCDC_ClearDmaFifo();
    LCDC_ClearTxPixelCnt();

    LCDC_Cmd(DISABLE);
}

void rtk_lcd_hal_rect_fill(uint16_t xStart, uint16_t yStart, uint16_t w, uint16_t h,
                           uint32_t color)
{
    rtk_lcd_hal_set_window(xStart, yStart, w, h);

    uint32_t buf[128];
    for (int i = 0; i < 64; i++)
    {
        buf[i] = color;
    }
    LCDC_DMA_InitTypeDef LCDC_DMA_InitStruct = {0};
    LCDC_DMA_StructInit(&LCDC_DMA_InitStruct);
    LCDC_DMA_InitStruct.LCDC_DMA_ChannelNum          = LCDC_DMA_CHANNEL_NUM;
    LCDC_DMA_InitStruct.LCDC_DMA_DIR                 = DMA_DIR_PeripheralToMemory;
    LCDC_DMA_InitStruct.LCDC_DMA_SourceInc           = DMA_SourceInc_Fix;
    LCDC_DMA_InitStruct.LCDC_DMA_DestinationInc      = DMA_DestinationInc_Fix;
    LCDC_DMA_InitStruct.LCDC_DMA_SourceDataSize      = DMA_DataSize_Word;
    LCDC_DMA_InitStruct.LCDC_DMA_DestinationDataSize = DMA_DataSize_Word;
    LCDC_DMA_InitStruct.LCDC_DMA_SourceMsize         = DMA_Msize_8;
    LCDC_DMA_InitStruct.LCDC_DMA_DestinationMsize    = DMA_Msize_8;
    LCDC_DMA_InitStruct.LCDC_DMA_SourceAddr          = 0x00500000;
    LCDC_DMA_InitStruct.LCDC_DMA_Multi_Block_En      = 0;
    LCDC_DMA_Init(LCDC_DMA_CHANNEL_INDEX, &LCDC_DMA_InitStruct);

    LCDC_ClearDmaFifo();
    LCDC_ClearTxPixelCnt();

    LCDC_SwitchMode(LCDC_AUTO_MODE);
    LCDC_SwitchDirect(LCDC_TX_MODE);
    DBIB_BypassCmdByteCmd(DISABLE);

    uint8_t cmd[2] = {0x2C, 0x00};
    LCDC_DBIB_SetCmdSequence(cmd, 2);

    LCDC_SetTxPixelLen(w * h);

    LCDC_Cmd(ENABLE);


    LCDC_DMA_SetSourceAddress(LCDC_DMA_CHANNEL_INDEX, (uint32_t)buf);
    LCDC_DMAChannelCmd(LCDC_DMA_CHANNEL_NUM, ENABLE);
    LCDC_DmaCmd(ENABLE);
    LCDC_AutoWriteCmd(ENABLE);

}

void rtk_lcd_hal_transfer_done(void)
{
    LCDC_HANDLER_DMA_FIFO_CTRL_t handler_reg_0x18;
    do
    {
        handler_reg_0x18.d32 = LCDC_HANDLER->DMA_FIFO_CTRL;
    }
    while (handler_reg_0x18.b.dma_enable != RESET);
    LCDC_HANDLER_OPERATE_CTR_t handler_reg_0x14;
    do
    {
        handler_reg_0x14.d32 = LCDC_HANDLER->OPERATE_CTR;
    }
    while (handler_reg_0x14.b.auto_write_start != RESET);

    LCDC_ClearDmaFifo();
    LCDC_ClearTxPixelCnt();

    LCDC_Cmd(DISABLE);
}

void nt35510_read(uint16_t command, uint8_t *pBuf, uint32_t len)
{
    LCDC_Cmd(DISABLE);
    LCDC_SwitchMode(LCDC_MANUAL_MODE);
    LCDC_SwitchDirect(LCDC_TX_MODE);
    LCDC_Cmd(ENABLE);
    nt35510_write_cmd(command);

    LCDC_Cmd(DISABLE);
    LCDC_SwitchMode(LCDC_MANUAL_MODE);
    LCDC_SwitchDirect(LCDC_RX_MODE);
    LCDC_Cmd(ENABLE);
    /* Read data */
    DBIB_ReceiveData(pBuf, len);

    DBIB_SetCS();
}


void rtk_lcd_hal_init(void)
{
//    *(uint32_t *)0x400e2148 |= BIT2 | BIT3;     //BIT2 disp_ck_en, BIT3 disp_func_en
//    *(uint32_t *)0x400e2144 |=
//        BIT22;           //display_mux_gclk's enable 0: no clock 1: from CLK_DISPLAY_SRC_MUX1

    RCC_PeriphClockCmd(APBPeriph_DISP, APBPeriph_DISP_CLOCK, ENABLE);

    LCDC_InitTypeDef lcdc_init = {0};
    lcdc_init.LCDC_Interface = LCDC_IF_DBIB;
    lcdc_init.LCDC_PixelInputFarmat = LCDC_INPUT_ARGB8888;
    lcdc_init.LCDC_PixelOutpuFarmat = LCDC_OUTPUT_RGB888;
    lcdc_init.LCDC_PixelBitSwap = LCDC_SWAP_BYPASS; //lcdc_handler_cfg->LCDC_TeEn = LCDC_TE_DISABLE;
    lcdc_init.LCDC_TePolarity = LCDC_TE_EDGE_RISING;
    lcdc_init.LCDC_DmaThreshold =
        8;    //only support threshold = 8 for DMA MSIZE = 8; the other threshold setting will be support later
    LCDC_Init(&lcdc_init);


    LCDC_DBIBCfgTypeDef dbib_init = {0};
    dbib_init.DBIB_SPEED_SEL         = DBIB_CLOCK_DIV_2;
    dbib_init.DBIB_InitGuardTimeCmd  = DBIB_INIT_GUARD_TIME_DISABLE;
    dbib_init.DBIB_InitGuardTime     = DBIB_INIT_GUARD_TIME_1T;
    dbib_init.DBIB_CmdGuardTimeCmd   = DBIB_CMD_GUARD_TIME_DISABLE;
    dbib_init.DBIB_CmdGuardTime      = DBIB_CMD_GUARD_TIME_1T;
    dbib_init.DBIB_GuardTimeCmd      = DBIB_GUARD_TIME_DISABLE;
    dbib_init.DBIB_GuardTime         = DBIB_GUARD_TIME_1T;
    dbib_init.DBIB_WRDelay           = DBIB_WR_HALF_DELAY;
    DBIB_Init(&dbib_init);

    //lcdc_dbib_init_lcd();
}

void rtk_lcd_init(void)
{
    LCDC_SwitchMode(LCDC_MANUAL_MODE);
    LCDC_SwitchDirect(LCDC_TX_MODE);
    LCDC_Cmd(ENABLE);

    LCDC_LCD_SET_RST(false);
    platform_delay_ms(100);
    LCDC_LCD_SET_RST(true);
    platform_delay_ms(50);
    LCDC_LCD_SET_RST(false);
    platform_delay_ms(50);

    nt35510_seq_init();
}

void rtk_lcd_clear(void)
{
    rtk_lcd_hal_set_window(0, 0, NT35510_LCD_WIDTH, NT35510_LCD_HIGHT);

    uint32_t buf[128];
    for (int i = 0; i < 64; i++)
    {
        buf[i] = 0;
    }
    LCDC_DMA_InitTypeDef LCDC_DMA_InitStruct = {0};
    LCDC_DMA_StructInit(&LCDC_DMA_InitStruct);
    LCDC_DMA_InitStruct.LCDC_DMA_ChannelNum          = LCDC_DMA_CHANNEL_NUM;
    LCDC_DMA_InitStruct.LCDC_DMA_DIR                 = DMA_DIR_PeripheralToMemory;
    LCDC_DMA_InitStruct.LCDC_DMA_SourceInc           = DMA_SourceInc_Fix;
    LCDC_DMA_InitStruct.LCDC_DMA_DestinationInc      = DMA_DestinationInc_Fix;
    LCDC_DMA_InitStruct.LCDC_DMA_SourceDataSize      = DMA_DataSize_Word;
    LCDC_DMA_InitStruct.LCDC_DMA_DestinationDataSize = DMA_DataSize_Word;
    LCDC_DMA_InitStruct.LCDC_DMA_SourceMsize         = DMA_Msize_8;
    LCDC_DMA_InitStruct.LCDC_DMA_DestinationMsize    = DMA_Msize_8;
    LCDC_DMA_InitStruct.LCDC_DMA_SourceAddr          = 0x00500000;
    LCDC_DMA_InitStruct.LCDC_DMA_Multi_Block_En      = 0;
    LCDC_DMA_Init(LCDC_DMA_CHANNEL_INDEX, &LCDC_DMA_InitStruct);

    LCDC_ClearDmaFifo();
    LCDC_ClearTxPixelCnt();

    LCDC_SwitchMode(LCDC_AUTO_MODE);
    LCDC_SwitchDirect(LCDC_TX_MODE);
    DBIB_BypassCmdByteCmd(DISABLE);

    uint8_t cmd[2] = {0x2C, 0x00};
    LCDC_DBIB_SetCmdSequence(cmd, 2);

    LCDC_SetTxPixelLen(NT35510_LCD_WIDTH * NT35510_LCD_HIGHT);

    LCDC_Cmd(ENABLE);


    LCDC_DMA_SetSourceAddress(LCDC_DMA_CHANNEL_INDEX, (uint32_t)buf);
    LCDC_DMAChannelCmd(LCDC_DMA_CHANNEL_NUM, ENABLE);
    LCDC_DmaCmd(ENABLE);
    LCDC_AutoWriteCmd(ENABLE);
}

uint32_t rtk_lcd_hal_get_width(void)
{
    return NT35510_LCD_WIDTH;
}

uint32_t rtk_lcd_hal_get_height(void)
{
    return NT35510_LCD_HIGHT;
}

uint32_t rtk_lcd_hal_get_pixel_bits(void)
{
    return NT35510_DRV_PIXEL_BITS;
}

void rtk_lcd_hal_start_transfer(uint8_t *buf, uint32_t len)
{
}
