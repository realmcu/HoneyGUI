#include "st7701s_480480_rgb.h"
#include "rtl_lcdc_edpi.h"
#include "rtl_nvic.h"
#include "rtl_spi.h"
#include "rtl_pinmux.h"
#include "rtl_gpio.h"
#include "rtl_rcc.h"
#include "utils.h"
#include "mem_config.h"
#include "string.h"
#include "rtl_gdma.h"

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

#define LCDC_DATA23         P5_0
#define LCDC_DATA22         P5_1
#define LCDC_DATA21         P5_2
#define LCDC_DATA20         P5_3
#define LCDC_DATA19         P5_4
#define LCDC_DATA18         P5_5
#define LCDC_DATA17         P3_5
#define LCDC_DATA16         P3_4
#define LCDC_DATA15         P3_3
#define LCDC_DATA14         P3_2
#define LCDC_DATA13         P9_2
#define LCDC_DATA11         P4_6
#define LCDC_DATA10         P4_5
#define LCDC_DATA9          P4_4
#define LCDC_DATA8          P1_2
#define LCDC_DATA12         P4_7
#define LCDC_DATA6          P4_2
#define LCDC_DATA5          P4_1
#define LCDC_DATA7          P4_3
#define LCDC_DATA4          P4_0
#define LCDC_DATA3          P0_7
#define LCDC_DATA2          P0_6
#define LCDC_DATA1          P0_5
#define LCDC_DATA0          P0_4

#define LCDC_RGB_WRCLK      P0_2
#define LCDC_HSYNC          P1_5
#define LCDC_VSYNC          P0_1
#define LCDC_CSN_DE         P0_0

#define GPD0                P2_5
#define GPD1                P2_7
#define GPD2                P9_0
#define GPD3                P2_6

#define LCDC_RESET          GPD2

#define SPI0_SCK_PIN                               GPD0
#define SPI0_MOSI_PIN                              P3_6
#define SPI0_CS_PIN                                GPD3

#define LCDC_DMA_CHANNEL_NUM              0
#define LCDC_DMA_CHANNEL_INDEX            LCDC_DMA_Channel0

#define TRANSFER_DMA_CHANNEL_NUM              0
#define TRANSFER_DMA_CHANNEL_INDEX            GDMA_Channel0

static void *internal_frame_buffer1, *internal_frame_buffer2;
static bool using_frame_buffer1 = true;
static uint32_t pfb_probe = 0;

static void write_command(uint32_t cmd)
{
//    GPIO_WriteBit(GPIO_GetPort(SPI0_CS_PIN), GPIO_GetPin(SPI0_CS_PIN), (BitAction)(0));
    platform_delay_us(200);
    SPI_SendData(SPI0, cmd);
//    platform_delay_us(2);
//    GPIO_WriteBit(GPIO_GetPort(SPI0_CS_PIN), GPIO_GetPin(SPI0_CS_PIN), (BitAction)(1));
}
static void write_data(uint32_t data)
{
    uint32_t send = (data | 0x100);
//    GPIO_WriteBit(GPIOB, GPIO_GetPin(SPI0_CS_PIN), (BitAction)(0));
//    platform_delay_us(50);
    SPI_SendData(SPI0, send);
//    platform_delay_us(2);
//    GPIO_WriteBit(GPIOB, GPIO_GetPin(SPI0_CS_PIN), (BitAction)(1));
}

static void st7701s_reset_high(void)
{
    GPIO_WriteBit(GPIO_GetPort(LCDC_RESET), GPIO_GetPin(LCDC_RESET), 1);
}

static void st7701s_reset_low(void)
{
    GPIO_WriteBit(GPIO_GetPort(LCDC_RESET), GPIO_GetPin(LCDC_RESET), 0);
}

//--------------------initial code-----------------------------------------//
static void st7701s_gpio_init(void)
{
    Pad_Config(LCDC_RESET, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);


    Pinmux_Config(LCDC_RESET, DWGPIO);

    RCC_PeriphClockCmd(APBPeriph_GPIOA, APBPeriph_GPIOA_CLOCK, ENABLE);
    RCC_PeriphClockCmd(APBPeriph_GPIOB, APBPeriph_GPIOB_CLOCK, ENABLE);

    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin    = GPIO_GetPin(LCDC_RESET);
    GPIO_InitStruct.GPIO_Mode   = GPIO_MODE_OUT;
    GPIO_InitStruct.GPIO_ITCmd  = DISABLE;
    GPIO_Init(GPIO_GetPort(LCDC_RESET), &GPIO_InitStruct);
    st7701s_reset_high();
}

