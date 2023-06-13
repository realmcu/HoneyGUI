#include "board.h"
#include "rtl876x_rcc.h"
#include "rtl876x_tim.h"
#include "rtl876x_if8080.h"
#include "rtl876x_gpio.h"
#include "rtl876x_pinmux.h"
#include "rtl876x_gdma.h"
#include "rtl876x_nvic.h"
#include "trace.h"
#include "platform_utils.h"
#include "spic_2_qspi.h"


#define BIT_CMD_CH(x)           (((x) & 0x00000003) << 20)
#define BIT_DATA_CH(x)          (((x) & 0x00000003) << 18)
#define BIT_ADDR_CH(x)          (((x) & 0x00000003) << 16)
#define BIT_TMOD(x)             (((x) & 0x00000003) << 8)


#define BIT_RFNE                                (0x00000001 << 3)
#define BIT_ADDR_PHASE_LENGTH(x)                ((x) & 0x00000003)
#define BIT_RD_DUMMY_LENGTH(x)                  (((x) & 0x00000fff))

#define SO_DNUM_POS                    (0)
#define SO_DNUM_MSK                    (0x1 << SO_DNUM_POS)
#define SO_DNUM_CLR                    (~SO_DNUM_MSK)

#define LCD_QSPI_RST P2_0

void lcd_qspi_init(void)
{
    /* module init */
    RCC_PeriphClockCmd(APBPeriph_FLASH2, APBPeriph_FLASH2_CLOCK, (FunctionalState)ENABLE);

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
    Pad_Config(P0_6, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(P0_7, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);

    /* spic2 enable controller */
    SYSBLKCTRL->u_210.BITS_210.BIT_SOC_FLASH_EN2 = 1;
    /* spic2 master enable */
    SYSBLKCTRL->u_2A8.BITS_2A8.SPIC2_MASTER_EN = 1;

    /* RL6660_ECO_029 : Add option for SPIC2 TX DMA handshake signal */
    HAL_WRITE32(PERIPH_REG_BASE, 0x300, HAL_READ32(PERIPH_REG_BASE, 0x300) | BIT31);

    /* set cs and baudrate */
    SPIC2->ssienr = 0;
    SPIC2->ser = BIT0;
    SPIC2->baudr = 9;//read need low clock

    /* ctrlr2[17:16] = 2'b11 to remove WREN+RDSR (i.e., only PP cmd issue) */
    SPIC2->ctrlr2 |= (BIT16 | BIT17);
}

bool lcd_qspi_read(uint16_t addr, uint16_t data_len, uint8_t *data)
{
    uint32_t rd_num = 0;
    SPIC2->ssienr = DISABLE;

    //set addr channel and data channel in ctrl0 register.
    SPIC2->ctrlr0 &= ~(BIT_CMD_CH(3) | BIT_ADDR_CH(3) | BIT_DATA_CH(3));

    SPIC2->ctrlr2 &= SO_DNUM_CLR;//received data bit 0: SIO0 or bit 1:SIO1
    //set RX mode in ctrl0 register in user mode.
    SPIC2->ctrlr0 |= BIT_TMOD(3);
    //spic_set_ctrl1(data_len);
    SPIC2->ctrlr1 = data_len;

    SPIC2->addr_length = BIT_ADDR_PHASE_LENGTH(3);
    //set dummy_cycle in autolen register[11:0]
    HAL_UPDATE32(&SPIC2->auto_length, 0xfff, BIT_RD_DUMMY_LENGTH(5));

    addr = addr << 8;
    SPIC2->dr[0].word = 0x03 | __REV(addr);

    SPIC2->ssienr = ENABLE;

    while (rd_num < data_len)
    {
        if (SPIC2->sr & BIT_RFNE)
        {
            data[rd_num++] = SPIC2->dr[0].byte;
        }
    }

    SPIC2->ssienr = DISABLE;

    return true;
}

void lcd_qspi_read_test(void)
{
    //Pad_Config(P4_3, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_DOWN, PAD_OUT_ENABLE, PAD_OUT_LOW);
    uint8_t buf[10] = {0xff, 0xff};
    lcd_qspi_read(0x0C, 1, buf);//SH8601A read result 0x77
    DBG_DIRECT("[QSPI READ] 0x%x", buf[0]);
    //Pad_Config(P4_3, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
    while (1);
}

void lcd_qspi_set_reset(bool reset)
{
    if (reset)
    {
        Pad_Config(LCD_QSPI_RST, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_DOWN, PAD_OUT_ENABLE, PAD_OUT_LOW);
    }
    else
    {
        Pad_Config(LCD_QSPI_RST, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);
    }
}

void lcd_qspi_driver_init(void)
{
    lcd_qspi_init();

    lcd_qspi_set_reset(false);
    platform_delay_ms(100);
    lcd_qspi_set_reset(true);
    platform_delay_ms(50);
    lcd_qspi_set_reset(false);
    platform_delay_ms(50);

    lcd_qspi_read_test();
}



