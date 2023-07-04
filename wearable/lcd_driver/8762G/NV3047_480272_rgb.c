#include "NV3047_480272_rgb.h"
#include "rtl_lcdc_edpi.h"
#include "rtl_nvic.h"
#include "rtl_pinmux.h"
#include "rtl_gpio.h"
#include "rtl_rcc.h"
#include "utils.h"
#include "mem_config.h"
#include "string.h"
#include "rtl_gdma.h"



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

#define LCDC_RESET          P2_2


#define LCDC_DMA_CHANNEL_NUM              0
#define LCDC_DMA_CHANNEL_INDEX            LCDC_DMA_Channel0

#define TRANSFER_DMA_CHANNEL_NUM              0
#define TRANSFER_DMA_CHANNEL_INDEX            GDMA_Channel0


static void nv3047_reset_high(void)
{
    GPIO_WriteBit(GPIO_GetPort(LCDC_RESET), GPIO_GetPin(LCDC_RESET), 1);
}

static void nv3047_reset_low(void)
{
    GPIO_WriteBit(GPIO_GetPort(LCDC_RESET), GPIO_GetPin(LCDC_RESET), 0);
}

//--------------------initial code-----------------------------------------//
static void nv3047_gpio_init(void)
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
    nv3047_reset_high();
}


static void nv3047_dma_init(uint8_t *init_buffer)
{
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

    LCDC_SET_GROUP1_BLOCKSIZE(NV3047_480272_LCD_WIDTH * NV3047_DRV_PIXEL_BITS / 8);
    LCDC_SET_GROUP2_BLOCKSIZE(NV3047_480272_LCD_WIDTH * NV3047_DRV_PIXEL_BITS / 8);

    LCDC_DMALLI_InitTypeDef LCDC_DMA_LLI_Init = {0};

    LCDC_DMA_LLI_Init.g1_source_addr = (uint32_t)init_buffer;
    LCDC_DMA_LLI_Init.g2_source_addr = (uint32_t)((uint32_t)init_buffer + NV3047_480272_LCD_WIDTH *
                                                  NV3047_DRV_PIXEL_BITS / 8);
    LCDC_DMA_LLI_Init.g1_sar_offset = NV3047_480272_LCD_WIDTH * NV3047_DRV_PIXEL_BITS / 8 * 2;
    LCDC_DMA_LLI_Init.g2_sar_offset = NV3047_480272_LCD_WIDTH * NV3047_DRV_PIXEL_BITS / 8 * 2;

    LCDC_DMA_Infinite_Buf_Update((uint8_t *)init_buffer,
                                 (uint8_t *)init_buffer + NV3047_480272_LCD_WIDTH *
                                 NV3047_DRV_PIXEL_BITS / 8);
    LCDC_DMA_LinkList_Init(&LCDC_DMA_LLI_Init,
                           &LCDC_DMA_InitStruct);//LLI_TRANSFER or LLI_WITH_CONTIGUOUS_SAR

    LCDC_ClearDmaFifo();
    LCDC_ClearTxPixelCnt();

    LCDC_SwitchMode(LCDC_AUTO_MODE);
    LCDC_SwitchDirect(LCDC_TX_MODE);


    LCDC_SetTxPixelLen(NV3047_480272_LCD_WIDTH * NV3047_480272_LCD_HEIGHT);

    LCDC_Cmd(ENABLE);


    LCDC_DMA_SetSourceAddress(LCDC_DMA_CHANNEL_INDEX, (uint32_t)init_buffer);

    LCDC_DMA_MultiBlockCmd(ENABLE);

    LCDC_DMAChannelCmd(LCDC_DMA_CHANNEL_NUM, ENABLE);

    LCDC_DmaCmd(ENABLE);
    LCDC_AutoWriteCmd(ENABLE);
}


static void lcd_pad_and_clk_init(void)
{
    RCC_PeriphClockCmd(APBPeriph_DISP, APBPeriph_DISP_CLOCK, ENABLE);

    //from XTAL SOURCE = 40M
    PERIBLKCTRL_PERI_CLK->u_324.BITS_324.disp_ck_en = 1;
    PERIBLKCTRL_PERI_CLK->u_324.BITS_324.disp_func_en = 1;
    PERIBLKCTRL_PERI_CLK->u_324.BITS_324.r_disp_mux_clk_cg_en = 1;

    //From PLL1, SOURCE = 100M
    PERIBLKCTRL_PERI_CLK->u_324.BITS_324.r_disp_div_en = 1;
    PERIBLKCTRL_PERI_CLK->u_324.BITS_324.r_disp_clk_src_sel0 = 0; //pll1_peri(0) or pll2(1, pll2 = 160M)
    PERIBLKCTRL_PERI_CLK->u_324.BITS_324.r_disp_clk_src_sel1 = 1; //pll(1) or xtal(0)
    PERIBLKCTRL_PERI_CLK->u_324.BITS_324.r_disp_div_sel = 0; //div

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

    //Pad_Config(P2_3, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_DOWN, PAD_OUT_ENABLE, PAD_OUT_LOW);//HV
    //Pad_Config(P2_3, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);//DE

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

    nv3047_gpio_init();
    platform_delay_ms(1);
}