static void st7701s_spi_init(void)
{
    Pad_Config(SPI0_MOSI_PIN, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);
    Pad_Config(SPI0_SCK_PIN, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);
    Pad_Config(SPI0_CS_PIN, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);

    Pinmux_Deinit(SPI0_MOSI_PIN);
    Pinmux_Deinit(SPI0_SCK_PIN);
    Pinmux_Deinit(SPI0_CS_PIN);

    Pinmux_Config(SPI0_CS_PIN, SPI0_CSN_0_MASTER);
    Pinmux_Config(SPI0_SCK_PIN, SPI0_CLK_MASTER);
    Pinmux_Config(SPI0_MOSI_PIN, SPI0_MO_MASTER);

    RCC_PeriphClockCmd(APBPeriph_SPI0, APBPeriph_SPI0_CLOCK, ENABLE);

    SPI_InitTypeDef spi_init_struct;
    SPI_StructInit(&spi_init_struct);
    spi_init_struct.SPI_CPOL = SPI_CPOL_Low;
    spi_init_struct.SPI_CPHA = SPI_CPHA_1Edge;
    spi_init_struct.SPI_Direction = SPI_Direction_TxOnly;
    spi_init_struct.SPI_DataSize = SPI_DataSize_9b;
    spi_init_struct.SPI_BaudRatePrescaler = 64;
    spi_init_struct.SPI_FrameFormat = SPI_Frame_Motorola;
    SPI_Init(SPI0, &spi_init_struct);

    SPI_Cmd(SPI0, ENABLE);

}

static void st7701s_dma_init(void)
{
#if DMA_MULTIBLOCK
    LCDC_DMA_InitTypeDef LCDC_DMA_InitStruct = {0};
    LCDC_DMA_StructInit(&LCDC_DMA_InitStruct);
    LCDC_DMA_InitStruct.LCDC_DMA_ChannelNum          = LCDC_DMA_CHANNEL_NUM;
    LCDC_DMA_InitStruct.LCDC_DMA_DIR                 = DMA_DIR_PeripheralToMemory;
    LCDC_DMA_InitStruct.LCDC_DMA_SourceInc           = DMA_SourceInc_Inc;
    LCDC_DMA_InitStruct.LCDC_DMA_DestinationInc      = DMA_DestinationInc_Fix;
    LCDC_DMA_InitStruct.LCDC_DMA_SourceDataSize      = DMA_DataSize_Word;
    LCDC_DMA_InitStruct.LCDC_DMA_DestinationDataSize = DMA_DataSize_Word;
    LCDC_DMA_InitStruct.LCDC_DMA_SourceMsize         = DMA_Msize_64;
    LCDC_DMA_InitStruct.LCDC_DMA_DestinationMsize    = DMA_Msize_64;
    LCDC_DMA_InitStruct.LCDC_DMA_SourceAddr          = 0;

    LCDC_DMA_InitStruct.LCDC_DMA_Multi_Block_Mode   =
        LLI_TRANSFER;//LLI_TRANSFER or LLI_WITH_CONTIGUOUS_SAR
    LCDC_DMA_InitStruct.LCDC_DMA_Multi_Block_En     = 1;
    LCDC_DMA_InitStruct.LCDC_DMA_Multi_Block_Struct  = LCDC_DMA_LINKLIST_REG_BASE + 0x50;
    LCDC_DMA_Init(LCDC_DMA_CHANNEL_INDEX, &LCDC_DMA_InitStruct);

    LCDC_SET_GROUP1_BLOCKSIZE(ST7701S_480480_LCD_WIDTH * ST7701S_DRV_PIXEL_BITS / 8);
    LCDC_SET_GROUP2_BLOCKSIZE(ST7701S_480480_LCD_WIDTH * ST7701S_DRV_PIXEL_BITS / 8);

    LCDC_DMALLI_InitTypeDef LCDC_DMA_LLI_Init = {0};
    LCDC_DMA_LLI_Init.g1_source_addr = (uint32_t)internal_frame_buffer1;
    LCDC_DMA_LLI_Init.g1_sar_offset = ST7701S_480480_LCD_WIDTH * ST7701S_DRV_PIXEL_BITS / 8 * 2;

    LCDC_DMA_LLI_Init.g2_source_addr = (uint32_t)((uint32_t)internal_frame_buffer1 +
                                                  ST7701S_480480_LCD_WIDTH * ST7701S_DRV_PIXEL_BITS / 8);
    LCDC_DMA_LLI_Init.g2_sar_offset = ST7701S_480480_LCD_WIDTH * ST7701S_DRV_PIXEL_BITS / 8 * 2;
    LCDC_DMA_Infinite_Buf_Update((uint8_t *)internal_frame_buffer1,
                                 (uint8_t *)internal_frame_buffer1 + ST7701S_480480_LCD_WIDTH *
                                 ST7701S_DRV_PIXEL_BITS / 8);
    LCDC_DMA_LinkList_Init(&LCDC_DMA_LLI_Init,
                           &LCDC_DMA_InitStruct);//LLI_TRANSFER or LLI_WITH_CONTIGUOUS_SAR
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
    LCDC_DMA_InitStruct.LCDC_DMA_SourceAddr          = 0x00500000;
    LCDC_DMA_InitStruct.LCDC_DMA_Multi_Block_En      = 0;
    LCDC_DMA_Init(LCDC_DMA_CHANNEL_INDEX, &LCDC_DMA_InitStruct);
#endif
    LCDC_ClearDmaFifo();
    LCDC_ClearTxPixelCnt();

    LCDC_SwitchMode(LCDC_AUTO_MODE);
    LCDC_SwitchDirect(LCDC_TX_MODE);


    LCDC_SetTxPixelLen(ST7701S_480480_LCD_WIDTH * ST7701S_480480_LCD_WIDTH);

    LCDC_Cmd(ENABLE);


    LCDC_DMA_SetSourceAddress(LCDC_DMA_CHANNEL_INDEX, (uint32_t)internal_frame_buffer1);

#if DMA_MULTIBLOCK
    LCDC_DMA_MultiBlockCmd(ENABLE);
#endif
    LCDC_DMAChannelCmd(LCDC_DMA_CHANNEL_NUM, ENABLE);

    LCDC_DmaCmd(ENABLE);
    LCDC_AutoWriteCmd(ENABLE);
}

