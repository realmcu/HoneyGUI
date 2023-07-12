#include "rtl_hal_peripheral.h"
#include "st7796_320320_dbib.h"
#include "drv_gpio.h"

#define LCDC_DMA_CHANNEL_NUM              0
#define LCDC_DMA_CHANNEL_INDEX            LCDC_DMA_Channel0

#define DMA_MULTIBLOCK 0


void st7796_write_cmd(uint8_t command)
{
    DBIB_SetCS();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    DBIB_ResetCS();

    DBIB_SendCmd(command);
}



void st7796_write_data(uint8_t data)
{
    uint8_t data_buf[1];
    data_buf[0] = data;
    DBIB_SendData(data_buf, 1);
}

void lcd_lcd7796_power_on(void)
{

}

void lcd_lcd7796_power_off(void)
{
    st7796_write_cmd(0x28);
    st7796_write_cmd(0x10);
    //lcd_set_backlight(0);
}

static void lcd7796_seq_init(void)
{
    st7796_write_cmd(0x11);
    platform_delay_ms(120);
    st7796_write_cmd(0x36);
    st7796_write_data(0x48);

    st7796_write_cmd(0x35);//Tearing Effect Line On
    st7796_write_data(0x00);

    st7796_write_cmd(0x3A);
    st7796_write_data(0x05);

    st7796_write_cmd(0xF0);
    st7796_write_data(0xC3);

    st7796_write_cmd(0xF0);
    st7796_write_data(0x96);

    st7796_write_cmd(0xB4);
    st7796_write_data(0x01);
#if 1
    st7796_write_cmd(0xB1);     //refs page 217
    st7796_write_data(0x20);    //cur frame rate 40hz
    st7796_write_data(0x10);

    st7796_write_cmd(0xB5);
    st7796_write_data(0x2D);
    st7796_write_data(0x2D);
    st7796_write_data(0x00);
    st7796_write_data(0x04);

    st7796_write_cmd(0xB6);
    st7796_write_data(0x8A);
    st7796_write_data(0x07);
    st7796_write_data(0x27);
#else
    st7796_write_cmd(0xB1);
    st7796_write_data(0x20);
    st7796_write_data(0x10);

    st7796_write_cmd(0xB5);
    st7796_write_data(0x2D);
    st7796_write_data(0x2D);
    st7796_write_data(0x00);
    st7796_write_data(0x04);

    st7796_write_cmd(0xB6);
    st7796_write_data(0x8A);
    st7796_write_data(0x07);
    st7796_write_data(0x27);
#endif
    st7796_write_cmd(0xB7);
    st7796_write_data(0xC6);

    st7796_write_cmd(0xB9);
    st7796_write_data(0x02);
    st7796_write_data(0xE0);

    st7796_write_cmd(0xC0);
    st7796_write_data(0x80);
    st7796_write_data(0x25);

    st7796_write_cmd(0xC1);
    st7796_write_data(0x13);

    st7796_write_cmd(0xC5);
    st7796_write_data(0x1A);

    st7796_write_cmd(0xE8);
    st7796_write_data(0x40);
    st7796_write_data(0x8A);
    st7796_write_data(0x00);
    st7796_write_data(0x00);
    st7796_write_data(0x29);
    st7796_write_data(0x19);
    st7796_write_data(0xA5);
    st7796_write_data(0x33);

    st7796_write_cmd(0xE0);
    st7796_write_data(0xF0);
    st7796_write_data(0x04);
    st7796_write_data(0x08);
    st7796_write_data(0x09);
    st7796_write_data(0x08);
    st7796_write_data(0x25);
    st7796_write_data(0x2A);
    st7796_write_data(0x44);
    st7796_write_data(0x42);
    st7796_write_data(0x3C);
    st7796_write_data(0x17);
    st7796_write_data(0x17);
    st7796_write_data(0x29);
    st7796_write_data(0x2E);

    st7796_write_cmd(0xE1);
    st7796_write_data(0xF0);
    st7796_write_data(0x01);
    st7796_write_data(0x05);
    st7796_write_data(0x06);
    st7796_write_data(0x06);
    st7796_write_data(0x03);
    st7796_write_data(0x29);
    st7796_write_data(0x43);
    st7796_write_data(0x41);
    st7796_write_data(0x27);
    st7796_write_data(0x13);
    st7796_write_data(0x13);
    st7796_write_data(0x26);
    st7796_write_data(0x2C);

    st7796_write_cmd(0xF0);
    st7796_write_data(0x3C);

    st7796_write_cmd(0xF0);
    st7796_write_data(0x69);

    st7796_write_cmd(0x21);

    st7796_write_cmd(0x11);
    st7796_write_cmd(0x29);
}


