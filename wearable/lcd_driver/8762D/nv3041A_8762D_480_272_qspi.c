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


//DATA_RAM_FUNCTION
//static void rtl_lcd_qspi_write_cmd(uint16_t cmd) //total 4 byte, first byte is 0x02
//{
//    uint8_t sdat[] = {0x02, 0x00, cmd, 0x00};
//    spic2_spi_write(sdat, sizeof(sdat));
//}


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
    xStart = xStart + 0;
    xEnd = xEnd + 0;
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
    GDMA_InitStruct.GDMA_SourceInc           = DMA_SourceInc_Fix;
    GDMA_InitStruct.GDMA_DestinationInc      = DMA_DestinationInc_Fix;

    GDMA_Init(LCD_DMA_CHANNEL_INDEX, &GDMA_InitStruct);
    GDMA_INTConfig(LCD_DMA_CHANNEL_NUM, GDMA_INT_Transfer, ENABLE);


    rtk_lcd_hal_set_window(xStart, yStart, w, h);

    static uint32_t color_buf = 0;
    color_buf = (color >> 8) | (color << 8);
    color_buf = color_buf | color_buf << 16;

    GDMA_SetSourceAddress(LCD_DMA_CHANNEL_INDEX, (uint32_t)&color_buf);
    GDMA_SetDestinationAddress(LCD_DMA_CHANNEL_INDEX, (uint32_t)(&(SPIC2->dr[0].word)));


    uint32_t section_hight = 10;
    uint32_t left_line = h % section_hight;

    for (uint32_t i = 0; i < h / section_hight; i++)
    {
        GDMA_SetBufferSize(LCD_DMA_CHANNEL_INDEX, (w * section_hight * 2) >> 2);
        GDMA_Cmd(LCD_DMA_CHANNEL_NUM, ENABLE);
        rtk_lcd_hal_transfer_done();
    }
    if (left_line != 0)
    {
        GDMA_SetBufferSize(LCD_DMA_CHANNEL_INDEX, (w * left_line * 2) >> 2);
        GDMA_Cmd(LCD_DMA_CHANNEL_NUM, ENABLE);
        rtk_lcd_hal_transfer_done();
    }

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

    GDMA_SetBufferSize(LCD_DMA_CHANNEL_INDEX, len >> 1);

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

static void lcd_set_reset(bool reset)
{
    if (reset)
    {
        Pad_Config(P2_2, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_DOWN, PAD_OUT_ENABLE, PAD_OUT_LOW);
    }
    else
    {
        Pad_Config(P2_2, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);
    }
}

