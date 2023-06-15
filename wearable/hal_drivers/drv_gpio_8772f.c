/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author            Notes
 * 2018-11-06     howie        first version
 */


void drv_pin_clk_set(uint32_t pin)
{
    if (GPIO_GetPort(pin) == GPIOA)
    {
        RCC_PeriphClockCmd(APBPeriph_GPIOA, APBPeriph_GPIOA_CLOCK, ENABLE);
    }
    else if (GPIO_GetPort(pin) == GPIOB)
    {
        RCC_PeriphClockCmd(APBPeriph_GPIOB, APBPeriph_GPIOB_CLOCK, ENABLE);
    }
    else if (GPIO_GetPort(pin) == GPIOC)
    {
        RCC_PeriphClockCmd(APBPeriph_GPIOC, APBPeriph_GPIOC_CLOCK, ENABLE);
    }
    else if (GPIO_GetPort(pin) == GPIOD)
    {
        RCC_PeriphClockCmd(APBPeriph_GPIOD, APBPeriph_GPIOD_CLOCK, ENABLE);
    }
}
void DRV_GPIO_A0_Handler(void) {DRV_GPIO_HANDLER(P0_0);}
void DRV_GPIO_A1_Handler(void) {DRV_GPIO_HANDLER(P0_1);}
void DRV_GPIO_A2_Handler(void) {DRV_GPIO_HANDLER(P0_2);}
void DRV_GPIO_A3_Handler(void) {DRV_GPIO_HANDLER(P0_3);}
void DRV_GPIO_A4_Handler(void) {DRV_GPIO_HANDLER(P0_4);}
void DRV_GPIO_A5_Handler(void) {DRV_GPIO_HANDLER(P0_5);}
void DRV_GPIO_A6_Handler(void) {DRV_GPIO_HANDLER(P0_6);}
void DRV_GPIO_A7_Handler(void) {DRV_GPIO_HANDLER(P1_7);}
void DRV_GPIO_A8_Handler(void) {DRV_GPIO_HANDLER(P1_0);}
void DRV_GPIO_A9_Handler(void) {DRV_GPIO_HANDLER(P1_1);}
void DRV_GPIO_A10_Handler(void) {DRV_GPIO_HANDLER(P1_2);}
void DRV_GPIO_A11_Handler(void) {DRV_GPIO_HANDLER(P1_3);}
void DRV_GPIO_A12_Handler(void) {DRV_GPIO_HANDLER(P1_4);}
void DRV_GPIO_A13_Handler(void) {DRV_GPIO_HANDLER(P1_5);}
void DRV_GPIO_A14_Handler(void) {DRV_GPIO_HANDLER(P1_6);}
void DRV_GPIO_A15_Handler(void) {DRV_GPIO_HANDLER(P1_7);}
void DRV_GPIO_A16_Handler(void) {DRV_GPIO_HANDLER(P18_0);}
void DRV_GPIO_A17_Handler(void) {DRV_GPIO_HANDLER(P18_1);}
void DRV_GPIO_A18_Handler(void) {DRV_GPIO_HANDLER(P18_2);}
void DRV_GPIO_A19_Handler(void) {DRV_GPIO_HANDLER(P18_3);}
void DRV_GPIO_A20_Handler(void) {DRV_GPIO_HANDLER(P18_4);}
void DRV_GPIO_A21_Handler(void) {DRV_GPIO_HANDLER(P18_5);}
void DRV_GPIO_A22_Handler(void) {DRV_GPIO_HANDLER(P18_6);}
void DRV_GPIO_A23_Handler(void) {DRV_GPIO_HANDLER(P18_7);}
void DRV_GPIO_A24_Handler(void) {DRV_GPIO_HANDLER(P6_0);}
void DRV_GPIO_A25_Handler(void) {DRV_GPIO_HANDLER(P6_1);}
void DRV_GPIO_A26_Handler(void) {DRV_GPIO_HANDLER(P6_2);}
void DRV_GPIO_A27_Handler(void) {DRV_GPIO_HANDLER(P6_3);}
void DRV_GPIO_A28_Handler(void) {DRV_GPIO_HANDLER(P6_4);}
void DRV_GPIO_A29_Handler(void) {DRV_GPIO_HANDLER(P6_5);}
void DRV_GPIO_A30_Handler(void) {DRV_GPIO_HANDLER(P6_6);}
void DRV_GPIO_A31_Handler(void) {DRV_GPIO_HANDLER(P6_7);}