void lcd_lcd7796_clear(uint8_t *buf, uint32_t len)
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
    LCDC_DMA_InitStruct.LCDC_DMA_SourceAddr          = 0x00500000;
    LCDC_DMA_InitStruct.LCDC_DMA_Multi_Block_En      = 0;
    LCDC_DMA_Init(LCDC_DMA_CHANNEL_INDEX, &LCDC_DMA_InitStruct);

    LCDC_ClearDmaFifo();
    LCDC_ClearTxPixelCnt();

    LCDC_SwitchMode(LCDC_AUTO_MODE);
    LCDC_SwitchDirect(LCDC_TX_MODE);
    DBIB_BypassCmdByteCmd(DISABLE);

    uint8_t cmd[1] = {0x2C};
    LCDC_DBIB_SetCmdSequence(cmd, 1);

    LCDC_SetTxPixelLen(len);

    LCDC_Cmd(ENABLE);


    LCDC_DMA_SetSourceAddress(LCDC_DMA_CHANNEL_INDEX, (uint32_t)buf);
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

void lcd7796_read(uint16_t command, uint8_t *pBuf, uint32_t len)
{
    LCDC_Cmd(DISABLE);
    LCDC_SwitchMode(LCDC_MANUAL_MODE);
    LCDC_SwitchDirect(LCDC_TX_MODE);
    LCDC_Cmd(ENABLE);
    st7796_write_cmd(command);

    LCDC_Cmd(DISABLE);
    LCDC_SwitchMode(LCDC_MANUAL_MODE);
    LCDC_SwitchDirect(LCDC_RX_MODE);
    LCDC_Cmd(ENABLE);
    /* Read data */
    DBIB_ReceiveData(pBuf, len);

    DBIB_SetCS();
}

void rtk_lcd_hal_set_window(uint16_t xStart, uint16_t yStart, uint16_t w, uint16_t h)
{
    LCDC_Cmd(DISABLE);
    LCDC_SwitchMode(LCDC_MANUAL_MODE);
    LCDC_SwitchDirect(LCDC_TX_MODE);
    LCDC_Cmd(ENABLE);
    uint16_t xEnd = xStart + w - 1;
    uint16_t yEnd = yStart + h - 1;

    st7796_write_cmd(0x2a);
    st7796_write_data(xStart >> 8);
    st7796_write_data(xStart & 0xff);
    st7796_write_data(xEnd >> 8);
    st7796_write_data(xEnd & 0xff);

    st7796_write_cmd(0x2b);
    st7796_write_data(yStart >> 8);
    st7796_write_data(yStart & 0xff);
    st7796_write_data(yEnd >> 8);
    st7796_write_data(yEnd & 0xff);

    DBIB_SetCS();
}

