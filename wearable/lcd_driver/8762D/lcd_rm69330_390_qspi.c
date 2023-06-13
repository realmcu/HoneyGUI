#include "board.h"
#include "app_section.h"
#include "rtl876x_rcc.h"
#include "rtl876x_tim.h"
#include "rtl876x_if8080.h"
#include "rtl876x_gpio.h"
#include "rtl876x_pinmux.h"
#include "rtl876x_gdma.h"
#include "rtl876x_nvic.h"
#include "trace.h"
#include "lcd_rm69330_390_qspi.h"
#include "platform_utils.h"
#include "spic_2_qspi.h"
#include "wristband_gui.h"

#define BIT_CMD_CH(x)           (((x) & 0x00000003) << 20)
#define BIT_DATA_CH(x)          (((x) & 0x00000003) << 18)
#define BIT_ADDR_CH(x)          (((x) & 0x00000003) << 16)
#define BIT_TMOD(x)             (((x) & 0x00000003) << 8)

#define BIT_TXSIM               (0x00000001 << 9)
#define BIT_SEQ_EN              (0x00000001 << 3)


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

DATA_RAM_FUNCTION
void spic2_spi_write(uint8_t *buf, uint32_t len)
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
void rtl_rm69330_qspi_write_cmd(uint16_t cmd) //total 4 byte, first byte is 0x02
{
    uint8_t sdat[] = {0x02, 0x00, cmd, 0x00};
    spic2_spi_write(sdat, sizeof(sdat));
}


DATA_RAM_FUNCTION
void rtl_rm69330_qspi_cmd_param(uint8_t cmd, uint8_t data) //total 5 byte, first byte is 0x02
{
    uint8_t sdat[] = {0x02, 0x00, cmd, 0x00, data};
    spic2_spi_write(sdat, sizeof(sdat));
}

DATA_RAM_FUNCTION
void rtl_rm69330_qspi_cmd_param4(uint8_t cmd, uint8_t *data) //total 8 byte, first byte is 0x02
{
    uint8_t sdat[] = {0x02, 0x00, cmd, 0x00, data[0], data[1], data[2], data[3]};
    spic2_spi_write(sdat, sizeof(sdat));
}

DATA_RAM_FUNCTION
void rtl_rm69330_qspi_enter_data_output_mode(void) //total 4 byte, first byte is 0x32
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

void lcd_rm69330_qspi_390_set_window(uint16_t xStart, uint16_t yStart, uint16_t xEnd, uint16_t yEnd)
{
    uint8_t data[4];
    xStart = xStart + 6;
    xEnd = xEnd + 6;
    yStart = yStart + 0;
    yEnd = yEnd + 0;

    data[0] = xStart >> 8;
    data[1] = xStart & 0xff;
    data[2] = xEnd >> 8;
    data[3] = xEnd & 0xff;
    rtl_rm69330_qspi_cmd_param4(0x2A, data);


    data[0] = yStart >> 8;
    data[1] = yStart & 0xff;
    data[2] = yEnd >> 8;
    data[3] = yEnd & 0xff;
    rtl_rm69330_qspi_cmd_param4(0x2B, data);

    rtl_rm69330_qspi_enter_data_output_mode();
}