void DRV_GPIO_B0_Handler(void) {DRV_GPIO_HANDLER(P7_0);}
void DRV_GPIO_B1_Handler(void) {DRV_GPIO_HANDLER(P7_1);}
void DRV_GPIO_B2_Handler(void) {DRV_GPIO_HANDLER(P7_2);}
void DRV_GPIO_B3_Handler(void) {DRV_GPIO_HANDLER(P7_3);}
void DRV_GPIO_B4_Handler(void) {DRV_GPIO_HANDLER(P7_4);}
void DRV_GPIO_B5_Handler(void) {DRV_GPIO_HANDLER(P7_5);}
void DRV_GPIO_B6_Handler(void) {DRV_GPIO_HANDLER(P7_6);}
void DRV_GPIO_B7_Handler(void) {DRV_GPIO_HANDLER(P7_7);}
void DRV_GPIO_B8_Handler(void) {DRV_GPIO_HANDLER(P8_0);}
void DRV_GPIO_B9_Handler(void) {DRV_GPIO_HANDLER(P8_1);}
void DRV_GPIO_B10_Handler(void) {DRV_GPIO_HANDLER(P8_2);}
void DRV_GPIO_B11_Handler(void) {DRV_GPIO_HANDLER(P8_3);}
void DRV_GPIO_B12_Handler(void) {DRV_GPIO_HANDLER(P8_4);}
void DRV_GPIO_B13_Handler(void) {DRV_GPIO_HANDLER(P8_5);}
void DRV_GPIO_B14_Handler(void) {DRV_GPIO_HANDLER(P8_6);}
void DRV_GPIO_B15_Handler(void) {DRV_GPIO_HANDLER(P8_7);}
void DRV_GPIO_B16_Handler(void) {DRV_GPIO_HANDLER(P5_0);}
void DRV_GPIO_B17_Handler(void) {DRV_GPIO_HANDLER(P5_1);}
void DRV_GPIO_B18_Handler(void) {DRV_GPIO_HANDLER(P5_2);}
void DRV_GPIO_B19_Handler(void) {DRV_GPIO_HANDLER(P5_3);}
void DRV_GPIO_B20_Handler(void) {DRV_GPIO_HANDLER(P5_4);}
void DRV_GPIO_B21_Handler(void) {DRV_GPIO_HANDLER(P5_5);}
void DRV_GPIO_B22_Handler(void) {DRV_GPIO_HANDLER(P5_6);}
void DRV_GPIO_B23_Handler(void) {DRV_GPIO_HANDLER(P5_7);}
void DRV_GPIO_B24_Handler(void) {DRV_GPIO_HANDLER(P19_0);}
void DRV_GPIO_B25_Handler(void) {DRV_GPIO_HANDLER(P19_1);}
void DRV_GPIO_B26_Handler(void) {DRV_GPIO_HANDLER(P19_2);}
void DRV_GPIO_B27_Handler(void) {DRV_GPIO_HANDLER(P19_3);}
void DRV_GPIO_B28_Handler(void) {DRV_GPIO_HANDLER(P19_4);}
void DRV_GPIO_B29_Handler(void) {DRV_GPIO_HANDLER(P19_5);}
void DRV_GPIO_B30_Handler(void) {DRV_GPIO_HANDLER(P19_6);}
void DRV_GPIO_B31_Handler(void) {DRV_GPIO_HANDLER(P19_7);}

