#include "board.h"
#include "app_section.h"
#include "wristband_gui.h"
#include "rtl876x_spi.h"
#include "trace.h"
#include "lcd_st7789_240_spi.h"
#include "platform_utils.h"
#include "rtl876x_rcc.h"
#include "rtl876x_tim.h"
#include "rtl876x_gpio.h"
#include "rtl876x_pinmux.h"
#include "rtl876x_gdma.h"
#include "rtl876x_nvic.h"
#include "flash_device.h"
#include "trace.h"
static bool te_ready;
void rtl_gui_lcd_sectionconfig(uint16_t xStart, uint16_t yStart, uint16_t xEnd, uint16_t yEnd);

GDMA_LLIDef *GDMA_LLIStruct = NULL;
const T_RTK_GENERAL_GUI_CONFIG rtk_gui_config =
{
    .lcd_width = LCD_WIDTH,
    .lcd_hight = LCD_HIGHT,
    .lcd_section_height = LCD_SECTION_HEIGHT,
    .pixel_format = PIXEL_FORMAT,
    .pixel_bytes = PIXEL_BYTES,
    .lcd_section_byte_len = LCD_SECTION_BYTE_LEN,
    .max_section_count = MAX_SECTION_COUNT,
    .total_section_count = TOTAL_SECTION_COUNT,
};