void lcd_rm69330_qspi_390_init(void)
{
    rtl_rm69330_qspi_cmd_param(0xFE, 0x01);
    rtl_rm69330_qspi_cmd_param(0x05, 0x10);
    rtl_rm69330_qspi_cmd_param(0x06, 0x62);
    rtl_rm69330_qspi_cmd_param(0x0D, 0x00);
    rtl_rm69330_qspi_cmd_param(0x0E, 0x81);
    rtl_rm69330_qspi_cmd_param(0x0F, 0x81);
    rtl_rm69330_qspi_cmd_param(0x10, 0x11);
    rtl_rm69330_qspi_cmd_param(0x11, 0x81);
    rtl_rm69330_qspi_cmd_param(0x12, 0x81);

    rtl_rm69330_qspi_cmd_param(0x13, 0x80);

    rtl_rm69330_qspi_cmd_param(0x14, 0x80);

    rtl_rm69330_qspi_cmd_param(0x15, 0x81);

    rtl_rm69330_qspi_cmd_param(0x16, 0x81);

    rtl_rm69330_qspi_cmd_param(0x18, 0x66);

    rtl_rm69330_qspi_cmd_param(0x19, 0x88);

    rtl_rm69330_qspi_cmd_param(0x5B, 0x10);

    rtl_rm69330_qspi_cmd_param(0x5C, 0x55);

    rtl_rm69330_qspi_cmd_param(0x62, 0x19);

    rtl_rm69330_qspi_cmd_param(0x63, 0x19);

    rtl_rm69330_qspi_cmd_param(0x70, 0x55);
    rtl_rm69330_qspi_cmd_param(0x74, 0x0C);

    rtl_rm69330_qspi_cmd_param(0xC5, 0x10);

    rtl_rm69330_qspi_cmd_param(0x25, 0x03);

    rtl_rm69330_qspi_cmd_param(0x26, 0x80);

    rtl_rm69330_qspi_cmd_param(0x27, 0x08);

    rtl_rm69330_qspi_cmd_param(0x28, 0x08);

    rtl_rm69330_qspi_cmd_param(0x2A, 0x23);

    rtl_rm69330_qspi_cmd_param(0x2B, 0x80);

    rtl_rm69330_qspi_cmd_param(0x2D, 0x08);

    rtl_rm69330_qspi_cmd_param(0x2F, 0x08);

    rtl_rm69330_qspi_cmd_param(0x30, 0x43);
    rtl_rm69330_qspi_cmd_param(0x66, 0x90);

    rtl_rm69330_qspi_cmd_param(0x72, 0x1A);

    rtl_rm69330_qspi_cmd_param(0x73, 0x13);

    rtl_rm69330_qspi_cmd_param(0xFE, 0x01);

    //rtl_rm69330_qspi_cmd_param(0x6A,0x03);
    rtl_rm69330_qspi_cmd_param(0x6A, 0x17);

    rtl_rm69330_qspi_cmd_param(0x1B, 0x00);

    rtl_rm69330_qspi_cmd_param(0x1D, 0x03);

    rtl_rm69330_qspi_cmd_param(0x1E, 0x03);

    rtl_rm69330_qspi_cmd_param(0x1F, 0x0C);

    rtl_rm69330_qspi_cmd_param(0x20, 0x03);

    rtl_rm69330_qspi_cmd_param(0xFE, 0x01);

    rtl_rm69330_qspi_cmd_param(0x36, 0x00);

    rtl_rm69330_qspi_cmd_param(0x6c, 0x80);
    rtl_rm69330_qspi_cmd_param(0x6d, 0x19);
    rtl_rm69330_qspi_cmd_param(0xfe, 0x04);
    rtl_rm69330_qspi_cmd_param(0x63, 0x00);
    rtl_rm69330_qspi_cmd_param(0x64, 0x0e);
    rtl_rm69330_qspi_cmd_param(0xfe, 0x02);

    {

        const uint8_t gama_value[] =
        {
            0x0, 0x1, 0x0, 0x1, 0x0, 0xF, 0x0, 0x1F,
            0x1, 0x76, 0x1, 0x90, 0x1, 0xA5, 0x1, 0xB5,
            0x1, 0xC2, 0x1, 0xCE, 0x1, 0xD8, 0x1, 0xE2,
            0x1, 0xF3, 0x2, 0x3, 0x2, 0x11, 0x2, 0x2A,
            0x2, 0x40, 0x2, 0x55, 0x2, 0x69, 0x2, 0x7C,
            0x2, 0x8E, 0x2, 0xA0, 0x0, 0x2, 0x0, 0xB2,
            0x2, 0xC4, 0x2, 0xD6, 0x2, 0xE8, 0x2, 0xFA,
            0x0, 0x1, 0x0, 0x1, 0x0, 0x1, 0x0, 0x87, 0x1,
            0x97, 0x1, 0xB5, 0x1, 0xC4, 0x1, 0xD1, 0x1,
            0xDD, 0x1, 0xE7, 0x1, 0xF1, 0x1, 0xFB, 0x2,
            0x4, 0x2, 0x14, 0x2, 0x23, 0x2, 0x0, 0x30, 0x2,
            0x49, 0x2, 0x5F, 0x2, 0x74, 0x2, 0x88, 0x2, 0x9B,
            0x2, 0xAD, 0x2, 0xBF, 0x2, 0xD1, 0x2, 0xE2, 0x2,
            0xF4, 0x3, 0x5, 0x3, 0x17, 0x0, 0x1, 0x0, 0x1, 0x1,
            0xC5, 0x1, 0xD5, 0x1, 0xDE, 0x1, 0xEE, 0x1, 0xFB,
            0x2, 0x7, 0x2, 0x11, 0x2, 0x1C, 0x2, 0x27, 0x2, 0x30,
            0x2, 0x41, 0x2, 0x52, 0x2, 0x61, 0x2, 0x7D, 0x2,
            0x96, 0x2, 0xAE, 0x2, 0xC5, 0x2, 0xDB, 0x2, 0xF0,
            0x3, 0x5, 0x3, 0x1A, 0x3, 0x2F, 0x0, 0x3, 0x44,
            0x3, 0x58, 0x3, 0x6D
        };
        for (uint16_t i = 0; i < sizeof(gama_value); i++)
        {
            rtl_rm69330_qspi_cmd_param(i, gama_value[i]);
        }

    }


    rtl_rm69330_qspi_cmd_param(0xA9, 0x40);

    rtl_rm69330_qspi_cmd_param(0xaa, 0xb8);
    rtl_rm69330_qspi_cmd_param(0xab, 0x01);
    rtl_rm69330_qspi_cmd_param(0xfe, 0x03);
    rtl_rm69330_qspi_cmd_param(0xa9, 0x40);

    rtl_rm69330_qspi_cmd_param(0xAA, 0x90);

    rtl_rm69330_qspi_cmd_param(0xAB, 0x01);

    rtl_rm69330_qspi_cmd_param(0xFE, 0x01);

    rtl_rm69330_qspi_cmd_param(0x3A, 0x00);

    rtl_rm69330_qspi_cmd_param(0x3B, 0x41);

    rtl_rm69330_qspi_cmd_param(0x3D, 0x17);

    rtl_rm69330_qspi_cmd_param(0x3F, 0x42);

    rtl_rm69330_qspi_cmd_param(0x40, 0x17);
    rtl_rm69330_qspi_cmd_param(0x41, 0x06);

    rtl_rm69330_qspi_cmd_param(0x37, 0x0C);

    rtl_rm69330_qspi_cmd_param(0xFE, 0x0C);

    rtl_rm69330_qspi_cmd_param(0x07, 0x1F);

    rtl_rm69330_qspi_cmd_param(0x08, 0x2F);

    rtl_rm69330_qspi_cmd_param(0x09, 0x3F);

    rtl_rm69330_qspi_cmd_param(0x0A, 0x4F);

    rtl_rm69330_qspi_cmd_param(0x0B, 0x5F);

    rtl_rm69330_qspi_cmd_param(0x0C, 0x6F);
    rtl_rm69330_qspi_cmd_param(0x0D, 0xFF);

    rtl_rm69330_qspi_cmd_param(0x0E, 0xFF);

    rtl_rm69330_qspi_cmd_param(0x0F, 0xFF);

    rtl_rm69330_qspi_cmd_param(0x10, 0xFF);

    rtl_rm69330_qspi_cmd_param(0xFE, 0x01);

    rtl_rm69330_qspi_cmd_param(0x42, 0x14);

    rtl_rm69330_qspi_cmd_param(0x43, 0x41);

    rtl_rm69330_qspi_cmd_param(0x44, 0x25);

    rtl_rm69330_qspi_cmd_param(0x45, 0x52);

    rtl_rm69330_qspi_cmd_param(0x46, 0x36);

    rtl_rm69330_qspi_cmd_param(0x47, 0x63);
    rtl_rm69330_qspi_cmd_param(0x48, 0x41);

    rtl_rm69330_qspi_cmd_param(0x49, 0x14);

    rtl_rm69330_qspi_cmd_param(0x4A, 0x52);

    rtl_rm69330_qspi_cmd_param(0x4B, 0x25);

    rtl_rm69330_qspi_cmd_param(0x4C, 0x63);

    rtl_rm69330_qspi_cmd_param(0x4D, 0x36);

    rtl_rm69330_qspi_cmd_param(0x4E, 0x36);
    rtl_rm69330_qspi_cmd_param(0x4F, 0x63);
    rtl_rm69330_qspi_cmd_param(0x50, 0x25);
    rtl_rm69330_qspi_cmd_param(0x51, 0x52);
    rtl_rm69330_qspi_cmd_param(0x52, 0x14);

    rtl_rm69330_qspi_cmd_param(0x53, 0x41);

    rtl_rm69330_qspi_cmd_param(0x54, 0x63);

    rtl_rm69330_qspi_cmd_param(0x55, 0x36);

    rtl_rm69330_qspi_cmd_param(0x56, 0x52);

    rtl_rm69330_qspi_cmd_param(0x57, 0x25);

    rtl_rm69330_qspi_cmd_param(0x58, 0x41);

    rtl_rm69330_qspi_cmd_param(0x59, 0x14);

    rtl_rm69330_qspi_cmd_param(0xFE, 0x04); //
    rtl_rm69330_qspi_cmd_param(0x5D, 0x01); //
    rtl_rm69330_qspi_cmd_param(0x75, 0x08); //

    rtl_rm69330_qspi_cmd_param(0xFE, 0x04);
    rtl_rm69330_qspi_cmd_param(0x5E, 0x9F);

    rtl_rm69330_qspi_cmd_param(0x5F, 0x43);

    rtl_rm69330_qspi_cmd_param(0x60, 0xFF);

    rtl_rm69330_qspi_cmd_param(0x61, 0xFF);

    rtl_rm69330_qspi_cmd_param(0x62, 0xFF);

    rtl_rm69330_qspi_cmd_param(0xFE, 0x04);

    rtl_rm69330_qspi_cmd_param(0x76, 0xFF);

    rtl_rm69330_qspi_cmd_param(0x77, 0xFF);

    rtl_rm69330_qspi_cmd_param(0x78, 0x10);

    rtl_rm69330_qspi_cmd_param(0x79, 0xF2);

    rtl_rm69330_qspi_cmd_param(0x7A, 0xFF);
    rtl_rm69330_qspi_cmd_param(0xFE, 0x04);

    rtl_rm69330_qspi_cmd_param(0x00, 0x8D);

    rtl_rm69330_qspi_cmd_param(0x01, 0x00);

    rtl_rm69330_qspi_cmd_param(0x02, 0x00);

    rtl_rm69330_qspi_cmd_param(0x03, 0x05);

    rtl_rm69330_qspi_cmd_param(0x04, 0x00);

    rtl_rm69330_qspi_cmd_param(0x05, 0x05);

    rtl_rm69330_qspi_cmd_param(0x06, 0x00);

    rtl_rm69330_qspi_cmd_param(0x07, 0x00);

    rtl_rm69330_qspi_cmd_param(0x08, 0x00);

    rtl_rm69330_qspi_cmd_param(0xFE, 0x04);

    rtl_rm69330_qspi_cmd_param(0x09, 0xCC);

    rtl_rm69330_qspi_cmd_param(0x0A, 0x00);
    rtl_rm69330_qspi_cmd_param(0x0B, 0x02);

    rtl_rm69330_qspi_cmd_param(0x0C, 0x00);

    rtl_rm69330_qspi_cmd_param(0x0D, 0x60);

    rtl_rm69330_qspi_cmd_param(0x0E, 0x06);

    rtl_rm69330_qspi_cmd_param(0x0F, 0x2C);

    rtl_rm69330_qspi_cmd_param(0x10, 0x53);

    rtl_rm69330_qspi_cmd_param(0x11, 0x00);

    rtl_rm69330_qspi_cmd_param(0xFE, 0x04);

    rtl_rm69330_qspi_cmd_param(0x12, 0xCC);

    rtl_rm69330_qspi_cmd_param(0x13, 0x00);
    rtl_rm69330_qspi_cmd_param(0x14, 0x02);

    rtl_rm69330_qspi_cmd_param(0x15, 0x00);

    rtl_rm69330_qspi_cmd_param(0x16, 0x60);

    rtl_rm69330_qspi_cmd_param(0x17, 0x05);

    rtl_rm69330_qspi_cmd_param(0x18, 0x2C);

    rtl_rm69330_qspi_cmd_param(0x19, 0x53);

    rtl_rm69330_qspi_cmd_param(0x1A, 0x00);

    rtl_rm69330_qspi_cmd_param(0xFE, 0x04);

    rtl_rm69330_qspi_cmd_param(0x1B, 0xDC);
    rtl_rm69330_qspi_cmd_param(0x1C, 0x00);

    rtl_rm69330_qspi_cmd_param(0x1D, 0x04);

    rtl_rm69330_qspi_cmd_param(0x1E, 0x02);

    rtl_rm69330_qspi_cmd_param(0x1F, 0x18);

    rtl_rm69330_qspi_cmd_param(0x20, 0x06);

    rtl_rm69330_qspi_cmd_param(0x21, 0x3D);

    rtl_rm69330_qspi_cmd_param(0x22, 0x75);

    rtl_rm69330_qspi_cmd_param(0x23, 0x00);
    rtl_rm69330_qspi_cmd_param(0xFE, 0x04);

    rtl_rm69330_qspi_cmd_param(0x24, 0xDC);

    rtl_rm69330_qspi_cmd_param(0x25, 0x00);

    rtl_rm69330_qspi_cmd_param(0x26, 0x04);

    rtl_rm69330_qspi_cmd_param(0x27, 0x02);

    rtl_rm69330_qspi_cmd_param(0x28, 0x18);

    rtl_rm69330_qspi_cmd_param(0x29, 0x04);

    rtl_rm69330_qspi_cmd_param(0x2A, 0x3D);

    rtl_rm69330_qspi_cmd_param(0x2B, 0x75);

    rtl_rm69330_qspi_cmd_param(0x2D, 0x00);

    rtl_rm69330_qspi_cmd_param(0xFE, 0x04);

    rtl_rm69330_qspi_cmd_param(0x53, 0x8A);
    rtl_rm69330_qspi_cmd_param(0x54, 0x78);

    rtl_rm69330_qspi_cmd_param(0x55, 0x08);

    rtl_rm69330_qspi_cmd_param(0x56, 0x0A);

    rtl_rm69330_qspi_cmd_param(0x58, 0x2A);

    rtl_rm69330_qspi_cmd_param(0x59, 0x00);

    rtl_rm69330_qspi_cmd_param(0x65, 0x02);
    rtl_rm69330_qspi_cmd_param(0x66, 0x0A);

    rtl_rm69330_qspi_cmd_param(0x67, 0x00);

    rtl_rm69330_qspi_cmd_param(0xFE, 0x07);

    rtl_rm69330_qspi_cmd_param(0x15, 0x04);

    //rtl_rm69330_qspi_cmd_param(0xFE,0x0a);
    //rtl_rm69330_qspi_cmd_param(0xc6,0x1f);
    //rtl_rm69330_qspi_cmd_param(0xc7,0x80);

    rtl_rm69330_qspi_cmd_param(0xFE, 0x00);

    rtl_rm69330_qspi_cmd_param(0xC4, 0x80);
    rtl_rm69330_qspi_cmd_param(0x35, 0x00);


    rtl_rm69330_qspi_cmd_param(0x51, 0xAF);
    rtl_rm69330_qspi_cmd_param(0x3A, 0x55); //RGB 565
    //rtl_rm69330_qspi_cmd_param(0x3A,0x77);   //RGB888

    rtl_rm69330_qspi_write_cmd(0x11);
    rtl_rm69330_qspi_write_cmd(0x29);
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
void rtl_gui_dma_single_block_init(uint32_t dir_type)
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

    if (dir_type == GDMA_DIR_MemoryToMemory)
    {
        GDMA_InitStruct.GDMA_DIR                 = dir_type;
        GDMA_InitStruct.GDMA_SourceInc           = DMA_SourceInc_Inc;
        GDMA_InitStruct.GDMA_DestinationInc      = DMA_DestinationInc_Inc;
    }
    else if (dir_type == GDMA_DIR_MemoryToPeripheral)
    {
        GDMA_InitStruct.GDMA_DestHandshake       = GDMA_Handshake_8080_TX;
        GDMA_InitStruct.GDMA_DIR                 = dir_type;
        GDMA_InitStruct.GDMA_SourceInc           = DMA_SourceInc_Inc;
        GDMA_InitStruct.GDMA_DestinationInc      = DMA_DestinationInc_Fix;
    }

    GDMA_Init(LCD_DMA_CHANNEL_INDEX, &GDMA_InitStruct);
    GDMA_INTConfig(LCD_DMA_CHANNEL_NUM, GDMA_INT_Transfer, ENABLE);
}