void DRV_GPIO_C0_Handler(void) {DRV_GPIO_HANDLER(P9_0);}
void DRV_GPIO_C1_Handler(void) {DRV_GPIO_HANDLER(P9_1);}
void DRV_GPIO_C2_Handler(void) {DRV_GPIO_HANDLER(P9_2);}
void DRV_GPIO_C3_Handler(void) {DRV_GPIO_HANDLER(P9_3);}
void DRV_GPIO_C4_Handler(void) {DRV_GPIO_HANDLER(P9_4);}
void DRV_GPIO_C5_Handler(void) {DRV_GPIO_HANDLER(P9_5);}
void DRV_GPIO_C6_Handler(void) {DRV_GPIO_HANDLER(P9_6);}
void DRV_GPIO_C7_Handler(void) {DRV_GPIO_HANDLER(P9_7);}
void DRV_GPIO_C8_Handler(void) {DRV_GPIO_HANDLER(P10_0);}
void DRV_GPIO_C9_Handler(void) {DRV_GPIO_HANDLER(P10_1);}
void DRV_GPIO_C10_Handler(void) {DRV_GPIO_HANDLER(P10_2);}
void DRV_GPIO_C11_Handler(void) {DRV_GPIO_HANDLER(P10_3);}
void DRV_GPIO_C12_Handler(void) {DRV_GPIO_HANDLER(P10_4);}
void DRV_GPIO_C13_Handler(void) {DRV_GPIO_HANDLER(P10_5);}
void DRV_GPIO_C14_Handler(void) {DRV_GPIO_HANDLER(P10_6);}
void DRV_GPIO_C15_Handler(void) {DRV_GPIO_HANDLER(P10_7);}
void DRV_GPIO_C16_Handler(void) {DRV_GPIO_HANDLER(P11_0);}
void DRV_GPIO_C17_Handler(void) {DRV_GPIO_HANDLER(P11_1);}
void DRV_GPIO_C18_Handler(void) {DRV_GPIO_HANDLER(P11_2);}
void DRV_GPIO_C19_Handler(void) {DRV_GPIO_HANDLER(P11_3);}
void DRV_GPIO_C20_Handler(void) {DRV_GPIO_HANDLER(P11_4);}
void DRV_GPIO_C21_Handler(void) {DRV_GPIO_HANDLER(P11_5);}
void DRV_GPIO_C22_Handler(void) {DRV_GPIO_HANDLER(P11_6);}
void DRV_GPIO_C23_Handler(void) {DRV_GPIO_HANDLER(P11_7);}
void DRV_GPIO_C24_Handler(void) {DRV_GPIO_HANDLER(P12_0);}
void DRV_GPIO_C25_Handler(void) {DRV_GPIO_HANDLER(P12_1);}
void DRV_GPIO_C26_Handler(void) {DRV_GPIO_HANDLER(P12_2);}
void DRV_GPIO_C27_Handler(void) {DRV_GPIO_HANDLER(P12_3);}
void DRV_GPIO_C28_Handler(void) {DRV_GPIO_HANDLER(P12_4);}
void DRV_GPIO_C29_Handler(void) {DRV_GPIO_HANDLER(P12_5);}
void DRV_GPIO_C30_Handler(void) {DRV_GPIO_HANDLER(P12_6);}
void DRV_GPIO_C31_Handler(void) {DRV_GPIO_HANDLER(P12_7);}

