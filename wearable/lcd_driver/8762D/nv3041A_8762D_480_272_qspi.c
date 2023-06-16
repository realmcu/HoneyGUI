#include "board.h"
#include "app_section.h"
#include "rtl876x_rcc.h"
#include "rtl876x_tim.h"
#include "rtl876x_gpio.h"
#include "rtl876x_pinmux.h"
#include "rtl876x_gdma.h"
#include "rtl876x_nvic.h"
#include "trace.h"
#include "nv3041A_8762D_480_272_qspi.h"
#include "platform_utils.h"
#include "qspi_lcd_platform.h"



#define BIT_CMD_CH(x)           (((x) & 0x00000003) << 20)
#define BIT_DATA_CH(x)          (((x) & 0x00000003) << 18)
#define BIT_ADDR_CH(x)          (((x) & 0x00000003) << 16)
#define BIT_TMOD(x)             (((x) & 0x00000003) << 8)

#define BIT_TXSIM               (0x00000001 << 9)
#define BIT_SEQ_EN              (0x00000001 << 3)





DATA_RAM_FUNCTION
static void spic2_spi_write(uint8_t *buf, uint32_t len)
{
    SPIC2->ser = BIT0;
    SPIC2->ssienr = 0x00;//disable SPIC2

    SPIC2->ctrlr0 &= ~(BIT_CMD_CH(3) | BIT_ADDR_CH(3) | BIT_DATA_CH(3));//SET CHANNEL NUM
    SPIC2->ctrlr0 &= ~(BIT_TMOD(3)); //tx mode

    for (uint32_t i = 0; i < len; i++)
    {
        SPIC2->dr[0].byte = buf[i];
    }
    SPIC2->ssienr = 0x01;//enable SPIC2

    while (SPIC2->sr & BIT0); // wait bus busy

    SPIC2->ssienr = 0x00;//disable SPIC2
}


DATA_RAM_FUNCTION
static void rtl_lcd_qspi_write_cmd(uint16_t cmd) //total 4 byte, first byte is 0x02
{
    uint8_t sdat[] = {0x02, 0x00, cmd, 0x00};
    spic2_spi_write(sdat, sizeof(sdat));
}


DATA_RAM_FUNCTION
static void rtl_lcd_qspi_cmd_param(uint8_t cmd, uint8_t data) //total 5 byte, first byte is 0x02
{
    uint8_t sdat[] = {0x02, 0x00, cmd, 0x00, data};
    spic2_spi_write(sdat, sizeof(sdat));
}

DATA_RAM_FUNCTION
static void rtl_lcd_qspi_cmd_param4(uint8_t cmd, uint8_t *data) //total 8 byte, first byte is 0x02
{
    uint8_t sdat[] = {0x02, 0x00, cmd, 0x00, data[0], data[1], data[2], data[3]};
    spic2_spi_write(sdat, sizeof(sdat));
}


DATA_RAM_FUNCTION
static void rtl_lcd_qspi_enter_data_output_mode(void) //total 4 byte, first byte is 0x32
{
    SPIC2->ser = BIT0;//select CS0
    SPIC2->ssienr = 0x00;//disable SPIC2

    SPIC2->ctrlr0 &= ~(BIT_CMD_CH(3) | BIT_ADDR_CH(3) | BIT_DATA_CH(3));//SET CHANNEL NUM
    SPIC2->ctrlr0 |= (BIT_CMD_CH(0) | BIT_ADDR_CH(0) | BIT_DATA_CH(2));
    SPIC2->ctrlr0 &= ~(BIT_TMOD(3)); //set tx mode

    SPIC2->imr |= BIT_TXSIM;
    SPIC2->ctrlr2 |= BIT_SEQ_EN;

    /* must push cmd and address before SPIC enable */
    uint32_t first_word = 0x32 | __REV(0x002c00);
    SPIC2->dr[0].word = first_word;

    SPIC2->dmacr = 2;

    /* change this value can not influence the result. the wave is split into two periods. the first is 32 bytes. */
    SPIC2->dmatdlr = 4; /* no any influence. */



    SPIC2->ssienr = 0x01;//enable SPIC2
    /*then , we can push data to FIFO*/


    //SPIC2->ctrlr2 &= ~ BIT_SEQ_EN;
    //SPIC2->imr &= ~ BIT_TXSIM;

    //SPIC2->ssienr = 0x00;//disable SPIC2
}