void rtl_gui_dma_single_block_start(uint32_t destination_addr, uint32_t source_addr, uint32_t len)
{
    GDMA_SetBufferSize(LCD_DMA_CHANNEL_INDEX, len >> 2);
    if (destination_addr != 0)
    {
        GDMA_SetDestinationAddress(LCD_DMA_CHANNEL_INDEX, destination_addr);
    }
    else
    {
        GDMA_SetDestinationAddress(LCD_DMA_CHANNEL_INDEX, (uint32_t)(&(SPIC2->dr[0].word)));
    }
    GDMA_SetSourceAddress(LCD_DMA_CHANNEL_INDEX, (uint32_t)(source_addr));
    GDMA_Cmd(LCD_DMA_CHANNEL_NUM, ENABLE);
}

void rtl_gui_wait_dma_transfer(void)
{
    while (GDMA_GetTransferINTStatus(LCD_DMA_CHANNEL_NUM) != SET);
    GDMA_ClearINTPendingBit(LCD_DMA_CHANNEL_NUM, GDMA_INT_Transfer);
}

void rtl_gui_wait_lcd_control_transfer(uint32_t count)
{
    while (GDMA_GetTransferINTStatus(LCD_DMA_CHANNEL_NUM) != SET);
    GDMA_ClearINTPendingBit(LCD_DMA_CHANNEL_NUM, GDMA_INT_Transfer);
    return; //return for qspi
}