void DRV_GPIO_D0_Handler(void) {DRV_GPIO_HANDLER(P13_0);}
void DRV_GPIO_D1_Handler(void) {DRV_GPIO_HANDLER(P13_1);}
void DRV_GPIO_D2_Handler(void) {DRV_GPIO_HANDLER(P13_2);}
void DRV_GPIO_D3_Handler(void) {DRV_GPIO_HANDLER(P13_3);}
void DRV_GPIO_D4_Handler(void) {DRV_GPIO_HANDLER(P13_4);}
void DRV_GPIO_D5_Handler(void) {DRV_GPIO_HANDLER(P13_5);}
void DRV_GPIO_D6_Handler(void) {DRV_GPIO_HANDLER(P13_6);}
void DRV_GPIO_D7_Handler(void) {DRV_GPIO_HANDLER(P13_7);}
void DRV_GPIO_D8_Handler(void) {DRV_GPIO_HANDLER(P14_0);}
void DRV_GPIO_D9_Handler(void) {DRV_GPIO_HANDLER(P14_1);}
void DRV_GPIO_D10_Handler(void) {DRV_GPIO_HANDLER(P14_2);}
void DRV_GPIO_D11_Handler(void) {DRV_GPIO_HANDLER(P14_3);}
void DRV_GPIO_D12_Handler(void) {DRV_GPIO_HANDLER(P14_4);}
void DRV_GPIO_D13_Handler(void) {DRV_GPIO_HANDLER(P14_5);}
void DRV_GPIO_D14_Handler(void) {DRV_GPIO_HANDLER(P14_6);}
void DRV_GPIO_D15_Handler(void) {DRV_GPIO_HANDLER(P14_7);}
void DRV_GPIO_D16_Handler(void) {DRV_GPIO_HANDLER(P15_0);}
void DRV_GPIO_D17_Handler(void) {DRV_GPIO_HANDLER(P15_1);}
void DRV_GPIO_D18_Handler(void) {DRV_GPIO_HANDLER(P15_2);}
void DRV_GPIO_D19_Handler(void) {DRV_GPIO_HANDLER(P15_3);}
void DRV_GPIO_D20_Handler(void) {DRV_GPIO_HANDLER(P15_4);}
void DRV_GPIO_D21_Handler(void) {DRV_GPIO_HANDLER(P15_5);}
void DRV_GPIO_D22_Handler(void) {DRV_GPIO_HANDLER(P15_6);}
void DRV_GPIO_D23_Handler(void) {DRV_GPIO_HANDLER(P15_7);}
void DRV_GPIO_D24_Handler(void) {DRV_GPIO_HANDLER(P16_0);}
void DRV_GPIO_D25_Handler(void) {DRV_GPIO_HANDLER(P16_1);}
void DRV_GPIO_D26_Handler(void) {DRV_GPIO_HANDLER(P16_2);}
void DRV_GPIO_D27_Handler(void) {DRV_GPIO_HANDLER(P16_3);}
void DRV_GPIO_D28_Handler(void) {DRV_GPIO_HANDLER(P16_4);}
void DRV_GPIO_D29_Handler(void) {DRV_GPIO_HANDLER(P16_5);}
void DRV_GPIO_D30_Handler(void) {DRV_GPIO_HANDLER(P16_6);}
void DRV_GPIO_D31_Handler(void) {DRV_GPIO_HANDLER(P16_7);}