void rtk_lcd_hal_init(void)
{

    lcd_pad_and_clk_init();

    LCDC_InitTypeDef lcdc_init = {0};
    lcdc_init.LCDC_Interface = LCDC_IF_DPI;
    lcdc_init.LCDC_PixelInputFarmat = LCDC_INPUT_RGB565;
    lcdc_init.LCDC_PixelOutpuFarmat = LCDC_OUTPUT_RGB888;
    lcdc_init.LCDC_PixelBitSwap = LCDC_SWAP_BYPASS; //lcdc_handler_cfg->LCDC_TeEn = LCDC_TE_DISABLE;
    lcdc_init.LCDC_GroupSel = 1;

    lcdc_init.LCDC_DmaThreshold =
        8;    //only support threshold = 8 for DMA MSIZE = 8; the other threshold setting will be support later
    lcdc_init.LCDC_InfiniteModeEn = 1;
    LCDC_Init(&lcdc_init);


    uint32_t HSA = 0, HFP = 8, HBP = 43, HACT = NV3047_480272_LCD_WIDTH;
    uint32_t VSA = 0, VFP = 8, VBP = 12, VACT = NV3047_480272_LCD_HEIGHT;


    LCDC_eDPICfgTypeDef eDPICfg;//480*640  ---->   500 * 660
    eDPICfg.eDPI_ClockDiv = 11;

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
    eDPICfg.eDPI_ColorMap = EDPI_PIXELFORMAT_RGB888;//for RGB888
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
    eDPICfg.eDPI_LineBufferPixelThreshold = eDPICfg.eDPI_TotalWidth / 2;

    EDPI_Init(&eDPICfg);

//    EDPI_RGB_COMPATIBLE_t edpi_reg_0x58 = {.d32 = EDPI->EDPI_RGB_COMPATIBLE};
//    edpi_reg_0x58.b.hsync_constant = EDPI_HSYNC_CONSTANT_HIGH;
//    edpi_reg_0x58.b.vsync_constant = EDPI_VSYNC_CONSTANT_HIGH;
//    edpi_reg_0x58.b.de_constant = EDPI_DE_CONSTANT_NONE;
//    EDPI->EDPI_RGB_COMPATIBLE = edpi_reg_0x58.d32;

//    EDPI_RGB_COMPATIBLE_t edpi_reg_0x58 = {.d32 = EDPI->EDPI_RGB_COMPATIBLE};
//    edpi_reg_0x58.b.hsync_constant = EDPI_HSYNC_CONSTANT_NONE;
//    edpi_reg_0x58.b.vsync_constant = EDPI_VSYNC_CONSTANT_NONE;
//    edpi_reg_0x58.b.de_constant = EDPI_DE_CONSTANT_LOW;
//    EDPI->EDPI_RGB_COMPATIBLE = edpi_reg_0x58.d32;

    nv3047_reset_high();
    platform_delay_ms(200);
    nv3047_reset_low();
    platform_delay_ms(200);
    nv3047_reset_high();
    platform_delay_ms(200);
    //*******************************/

    uint8_t *pixel = (uint8_t *)SPIC1_ADDR;

    for (uint32_t i = 0; i < NV3047_480272_LCD_WIDTH * NV3047_480272_LCD_HEIGHT * 3; i = i + 3)
    {
        pixel[i] = 0xFF;
        pixel[i + 1] = 0;
        pixel[i + 2] = 0;
    }

    rtk_lcd_hal_update_framebuffer((uint8_t *)SPIC1_ADDR,
                                   NV3047_480272_LCD_WIDTH * NV3047_480272_LCD_HEIGHT);
}



static bool flush_first = true;

void rtk_lcd_hal_update_framebuffer(uint8_t *buf, uint32_t len)
{
    if (flush_first == true)
    {
        nv3047_dma_init(buf);
        flush_first = false;
    }
    else
    {
        LCDC_DMA_Infinite_Buf_Update(buf,
                                     (uint32_t)buf + NV3047_480272_LCD_WIDTH * NV3047_DRV_PIXEL_BITS / 8);
    }
}


void rtk_lcd_hal_start_transfer(uint8_t *buf, uint32_t len)
{
    return;//todo by howie
}

void rtk_lcd_hal_transfer_done(void)
{
    return;//todo by howie
}

void rtk_lcd_hal_set_window(uint16_t xStart, uint16_t yStart, uint16_t w, uint16_t h)
{
    return;//todo by howie
}

uint32_t rtk_lcd_hal_get_width(void)
{
    return NV3047_480272_LCD_WIDTH;
}

uint32_t rtk_lcd_hal_get_height(void)
{
    return NV3047_480272_LCD_HEIGHT;
}

uint32_t rtk_lcd_hal_get_pixel_bits(void)
{
    return NV3047_DRV_PIXEL_BITS;
}
uint32_t rtk_lcd_hal_power_off(void)
{
    return 0;
}
uint32_t rtk_lcd_hal_power_on(void)
{
    return 0;
}
uint32_t rtk_lcd_hal_dlps_restore(void)
{
    return 0;
}





