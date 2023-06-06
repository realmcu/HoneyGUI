#include "LPM013M126A_176176_spi.h"
#include "LPM013M126A_176176_spi_data.h"
#include "rtl_lcdc_dbic.h"
#include "utils.h"
#include "rtl_pinmux.h"
#include "rtl_gpio.h"
#include "rtl_rcc.h"
#include "trace.h"
#include "drv_gpio.h"
#include "rtl_spi.h"


#define LPM_SCK_PIN                                     P4_0
#define LPM_MOSI_PIN                                    P4_2
#define LPM_CS_PIN                                      P4_3
#define LPM_DISP_PIN                                    P4_1
#define LPM_EXTCOMIN_PIN                                P0_6
static void st7701s_spi_init(void)
{
    Pad_Config(LPM_MOSI_PIN, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);
    Pad_Config(LPM_SCK_PIN, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);
    Pinmux_Deinit(LPM_MOSI_PIN);
    Pinmux_Deinit(LPM_SCK_PIN);
    Pinmux_Config(LPM_SCK_PIN, SPI0_CLK_MASTER);
    Pinmux_Config(LPM_MOSI_PIN, SPI0_MO_MASTER);
    RCC_PeriphClockCmd(APBPeriph_SPI0, APBPeriph_SPI0_CLOCK, ENABLE);
    SPI_InitTypeDef spi_init_struct;
    SPI_StructInit(&spi_init_struct);
    spi_init_struct.SPI_CPOL = SPI_CPOL_Low;
    spi_init_struct.SPI_CPHA = SPI_CPHA_1Edge;
    spi_init_struct.SPI_Direction = SPI_Direction_TxOnly;
    spi_init_struct.SPI_DataSize = SPI_DataSize_16b;
    spi_init_struct.SPI_BaudRatePrescaler = 64;
    spi_init_struct.SPI_FrameFormat = SPI_Frame_Motorola;
    SPI_Init(SPI0, &spi_init_struct);
    SPI_Cmd(SPI0, ENABLE);
}
static void disp_on()
{
    drv_pin_mode(LPM_DISP_PIN, PIN_MODE_OUTPUT);
    drv_pin_write(LPM_DISP_PIN, 1);
}
static void disp_off()
{
    drv_pin_mode(LPM_DISP_PIN, PIN_MODE_OUTPUT);
    drv_pin_write(LPM_DISP_PIN, 0);
}
static void extcomin_off()
{
    drv_pin_mode(LPM_EXTCOMIN_PIN, PIN_MODE_OUTPUT);
    drv_pin_write(LPM_EXTCOMIN_PIN, 0);
}
static void cs_off()
{
    drv_pin_mode(LPM_CS_PIN, PIN_MODE_OUTPUT);
    drv_pin_write(LPM_CS_PIN, 0);
}
static void cs_on()
{
    drv_pin_mode(LPM_CS_PIN, PIN_MODE_OUTPUT);
    drv_pin_write(LPM_CS_PIN, 1);
}
static void si_off()
{
    drv_pin_mode(LPM_MOSI_PIN, PIN_MODE_OUTPUT);
    drv_pin_write(LPM_MOSI_PIN, 0);
}
static bool judge_bit1(uint16_t number, uint16_t bit_index)
{
    int num = number;
    int k = bit_index;

    int mask = 1 << (k);
    if (num & mask)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
static void write_command(uint32_t cmd)
{
//    GPIO_WriteBit(GPIO_GetPort(SPI0_CS_PIN), GPIO_GetPin(SPI0_CS_PIN), (BitAction)(0));
    //platform_delay_us(300);

    SPI_SendData(SPI0, cmd);
//    platform_delay_us(2);
//    GPIO_WriteBit(GPIO_GetPort(SPI0_CS_PIN), GPIO_GetPin(SPI0_CS_PIN), (BitAction)(1));

}
static void blit_565_to_1110(const unsigned char *pixel_565)
{
    for (int j = 0; j < 176; j++)
    {
        write_command(36864 + j);
        for (int i = 0; i < 44; i++)
        {
            uint16_t output = 0;
            for (int n = 0; n < 4; n++)
            {
                uint16_t p1 = (pixel_565[i * 8 + 1 + 2 * n + j * 176 * 2] << 8) + pixel_565[i * 8 + 0 + 2 * n + j *
                                                                                            176 * 2];

                if (judge_bit1(p1, 15)) { output = output | (1 << (15 - 4 * n)); }
                if (judge_bit1(p1, 10)) { output = output | (1 << (14 - 4 * n)); }
                if (judge_bit1(p1, 4)) { output = output | (1 << (13 - 4 * n)); }

            }

            write_command(output);
        }
    }
    write_command(0);
}
static void blit_565_to_1110_length(const unsigned char *pixel_565, uint32_t l)
{
    for (int j = 0; j < l / 176; j++)
    {
        write_command(36864 + j);
        for (int i = 0; i < 44; i++)
        {
            uint16_t output = 0;
            for (int n = 0; n < 4; n++)
            {
                uint16_t p1 = (pixel_565[i * 8 + 1 + 2 * n + j * 176 * 2] << 8) + pixel_565[i * 8 + 0 + 2 * n + j *
                                                                                            176 * 2];

                if (judge_bit1(p1, 15)) { output = output | (1 << (15 - 4 * n)); }
                if (judge_bit1(p1, 10)) { output = output | (1 << (14 - 4 * n)); }
                if (judge_bit1(p1, 4)) { output = output | (1 << (13 - 4 * n)); }

            }

            write_command(output);
        }
    }
    write_command(0);
}
void rtk_lcd_hal_init(void)
{
    cs_off();
    extcomin_off();
    disp_off();
    si_off();
    st7701s_spi_init();
    platform_delay_us(10000);
    disp_on();
    platform_delay_us(400);
    cs_on();
#define PICTURE_BIN_HEAD_LENGTH 8
    blit_565_to_1110(rect849 + PICTURE_BIN_HEAD_LENGTH);
    platform_delay_us(300);
    cs_off();
}
void rtk_lcd_hal_set_window(uint16_t xStart, uint16_t yStart, uint16_t w, uint16_t h)
{
    return;
}
void rtk_lcd_hal_update_framebuffer(uint8_t *buf, uint32_t len)
{
    cs_off();
    extcomin_off();
    disp_off();
    si_off();
    st7701s_spi_init();
    platform_delay_us(10000);
    disp_on();
    platform_delay_us(400);
    cs_on();
#define PICTURE_BIN_HEAD_LENGTH 8
    blit_565_to_1110_length(buf, len);
    platform_delay_us(300);
    cs_off();
    return;
}
void rtk_lcd_hal_rect_fill(uint16_t xStart, uint16_t yStart, uint16_t w, uint16_t h,
                           uint32_t color)
{
    cs_off();
    extcomin_off();
    //disp_off();
    si_off();
    st7701s_spi_init();
    platform_delay_us(10000);
    disp_on();
    platform_delay_us(400);
    cs_on();
    {

        for (int j = yStart; j < yStart + h; j++)
        {
            write_command(36864 + j);
            for (int i = 0; i < 44; i++)
            {
                uint16_t output = 0;
                for (int n = 0; n < 4; n++)
                {
                    uint16_t p1 = 0;
                    if (i * 4 + n >= xStart && i * 4 + n <= (xStart + w))
                    {
                        p1 = (uint16_t)color;
                    }
                    if (judge_bit1(p1, 15)) { output = output | (1 << (15 - 4 * n)); }
                    if (judge_bit1(p1, 10)) { output = output | (1 << (14 - 4 * n)); }
                    if (judge_bit1(p1, 4)) { output = output | (1 << (13 - 4 * n)); }

                }

                write_command(output);
            }
        }
        write_command(0);

    }

    platform_delay_us(300);
    //cs_off();


    return;
}
void rtk_lcd_hal_start_transfer(uint8_t *buf, uint32_t len)
{
    return;
}
void rtk_lcd_hal_transfer_done(void)
{
    return;
}
uint32_t rtk_lcd_hal_get_width(void)
{
    return LPM013M126A_LCD_WIDTH;
}
uint32_t rtk_lcd_hal_get_height(void)
{
    return LPM013M126A_LCD_HIGHT;
}
uint32_t rtk_lcd_hal_get_pixel_bits(void)
{
    return LPM013M126A_DRV_PIXEL_BITS;
}
