#include "icna3311_280x456_qspi.h"
#include "utils.h"
#include "rtl_pinmux.h"
#include "rtl_hal_peripheral.h"
#include "trace.h"

#define LCDC_DMA_CHANNEL_NUM              0
#define LCDC_DMA_CHANNEL_INDEX            LCDC_DMA_Channel0

#define BIT_CMD_CH(x)           (((x) & 0x00000003) << 20)
#define BIT_DATA_CH(x)          (((x) & 0x00000003) << 18)
#define BIT_ADDR_CH(x)          (((x) & 0x00000003) << 16)
#define BIT_TMOD(x)             (((x) & 0x00000003) << 8)

#define BIT_TXSIM               (0x00000001 << 9)
#define BIT_SEQ_EN              (0x00000001 << 3)



static void qspi_write(uint8_t *buf, uint32_t len)
{
    DBIC_Cmd(DISABLE);//disable DBIC
    DBIC->CTRLR0 &= ~(BIT_CMD_CH(3) | BIT_ADDR_CH(3) | BIT_DATA_CH(3));//SET CHANNEL NUM
    DBIC->CTRLR0 &= ~(BIT_TMOD(3)); //tx mode

    DBIC_CmdLength(1);
    DBIC_AddrLength(3);
    DBIC_TX_NDF(len - 4);
    DBIC_Cmd(ENABLE);
    for (uint32_t i = 0; i < len; i++)
    {
        DBIC->DR[0].byte = buf[i];
    }
    while (DBIC->SR & BIT0); // wait bus busy
    DBIC_Cmd(DISABLE);//disable DBIC
}

static void incna3311_cmd(uint8_t cmd) //total 5 byte, first byte is 0x02
{
    uint8_t sdat[] = {0x02, 0x00, cmd, 0x00};
    qspi_write(sdat, sizeof(sdat));
}


static void incna3311_cmd_param1(uint8_t cmd, uint8_t data) //total 5 byte, first byte is 0x02
{
    uint8_t sdat[] = {0x02, 0x00, cmd, 0x00, data};
    qspi_write(sdat, sizeof(sdat));
}

static void incna3311_cmd_param4(uint8_t cmd, uint8_t *data) //total 8 byte, first byte is 0x02
{
    uint8_t sdat[] = {0x02, 0x00, cmd, 0x00, data[0], data[1], data[2], data[3]};
    qspi_write(sdat, sizeof(sdat));
}



static void incna3311_enter_data_output_mode(uint32_t len_byte) //total 4 byte, first byte is 0x32
{
    LCDC_AXIMUXMode(LCDC_FW_MODE);
    DBIC_SwitchMode(DBIC_USER_MODE);
    DBIC_SwitchDirect(DBIC_TMODE_TX);
    DBIC_CmdLength(1);
    DBIC_AddrLength(3);
    DBIC_TX_NDF(len_byte);

    DBIC->CTRLR0 &= ~(BIT_CMD_CH(3) | BIT_ADDR_CH(3) | BIT_DATA_CH(3));//SET CHANNEL NUM
    DBIC->CTRLR0 |= (BIT_CMD_CH(0) | BIT_ADDR_CH(0) | BIT_DATA_CH(2));

    /* must push cmd and address to handler before SPIC enable */
    LCDC_SPICCmd(0x32);
    LCDC_SPICAddr(0x002c00);


    DBIC->DMACR = 2;

    /* change this value can not influence the result. the wave is split into two periods. the first is 32 bytes. */
    DBIC->DMATDLR = 4; /* no any influence. */

    DBIC->FLUSH_FIFO = BIT1;
    LCDC_AXIMUXMode(LCDC_HW_MODE);
}