void rtl_gui_lcd_sectionconfig(uint16_t xStart, uint16_t yStart, uint16_t xEnd, uint16_t yEnd)
{
    lcd_rm69330_qspi_390_set_window(xStart, yStart, xEnd, yEnd);
}

void lcd_dma_set_multi_block_for_y(uint32_t picture0_addr, uint32_t picture1_addr,
                                   uint32_t offset,
                                   uint32_t yStart, uint32_t yEnd)//offset mean left menu not display in current menu
{
//    GDMA_LLIStruct = (GDMA_LLIDef *)dma_link_list_buff;
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
    GDMA_InitStruct.GDMA_BufferSize          = LCD_WIDTH * PIXEL_BYTES / 4;
    GDMA_InitStruct.GDMA_SourceInc           = DMA_SourceInc_Inc;
    GDMA_InitStruct.GDMA_DestinationInc      = DMA_DestinationInc_Fix;
    GDMA_InitStruct.GDMA_SourceDataSize      = GDMA_DataSize_Word;
    GDMA_InitStruct.GDMA_DestinationDataSize = GDMA_DataSize_Word;
    GDMA_InitStruct.GDMA_SourceMsize         = GDMA_Msize_8;
    GDMA_InitStruct.GDMA_DestinationMsize    = GDMA_Msize_1;
    GDMA_InitStruct.GDMA_SourceAddr          = 0;
    GDMA_InitStruct.GDMA_DestinationAddr     = (uint32_t)(&(SPIC2->dr[0].word));
    GDMA_InitStruct.GDMA_DestHandshake       = GDMA_Handshake_SPIC2_TX;

    GDMA_InitStruct.GDMA_Multi_Block_Mode   = LLI_TRANSFER;//LLI_TRANSFER;
    GDMA_InitStruct.GDMA_Multi_Block_En     = 1;
    GDMA_InitStruct.GDMA_Multi_Block_Struct = (uint32_t)GDMA_LLIStruct;


    for (uint32_t i = 0; i < LCD_HIGHT; i++)
    {
        GDMA_LLIStruct[i].DAR = (uint32_t)(&(SPIC2->dr[0].word));
        GDMA_LLIStruct[i].LLP = (uint32_t)&GDMA_LLIStruct[i + 1];
        /* Configure low 32 bit of CTL register */
        GDMA_LLIStruct[i].CTL_LOW = BIT(0)
                                    | (GDMA_InitStruct.GDMA_DestinationDataSize << 1)
                                    | (GDMA_InitStruct.GDMA_SourceDataSize << 4)
                                    | (GDMA_InitStruct.GDMA_DestinationInc << 7)
                                    | (GDMA_InitStruct.GDMA_SourceInc << 9)
                                    | (GDMA_InitStruct.GDMA_DestinationMsize << 11)
                                    | (GDMA_InitStruct.GDMA_SourceMsize << 14)
                                    | (GDMA_InitStruct.GDMA_DIR << 20)
                                    | (GDMA_InitStruct.GDMA_Multi_Block_Mode & LLP_SELECTED_BIT);
        /* Configure high 32 bit of CTL register */
        GDMA_LLIStruct[i].CTL_HIGH = LCD_WIDTH * PIXEL_BYTES / 4;
    }
    for (uint32_t i = 0; i < LCD_HIGHT; i++)
    {
        if (i < (LCD_HIGHT - offset))
        {
            GDMA_LLIStruct[i].SAR = picture0_addr + offset * PIXEL_BYTES * LCD_WIDTH + i * PIXEL_BYTES *
                                    LCD_WIDTH;
        }
        else
        {
            GDMA_LLIStruct[i].SAR = picture1_addr + (i + offset - LCD_HIGHT) * PIXEL_BYTES *
                                    LCD_WIDTH;
        }
    }
    GDMA_LLIStruct[LCD_HIGHT - 1].LLP = 0;
    GDMA_LLIStruct[LCD_HIGHT - 1].CTL_LOW = BIT(0)
                                            | (GDMA_InitStruct.GDMA_DestinationDataSize << 1)
                                            | (GDMA_InitStruct.GDMA_SourceDataSize << 4)
                                            | (GDMA_InitStruct.GDMA_DestinationInc << 7)
                                            | (GDMA_InitStruct.GDMA_SourceInc << 9)
                                            | (GDMA_InitStruct.GDMA_DestinationMsize << 11)
                                            | (GDMA_InitStruct.GDMA_SourceMsize << 14)
                                            | (GDMA_InitStruct.GDMA_DIR << 20);


    GDMA_Init(LCD_DMA_CHANNEL_INDEX, &GDMA_InitStruct);
    GDMA_INTConfig(LCD_DMA_CHANNEL_NUM, GDMA_INT_Transfer, ENABLE);
    GDMA_Cmd(LCD_DMA_CHANNEL_NUM, ENABLE);
}
static GDMA_LLIDef GDMA_LLIStruct_LAST;
void GDMA_Config_LLIStruct(uint32_t g1_addr, uint32_t g2_addr, \
                           uint32_t offset, \
                           GDMA_InitTypeDef *GDMA_InitStruct)//offset mean up menu not display in current menu
{
    GDMA_LLIDef GDMA_LLIStruct_G1;
    GDMA_LLIDef GDMA_LLIStruct_G2;

    IF8080_GDMALLIOFTTypeDef GDMA_LLIStruct_G1_oft;
    IF8080_GDMALLIOFTTypeDef GDMA_LLIStruct_G2_oft;

    GDMA_LLIStruct_G1_oft.SAR_OFT = LCD_WIDTH * PIXEL_BYTES;
    GDMA_LLIStruct_G1_oft.DAR_OFT = 0;
    GDMA_LLIStruct_G2_oft.SAR_OFT = LCD_WIDTH * PIXEL_BYTES;
    GDMA_LLIStruct_G2_oft.DAR_OFT = 0;

    GDMA_LLIStruct_G1.SAR = (uint32_t)(g1_addr + offset * PIXEL_BYTES);
    GDMA_LLIStruct_G1.DAR = (uint32_t)(&(SPIC2->dr[0].word));
    GDMA_LLIStruct_G1.LLP = 0;
    /* configure low 32 bit of CTL register */
    GDMA_LLIStruct_G1.CTL_LOW = BIT(0)
                                | (GDMA_InitStruct->GDMA_DestinationDataSize << 1)
                                | (GDMA_InitStruct->GDMA_SourceDataSize << 4)
                                | (GDMA_InitStruct->GDMA_DestinationInc << 7)
                                | (GDMA_InitStruct->GDMA_SourceInc << 9)
                                | (GDMA_InitStruct->GDMA_DestinationMsize << 11)
                                | (GDMA_InitStruct->GDMA_SourceMsize << 14)
                                | (GDMA_InitStruct->GDMA_DIR << 20)
                                | (GDMA_InitStruct->GDMA_Multi_Block_Mode & LLP_SELECTED_BIT);
    /* configure high 32 bit of CTL register */ /* this means buffer size */
    GDMA_LLIStruct_G1.CTL_HIGH = LCD_WIDTH * PIXEL_BYTES / 4 - offset * PIXEL_BYTES / 4;

    GDMA_LLIStruct_G2.SAR = (uint32_t)(g2_addr);
    GDMA_LLIStruct_G2.DAR = (uint32_t)(&(SPIC2->dr[0].word));
    GDMA_LLIStruct_G2.LLP = 0;
    /* configure low 32 bit of CTL register */
    GDMA_LLIStruct_G2.CTL_LOW = BIT(0)
                                | (GDMA_InitStruct->GDMA_DestinationDataSize << 1)
                                | (GDMA_InitStruct->GDMA_SourceDataSize << 4)
                                | (GDMA_InitStruct->GDMA_DestinationInc << 7)
                                | (GDMA_InitStruct->GDMA_SourceInc << 9)
                                | (GDMA_InitStruct->GDMA_DestinationMsize << 11)
                                | (GDMA_InitStruct->GDMA_SourceMsize << 14)
                                | (GDMA_InitStruct->GDMA_DIR << 20)
                                | (GDMA_InitStruct->GDMA_Multi_Block_Mode & LLP_SELECTED_BIT);
    /* configure high 32 bit of CTL register */
    GDMA_LLIStruct_G2.CTL_HIGH = offset * PIXEL_BYTES / 4;

    GDMA_LLIStruct_LAST.LLP = 0;
    GDMA_LLIStruct_LAST.SAR = (uint32_t)(g2_addr + LCD_WIDTH * (LCD_HIGHT - 1) * PIXEL_BYTES);
    GDMA_LLIStruct_LAST.DAR = (uint32_t)(&(SPIC2->dr[0].word));
    /* configure low 32 bit of CTL register */
    GDMA_LLIStruct_LAST.CTL_LOW = BIT(0)
                                  | (GDMA_InitStruct->GDMA_DestinationDataSize << 1)
                                  | (GDMA_InitStruct->GDMA_SourceDataSize << 4)
                                  | (GDMA_InitStruct->GDMA_DestinationInc << 7)
                                  | (GDMA_InitStruct->GDMA_SourceInc << 9)
                                  | (GDMA_InitStruct->GDMA_DestinationMsize << 11)
                                  | (GDMA_InitStruct->GDMA_SourceMsize << 14)
                                  | (GDMA_InitStruct->GDMA_DIR << 20);
    /* configure high 32 bit of CTL register */
    GDMA_LLIStruct_LAST.CTL_HIGH = offset * PIXEL_BYTES / 4;

    IF8080_GDMALLIConfig((IF8080_GDMALLITypeDef *)(&GDMA_LLIStruct_G1),
                         (IF8080_GDMALLITypeDef *)(&GDMA_LLIStruct_G2),
                         (IF8080_GDMALLIOFTTypeDef *)(&GDMA_LLIStruct_G1_oft),
                         (IF8080_GDMALLIOFTTypeDef *)(&GDMA_LLIStruct_G2_oft),
                         LCD_HIGHT * 2 - 1,
                         (uint32_t)(&GDMA_LLIStruct_LAST));
}
void lcd_dma_set_multi_block_for_x(uint32_t picture0_addr, uint32_t picture1_addr,
                                   uint32_t offset,
                                   uint32_t yStart, uint32_t yEnd)//offset mean up menu not display in current menu
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
    GDMA_InitStruct.GDMA_BufferSize          = LCD_WIDTH * PIXEL_BYTES / 4;
    GDMA_InitStruct.GDMA_SourceInc           = DMA_SourceInc_Inc;
    GDMA_InitStruct.GDMA_DestinationInc      = DMA_DestinationInc_Fix;
    GDMA_InitStruct.GDMA_SourceDataSize      = GDMA_DataSize_Word;
    GDMA_InitStruct.GDMA_DestinationDataSize = GDMA_DataSize_Word;
    GDMA_InitStruct.GDMA_SourceMsize         = GDMA_Msize_8;
    GDMA_InitStruct.GDMA_DestinationMsize    = GDMA_Msize_1;
    GDMA_InitStruct.GDMA_SourceAddr          = 0;
    GDMA_InitStruct.GDMA_DestinationAddr     = (uint32_t)(&(SPIC2->dr[0].word));
    GDMA_InitStruct.GDMA_DestHandshake       = GDMA_Handshake_SPIC2_TX;

    GDMA_InitStruct.GDMA_Multi_Block_Mode   = LLI_TRANSFER;//LLI_TRANSFER;
    GDMA_InitStruct.GDMA_Multi_Block_En     = 1;

    GDMA_InitStruct.GDMA_Multi_Block_Struct  = (uint32_t)IF8080_LLI_REG1_GDMA_BASE;