const IRQn_Type table_irq[GPIO_NUM_MAX] =
{
    GPIOA0_IRQn,
    GPIOA1_IRQn,
    GPIOA2_IRQn,
    GPIOA3_IRQn,
    GPIOA4_IRQn,
    GPIOA5_IRQn,
    GPIOA6_IRQn,
    GPIOA7_IRQn,
    GPIOA8_IRQn,
    GPIOA9_IRQn,
    GPIOA10_IRQn,
    GPIOA11_IRQn,
    GPIOA12_IRQn,
    GPIOA13_IRQn,
    GPIOA14_IRQn,
    GPIOA15_IRQn,
    GPIOA16_IRQn,
    GPIOA17_IRQn,
    GPIOA18_IRQn,
    GPIOA19_IRQn,
    GPIOA20_IRQn,
    GPIOA21_IRQn,
    GPIOA22_IRQn,
    GPIOA23_IRQn,
    GPIOA24_IRQn,
    GPIOA25_IRQn,
    GPIOA26_IRQn,
    GPIOA27_IRQn,
    GPIOA28_IRQn,
    GPIOA29_IRQn,
    GPIOA30_IRQn,
    GPIOA31_IRQn,
    GPIOB0_IRQn,
    GPIOB1_IRQn,
    GPIOB2_IRQn,
    GPIOB3_IRQn,
    GPIOB4_IRQn,
    GPIOB5_IRQn,
    GPIOB6_IRQn,
    GPIOB7_IRQn,
    GPIOB8_IRQn,
    GPIOB9_IRQn,
    GPIOB10_IRQn,
    GPIOB11_IRQn,
    GPIOB12_IRQn,
    GPIOB13_IRQn,
    GPIOB14_IRQn,
    GPIOB15_IRQn,
    GPIOB16_IRQn,
    GPIOB17_IRQn,
    GPIOB18_IRQn,
    GPIOB19_IRQn,
    GPIOB20_IRQn,
    GPIOB21_IRQn,
    GPIOB22_IRQn,
    GPIOB23_IRQn,
    GPIOB24_IRQn,
    GPIOB25_IRQn,
    GPIOB26_IRQn,
    GPIOB27_IRQn,
    GPIOB28_IRQn,
    GPIOB29_IRQn,
    GPIOB30_IRQn,
    GPIOB31_IRQn,
    GPIOC0_IRQn,
    GPIOC1_IRQn,
    GPIOC2_IRQn,
    GPIOC3_IRQn,
    GPIOC4_IRQn,
    GPIOC5_IRQn,
    GPIOC6_IRQn,
    GPIOC7_IRQn,
    GPIOC8_IRQn,
    GPIOC9_IRQn,
    GPIOC10_IRQn,
    GPIOC11_IRQn,
    GPIOC12_IRQn,
    GPIOC13_IRQn,
    GPIOC14_IRQn,
    GPIOC15_IRQn,
    GPIOC16_IRQn,
    GPIOC17_IRQn,
    GPIOC18_IRQn,
    GPIOC19_IRQn,
    GPIOC20_IRQn,
    GPIOC21_IRQn,
    GPIOC22_IRQn,
    GPIOC23_IRQn,
    GPIOC24_IRQn,
    GPIOC25_IRQn,
    GPIOC26_IRQn,
    GPIOC27_IRQn,
    GPIOC28_IRQn,
    GPIOC29_IRQn,
    GPIOC30_IRQn,
    GPIOC31_IRQn,
    GPIOD0_IRQn,
    GPIOD1_IRQn,
    GPIOD2_IRQn,
    GPIOD3_IRQn,
    GPIOD4_IRQn,
    GPIOD5_IRQn,
    GPIOD6_IRQn,
    GPIOD7_IRQn,
    GPIOD8_IRQn,
    GPIOD9_IRQn,
    GPIOD10_IRQn,
    GPIOD11_IRQn,
    GPIOD12_IRQn,
    GPIOD13_IRQn,
    GPIOD14_IRQn,
    GPIOD15_IRQn,
    GPIOD16_IRQn,
    GPIOD17_IRQn,
    GPIOD18_IRQn,
    GPIOD19_IRQn,
    GPIOD20_IRQn,
    GPIOD21_IRQn,
    GPIOD22_IRQn,
    GPIOD23_IRQn,
    GPIOD24_IRQn,
    GPIOD25_IRQn,
    GPIOD26_IRQn,
    GPIOD27_IRQn,
    GPIOD28_IRQn,
    GPIOD29_IRQn,
    GPIOD30_IRQn,
    GPIOD31_IRQn,
};
const IRQ_Fun table_func[GPIO_NUM_MAX] =
{
    DRV_GPIO_A0_Handler,
    DRV_GPIO_A1_Handler,
    DRV_GPIO_A2_Handler,
    DRV_GPIO_A3_Handler,
    DRV_GPIO_A4_Handler,
    DRV_GPIO_A5_Handler,
    DRV_GPIO_A6_Handler,
    DRV_GPIO_A7_Handler,
    DRV_GPIO_A8_Handler,
    DRV_GPIO_A9_Handler,
    DRV_GPIO_A10_Handler,
    DRV_GPIO_A11_Handler,
    DRV_GPIO_A12_Handler,
    DRV_GPIO_A13_Handler,
    DRV_GPIO_A14_Handler,
    DRV_GPIO_A15_Handler,
    DRV_GPIO_A16_Handler,
    DRV_GPIO_A17_Handler,
    DRV_GPIO_A18_Handler,
    DRV_GPIO_A19_Handler,
    DRV_GPIO_A20_Handler,
    DRV_GPIO_A21_Handler,
    DRV_GPIO_A22_Handler,
    DRV_GPIO_A23_Handler,
    DRV_GPIO_A24_Handler,
    DRV_GPIO_A25_Handler,
    DRV_GPIO_A26_Handler,
    DRV_GPIO_A27_Handler,
    DRV_GPIO_A28_Handler,
    DRV_GPIO_A29_Handler,
    DRV_GPIO_A30_Handler,
    DRV_GPIO_A31_Handler,
    DRV_GPIO_B0_Handler,
    DRV_GPIO_B1_Handler,
    DRV_GPIO_B2_Handler,
    DRV_GPIO_B3_Handler,
    DRV_GPIO_B4_Handler,
    DRV_GPIO_B5_Handler,
    DRV_GPIO_B6_Handler,
    DRV_GPIO_B7_Handler,
    DRV_GPIO_B8_Handler,
    DRV_GPIO_B9_Handler,
    DRV_GPIO_B10_Handler,
    DRV_GPIO_B11_Handler,
    DRV_GPIO_B12_Handler,
    DRV_GPIO_B13_Handler,
    DRV_GPIO_B14_Handler,
    DRV_GPIO_B15_Handler,
    DRV_GPIO_B16_Handler,
    DRV_GPIO_B17_Handler,
    DRV_GPIO_B18_Handler,
    DRV_GPIO_B19_Handler,
    DRV_GPIO_B20_Handler,
    DRV_GPIO_B21_Handler,
    DRV_GPIO_B22_Handler,
    DRV_GPIO_B23_Handler,
    DRV_GPIO_B24_Handler,
    DRV_GPIO_B25_Handler,
    DRV_GPIO_B26_Handler,
    DRV_GPIO_B27_Handler,
    DRV_GPIO_B28_Handler,
    DRV_GPIO_B29_Handler,
    DRV_GPIO_B30_Handler,
    DRV_GPIO_B31_Handler,
    DRV_GPIO_C0_Handler,
    DRV_GPIO_C1_Handler,
    DRV_GPIO_C2_Handler,
    DRV_GPIO_C3_Handler,
    DRV_GPIO_C4_Handler,
    DRV_GPIO_C5_Handler,
    DRV_GPIO_C6_Handler,
    DRV_GPIO_C7_Handler,
    DRV_GPIO_C8_Handler,
    DRV_GPIO_C9_Handler,
    DRV_GPIO_C10_Handler,
    DRV_GPIO_C11_Handler,
    DRV_GPIO_C12_Handler,
    DRV_GPIO_C13_Handler,
    DRV_GPIO_C14_Handler,
    DRV_GPIO_C15_Handler,
    DRV_GPIO_C16_Handler,
    DRV_GPIO_C17_Handler,
    DRV_GPIO_C18_Handler,
    DRV_GPIO_C19_Handler,
    DRV_GPIO_C20_Handler,
    DRV_GPIO_C21_Handler,
    DRV_GPIO_C22_Handler,
    DRV_GPIO_C23_Handler,
    DRV_GPIO_C24_Handler,
    DRV_GPIO_C25_Handler,
    DRV_GPIO_C26_Handler,
    DRV_GPIO_C27_Handler,
    DRV_GPIO_C28_Handler,
    DRV_GPIO_C29_Handler,
    DRV_GPIO_C30_Handler,
    DRV_GPIO_C31_Handler,
    DRV_GPIO_D0_Handler,
    DRV_GPIO_D1_Handler,
    DRV_GPIO_D2_Handler,
    DRV_GPIO_D3_Handler,
    DRV_GPIO_D4_Handler,
    DRV_GPIO_D5_Handler,
    DRV_GPIO_D6_Handler,
    DRV_GPIO_D7_Handler,
    DRV_GPIO_D8_Handler,
    DRV_GPIO_D9_Handler,
    DRV_GPIO_D10_Handler,
    DRV_GPIO_D11_Handler,
    DRV_GPIO_D12_Handler,
    DRV_GPIO_D13_Handler,
    DRV_GPIO_D14_Handler,
    DRV_GPIO_D15_Handler,
    DRV_GPIO_D16_Handler,
    DRV_GPIO_D17_Handler,
    DRV_GPIO_D18_Handler,
    DRV_GPIO_D19_Handler,
    DRV_GPIO_D20_Handler,
    DRV_GPIO_D21_Handler,
    DRV_GPIO_D22_Handler,
    DRV_GPIO_D23_Handler,
    DRV_GPIO_D24_Handler,
    DRV_GPIO_D25_Handler,
    DRV_GPIO_D26_Handler,
    DRV_GPIO_D27_Handler,
    DRV_GPIO_D28_Handler,
    DRV_GPIO_D29_Handler,
    DRV_GPIO_D30_Handler,
    DRV_GPIO_D31_Handler,
};
const VECTORn_Type table_vector[GPIO_NUM_MAX] =
{
    GPIO_A0_VECTORn,
    GPIO_A1_VECTORn,
    GPIOA2_VECTORn,
    GPIOA3_VECTORn,
    GPIOA4_VECTORn,
    GPIOA5_VECTORn,
    GPIOA6_VECTORn,
    GPIOA7_VECTORn,
    GPIOA8_VECTORn,
    GPIOA9_VECTORn,
    GPIOA10_VECTORn,
    GPIOA11_VECTORn,
    GPIOA12_VECTORn,
    GPIOA13_VECTORn,
    GPIOA14_VECTORn,
    GPIOA15_VECTORn,
    GPIOA16_VECTORn,
    GPIOA17_VECTORn,
    GPIOA18_VECTORn,
    GPIOA19_VECTORn,
    GPIOA20_VECTORn,
    GPIOA21_VECTORn,
    GPIOA22_VECTORn,
    GPIOA23_VECTORn,
    GPIOA24_VECTORn,
    GPIOA25_VECTORn,
    GPIOA26_VECTORn,
    GPIOA27_VECTORn,
    GPIOA28_VECTORn,
    GPIOA29_VECTORn,
    GPIOA30_VECTORn,
    GPIOA31_VECTORn,
    GPIOB0_VECTORn,
    GPIOB1_VECTORn,
    GPIOB2_VECTORn,
    GPIOB3_VECTORn,
    GPIOB4_VECTORn,
    GPIOB5_VECTORn,
    GPIOB6_VECTORn,
    GPIOB7_VECTORn,
    GPIOB8_VECTORn,
    GPIOB9_VECTORn,
    GPIOB10_VECTORn,
    GPIOB11_VECTORn,
    GPIOB12_VECTORn,
    GPIOB13_VECTORn,
    GPIOB14_VECTORn,
    GPIOB15_VECTORn,
    GPIOB16_VECTORn,
    GPIOB17_VECTORn,
    GPIOB18_VECTORn,
    GPIOB19_VECTORn,
    GPIOB20_VECTORn,
    GPIOB21_VECTORn,
    GPIOB22_VECTORn,
    GPIOB23_VECTORn,
    GPIOB24_VECTORn,
    GPIOB25_VECTORn,
    GPIOB26_VECTORn,
    GPIOB27_VECTORn,
    GPIOB28_VECTORn,
    GPIOB29_VECTORn,
    GPIOB30_VECTORn,
    GPIOB31_VECTORn,
    GPIOC0_VECTORn,
    GPIOC1_VECTORn,
    GPIOC2_VECTORn,
    GPIOC3_VECTORn,
    GPIOC4_VECTORn,
    GPIOC5_VECTORn,
    GPIOC6_VECTORn,
    GPIOC7_VECTORn,
    GPIOC8_VECTORn,
    GPIOC9_VECTORn,
    GPIOC10_VECTORn,
    GPIOC11_VECTORn,
    GPIOC12_VECTORn,
    GPIOC13_VECTORn,
    GPIOC14_VECTORn,
    GPIOC15_VECTORn,
    GPIOC16_VECTORn,
    GPIOC17_VECTORn,
    GPIOC18_VECTORn,
    GPIOC19_VECTORn,
    GPIOC20_VECTORn,
    GPIOC21_VECTORn,
    GPIOC22_VECTORn,
    GPIOC23_VECTORn,
    GPIOC24_VECTORn,
    GPIOC25_VECTORn,
    GPIOC26_VECTORn,
    GPIOC27_VECTORn,
    GPIOC28_VECTORn,
    GPIOC29_VECTORn,
    GPIOC30_VECTORn,
    GPIOC31_VECTORn,
    GPIOD0_VECTORn,
    GPIOD1_VECTORn,
    GPIOD2_VECTORn,
    GPIOD3_VECTORn,
    GPIOD4_VECTORn,
    GPIOD5_VECTORn,
    GPIOD6_VECTORn,
    GPIOD7_VECTORn,
    GPIOD8_VECTORn,
    GPIOD9_VECTORn,
    GPIOD10_VECTORn,
    GPIOD11_VECTORn,
    GPIOD12_VECTORn,
    GPIOD13_VECTORn,
    GPIOD14_VECTORn,
    GPIOD15_VECTORn,
    GPIOD16_VECTORn,
    GPIOD17_VECTORn,
    GPIOD18_VECTORn,
    GPIOD19_VECTORn,
    GPIOD20_VECTORn,
    GPIOD21_VECTORn,
    GPIOD22_VECTORn,
    GPIOD23_VECTORn,
    GPIOD24_VECTORn,
    GPIOD25_VECTORn,
    GPIOD26_VECTORn,
    GPIOD27_VECTORn,
    GPIOD28_VECTORn,
    GPIOD29_VECTORn,
    GPIOD30_VECTORn,
    GPIOD31_VECTORn,
};