void lcd_set_backlight(uint32_t percent)
{
    Pad_Config(LCD_SPI_BL, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
    extern bool allowed_lcd_backlight_enter_dlps;
    if (percent != 0)
    {
        TIM_Cmd(BL_PWM_TIM, DISABLE);
        TIM_PWMChangeFreqAndDuty(BL_PWM_TIM, (percent) * 10, (100 - percent) * 10);
        TIM_Cmd(BL_PWM_TIM, ENABLE);
        Pad_Config(LCD_SPI_BL, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE, PAD_OUT_HIGH);
        allowed_lcd_backlight_enter_dlps = false;
    }
    else
    {
        TIM_Cmd(BL_PWM_TIM, DISABLE);
        Pad_Config(LCD_SPI_BL, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_DOWN, PAD_OUT_DISABLE, PAD_OUT_LOW);
        allowed_lcd_backlight_enter_dlps = true;
    }
}
DATA_RAM_FUNCTION
void st7789_write_cmd(uint8_t command)
{
    while (SPI_GetFlagState(LCD_SPI_BUS, SPI_FLAG_BUSY));
    /* switch to CS0 signal */
    SPI_SetCSNumber(LCD_SPI_BUS, 0);
    SPI_Cmd(LCD_SPI_BUS, DISABLE);
    SPI_Change_CLK(LCD_SPI_BUS, 2);
    SPI_Cmd(LCD_SPI_BUS, ENABLE);
    GPIO_ResetBits(LCD_SPI_DC_PIN);
    while (SPI_GetFlagState(LCD_SPI_BUS, SPI_FLAG_TFE) == false);
    SPI_SendData(LCD_SPI_BUS, command);

    /* Waiting for SPI data transfer to end */
    while (SPI_GetFlagState(LCD_SPI_BUS, SPI_FLAG_BUSY));
    GPIO_SetBits(LCD_SPI_DC_PIN);
}

DATA_RAM_FUNCTION
void st7789_write_data(uint8_t data)
{
    while (SPI_GetFlagState(LCD_SPI_BUS, SPI_FLAG_TFE) == false);
    SPI_SendData(LCD_SPI_BUS, data);

    /* Waiting for SPI data transfer to end */
    while (SPI_GetFlagState(LCD_SPI_BUS, SPI_FLAG_BUSY)); //Howie todo, remove later
}

void lcd_st7789_set_window(uint16_t xStart, uint16_t yStart, uint16_t xEnd, uint16_t yEnd)
{
    /*this is a bug here if lcd size bigger than uint8_t*/
    st7789_write_cmd(0x2A); //Set Column Address

//    xStart = xStart + 0x1A;
//    xEnd = xEnd + 0x1A;
    yStart = yStart + 80;
    yEnd = yEnd + 80;
    while (!(LCD_SPI_BUS->SR & BIT(1)));
    LCD_SPI_BUS->DR[0] = xStart >> 8;
    while (!(LCD_SPI_BUS->SR & BIT(1)));
    LCD_SPI_BUS->DR[0] = xStart;
    while (!(LCD_SPI_BUS->SR & BIT(1)));
    LCD_SPI_BUS->DR[0] = xEnd >> 8;
    while (!(LCD_SPI_BUS->SR & BIT(1)));
    LCD_SPI_BUS->DR[0] = xEnd;

    st7789_write_cmd(0x2B); //Set Page Address

    LCD_SPI_BUS->DR[0] = yStart >> 8;
    while (!(LCD_SPI_BUS->SR & BIT(1)));
    LCD_SPI_BUS->DR[0] = yStart;
    while (!(LCD_SPI_BUS->SR & BIT(1)));
    LCD_SPI_BUS->DR[0] = yEnd >> 8;
    while (!(LCD_SPI_BUS->SR & BIT(1)));
    LCD_SPI_BUS->DR[0] = yEnd;

    st7789_write_cmd(0x2C);

}



void lcd_st7789_init(void)
{
#if 1
    st7789_write_cmd(0x35);
    st7789_write_data(0x00);

    st7789_write_cmd(0x44);
    st7789_write_data(0x00);
    st7789_write_data(0x50);
#if FEATURE_PSRAM
    st7789_write_cmd(0x44);
    st7789_write_data(0x00);
    st7789_write_data(0xA0); // if use psram set tear scanline here
#else
    st7789_write_cmd(0x44);
    st7789_write_data(0x00);
    st7789_write_data(0x50);
#endif

    st7789_write_cmd(0x36);
    st7789_write_data(0x40);

    st7789_write_cmd(0x3A);
    st7789_write_data(0x05);

    st7789_write_cmd(0xB2);
    st7789_write_data(0x0C);
    st7789_write_data(0x0C);
    st7789_write_data(0x00);
    st7789_write_data(0x33);
    st7789_write_data(0x33);

    st7789_write_cmd(0xB7);
    st7789_write_data(0x35);

    st7789_write_cmd(0xBB);
    st7789_write_data(0x19);

    st7789_write_cmd(0xC0);
    st7789_write_data(0x2C);

    st7789_write_cmd(0xC2);
    st7789_write_data(0x01);

    st7789_write_cmd(0xC3);
    st7789_write_data(0x12);

    st7789_write_cmd(0xC4);
    st7789_write_data(0x20);

    st7789_write_cmd(0xC6);
    st7789_write_data(0x1F);

    st7789_write_cmd(0xD0);
    st7789_write_data(0xA4);
    st7789_write_data(0xA1);

    st7789_write_cmd(0xE0);
    st7789_write_data(0xD0);
    st7789_write_data(0x04);
    st7789_write_data(0x0D);
    st7789_write_data(0x11);
    st7789_write_data(0x13);
    st7789_write_data(0x2B);
    st7789_write_data(0x3F);
    st7789_write_data(0x54);
    st7789_write_data(0x4C);
    st7789_write_data(0x18);
    st7789_write_data(0x0D);
    st7789_write_data(0x0B);
    st7789_write_data(0x1F);
    st7789_write_data(0x23);

    st7789_write_cmd(0xE1);
    st7789_write_data(0xD0);
    st7789_write_data(0x04);
    st7789_write_data(0x0C);
    st7789_write_data(0x11);
    st7789_write_data(0x13);
    st7789_write_data(0x2C);
    st7789_write_data(0x3F);
    st7789_write_data(0x44);
    st7789_write_data(0x51);
    st7789_write_data(0x2F);
    st7789_write_data(0x1F);
    st7789_write_data(0x1F);
    st7789_write_data(0x20);
    st7789_write_data(0x23);

    st7789_write_cmd(0xE4);
    st7789_write_data(0x27);
    st7789_write_data(0x00);
    st7789_write_data(0x11);

    st7789_write_cmd(0x21);

    st7789_write_cmd(0x11);
    platform_delay_ms(120);
#endif
}

void lcd_st7789_frame_rate(uint8_t RTNA, uint8_t FPA, uint8_t BPA)
{

    st7789_write_cmd(0xB2);
    st7789_write_data(BPA & 0x3f);
    st7789_write_data(FPA & 0x3f);
    st7789_write_data(0x00);
    st7789_write_data(0x33);
    st7789_write_data(0x33);

    st7789_write_cmd(0xC6);
    st7789_write_data(RTNA & 0x1F);
}

void lcd_te_device_init(void)
{
    Pinmux_Config(LCD_FRAME_SYNC, DWGPIO);
    Pad_Config(LCD_FRAME_SYNC, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE,
               PAD_OUT_LOW);
    /* Enable GPIO */
    RCC_PeriphClockCmd(APBPeriph_GPIO,  APBPeriph_GPIO_CLOCK,  ENABLE);

    /* Initialize GPIO as interrupt mode */
    GPIO_InitTypeDef GPIO_Param;
    GPIO_StructInit(&GPIO_Param);
    GPIO_Param.GPIO_Pin = GPIO_GetPin(LCD_FRAME_SYNC);
    GPIO_Param.GPIO_Mode = GPIO_Mode_IN;
    GPIO_Param.GPIO_ITCmd = ENABLE;
    GPIO_Param.GPIO_ITTrigger = GPIO_INT_Trigger_EDGE;
    GPIO_Param.GPIO_ITPolarity = GPIO_INT_POLARITY_ACTIVE_LOW;
    GPIO_Init(&GPIO_Param);

    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = GPIO_FRAME_SYNC_IRQ;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 3;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    GPIO_INTConfig(GPIO_GetPin(LCD_FRAME_SYNC), DISABLE);
    GPIO_MaskINTConfig(GPIO_GetPin(LCD_FRAME_SYNC), ENABLE);
}


bool rtl_gui_lcd_te_get_status(void)
{
    return te_ready;
}
static void lcd_change_te_status(bool status)
{
    te_ready = status;
}
void rtl_gui_lcd_te_enable(void)
{
    GPIO_INTConfig(GPIO_GetPin(LCD_FRAME_SYNC), ENABLE);
    GPIO_MaskINTConfig(GPIO_GetPin(LCD_FRAME_SYNC), DISABLE);
    lcd_change_te_status(false);
}

void rtl_gui_lcd_te_disable(void)
{
    GPIO_INTConfig(GPIO_GetPin(LCD_FRAME_SYNC), DISABLE);
    GPIO_MaskINTConfig(GPIO_GetPin(LCD_FRAME_SYNC), ENABLE);
    GPIO_ClearINTPendingBit(GPIO_GetPin(LCD_FRAME_SYNC));
    lcd_change_te_status(false);
}

void lcd_frame_sync_handler(void)
{
    lcd_change_te_status(true);
}
void lcd_pad_init(void)
{
    Pinmux_Config(LCD_SPI_CLK, LCD_SPI_FUNC_CLK);
    Pinmux_Config(LCD_SPI_MOSI, LCD_SPI_FUNC_MOSI);
    Pinmux_Config(LCD_SPI_CS, LCD_SPI_FUNC_CS);

    Pinmux_Config(LCD_SPI_DC, DWGPIO);

    Pad_Config(LCD_SPI_CLK, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_HIGH);
    Pad_Config(LCD_SPI_MOSI, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE,
               PAD_OUT_HIGH);
    Pad_Config(LCD_SPI_CS, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);
    Pad_Config(LCD_SPI_DC, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);

    /*BL AND RESET ARE NOT FIX*/
    Pad_Config(LCD_SPI_BL, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pinmux_Config(LCD_SPI_BL, timer_pwm4);

}
void lcd_enter_dlps(void)
{
    Pad_Config(LCD_SPI_CLK, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(LCD_SPI_MOSI, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);

    Pad_Config(LCD_SPI_CS, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(LCD_SPI_DC, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);

    Pad_Config(LCD_SPI_BL, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_DOWN, PAD_OUT_ENABLE, PAD_OUT_LOW);
}
void lcd_exit_dlps(void)
{
    Pad_Config(LCD_SPI_CLK, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(LCD_SPI_MOSI, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE,
               PAD_OUT_HIGH);
    Pad_Config(LCD_SPI_CS, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);
    Pad_Config(LCD_SPI_DC, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);

    Pad_Config(LCD_SPI_BL, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE, PAD_OUT_HIGH);
}


/**
  * @brief  writband lcd device init set IO config here
  * @param  None
  * @retval None
  */
void lcd_device_init(void)
{
    lcd_pad_init();

    /* Open timer clock */
    RCC_PeriphClockCmd(APBPeriph_TIMER, APBPeriph_TIMER_CLOCK, ENABLE);
    TIM_TimeBaseInitTypeDef TIM_InitStruct;
    TIM_StructInit(&TIM_InitStruct);
    TIM_InitStruct.TIM_PWM_En = PWM_ENABLE;
    /* Set period */
    TIM_InitStruct.TIM_Period = 10 - 1 ;
    /* Set PWM high count and low count */
    TIM_InitStruct.TIM_PWM_High_Count = 900 - 1 ;
    TIM_InitStruct.TIM_PWM_Low_Count = 100 - 1 ;
    /* Set timer mode User_define */
    TIM_InitStruct.TIM_Mode = TIM_Mode_UserDefine;
    /* Set timer clock source divider 40, timer pclk = 40M/4 */
    TIM_InitStruct.TIM_SOURCE_DIV = TIM_CLOCK_DIVIDER_40;
    TIM_TimeBaseInit(BL_PWM_TIM, &TIM_InitStruct);
    TIM_Cmd(BL_PWM_TIM, DISABLE);

    RCC_PeriphClockCmd(APBPeriph_GPIO, APBPeriph_GPIO_CLOCK, ENABLE);
    SPI_DeInit(LCD_SPI_BUS);
    RCC_PeriphClockCmd(LCD_SPI_APBPeriph, LCD_SPI_APBClock, ENABLE);
    SPI_InitTypeDef  SPI_InitStructure;

    SPI_StructInit(&SPI_InitStructure);
    SPI_InitStructure.SPI_Direction   = SPI_Direction_FullDuplex;
    SPI_InitStructure.SPI_Mode        = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize    = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL        = SPI_CPOL_High;
    SPI_InitStructure.SPI_CPHA        = SPI_CPHA_2Edge;
    SPI_InitStructure.SPI_BaudRatePrescaler  = SPI_BaudRatePrescaler_2;
    SPI_InitStructure.SPI_FrameFormat = SPI_Frame_Motorola;
    SPI_InitStructure.SPI_NDF         = 0;
    SPI_InitStructure.SPI_TxWaterlevel = 30;
    SPI_InitStructure.SPI_TxDmaEn = ENABLE;

    SPI_Init(LCD_SPI_BUS, &SPI_InitStructure);
    SPI_Cmd(LCD_SPI_BUS, ENABLE);

    RCC_PeriphClockCmd(APBPeriph_GPIO, APBPeriph_GPIO_CLOCK, ENABLE);

    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_ControlMode = GPIO_SOFTWARE_MODE;
    GPIO_InitStruct.GPIO_Pin  = GPIO_GetPin(LCD_SPI_DC);
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_ITCmd = DISABLE;
    GPIO_Init(&GPIO_InitStruct);
    GPIO_SetBits(GPIO_GetPin(LCD_SPI_BL));
}
void rtl_gui_dma_single_block_init(uint32_t dir_type)
{

}


void lcd_dma_single_block_start(uint32_t destination_addr, uint32_t source_addr, uint32_t len)
{
    GDMA_SetBufferSize(LCD_DMA_CHANNEL_INDEX, len);
    GDMA_SetSourceAddress(LCD_DMA_CHANNEL_INDEX, (uint32_t)source_addr);
    GDMA_Cmd(LCD_DMA_CHANNEL_NUM, ENABLE);
    SPI_GDMACmd(LCD_SPI_BUS, SPI_GDMAReq_Tx, ENABLE);
}

void lcd_wait_dma_transfer(void)
{
    while (GDMA_GetTransferINTStatus(LCD_DMA_CHANNEL_NUM) != SET);
    GDMA_ClearINTPendingBit(LCD_DMA_CHANNEL_NUM, GDMA_INT_Transfer);
    while (SPI_GetFlagState(LCD_SPI_BUS, SPI_FLAG_TFE) == false);
}

void lcd_wait_lcd_control_transfer(uint32_t count)
{
    while (GDMA_GetTransferINTStatus(LCD_DMA_CHANNEL_NUM) != SET);
    GDMA_ClearINTPendingBit(LCD_DMA_CHANNEL_NUM, GDMA_INT_Transfer);
    while (SPI_GetFlagState(LCD_SPI_BUS, SPI_FLAG_TFE) == false);
}

void rtl_gui_lcd_clear(uint16_t xStart, uint16_t yStart, uint16_t xEnd, uint16_t yEnd,
                       uint32_t color)
{
    rtl_gui_lcd_sectionconfig(xStart, yStart, xEnd, yEnd);
    while (SPI_GetFlagState(LCD_SPI_BUS, SPI_FLAG_BUSY));
    for (uint16_t i = xStart; i <= xEnd; i++)
    {
        for (uint16_t j = yStart; j <= yEnd; j++)
        {
            while (!(LCD_SPI_BUS->SR & BIT(1)));
            LCD_SPI_BUS->DR[0] = color >> 8;
            while (!(LCD_SPI_BUS->SR & BIT(1)));
            LCD_SPI_BUS->DR[0] = color;
        }
    }
    while (SPI_GetFlagState(LCD_SPI_BUS, SPI_FLAG_BUSY));
}

void rtl_gui_dma_single_block_start(uint32_t destination_addr, uint32_t source_addr, uint32_t len)
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
    GDMA_InitStruct.GDMA_DIR                 = GDMA_DIR_MemoryToPeripheral;
    GDMA_InitStruct.GDMA_BufferSize          = LCD_WIDTH * LCD_SECTION_HEIGHT * PIXEL_BYTES;
    GDMA_InitStruct.GDMA_SourceInc           = DMA_SourceInc_Inc;
    GDMA_InitStruct.GDMA_DestinationInc      = DMA_DestinationInc_Fix;
    GDMA_InitStruct.GDMA_SourceDataSize      = GDMA_DataSize_Byte;
    GDMA_InitStruct.GDMA_DestinationDataSize = GDMA_DataSize_Byte;
    GDMA_InitStruct.GDMA_SourceMsize         = GDMA_Msize_4;
    GDMA_InitStruct.GDMA_DestinationMsize    = GDMA_Msize_4;
    GDMA_InitStruct.GDMA_SourceAddr          = 0;
    GDMA_InitStruct.GDMA_DestinationAddr     = (uint32_t)(LCD_SPI_BUS->DR);
    GDMA_InitStruct.GDMA_DestHandshake       = LCD_SPI_DMA_TX_HANDSHAKE;

    GDMA_Init(LCD_DMA_CHANNEL_INDEX, &GDMA_InitStruct);
    GDMA_INTConfig(LCD_DMA_CHANNEL_NUM, GDMA_INT_Transfer, ENABLE);
}

void rtl_gui_wait_dma_transfer(void)
{
    lcd_wait_dma_transfer();
}

void rtl_gui_wait_lcd_control_transfer(uint32_t count)
{
    lcd_wait_lcd_control_transfer(count);
}


void rtl_gui_lcd_sectionconfig(uint16_t xStart, uint16_t yStart, uint16_t xEnd, uint16_t yEnd)
{
    lcd_st7789_set_window(xStart, yStart, xEnd, yEnd);
}

DATA_RAM_FUNCTION
void rtl_gui_refresh_by_dma_internal(uint8_t *readbuf, uint32_t count_for_section)
{
    uint32_t length = rtk_gui_config.lcd_width * rtk_gui_config.lcd_section_height *
                      rtk_gui_config.pixel_bytes;

    if (count_for_section == 0)
    {
        rtl_gui_dma_single_block_init(0x00000001);
        rtl_gui_lcd_sectionconfig(0, 0, rtk_gui_config.lcd_width - 1, rtk_gui_config.lcd_hight - 1);
        rtl_gui_dma_single_block_start(NULL, (uint32_t)readbuf, length);
    }
    else if (count_for_section == rtk_gui_config.total_section_count - 1)
    {
        rtl_gui_wait_dma_transfer();

        if ((rtk_gui_config.lcd_hight - count_for_section * rtk_gui_config.lcd_section_height) %
            rtk_gui_config.lcd_section_height)
        {
            length = ((rtk_gui_config.lcd_hight - count_for_section * rtk_gui_config.lcd_section_height) %
                      rtk_gui_config.lcd_section_height) *
                     rtk_gui_config.lcd_width * rtk_gui_config.pixel_bytes;
        }
        rtl_gui_dma_single_block_start(NULL, (uint32_t)readbuf, length);
        rtl_gui_wait_lcd_control_transfer(rtk_gui_config.lcd_hight * rtk_gui_config.lcd_width *
                                          rtk_gui_config.pixel_bytes);
    }
    else
    {
        rtl_gui_wait_dma_transfer();
        rtl_gui_dma_single_block_start(NULL, (uint32_t)readbuf, length);
    }

}

void rtl_gui_lcd_power_on(void)
{
    st7789_write_cmd(0x11);
    st7789_write_cmd(0x29);
    lcd_set_backlight(100);
}

void rtl_gui_lcd_power_off(void)
{
    st7789_write_cmd(0x28);
    st7789_write_cmd(0x10);
    platform_delay_ms(150);
    lcd_set_backlight(0);
}


void lcd_set_reset(bool reset)
{
    if (reset)
    {
        Pad_Config(LCD_SPI_RST, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_DOWN, PAD_OUT_ENABLE, PAD_OUT_LOW);
    }
    else
    {
        Pad_Config(LCD_SPI_RST, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);
    }
}

void lcd_driver_init(void)
{
    lcd_device_init();
    lcd_set_reset(true);
    platform_delay_ms(50);
    lcd_set_reset(false);
    platform_delay_ms(50);
    lcd_te_device_init();
    lcd_st7789_init();

}



