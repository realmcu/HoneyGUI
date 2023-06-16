#include "board.h"
#include "st7701s_360640_rgb.h"
#include "rtl_lcdc_edpi.h"
#include <rtdevice.h>
#include "rtl_nvic.h"

/**************************************
NOTE:VCI=3.3V,IOVCC=1.8V,
Display resolution:480*640
params->vertical_sync_active=2
params->vertical_backporch=10
params->vertical_frontporch=8
params->horizontal_sync_active=2
params->horizontal_backporch=10
params->horizontal_frontporch=8
params->RGB.PLL_CLOCK=(19.8)
**************************************/

static struct rt_spi_device spi_device;
static void write_command(uint32_t cmd)
{
    struct rt_spi_message message;
    message.send_buf   = &cmd;
    message.recv_buf   = RT_NULL;
    message.length     = 1;
    message.cs_take    = 1;
    message.cs_release = 0;
    message.next = RT_NULL;
    rt_spi_transfer_message(&spi_device, &message);
}
static void write_data(uint32_t data)
{
    uint32_t send = data | 0x100;

    struct rt_spi_message message;
    message.send_buf   = &send;
    message.recv_buf   = RT_NULL;
    message.length     = 1;
    message.cs_take    = 0;
    message.cs_release = 0;
    message.next = RT_NULL;
    rt_spi_transfer_message(&spi_device, &message);
}

static void st7701s_reset_high(void)
{
    rt_pin_write(P2_0, 1);
}
static void st7701s_reset_low(void)
{
    rt_pin_write(P2_0, 0);
}

//--------------------initial code-----------------------------------------//
void st7701s_init(void)
{
    rt_device_t bus = rt_device_find("spi0");
    spi_device.bus = (struct rt_spi_bus *)bus;
    struct rt_spi_configuration cfg;
    cfg.data_width = 9;
    cfg.mode = RT_SPI_MASTER | RT_SPI_CPHA | RT_SPI_CPOL;
    cfg.max_hz = 1000000;
    rt_spi_configure(&spi_device, &cfg);

    rt_pin_mode(P2_0, PIN_MODE_OUTPUT);
    rt_pin_mode(P2_2, PIN_MODE_OUTPUT);
    rt_pin_write(P2_2, 1);


    LCDC_InitTypeDef lcdc_init = {0};
    lcdc_init.LCDC_Interface = LCDC_IF_DPI;
    lcdc_init.LCDC_PixelInputFarmat = LCDC_INPUT_RGB565;
    lcdc_init.LCDC_PixelOutpuFarmat = LCDC_OUTPUT_RGB565;
    lcdc_init.LCDC_PixelBitSwap = LCDC_SWAP_BYPASS; //lcdc_handler_cfg->LCDC_TeEn = LCDC_TE_DISABLE;

    lcdc_init.LCDC_DmaThreshold =
        8;    //only support threshold = 8 for DMA MSIZE = 8; the other threshold setting will be support later
    LCDC_Init(&lcdc_init);

    LCDC_MaskINTConfig(LCDC_MASK_TX_AUTO_DONE_INT_MSK, DISABLE);
    LCDC_ClearINTPendingBit(LCDC_TX_AUTO_DONE_INT_CLR);
    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = Display_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 3;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);
    rt_kprintf("LCDC_HANDLER->DMA_FIFO_IMR = 0x%x\n", LCDC_HANDLER->DMA_FIFO_IMR);
    rt_kprintf("LCDC_HANDLER->DMA_FIFO_SR = 0x%x\n", LCDC_HANDLER->DMA_FIFO_SR);
    rt_kprintf("LCDC_HANDLER->DMA_FIFO_ICR = 0x%x\n", LCDC_HANDLER->DMA_FIFO_ICR);

    uint32_t HSA = 2, HFP = 8, HBP = 10, HACT = ST7701S_360640_LCD_WIDTH;
    uint32_t VSA = 2, VFP = 8, VBP = 10, VACT = ST7701S_360640_LCD_HIGHT;

    LCDC_eDPICfgTypeDef eDPICfg;//480*640  ---->   500 * 660
    eDPICfg.eDPI_ClockDiv1 = 1;
    eDPICfg.eDPI_ClockDiv2 = 2;
    eDPICfg.eDPI_ClockDiv3 = 1;

    eDPICfg.eDPI_HoriSyncWidth = HSA;
    eDPICfg.eDPI_VeriSyncHeight = VSA;
    eDPICfg.eDPI_AccumulatedHBP = HSA + HBP;
    eDPICfg.eDPI_AccumulatedVBP = VSA + VBP;
    eDPICfg.eDPI_AccumulatedActiveW = HSA + HBP + HACT;
    eDPICfg.eDPI_AccumulatedActiveH = VSA + VBP + VACT;
    eDPICfg.eDPI_TotalWidth = HSA + HBP + HACT + HFP;
    eDPICfg.eDPI_TotalHeight = VSA + VBP + VACT + VFP;
    eDPICfg.eDPI_HoriSyncPolarity = 0;
    eDPICfg.eDPI_VeriSyncPolarity = 0;
    eDPICfg.eDPI_DataEnPolarity = 0;
    eDPICfg.eDPI_LineIntMask = 1;
    //eDPICfg.eDPI_LineIntFlag = 0;
    //eDPICfg.eDPI_LineIntClr = 0;
    //eDPICfg.eDPI_ColorMap = EDPI_PIXELFORMAT_RGB888;//for RGB888
    eDPICfg.eDPI_ColorMap = EDPI_PIXELFORMAT_RGB565_2;//for RGB888
    eDPICfg.eDPI_OperateMode = 0;//video mode
    eDPICfg.eDPI_LcdArc = 0;
    eDPICfg.eDPI_ShutdnPolarity = 0;
    eDPICfg.eDPI_ColorModePolarity = 0;
    eDPICfg.eDPI_ShutdnEn = 0;
    eDPICfg.eDPI_ColorModeEn = 0;
    eDPICfg.eDPI_UpdateCfgEn = 0;
    eDPICfg.eDPI_TearReq = 0;
    eDPICfg.eDPI_Halt = 0;
    eDPICfg.eDPI_CmdMaxLatency = 0;//todo
    //eDPICfg.eDPI_LineBufferPixelNum = eDPICfg.eDPI_TotalWidth * 4;
    eDPICfg.eDPI_LineBufferPixelThreshold = eDPICfg.eDPI_TotalWidth / 2;

    EDPI_Init(&eDPICfg);

    st7701s_reset_high();
    rt_thread_mdelay(1);
    st7701s_reset_low();
    rt_thread_mdelay(10);
    st7701s_reset_high();
    rt_thread_mdelay(120);
