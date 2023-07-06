#include "board.h"
#include "lcd_st7796_320.h"
#include "app_section.h"
#include "flash_device.h"
#include "rtl_hal_peripheral.h"
#include "trace.h"



GDMA_LLIDef *GDMA_LLIStruct = NULL;

DATA_RAM_FUNCTION
static void st7796_write_cmd(uint8_t command)
{
    IF8080_SetCS();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    IF8080_ResetCS();
    IF8080_SendCommand(command);
}

DATA_RAM_FUNCTION
static void st7796_write_data(uint8_t data)
{
    IF8080_SendData(&data, 1);
}

static void lcd_set_reset(bool reset)
{
    if (reset)
    {
        Pad_Config(LCD_8080_RST, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_DOWN, PAD_OUT_ENABLE, PAD_OUT_LOW);
    }
    else
    {
        Pad_Config(LCD_8080_RST, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);
    }
}



static void lcd_pad_init(void)
{
    Pad_Config(LCD_8080_D0, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(LCD_8080_D1, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(LCD_8080_D2, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(LCD_8080_D3, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(LCD_8080_D4, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(LCD_8080_D5, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(LCD_8080_D6, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(LCD_8080_D7, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);

    /* CS */
    Pad_Config(LCD_8080_CS, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    /* DCX */
    Pad_Config(LCD_8080_DCX, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    /* RD */
    Pad_Config(LCD_8080_RD, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    /* WR */
    Pad_Config(LCD_8080_WR, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);

    /* 8080 interface: D0~D7 */
    Pinmux_Config(LCD_8080_D0, IDLE_MODE);
    Pinmux_Config(LCD_8080_D1, IDLE_MODE);
    Pinmux_Config(LCD_8080_D2, IDLE_MODE);
    Pinmux_Config(LCD_8080_D3, IDLE_MODE);
    Pinmux_Config(LCD_8080_D4, IDLE_MODE);
    Pinmux_Config(LCD_8080_D5, IDLE_MODE);
    Pinmux_Config(LCD_8080_D6, IDLE_MODE);
    Pinmux_Config(LCD_8080_D7, IDLE_MODE);

    /* CS */
    Pinmux_Config(LCD_8080_CS, IDLE_MODE);
    /* DCX */
    Pinmux_Config(LCD_8080_DCX, IDLE_MODE);
    /* RD */
    Pinmux_Config(LCD_8080_RD, IDLE_MODE);
    /* WR */
    Pinmux_Config(LCD_8080_WR, IDLE_MODE);

    /*BL AND RESET ARE NOT FIX*/
    Pad_Config(LCD_8080_BL, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);

    // Pinmux_Config(LCD_8080_BL, timer_pwm4);

}
uint32_t rtk_lcd_hal_power_off(void)
{
    Pad_Config(LCD_8080_D0, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(LCD_8080_D1, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(LCD_8080_D2, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(LCD_8080_D3, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(LCD_8080_D4, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(LCD_8080_D5, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(LCD_8080_D6, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(LCD_8080_D7, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);

    /* CS */
    Pad_Config(LCD_8080_CS, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
    /* DCX */
    Pad_Config(LCD_8080_DCX, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
    /* RD */
    Pad_Config(LCD_8080_RD, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
    /* WR */
    Pad_Config(LCD_8080_WR, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);

    Pad_Config(LCD_8080_BL, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_DOWN, PAD_OUT_ENABLE, PAD_OUT_LOW);
    return 0;
}
uint32_t rtk_lcd_hal_power_on(void)
{
    Pad_Config(LCD_8080_D0, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(LCD_8080_D1, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(LCD_8080_D2, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(LCD_8080_D3, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(LCD_8080_D4, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(LCD_8080_D5, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(LCD_8080_D6, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(LCD_8080_D7, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);

    /* CS */
    Pad_Config(LCD_8080_CS, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    /* DCX */
    Pad_Config(LCD_8080_DCX, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    /* RD */
    Pad_Config(LCD_8080_RD, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    /* WR */
    Pad_Config(LCD_8080_WR, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);

    Pad_Config(LCD_8080_BL, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);
    return 0;
}


/**
  * @brief  writband lcd device init set IO config here
  * @param  None
  * @retval None
  */
void lcd_device_init(void)
{
    RCC_PeriphClockCmd(APBPeriph_IF8080, APBPeriph_IF8080_CLOCK, DISABLE);
    RCC_PeriphClockCmd(APBPeriph_IF8080, APBPeriph_IF8080_CLOCK, ENABLE);
#if IF8080_PINGROUP_1
    IF8080_PinGroupConfig(IF8080_PinGroup_1);
#else
    IF8080_PinGroupConfig(IF8080_PinGroup_2);
#endif
    IF8080_InitTypeDef IF8080_InitStruct;
    IF8080_StructInit(&IF8080_InitStruct);
    extern uint32_t SystemCpuClock;
    if (SystemCpuClock == 100000000)
    {
        IF8080_InitStruct.IF8080_ClockDiv          = IF8080_CLOCK_DIV_5;
    }
    else if (SystemCpuClock == 90000000)
    {
        IF8080_InitStruct.IF8080_ClockDiv          = IF8080_CLOCK_DIV_5;
    }
    else if (SystemCpuClock == 8000000)
    {
        IF8080_InitStruct.IF8080_ClockDiv          = IF8080_CLOCK_DIV_4;
    }
    else if (SystemCpuClock == 40000000)
    {
        IF8080_InitStruct.IF8080_ClockDiv          = IF8080_CLOCK_DIV_2;
    }
    IF8080_InitStruct.IF8080_Mode              = IF8080_MODE_MANUAL;
    IF8080_InitStruct.IF8080_AutoModeDirection = IF8080_Auto_Mode_Direction_WRITE;
    IF8080_InitStruct.IF8080_GuardTimeCmd      = IF8080_GUARD_TIME_DISABLE;
    IF8080_InitStruct.IF8080_GuardTime         = IF8080_GUARD_TIME_2T;
    IF8080_InitStruct.IF8080_8BitSwap          = IF8080_8BitSwap_ENABLE;
    IF8080_InitStruct.IF8080_16BitSwap         = IF8080_16BitSwap_DISABLE;
    IF8080_InitStruct.IF8080_TxThr             = 10;
    IF8080_InitStruct.IF8080_TxDMACmd          = IF8080_TX_DMA_DISABLE;
    IF8080_InitStruct.IF8080_VsyncCmd          = IF8080_VSYNC_DISABLE;
    IF8080_InitStruct.IF8080_VsyncPolarity     = IF8080_VSYNC_POLARITY_FALLING;
    IF8080_Init(&IF8080_InitStruct);
    lcd_pad_init();
}
void rtl_gui_dma_single_block_init(void)
{
    RCC_PeriphClockCmd(APBPeriph_GDMA, APBPeriph_GDMA_CLOCK, ENABLE);
    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = LCD_DMA_CHANNEL_IRQ;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 3;
    NVIC_InitStruct.NVIC_IRQChannelCmd = DISABLE;
    NVIC_Init(&NVIC_InitStruct);
    /* Initialize GDMA peripheral */
    GDMA_InitTypeDef GDMA_InitStruct;
    GDMA_StructInit(&GDMA_InitStruct);
    GDMA_InitStruct.GDMA_ChannelNum          = LCD_DMA_CHANNEL_NUM;

    GDMA_InitStruct.GDMA_SourceDataSize      = GDMA_DataSize_Word;
    GDMA_InitStruct.GDMA_DestinationDataSize = GDMA_DataSize_Word;

    GDMA_InitStruct.GDMA_SourceMsize         = GDMA_Msize_8;
    GDMA_InitStruct.GDMA_DestinationMsize    = GDMA_Msize_8;
    GDMA_InitStruct.GDMA_DestHandshake       = GDMA_Handshake_8080_TX;
    GDMA_InitStruct.GDMA_DIR                 = GDMA_DIR_MemoryToPeripheral;
    GDMA_InitStruct.GDMA_SourceInc           = DMA_SourceInc_Inc;
    GDMA_InitStruct.GDMA_DestinationInc      = DMA_DestinationInc_Fix;

    GDMA_Init(LCD_DMA_CHANNEL_INDEX, &GDMA_InitStruct);
    GDMA_INTConfig(LCD_DMA_CHANNEL_NUM, GDMA_INT_Transfer, ENABLE);
}


void lcd_dma_single_block_start(uint8_t *source_addr, uint32_t len)
{
    GDMA_SetBufferSize(LCD_DMA_CHANNEL_INDEX, len >> 2);
    GDMA_SetDestinationAddress(LCD_DMA_CHANNEL_INDEX, (uint32_t)(&(IF8080->FIFO)));
    GDMA_SetSourceAddress(LCD_DMA_CHANNEL_INDEX, (uint32_t)source_addr);
    GDMA_Cmd(LCD_DMA_CHANNEL_NUM, ENABLE);
}



static void lcd_st7796_init(void)
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
    st7796_write_cmd(0xB1);     //aim at gui_temp_test_4 setting mode menu no psram diplay
    st7796_write_data(0x20);    //frame rate 64hz
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
}
#include "rtl876x_tim.h"
void lcd_set_backlight(uint32_t percent)
{
    if (percent)
    {
        if (percent > 100)
        {
            percent = 100;
        }
        TIM_Cmd(BL_PWM_TIM, DISABLE);
        TIM_PWMChangeFreqAndDuty(BL_PWM_TIM, percent * 10, (100 - percent) * 10);
        TIM_Cmd(BL_PWM_TIM, ENABLE);
        Pad_Config(LCD_8080_BL, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE,
                   PAD_OUT_HIGH);
    }
    else
    {
        TIM_Cmd(BL_PWM_TIM, DISABLE);
        Pad_Config(LCD_8080_BL, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_DOWN, PAD_OUT_ENABLE, PAD_OUT_LOW);
    }
    return;
}

void rtk_lcd_hal_init(void)
{
    lcd_device_init();
    lcd_set_reset(true);
    platform_delay_ms(50);
    lcd_set_reset(false);
    platform_delay_ms(50);
    lcd_st7796_init();

    st7796_write_cmd(0x11);
    st7796_write_cmd(0x29);
    // lcd_set_backlight(100);
}


void rtk_lcd_hal_set_window(uint16_t xStart, uint16_t yStart, uint16_t w, uint16_t h)
{
    IF8080_SwitchMode(IF8080_MODE_MANUAL);
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
    IF8080_SetCS();
    /* Enable Auto mode */
    IF8080_SwitchMode(IF8080_MODE_AUTO);

    IF8080_ClearTxCounter();

    /* Configure command */
    uint8_t cmd[1] = {0x2c};
    IF8080_SetCmdSequence(cmd, 1);

    /* Enable GDMA TX */
    IF8080_GDMACmd(ENABLE);

    /* Configure pixel number */
    uint32_t len = w * h * ST7796_DRV_PIXEL_BITS >> 3;
    IF8080_SetTxDataLen(len);

    /* Start output */
    IF8080_AutoModeCmd(IF8080_Auto_Mode_Direction_WRITE, ENABLE);
}








void rtk_lcd_hal_update_framebuffer(uint8_t *buf, uint32_t len)
{
    RCC_PeriphClockCmd(APBPeriph_GDMA, APBPeriph_GDMA_CLOCK, ENABLE);
    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = LCD_DMA_CHANNEL_IRQ;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 3;
    NVIC_InitStruct.NVIC_IRQChannelCmd = DISABLE;
    NVIC_Init(&NVIC_InitStruct);
    /* Initialize GDMA peripheral */
    GDMA_InitTypeDef GDMA_InitStruct;
    GDMA_StructInit(&GDMA_InitStruct);
    GDMA_InitStruct.GDMA_ChannelNum          = LCD_DMA_CHANNEL_NUM;

    GDMA_InitStruct.GDMA_SourceDataSize      = GDMA_DataSize_Word;
    GDMA_InitStruct.GDMA_DestinationDataSize = GDMA_DataSize_Word;

    GDMA_InitStruct.GDMA_SourceMsize         = GDMA_Msize_8;
    GDMA_InitStruct.GDMA_DestinationMsize    = GDMA_Msize_8;
    GDMA_InitStruct.GDMA_DestHandshake       = GDMA_Handshake_8080_TX;
    GDMA_InitStruct.GDMA_DIR                 = GDMA_DIR_MemoryToPeripheral;
    GDMA_InitStruct.GDMA_SourceInc           = DMA_SourceInc_Inc;
    GDMA_InitStruct.GDMA_DestinationInc      = DMA_DestinationInc_Fix;

    GDMA_Init(LCD_DMA_CHANNEL_INDEX, &GDMA_InitStruct);
    GDMA_INTConfig(LCD_DMA_CHANNEL_NUM, GDMA_INT_Transfer, ENABLE);

    GDMA_SetBufferSize(LCD_DMA_CHANNEL_INDEX, len >> 2);

    GDMA_SetDestinationAddress(LCD_DMA_CHANNEL_INDEX, (uint32_t)(&(IF8080->FIFO)));
    GDMA_SetSourceAddress(LCD_DMA_CHANNEL_INDEX, (uint32_t)buf);
    GDMA_Cmd(LCD_DMA_CHANNEL_NUM, ENABLE);
}

void rtk_lcd_hal_rect_fill(uint16_t xStart, uint16_t yStart, uint16_t w, uint16_t h,
                           uint32_t color)
{

}


static uint32_t pfb_tx_xnt = 0;
void rtk_lcd_hal_start_transfer(uint8_t *buf, uint32_t len)
{
    pfb_tx_xnt = len;
    rtl_gui_dma_single_block_init();
    lcd_dma_single_block_start(buf, len * 2);
}
void rtk_lcd_hal_transfer_done(void)
{
    while (GDMA_GetTransferINTStatus(LCD_DMA_CHANNEL_NUM) != SET);
    GDMA_ClearINTPendingBit(LCD_DMA_CHANNEL_NUM, GDMA_INT_Transfer);
    uint32_t counter = 0;
    while (counter != (pfb_tx_xnt * 2)) //LCD_SECTION_HEIGHT
    {
        counter = IF8080_GetTxCounter();
    }
    IF8080_SwitchMode(IF8080_MODE_MANUAL);
}
uint32_t rtk_lcd_hal_get_width(void)
{
    return ST7796_LCD_WIDTH;
}
uint32_t rtk_lcd_hal_get_height(void)
{
    return ST7796_LCD_HIGHT;
}
uint32_t rtk_lcd_hal_get_pixel_bits(void)
{
    return ST7796_DRV_PIXEL_BITS;
}