void rtk_lcd_hal_set_window(uint16_t xStart, uint16_t yStart, uint16_t w, uint16_t h)
{
    uint8_t data[4];
    uint16_t xEnd = xStart + w - 1;
    uint16_t yEnd = yStart + h - 1;
    xStart = xStart + 6;
    xEnd = xEnd + 6;
    yStart = yStart + 0;
    yEnd = yEnd + 0;

    data[0] = xStart >> 8;
    data[1] = xStart & 0xff;
    data[2] = xEnd >> 8;
    data[3] = xEnd & 0xff;
    rtl_lcd_qspi_cmd_param4(0x2A, data);


    data[0] = yStart >> 8;
    data[1] = yStart & 0xff;
    data[2] = yEnd >> 8;
    data[3] = yEnd & 0xff;
    rtl_lcd_qspi_cmd_param4(0x2B, data);

    rtl_lcd_qspi_enter_data_output_mode();
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
    GDMA_InitStruct.GDMA_SourceMsize         = GDMA_Msize_1;
    GDMA_InitStruct.GDMA_DestinationMsize    = GDMA_Msize_1;

    GDMA_InitStruct.GDMA_DestHandshake       = GDMA_Handshake_SPIC2_TX;
    GDMA_InitStruct.GDMA_DIR                 = GDMA_DIR_MemoryToPeripheral;
    GDMA_InitStruct.GDMA_SourceInc           = DMA_SourceInc_Inc;
    GDMA_InitStruct.GDMA_DestinationInc      = DMA_DestinationInc_Fix;

    GDMA_Init(LCD_DMA_CHANNEL_INDEX, &GDMA_InitStruct);
    GDMA_INTConfig(LCD_DMA_CHANNEL_NUM, GDMA_INT_Transfer, ENABLE);

    GDMA_SetBufferSize(LCD_DMA_CHANNEL_INDEX, len >> 2);

    GDMA_SetDestinationAddress(LCD_DMA_CHANNEL_INDEX, (uint32_t)(&(SPIC2->dr[0].word)));
    GDMA_SetSourceAddress(LCD_DMA_CHANNEL_INDEX, (uint32_t)buf);
    GDMA_Cmd(LCD_DMA_CHANNEL_NUM, ENABLE);
}

void rtk_lcd_hal_rect_fill(uint16_t xStart, uint16_t yStart, uint16_t w, uint16_t h,
                           uint32_t color)
{

}

void rtk_lcd_hal_start_transfer(uint8_t *buf, uint32_t len)
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
    GDMA_InitStruct.GDMA_SourceMsize         = GDMA_Msize_1;
    GDMA_InitStruct.GDMA_DestinationMsize    = GDMA_Msize_1;

    GDMA_InitStruct.GDMA_DestHandshake       = GDMA_Handshake_SPIC2_TX;
    GDMA_InitStruct.GDMA_DIR                 = GDMA_DIR_MemoryToPeripheral;
    GDMA_InitStruct.GDMA_SourceInc           = DMA_SourceInc_Inc;
    GDMA_InitStruct.GDMA_DestinationInc      = DMA_DestinationInc_Fix;

    GDMA_Init(LCD_DMA_CHANNEL_INDEX, &GDMA_InitStruct);
    GDMA_INTConfig(LCD_DMA_CHANNEL_NUM, GDMA_INT_Transfer, ENABLE);

    GDMA_SetBufferSize(LCD_DMA_CHANNEL_INDEX, len >> 2);

    GDMA_SetDestinationAddress(LCD_DMA_CHANNEL_INDEX, (uint32_t)(&(SPIC2->dr[0].word)));
    GDMA_SetSourceAddress(LCD_DMA_CHANNEL_INDEX, (uint32_t)buf);
    GDMA_Cmd(LCD_DMA_CHANNEL_NUM, ENABLE);
}
void rtk_lcd_hal_transfer_done(void)
{
    while (GDMA_GetTransferINTStatus(LCD_DMA_CHANNEL_NUM) != SET);
    GDMA_ClearINTPendingBit(LCD_DMA_CHANNEL_NUM, GDMA_INT_Transfer);
}

uint32_t rtk_lcd_hal_get_width(void)
{
    return NV3041A_LCD_WIDTH;
}
uint32_t rtk_lcd_hal_get_height(void)
{
    return NV3041A_LCD_HIGHT;
}
uint32_t rtk_lcd_hal_get_pixel_bits(void)
{
    return NV3041A_DRV_PIXEL_BITS;
}


static void lcd_pad_init(void)
{

    /*BL AND RESET ARE NOT FIX*/
    /*BL AND RESET ARE NOT FIX*/

    Pad_Config(QSPI_LCD_POWER, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);

}
void rtk_lcd_hal_init(void)
{
    lcd_pad_init();
    //do qspi init here
    //TODO DO RESET
    qspi_lcd_platform_init();

    rtl_lcd_qspi_write_cmd(0xff);
    rtl_lcd_qspi_cmd_param(0xFF, 0xFF);

}