#if PIXEL_FORMAT == RGB16BIT_565
    offset = (offset >> 1) << 1;
#elif PIXEL_FORMAT == RGB24BIT_888
    offset = (offset >> 2) << 2;
#endif
    GDMA_Init(LCD_DMA_CHANNEL_INDEX, &GDMA_InitStruct);

    GDMA_Config_LLIStruct(picture0_addr, picture1_addr, offset, &GDMA_InitStruct);

    GDMA_INTConfig(LCD_DMA_CHANNEL_NUM, GDMA_INT_Transfer, ENABLE);
    GDMA_Cmd(LCD_DMA_CHANNEL_NUM, ENABLE);
}


void rtl_gui_lcd_clear(uint16_t xStart, uint16_t yStart, uint16_t xEnd, uint16_t yEnd,
                       uint32_t color)
{
    static uint32_t color_buf = 0;
    color_buf = (color >> 8) | (color << 8);
    color_buf = color_buf | color_buf << 16;
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

    GDMA_InitStruct.GDMA_DIR                 = GDMA_DIR_MemoryToPeripheral;
    GDMA_InitStruct.GDMA_SourceInc           = DMA_SourceInc_Fix;
    GDMA_InitStruct.GDMA_DestinationInc      = DMA_DestinationInc_Fix;
    GDMA_InitStruct.GDMA_DestHandshake       = GDMA_Handshake_8080_TX;

    GDMA_Init(LCD_DMA_CHANNEL_INDEX, &GDMA_InitStruct);
    GDMA_INTConfig(LCD_DMA_CHANNEL_NUM, GDMA_INT_Transfer, ENABLE);

    rtl_gui_lcd_sectionconfig(xStart, yStart, xEnd, yEnd);

    for (uint8_t i = 0; i < TOTAL_SECTION_COUNT - 1; i++)
    {
        rtl_gui_dma_single_block_start(NULL, (uint32_t)&color_buf,
                                       LCD_WIDTH * LCD_SECTION_HEIGHT * PIXEL_BYTES);
        rtl_gui_wait_dma_transfer();
    }
    uint32_t last_len = 0;
    if (LCD_HIGHT % LCD_SECTION_HEIGHT == 0)
    {
        last_len = LCD_SECTION_HEIGHT * LCD_WIDTH * PIXEL_BYTES;
    }
    else
    {
        last_len = (LCD_HIGHT % LCD_SECTION_HEIGHT) * LCD_WIDTH * PIXEL_BYTES;
    }
    rtl_gui_dma_single_block_start(NULL, (uint32_t)&color_buf, last_len);
    rtl_gui_wait_dma_transfer();
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
void lcd_pad_init(void)
{
    Pinmux_Config(P4_3, IDLE_MODE); //CS
    Pinmux_Config(P4_0, IDLE_MODE); //CLK
    Pinmux_Config(P4_2, IDLE_MODE); //D0
    Pinmux_Config(P4_1, IDLE_MODE); //D1
    Pinmux_Config(P0_6, IDLE_MODE); //D2
    Pinmux_Config(P0_7, IDLE_MODE); //D3

    Pad_Config(P4_3, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(P4_0, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(P4_2, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(P4_1, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(P0_7, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(P0_6, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);

    /*BL AND RESET ARE NOT FIX*/
    /*BL AND RESET ARE NOT FIX*/

    Pad_Config(QSPI_LCD_POWER, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);

}
void lcd_device_init(void)
{
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
    TIM_InitStruct.TIM_SOURCE_DIV = TIM_CLOCK_DIVIDER_4;
    TIM_TimeBaseInit(BL_PWM_TIM, &TIM_InitStruct);

    TIM_Cmd(BL_PWM_TIM, DISABLE);


    lcd_pad_init();
    //do qspi init here
    spic_2_qspi_init();
    RCC_PeriphClockCmd(APBPeriph_IF8080, APBPeriph_IF8080_CLOCK, DISABLE);
    RCC_PeriphClockCmd(APBPeriph_IF8080, APBPeriph_IF8080_CLOCK, ENABLE);
}

void lcd_set_backlight(uint32_t percent)
{
    Pad_Config(QSPI_LCD_POWER, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);
    return;
}
void lcd_set_reset(bool reset)
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
void lcd_te_enable(void)
{
    GPIO_MaskINTConfig(GPIO_GetPin(LCD_FRAME_SYNC), ENABLE);
    GPIO_INTConfig(GPIO_GetPin(LCD_FRAME_SYNC), ENABLE);
    GPIO_ClearINTPendingBit(GPIO_GetPin(LCD_FRAME_SYNC));
    GPIO_MaskINTConfig(GPIO_GetPin(LCD_FRAME_SYNC), DISABLE);
}

void lcd_te_disable(void)
{
    GPIO_INTConfig(GPIO_GetPin(LCD_FRAME_SYNC), DISABLE);
    GPIO_MaskINTConfig(GPIO_GetPin(LCD_FRAME_SYNC), ENABLE);
    GPIO_ClearINTPendingBit(GPIO_GetPin(LCD_FRAME_SYNC));
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


    GPIO_INTConfig(GPIO_GetPin(LCD_FRAME_SYNC), DISABLE);
    GPIO_MaskINTConfig(GPIO_GetPin(LCD_FRAME_SYNC), ENABLE);
    GPIO_ClearINTPendingBit(GPIO_GetPin(LCD_FRAME_SYNC));

    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = GPIO_FRAME_SYNC_IRQ;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 3;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);
}

void lcd_enter_dlps(void)
{
    return;
}
void lcd_exit_dlps(void)
{
    lcd_device_init();
}
void rtl_gui_lcd_power_on(void)
{
    rtl_rm69330_qspi_write_cmd(0x11);
    rtl_rm69330_qspi_write_cmd(0x29);
}

void rtl_gui_lcd_power_off(void)
{
    rtl_rm69330_qspi_write_cmd(0x28);
    rtl_rm69330_qspi_write_cmd(0x10);
}
void lcd_driver_init(void)
{
    lcd_device_init();
    lcd_set_reset(true);
    platform_delay_ms(50);
    lcd_set_reset(false);
    platform_delay_ms(50);
    lcd_te_device_init();
    lcd_rm69330_qspi_390_init();

}