//*******************************/
#include "st7701s.txt"
    rt_spi_release(&spi_device);

}

#define LCDC_DMA_CHANNEL_NUM              0
#define LCDC_DMA_CHANNEL_INDEX            LCDC_DMA_Channel0

static uint32_t framebuffer = 0;
void lcd_st7701s_update_framebuffer(uint8_t *buf, uint32_t len)
{
    //LCDC_DMA_SetSourceAddress(LCDC_DMA_CHANNEL_INDEX, (uint32_t)buf);
    framebuffer = (uint32_t)buf;
}

void lcd_st7701s_init_framebuffer(uint8_t *buf, uint32_t len)
{
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

    LCDC_ClearDmaFifo();
    LCDC_ClearTxPixelCnt();

    LCDC_SwitchMode(LCDC_AUTO_MODE);
    LCDC_SwitchDirect(LCDC_TX_MODE);


    LCDC_SetTxPixelLen(len);

    LCDC_Cmd(ENABLE);


    LCDC_DMA_SetSourceAddress(LCDC_DMA_CHANNEL_INDEX, (uint32_t)buf);

    LCDC_DMAChannelCmd(LCDC_DMA_CHANNEL_NUM, ENABLE);
    LCDC_DmaCmd(ENABLE);
    LCDC_AutoWriteCmd(ENABLE);

}

void LCDC_IRQHandler(void)
{
    if (LCDC_GetINTStatus(LCDC_TX_AUTO_DONE_INT_MSK) == SET)
    {
        LCDC_ClearINTPendingBit(LCDC_TX_AUTO_DONE_INT_MSK);
        //rt_kprintf("LCDC_IRQHandler line = %d\n",__LINE__);
        LCDC_ClearDmaFifo();
        LCDC_ClearTxPixelCnt();

        LCDC_DMA_SetSourceAddress(LCDC_DMA_CHANNEL_INDEX, (uint32_t)framebuffer);
        LCDC_DMAChannelCmd(LCDC_DMA_CHANNEL_NUM, ENABLE);
        LCDC_DmaCmd(ENABLE);
        LCDC_AutoWriteCmd(ENABLE);
    }

}


//*******************************************
void EnterSleep(void)
{
    write_command(0x28);
    rt_thread_mdelay(10);
    write_command(0x10);
}

//*********************************************************
void ExitSleep(void)
{
    write_command(0x11);
    rt_thread_mdelay(120);
    write_command(0x29);
}