void rtk_lcd_hal_set_window(uint16_t xStart, uint16_t yStart, uint16_t w, uint16_t h)
{
    uint8_t data[4];
    uint16_t xEnd = xStart + w - 1;
    uint16_t yEnd = yStart + h - 1;

    xStart = xStart + 0x14;
    xEnd = xEnd + 0x14;

    data[0] = xStart >> 8;
    data[1] = xStart & 0xff;
    data[2] = xEnd >> 8;
    data[3] = xEnd & 0xff;
    incna3311_cmd_param4(0x2A, data);


    data[0] = yStart >> 8;
    data[1] = yStart & 0xff;
    data[2] = yEnd >> 8;
    data[3] = yEnd & 0xff;
    incna3311_cmd_param4(0x2B, data);

    uint32_t len_byte = (xEnd - xStart + 1) * (yEnd - yStart + 1) * OUTPUT_PIXEL_BYTES;
    incna3311_enter_data_output_mode(len_byte);
}

uint32_t rtk_lcd_hal_get_width(void)
{
    return ICNA3311_LCD_WIDTH;
}
uint32_t rtk_lcd_hal_get_height(void)
{
    return ICNA3311_LCD_HIGHT;
}

uint32_t rtk_lcd_hal_get_pixel_bits(void)
{
    return ICNA3311_DRV_PIXEL_BITS;
}

void rtk_lcd_hal_start_transfer(uint8_t *buf, uint32_t len)
{

#if (DMA_LINKLIST == 0)
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
    LCDC_DMA_InitStruct.LCDC_DMA_SourceAddr          = (uint32_t)buf;
    LCDC_DMA_InitStruct.LCDC_DMA_Multi_Block_En     = 0;
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

    LCDC_SET_GROUP1_BLOCKSIZE(ICNA3311_LCD_WIDTH * INPUT_PIXEL_BYTES);
    LCDC_SET_GROUP2_BLOCKSIZE(ICNA3311_LCD_WIDTH * INPUT_PIXEL_BYTES);

    /*16 pixel aligned for GPU*/
    LCDC_DMALLI_InitTypeDef LCDC_DMA_LLI_Init = {0};
    LCDC_DMA_LLI_Init.g1_source_addr = (uint32_t)buf;
    LCDC_DMA_LLI_Init.g1_sar_offset = ICNA3311_LCD_WIDTH * INPUT_PIXEL_BYTES * 2;

    LCDC_DMA_LLI_Init.g2_source_addr = (uint32_t)(buf + ICNA3311_LCD_WIDTH * INPUT_PIXEL_BYTES);
    LCDC_DMA_LLI_Init.g2_sar_offset = ICNA3311_LCD_WIDTH * INPUT_PIXEL_BYTES * 2;
    LCDC_DMA_LinkList_Init(&LCDC_DMA_LLI_Init,
                           &LCDC_DMA_InitStruct);//LLI_TRANSFER or LLI_WITH_CONTIGUOUS_SAR
#endif

    LCDC_ClearDmaFifo();
    LCDC_ClearTxPixelCnt();

    LCDC_SwitchMode(LCDC_AUTO_MODE);
    LCDC_SwitchDirect(LCDC_TX_MODE);

    LCDC_SetTxPixelLen(len);

    LCDC_Cmd(ENABLE);
#if DMA_LINKLIST
    LCDC_DMA_MultiBlockCmd(ENABLE);
#endif
    LCDC_DMAChannelCmd(LCDC_DMA_CHANNEL_NUM, ENABLE);
    LCDC_DmaCmd(ENABLE);
#if (TE_VALID == 1)
    LCDC_TeCmd(ENABLE);
#endif
#if (TE_VALID == 0)
    LCDC_AutoWriteCmd(ENABLE);
#endif

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
    LCDC_AXIMUXMode(LCDC_FW_MODE);
    LCDC_Cmd(DISABLE);
}