void rtk_lcd_hal_init(void)
{
    Pad_Config(LCDC_DATA0, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
    Pad_Config(LCDC_DATA1, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
    Pad_Config(LCDC_DATA2, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
    Pad_Config(LCDC_DATA3, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
    Pad_Config(LCDC_DATA4, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
    Pad_Config(LCDC_DATA5, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
    Pad_Config(LCDC_DATA6, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
    Pad_Config(LCDC_DATA7, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
    Pad_Config(LCDC_DATA8, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
    Pad_Config(LCDC_DATA9, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
    Pad_Config(LCDC_DATA10, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
    Pad_Config(LCDC_DATA11, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
    Pad_Config(LCDC_DATA12, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
    Pad_Config(LCDC_DATA13, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
    Pad_Config(LCDC_DATA14, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
    Pad_Config(LCDC_DATA15, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
    Pad_Config(LCDC_DATA16, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
    Pad_Config(LCDC_DATA17, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
    Pad_Config(LCDC_DATA18, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
    Pad_Config(LCDC_DATA19, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
    Pad_Config(LCDC_DATA20, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
    Pad_Config(LCDC_DATA21, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
    Pad_Config(LCDC_DATA22, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
    Pad_Config(LCDC_DATA23, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);

    Pad_Config(LCDC_RGB_WRCLK, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE,
               PAD_OUT_HIGH);
    Pad_Config(LCDC_HSYNC, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
    Pad_Config(LCDC_VSYNC, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
    Pad_Config(LCDC_CSN_DE, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);

    extern void Pad_Dedicated_Config(uint8_t Pin_Num, FunctionalState Status);
    Pad_Dedicated_Config(LCDC_DATA0, ENABLE);
    Pad_Dedicated_Config(LCDC_DATA1, ENABLE);
    Pad_Dedicated_Config(LCDC_DATA2, ENABLE);
    Pad_Dedicated_Config(LCDC_DATA3, ENABLE);
    Pad_Dedicated_Config(LCDC_DATA4, ENABLE);
    Pad_Dedicated_Config(LCDC_DATA5, ENABLE);
    Pad_Dedicated_Config(LCDC_DATA6, ENABLE);
    Pad_Dedicated_Config(LCDC_DATA7, ENABLE);
    Pad_Dedicated_Config(LCDC_DATA8, ENABLE);
    Pad_Dedicated_Config(LCDC_DATA9, ENABLE);
    Pad_Dedicated_Config(LCDC_DATA10, ENABLE);
    Pad_Dedicated_Config(LCDC_DATA11, ENABLE);
    Pad_Dedicated_Config(LCDC_DATA12, ENABLE);
    Pad_Dedicated_Config(LCDC_DATA13, ENABLE);
    Pad_Dedicated_Config(LCDC_DATA14, ENABLE);
    Pad_Dedicated_Config(LCDC_DATA15, ENABLE);
    Pad_Dedicated_Config(LCDC_DATA16, ENABLE);
    Pad_Dedicated_Config(LCDC_DATA17, ENABLE);
    Pad_Dedicated_Config(LCDC_DATA18, ENABLE);
    Pad_Dedicated_Config(LCDC_DATA19, ENABLE);
    Pad_Dedicated_Config(LCDC_DATA20, ENABLE);
    Pad_Dedicated_Config(LCDC_DATA21, ENABLE);
    Pad_Dedicated_Config(LCDC_DATA22, ENABLE);
    Pad_Dedicated_Config(LCDC_DATA23, ENABLE);

    Pad_Dedicated_Config(LCDC_RGB_WRCLK, ENABLE);
    Pad_Dedicated_Config(LCDC_HSYNC, ENABLE);
    Pad_Dedicated_Config(LCDC_VSYNC, ENABLE);
    Pad_Dedicated_Config(LCDC_CSN_DE, ENABLE);

    st7701s_gpio_init();
    platform_delay_ms(1);
    st7701s_spi_init();

    LCDC_InitTypeDef lcdc_init = {0};
    lcdc_init.LCDC_Interface = LCDC_IF_DPI;
    lcdc_init.LCDC_PixelInputFarmat = LCDC_INPUT_RGB888;
    lcdc_init.LCDC_PixelOutpuFarmat = LCDC_OUTPUT_RGB888;
    lcdc_init.LCDC_PixelBitSwap = LCDC_SWAP_BYPASS; //lcdc_handler_cfg->LCDC_TeEn = LCDC_TE_DISABLE;
    lcdc_init.LCDC_GroupSel = 1;

    lcdc_init.LCDC_DmaThreshold =
        8;    //only support threshold = 8 for DMA MSIZE = 8; the other threshold setting will be support later
    lcdc_init.LCDC_InfiniteModeEn = 1;
    LCDC_Init(&lcdc_init);

    internal_frame_buffer1 = (uint32_t *)SPIC1_ADDR;
    internal_frame_buffer2 = (uint32_t *)(SPIC1_ADDR + ST7701S_480480_LCD_WIDTH *
                                          ST7701S_480480_LCD_WIDTH * ST7701S_DRV_PIXEL_BITS / 8);
    memset(internal_frame_buffer1, 0x88,
           ST7701S_480480_LCD_WIDTH * ST7701S_480480_LCD_WIDTH * ST7701S_DRV_PIXEL_BITS / 8);
    memset(internal_frame_buffer2, 0xFF,
           ST7701S_480480_LCD_WIDTH * ST7701S_480480_LCD_WIDTH * ST7701S_DRV_PIXEL_BITS / 8);

#if !DMA_MULTIBLOCK
    LCDC_MaskINTConfig(LCDC_MASK_TX_AUTO_DONE_INT_MSK, DISABLE);
    LCDC_ClearINTPendingBit(LCDC_CLR_TX_AUTO_DONE_INT_CLR_MSK);
    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = Display_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 3;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);
#endif
//    DBG_DIRECT("LCDC_HANDLER->DMA_FIFO_IMR = 0x%x\n", LCDC_HANDLER->DMA_FIFO_IMR);
//    DBG_DIRECT("LCDC_HANDLER->DMA_FIFO_SR = 0x%x\n", LCDC_HANDLER->DMA_FIFO_SR);
//    DBG_DIRECT("LCDC_HANDLER->DMA_FIFO_ICR = 0x%x\n", LCDC_HANDLER->DMA_FIFO_ICR);

    uint32_t HSA = 8, HFP = 10, HBP = 50, HACT = ST7701S_480480_LCD_WIDTH;
    uint32_t VSA = 8, VFP = 10, VBP = 20, VACT = ST7701S_480480_LCD_HEIGHT;

    LCDC_eDPICfgTypeDef eDPICfg;//480*640  ---->   500 * 660
    eDPICfg.eDPI_ClockDiv = 0x3;

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
    eDPICfg.eDPI_DataEnPolarity = 1;
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
    platform_delay_ms(4);
    st7701s_reset_low();
    platform_delay_ms(30);
    st7701s_reset_high();
    platform_delay_ms(120);
//*******************************/
#include "st7701s_rgb.txt"

    st7701s_dma_init();
}

void rtk_lcd_hal_update_framebuffer(uint8_t *buf, uint32_t len)
{
    if (using_frame_buffer1)
    {
        using_frame_buffer1 = false;
        LCDC_DMA_Infinite_Buf_Update(internal_frame_buffer2,
                                     (uint32_t)internal_frame_buffer2 + ST7701S_480480_LCD_WIDTH * ST7701S_DRV_PIXEL_BITS / 8);
    }
    else
    {
        using_frame_buffer1 = true;
        LCDC_DMA_Infinite_Buf_Update(internal_frame_buffer1,
                                     (uint32_t)internal_frame_buffer1 + ST7701S_480480_LCD_WIDTH * ST7701S_DRV_PIXEL_BITS / 8);
    }
    pfb_probe = 0;
}


void rtk_lcd_hal_start_transfer(uint8_t *buf, uint32_t len)
{
    assert_param((uint32_t)buf % 4 == 0);
    uint8_t *dst = NULL;
    if (using_frame_buffer1)
    {
        dst = (uint8_t *)((uint32_t)internal_frame_buffer2 + pfb_probe * ST7701S_480480_LCD_WIDTH *
                          ST7701S_DRV_PIXEL_BITS / 8);
    }
    else
    {
        dst = (uint8_t *)((uint32_t)internal_frame_buffer1 + pfb_probe * ST7701S_480480_LCD_WIDTH *
                          ST7701S_DRV_PIXEL_BITS / 8);
    }
    RCC_PeriphClockCmd(APBPeriph_GDMA, APBPeriph_GDMA_CLOCK, ENABLE);
    GDMA_InitTypeDef GDMA_InitStruct;
    GDMA_StructInit(&GDMA_InitStruct);
    GDMA_InitStruct.GDMA_ChannelNum = 0;
    GDMA_InitStruct.GDMA_DIR = GDMA_DIR_MemoryToMemory;
    GDMA_InitStruct.GDMA_BufferSize = len * ST7701S_DRV_PIXEL_BITS / 8 / 4;
    GDMA_InitStruct.GDMA_SourceInc = DMA_SourceInc_Inc;
    GDMA_InitStruct.GDMA_DestinationInc = DMA_DestinationInc_Inc;
    GDMA_InitStruct.GDMA_SourceDataSize = GDMA_DataSize_Word;
    GDMA_InitStruct.GDMA_DestinationDataSize = GDMA_DataSize_Word;
    GDMA_InitStruct.GDMA_SourceMsize = GDMA_Msize_16;
    GDMA_InitStruct.GDMA_DestinationMsize = GDMA_Msize_16;
    GDMA_InitStruct.GDMA_SourceAddr = (uint32_t)buf;
    GDMA_InitStruct.GDMA_DestinationAddr = (uint32_t)dst;
    GDMA_Init(TRANSFER_DMA_CHANNEL_INDEX, &GDMA_InitStruct);
    GDMA_INTConfig(TRANSFER_DMA_CHANNEL_NUM, GDMA_INT_Transfer, ENABLE);
    GDMA_Cmd(TRANSFER_DMA_CHANNEL_NUM, ENABLE);
}

void rtk_lcd_hal_transfer_done(void)
{
    while (GDMA_GetTransferINTStatus(TRANSFER_DMA_CHANNEL_NUM) != SET);
    GDMA_ClearINTPendingBit(TRANSFER_DMA_CHANNEL_NUM, GDMA_INT_Transfer);
}

void rtk_lcd_hal_set_window(uint16_t xStart, uint16_t yStart, uint16_t w, uint16_t h)
{
    pfb_probe = yStart;
}

#if !DMA_MULTIBLOCK
void Display_Handler(void)
{
    if (LCDC_GetINTStatus(LCDC_INT_STATUS_TX_AUTO_DONE) == SET)
    {
        LCDC_ClearINTPendingBit(LCDC_CLR_TX_AUTO_DONE_INT_CLR_MSK);
//        DBG_DIRECT("LCDC_IRQHandler line = %d\n",__LINE__);

        LCDC_SwitchMode(LCDC_AUTO_MODE);
        LCDC_SwitchDirect(LCDC_TX_MODE);
        LCDC_DMA_SetSourceAddress(LCDC_DMA_CHANNEL_INDEX, (uint32_t)framebuffer);
        LCDC_DMAChannelCmd(LCDC_DMA_CHANNEL_NUM, ENABLE);
        LCDC_DmaCmd(ENABLE);
        LCDC_AutoWriteCmd(ENABLE);
    }
}
#endif


//*******************************************
void EnterSleep(void)
{
    write_command(0x28);
    platform_delay_ms(10);
    write_command(0x10);
}

//*********************************************************
void ExitSleep(void)
{
    write_command(0x11);
    platform_delay_ms(120);
    write_command(0x29);
}