void rtk_lcd_hal_start_transfer(uint8_t *buf, uint32_t len)
{
    if (((uint32_t)buf % 4) != 0)
    {
        while (1);
    }

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
    DBIB_BypassCmdByteCmd(DISABLE);

    uint8_t cmd[1] = {0x2C};
    LCDC_DBIB_SetCmdSequence(cmd, 1);

    LCDC_SetTxPixelLen(len);

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


void rtk_lcd_hal_update_framebuffer(uint8_t *buf, uint32_t len)
{
    if (((uint32_t)buf % 4) != 0)
    {
        while (1);
    }

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
    DBIB_BypassCmdByteCmd(DISABLE);

    uint8_t cmd[1] = {0x2C};
    LCDC_DBIB_SetCmdSequence(cmd, 1);

    LCDC_SetTxPixelLen(len);

    LCDC_Cmd(ENABLE);


    LCDC_DMA_SetSourceAddress(LCDC_DMA_CHANNEL_INDEX, (uint32_t)buf);

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


static void st7796_pad_config(void)
{
    Pad_Config(LCD_8080_D0, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
    Pad_Config(LCD_8080_D1, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
    Pad_Config(LCD_8080_D2, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
    Pad_Config(LCD_8080_D3, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
    Pad_Config(LCD_8080_D4, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
    Pad_Config(LCD_8080_D5, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
    Pad_Config(LCD_8080_D6, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
    Pad_Config(LCD_8080_D7, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
    Pad_Config(LCD_8080_CS, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
    Pad_Config(LCD_8080_DCX, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
    Pad_Config(LCD_8080_RD, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
    Pad_Config(LCD_8080_WR, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
    // Pad_Config(LCD_TE_SYNC, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);

    Pad_Dedicated_Config(LCD_8080_D0, ENABLE);
    Pad_Dedicated_Config(LCD_8080_D1, ENABLE);
    Pad_Dedicated_Config(LCD_8080_D2, ENABLE);
    Pad_Dedicated_Config(LCD_8080_D3, ENABLE);
    Pad_Dedicated_Config(LCD_8080_D4, ENABLE);
    Pad_Dedicated_Config(LCD_8080_D5, ENABLE);
    Pad_Dedicated_Config(LCD_8080_D6, ENABLE);
    Pad_Dedicated_Config(LCD_8080_D7, ENABLE);
    Pad_Dedicated_Config(LCD_8080_CS, ENABLE);
    Pad_Dedicated_Config(LCD_8080_DCX, ENABLE);
    Pad_Dedicated_Config(LCD_8080_RD, ENABLE);
    Pad_Dedicated_Config(LCD_8080_WR, ENABLE);
    // Pad_Dedicated_Config(LCD_TE_SYNC, ENABLE);

}
static void st7796_chip_reset(void)
{
    //no need HW do it
    drv_pin_mode(LCD_8080_RST, PIN_MODE_OUTPUT);
    drv_pin_write(LCD_8080_RST, 1);
    platform_delay_ms(100);
    drv_pin_write(LCD_8080_RST, 0);
    platform_delay_ms(50);
    drv_pin_write(LCD_8080_RST, 1);
    platform_delay_ms(50);

    drv_pin_mode(LCD_8080_BL, PIN_MODE_OUTPUT);
    drv_pin_write(LCD_8080_BL, 1);
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

    st7796_pad_config();
    st7796_chip_reset();
    LCDC_InitTypeDef lcdc_init = {0};
    lcdc_init.LCDC_Interface = LCDC_IF_DBIB;
    lcdc_init.LCDC_PixelInputFarmat = LCDC_INPUT_RGB565;
    lcdc_init.LCDC_PixelOutpuFarmat = LCDC_OUTPUT_RGB565;
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
    LCDC_SwitchMode(LCDC_MANUAL_MODE);
    LCDC_SwitchDirect(LCDC_TX_MODE);
    LCDC_Cmd(ENABLE);

    LCDC_LCD_SET_RST(false);
    platform_delay_ms(100);
    LCDC_LCD_SET_RST(true);
    platform_delay_ms(50);
    LCDC_LCD_SET_RST(false);
    platform_delay_ms(50);

    lcd7796_seq_init();

    uint8_t color[4] = {0x00, 0xFF, 0x00, 0X00};//B G R A
    lcd_lcd7796_clear(color, ST7796_LCD_WIDTH * ST7796_LCD_HIGHT);

}

//MSH_CMD_EXPORT(lcdc_dbib_lcd7796_480_800, lcd display test);