void rtk_lcd_hal_rect_fill(uint16_t xStart, uint16_t yStart, uint16_t w, uint16_t h, uint32_t color)
{
    LCDC_DMA_InitTypeDef LCDC_DMA_InitStruct = {0};
    LCDC_DMA_StructInit(&LCDC_DMA_InitStruct);
    LCDC_DMA_InitStruct.LCDC_DMA_ChannelNum          = LCDC_DMA_CHANNEL_NUM;
    LCDC_DMA_InitStruct.LCDC_DMA_DIR                 = DMA_DIR_PeripheralToMemory;
    LCDC_DMA_InitStruct.LCDC_DMA_SourceInc           = DMA_SourceInc_Fix;
    LCDC_DMA_InitStruct.LCDC_DMA_DestinationInc      = DMA_DestinationInc_Fix;
    LCDC_DMA_InitStruct.LCDC_DMA_SourceDataSize      = DMA_DataSize_Word;
    LCDC_DMA_InitStruct.LCDC_DMA_DestinationDataSize = DMA_DataSize_Word;
    LCDC_DMA_InitStruct.LCDC_DMA_SourceMsize         = DMA_Msize_1;
    LCDC_DMA_InitStruct.LCDC_DMA_DestinationMsize    = DMA_Msize_8;
    LCDC_DMA_InitStruct.LCDC_DMA_SourceAddr          = (uint32_t)(&color);
    LCDC_DMA_InitStruct.LCDC_DMA_Multi_Block_En     = 0;
    LCDC_DMA_Init(LCDC_DMA_CHANNEL_INDEX, &LCDC_DMA_InitStruct);

    LCDC_ClearDmaFifo();
    LCDC_ClearTxPixelCnt();

    LCDC_SwitchMode(LCDC_AUTO_MODE);
    LCDC_SwitchDirect(LCDC_TX_MODE);

    LCDC_SetTxPixelLen(w * h);

    LCDC_Cmd(ENABLE);

    LCDC_DMAChannelCmd(LCDC_DMA_CHANNEL_NUM, ENABLE);
    LCDC_DmaCmd(ENABLE);

#if (TE_VALID == 1)
    LCDC_TeCmd(ENABLE);
#endif
#if (TE_VALID == 0)
    LCDC_AutoWriteCmd(ENABLE);
#endif
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
#if (TE_VALID == 1)
    LCDC_TeCmd(DISABLE);
#endif

    LCDC_ClearDmaFifo();
    LCDC_ClearTxPixelCnt();
    LCDC_AXIMUXMode(LCDC_FW_MODE);
    LCDC_Cmd(DISABLE);
}

void rtk_lcd_hal_update_framebuffer(uint8_t *buf, uint32_t len)
{
#if (DMA_LINKLIST == 0)
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
    LCDC_DMA_InitStruct.LCDC_DMA_SourceAddr          = (uint32_t)buf;
    LCDC_DMA_InitStruct.LCDC_DMA_Multi_Block_En     = 0;
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

    LCDC_SET_GROUP1_BLOCKSIZE(ICNA3311_LCD_WIDTH * INPUT_PIXEL_BYTES);
    LCDC_SET_GROUP2_BLOCKSIZE(ICNA3311_LCD_WIDTH * INPUT_PIXEL_BYTES);

    /*16 pixel aligned for GPU*/
    LCDC_DMALLI_InitTypeDef LCDC_DMA_LLI_Init = {0};
    LCDC_DMA_LLI_Init.g1_source_addr = (uint32_t)buf;
    LCDC_DMA_LLI_Init.g1_sar_offset = ICNA3311_LCD_WIDTH * INPUT_PIXEL_BYTES * 2;

    LCDC_DMA_LLI_Init.g2_source_addr = (uint32_t)(buf + ICNA3311_LCD_WIDTH * INPUT_PIXEL_BYTES);
    LCDC_DMA_LLI_Init.g2_sar_offset = ICNA3311_LCD_WIDTH * INPUT_PIXEL_BYTES * 2;
    LCDC_DMA_LinkList_Init(&LCDC_DMA_LLI_Init,
                           &LCDC_DMA_InitStruct);//LLI_TRANSFER or LLI_WITH_CONTIGUOUS_SAR
#endif

    LCDC_ClearDmaFifo();
    LCDC_ClearTxPixelCnt();

    LCDC_SwitchMode(LCDC_AUTO_MODE);
    LCDC_SwitchDirect(LCDC_TX_MODE);

    LCDC_SetTxPixelLen(len);

    LCDC_Cmd(ENABLE);
#if DMA_LINKLIST
    LCDC_DMA_MultiBlockCmd(ENABLE);
#endif
    LCDC_DMAChannelCmd(LCDC_DMA_CHANNEL_NUM, ENABLE);
    LCDC_DmaCmd(ENABLE);
#if (TE_VALID == 1)
    LCDC_TeCmd(ENABLE);
#endif
#if (TE_VALID == 0)
    LCDC_AutoWriteCmd(ENABLE);
#endif

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
#if (TE_VALID == 1)
    LCDC_TeCmd(DISABLE);
#endif

    LCDC_ClearDmaFifo();
    LCDC_ClearTxPixelCnt();
    LCDC_AXIMUXMode(LCDC_FW_MODE);
    LCDC_Cmd(DISABLE);
}