static void lcd_seq_init(void)
{


//************* Start Initial Sequence **********//
    rtl_lcd_qspi_cmd_param(0xff, 0xa5);

    rtl_lcd_qspi_cmd_param(0xE7, 0x10);//TE_output_en

    rtl_lcd_qspi_cmd_param(0x35, 0x00);//TE_ interface_en

    rtl_lcd_qspi_cmd_param(0x36, 0xc0);

#if 1
    rtl_lcd_qspi_cmd_param(0x38, 0xff);//idle off
#else
    rtl_lcd_qspi_cmd_param(0x39, 0xff);//idle on
#endif

    rtl_lcd_qspi_cmd_param(0x3A, 0x01);//01---565/00---666

    rtl_lcd_qspi_cmd_param(0x40, 0x01);//01:IPS/00:TN

    rtl_lcd_qspi_cmd_param(0x41, 0x03);//01--8bit//03--16bit

    rtl_lcd_qspi_cmd_param(0x43, 0x01);// 00 msb/01 lsb

    rtl_lcd_qspi_cmd_param(0x44, 0x15);//VBP

    rtl_lcd_qspi_cmd_param(0x45, 0x15); //VFP

    rtl_lcd_qspi_cmd_param(0x7d, 0x03);//vdds_trim[2:0]

    rtl_lcd_qspi_cmd_param(0xc1,
                           0xbb);//avdd_clp_en avdd_clp[1:0] avcl_clp_en avcl_clp[1:0]  //0xbb    88  a2

    rtl_lcd_qspi_cmd_param(0xc2, 0x05);//vgl_clp_en vgl_clp[2:0]

    rtl_lcd_qspi_cmd_param(0xc3, 0x10);//vgl_clp_en vgl_clp[2:0]

    rtl_lcd_qspi_cmd_param(0xc6,
                           0x3e);//avdd_ratio_sel avcl_ratio_sel vgh_ratio_sel[1:0] vgl_ratio_sel[1:0]

    rtl_lcd_qspi_cmd_param(0xc7, 0x25);//mv_clk_sel[1:0] avdd_clk_sel[1:0] avcl_clk_sel[1:0]

    rtl_lcd_qspi_cmd_param(0xc8, 0x21);// VGL_CLK_sel

    rtl_lcd_qspi_cmd_param(0x7a, 0x51);// user_vgsp  //58

    rtl_lcd_qspi_cmd_param(0x6f, 0x49);// user_gvdd  //4F

    rtl_lcd_qspi_cmd_param(0x78, 0x57);// user_gvcl  //70

    rtl_lcd_qspi_cmd_param(0xc9, 0x00);

    rtl_lcd_qspi_cmd_param(0x67, 0x11);

    rtl_lcd_qspi_cmd_param(0x51, 0x0a);//gate_ed //gate_st_o[7:0]

    rtl_lcd_qspi_cmd_param(0x52, 0x7D);

    rtl_lcd_qspi_cmd_param(0x53, 0x0a);

    rtl_lcd_qspi_cmd_param(0x54, 0x7D);

//sorce
    rtl_lcd_qspi_cmd_param(0x46, 0x0a);

    rtl_lcd_qspi_cmd_param(0x47, 0x2a);

    rtl_lcd_qspi_cmd_param(0x48, 0x0a);

    rtl_lcd_qspi_cmd_param(0x49, 0x1a);

    rtl_lcd_qspi_cmd_param(0x44, 0x15);

    rtl_lcd_qspi_cmd_param(0x45, 0x15);

    rtl_lcd_qspi_cmd_param(0x73, 0x08);

    rtl_lcd_qspi_cmd_param(0x74, 0x10);

    rtl_lcd_qspi_cmd_param(0x56, 0x43);

    rtl_lcd_qspi_cmd_param(0x57, 0x42);

    rtl_lcd_qspi_cmd_param(0x58, 0x3c);

    rtl_lcd_qspi_cmd_param(0x59, 0x64);

    rtl_lcd_qspi_cmd_param(0x5a, 0x41);

    rtl_lcd_qspi_cmd_param(0x5b, 0x3C);

    rtl_lcd_qspi_cmd_param(0x5c, 0x02);

    rtl_lcd_qspi_cmd_param(0x5d, 0x3c);

    rtl_lcd_qspi_cmd_param(0x5e, 0x1f);

    rtl_lcd_qspi_cmd_param(0x60, 0x80);

    rtl_lcd_qspi_cmd_param(0x61, 0x3f);

    rtl_lcd_qspi_cmd_param(0x62, 0x21);

    rtl_lcd_qspi_cmd_param(0x63, 0x07);

    rtl_lcd_qspi_cmd_param(0x64, 0xe0);

    rtl_lcd_qspi_cmd_param(0x65, 0x02);

    rtl_lcd_qspi_cmd_param(0xca, 0x20);

    rtl_lcd_qspi_cmd_param(0xcb, 0x52);

    rtl_lcd_qspi_cmd_param(0xcc, 0x10);

    rtl_lcd_qspi_cmd_param(0xcD, 0x42);

    rtl_lcd_qspi_cmd_param(0xD0, 0x20);

    rtl_lcd_qspi_cmd_param(0xD1, 0x52);

    rtl_lcd_qspi_cmd_param(0xD2, 0x10);

    rtl_lcd_qspi_cmd_param(0xD3, 0x42);

    rtl_lcd_qspi_cmd_param(0xD4, 0x0a);

    rtl_lcd_qspi_cmd_param(0xD5, 0x32);

//gammma
    rtl_lcd_qspi_cmd_param(0x80, 0x00);

    rtl_lcd_qspi_cmd_param(0xA0, 0x00);

    rtl_lcd_qspi_cmd_param(0x81, 0x06);

    rtl_lcd_qspi_cmd_param(0xA1, 0x08);

    rtl_lcd_qspi_cmd_param(0x82, 0x03);

    rtl_lcd_qspi_cmd_param(0xA2, 0x03);

    rtl_lcd_qspi_cmd_param(0x86, 0x14);

    rtl_lcd_qspi_cmd_param(0xA6, 0x14);

    rtl_lcd_qspi_cmd_param(0x87, 0x2C);

    rtl_lcd_qspi_cmd_param(0xA7, 0x26);

    rtl_lcd_qspi_cmd_param(0x83, 0x37);

    rtl_lcd_qspi_cmd_param(0xA3, 0x37);

    rtl_lcd_qspi_cmd_param(0x84, 0x35);

    rtl_lcd_qspi_cmd_param(0xA4, 0x35);

    rtl_lcd_qspi_cmd_param(0x85, 0x3f);

    rtl_lcd_qspi_cmd_param(0xA5, 0x3f);

    rtl_lcd_qspi_cmd_param(0x88, 0x0A);

    rtl_lcd_qspi_cmd_param(0xA8, 0x0A);

    rtl_lcd_qspi_cmd_param(0x89, 0x13);

    rtl_lcd_qspi_cmd_param(0xA9, 0x12);

    rtl_lcd_qspi_cmd_param(0x8a, 0x18);

    rtl_lcd_qspi_cmd_param(0xAa, 0x19);

    rtl_lcd_qspi_cmd_param(0x8b, 0x0a);

    rtl_lcd_qspi_cmd_param(0xAb, 0x0a);

    rtl_lcd_qspi_cmd_param(0x8c, 0x17);

    rtl_lcd_qspi_cmd_param(0xAc, 0x0B);

    rtl_lcd_qspi_cmd_param(0x8d, 0x1A);

    rtl_lcd_qspi_cmd_param(0xAd, 0x09);

    rtl_lcd_qspi_cmd_param(0x8e, 0x1A);

    rtl_lcd_qspi_cmd_param(0xAe, 0x08);

    rtl_lcd_qspi_cmd_param(0x8f, 0x1F);

    rtl_lcd_qspi_cmd_param(0xAf, 0x00);

    rtl_lcd_qspi_cmd_param(0x90, 0x08);

    rtl_lcd_qspi_cmd_param(0xB0, 0x00);

    rtl_lcd_qspi_cmd_param(0x91, 0x10);

    rtl_lcd_qspi_cmd_param(0xB1, 0x06);

    rtl_lcd_qspi_cmd_param(0x92, 0x19);

    rtl_lcd_qspi_cmd_param(0xB2, 0x15);

    rtl_lcd_qspi_cmd_param(0xff, 0x00);

    rtl_lcd_qspi_cmd_param(0x11, 0x00);

    platform_delay_ms(50);

    rtl_lcd_qspi_cmd_param(0x29, 0x00);

    platform_delay_ms(50);
}

void rtk_lcd_hal_init(void)
{
    lcd_pad_init();
    qspi_lcd_platform_init();
    SPIC2->baudr = 2;//by howie, this lcd panel can't set QSPI 45MHz

    lcd_set_reset(true);
    platform_delay_ms(120);
    lcd_set_reset(false);
    platform_delay_ms(50);


    lcd_seq_init();

    rtk_lcd_hal_rect_fill(0, 0, 480, 272, 0xF0F0F0F0);

}

uint32_t rtk_lcd_hal_power_on(void)
{
    return 0;
}

uint32_t rtk_lcd_hal_power_off(void)
{
    return 0;
}