static void incna3311_pad_config(void)
{
    Pad_Config(LCD_QSPI_RST, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
    Pad_Config(LCD_QSPI_D0, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
    Pad_Config(LCD_QSPI_D1, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
    Pad_Config(LCD_QSPI_D2, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
    Pad_Config(LCD_QSPI_D3, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
    Pad_Config(LCD_QSPI_CS, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
    Pad_Config(LCD_QSPI_CLK, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
    Pad_Config(LCD_QSPI_TE, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);

    Pad_Dedicated_Config(LCD_QSPI_RST, ENABLE);
    Pad_Dedicated_Config(LCD_QSPI_D0, ENABLE);
    Pad_Dedicated_Config(LCD_QSPI_D1, ENABLE);
    Pad_Dedicated_Config(LCD_QSPI_D2, ENABLE);
    Pad_Dedicated_Config(LCD_QSPI_D3, ENABLE);
    Pad_Dedicated_Config(LCD_QSPI_CS, ENABLE);
    Pad_Dedicated_Config(LCD_QSPI_CLK, ENABLE);
    Pad_Dedicated_Config(LCD_QSPI_TE, ENABLE);

}
static void incna3311_chip_reset(void)
{
    //no need HW do it
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

void rtk_lcd_hal_init(void)
{
    /* config LCD dev info */
    RCC_PeriphClockCmd(APBPeriph_DISP, APBPeriph_DISP_CLOCK, ENABLE);

    //from XTAL SOURCE = 40M
    PERIBLKCTRL_PERI_CLK->u_324.BITS_324.disp_ck_en = 1;
    PERIBLKCTRL_PERI_CLK->u_324.BITS_324.disp_func_en = 1;
    PERIBLKCTRL_PERI_CLK->u_324.BITS_324.r_disp_mux_clk_cg_en = 1;

    //From PLL1, SOURCE = 125M
    PERIBLKCTRL_PERI_CLK->u_324.BITS_324.r_disp_div_en = 1;
    PERIBLKCTRL_PERI_CLK->u_324.BITS_324.r_disp_clk_src_sel0 = 0; //pll1_peri(0) or pll2(1, pll2 = 160M)
    PERIBLKCTRL_PERI_CLK->u_324.BITS_324.r_disp_clk_src_sel1 = 1; //pll(1) or xtal(0)
    PERIBLKCTRL_PERI_CLK->u_324.BITS_324.r_disp_div_sel = 1; //div

    incna3311_pad_config();
    incna3311_chip_reset();
    LCDC_InitTypeDef lcdc_init = {0};
    lcdc_init.LCDC_Interface = LCDC_IF_DBIC;
    lcdc_init.LCDC_GroupSel = 1; //QFN88 2 - QFN68 1
    lcdc_init.LCDC_PixelInputFarmat = LCDC_INPUT_RGB565;
    lcdc_init.LCDC_PixelOutpuFarmat = LCDC_OUTPUT_RGB565;
    lcdc_init.LCDC_PixelBitSwap = LCDC_SWAP_BYPASS; //lcdc_handler_cfg->LCDC_TeEn = LCDC_TE_DISABLE;
#if TE_VALID
    lcdc_init.LCDC_TeEn = ENABLE;
    lcdc_init.LCDC_TePolarity = LCDC_TE_EDGE_FALLING;
    lcdc_init.LCDC_TeInputMux = LCDC_TE_LCD_INPUT;
#endif
    lcdc_init.LCDC_DmaThreshold =
        8;    //only support threshold = 8 for DMA MSIZE = 8; the other threshold setting will be support later
    LCDC_Init(&lcdc_init);

    DBG_DIRECT("func = %s, line = %d", __func__, __LINE__);


    LCDC_DBICCfgTypeDef dbic_init = {0};
    dbic_init.DBIC_SPEED_SEL         = 1;
    dbic_init.DBIC_TxThr             = 0;//0 or 4
    dbic_init.DBIC_RxThr             = 0;
    dbic_init.SCPOL                  = DBIC_SCPOL_LOW;
    dbic_init.SCPH                   = DBIC_SCPH_1Edge;
    DBIC_Init(&dbic_init);

    LCDC_SwitchMode(LCDC_MANUAL_MODE);
    LCDC_SwitchDirect(LCDC_TX_MODE);
    LCDC_Cmd(ENABLE);

    LCDC_LCD_SET_RST(false);
    platform_delay_ms(100);
    LCDC_LCD_SET_RST(true);
    platform_delay_ms(120);
    LCDC_LCD_SET_RST(false);
    platform_delay_ms(50);

    DBG_DIRECT("func = %s, line = %d, DBIC->FLUSH_FIFO = 0x%x", __func__, __LINE__, &DBIC->FLUSH_FIFO);

    LCDC_AXIMUXMode(LCDC_FW_MODE);
    DBIC_SwitchMode(DBIC_USER_MODE);
    DBIC_SwitchDirect(DBIC_TMODE_TX);

    incna3311_cmd_param1(0xFE, 0x20);           /*switch command set*/
    incna3311_cmd_param1(0xF4, 0x5A);           /*password command description*/
    incna3311_cmd_param1(0xF5, 0x59);           /*password command description*/

    incna3311_cmd_param1(0xFE, 0x70);
    incna3311_cmd_param1(0x66, 0x47);           /*high brightness mode*/

    incna3311_cmd_param1(0xFE, 0x80);
    incna3311_cmd_param1(0x67, 0xA6);

    incna3311_cmd_param1(0xFE, 0x00);
    incna3311_cmd_param1(0xC4, 0x80);           /*set dual SPI mode*/
    incna3311_cmd_param1(0x35, 0x00);           /*set tear on*/
    incna3311_cmd_param1(0x51, 0xFF);           /*write display brightness*/
    incna3311_cmd_param1(0x53, 0x20);           /*write displya ctrl ,dimming*/
    incna3311_cmd_param1(0x63, 0xFF);           /*write HBM display brightness*/

    //  incna3311_cmd_param1(0x3A,0x77);            /*RGB888*/
    incna3311_cmd_param1(0x3A, 0x55);           /*RGB565*/

    uint8_t horizontal_position[] = {0x00, 0x14, 0x01, 0x2B};
    incna3311_cmd_param4(0x2A, horizontal_position);
    uint8_t vertical_position[] = {0x00, 0x00, 0x01, 0xC7};
    incna3311_cmd_param4(0x2B, vertical_position);

    incna3311_cmd(0x11);             /*sleep out*/
    platform_delay_ms(120);

    incna3311_cmd(0x29);             /*power on*/


    rtk_lcd_hal_set_window(0, 0, 280, 456);
    rtk_lcd_hal_rect_fill(0, 0, 280, 456, 0x0000ff00);
}

