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
    RCC_PeriphClockCmd(APBPeriph_GPIO, APBPeriph_GPIO_CLOCK, ENABLE);
}



void GPIO0_Handler(void) {DRV_GPIO_HANDLER(P0_0);}
void GPIO1_Handler(void) {DRV_GPIO_HANDLER(P0_1);}
void GPIO2_Handler(void) {DRV_GPIO_HANDLER(P0_2);}
void GPIO3_Handler(void) {DRV_GPIO_HANDLER(P0_3);}
void GPIO4_Handler(void) {DRV_GPIO_HANDLER(P0_4);}
void GPIO5_Handler(void) {DRV_GPIO_HANDLER(P0_5);}
void GPIO6_Handler(void) {DRV_GPIO_HANDLER(P0_6);}
void GPIO7_Handler(void) {DRV_GPIO_HANDLER(P0_7);}

void GPIO8_Handler(void)  {DRV_GPIO_HANDLER(P1_0);}
void GPIO9_Handler(void)  {DRV_GPIO_HANDLER(P1_1);}
void GPIO10_Handler(void) {DRV_GPIO_HANDLER(P1_2);}
void GPIO11_Handler(void) {DRV_GPIO_HANDLER(P1_3);}
void GPIO12_Handler(void) {DRV_GPIO_HANDLER(P1_4);}
void GPIO13_Handler(void) {DRV_GPIO_HANDLER(P1_5);}
void GPIO14_Handler(void) {DRV_GPIO_HANDLER(P1_6);}
void GPIO15_Handler(void) {DRV_GPIO_HANDLER(P1_7);}

void GPIO16_Handler(void) {DRV_GPIO_HANDLER(P2_0);}
void GPIO17_Handler(void) {DRV_GPIO_HANDLER(P2_1);}
void GPIO18_Handler(void) {DRV_GPIO_HANDLER(P2_2);}
void GPIO19_Handler(void) {DRV_GPIO_HANDLER(P2_3);}
void GPIO20_Handler(void) {DRV_GPIO_HANDLER(P2_4);}
void GPIO21_Handler(void) {DRV_GPIO_HANDLER(P2_5);}
void GPIO22_Handler(void) {DRV_GPIO_HANDLER(P2_6);}
void GPIO23_Handler(void) {DRV_GPIO_HANDLER(P2_7);}

void GPIO24_Handler(void) {DRV_GPIO_HANDLER(P3_0);}
void GPIO25_Handler(void) {DRV_GPIO_HANDLER(P3_1);}
void GPIO26_Handler(void) {DRV_GPIO_HANDLER(P3_2);}
void GPIO27_Handler(void) {DRV_GPIO_HANDLER(P3_3);}
void GPIO28_Handler(void) {DRV_GPIO_HANDLER(P3_4);}
void GPIO29_Handler(void) {DRV_GPIO_HANDLER(P3_5);}
void GPIO30_Handler(void) {DRV_GPIO_HANDLER(P3_6);}
void GPIO31_Handler(void) {DRV_GPIO_HANDLER(P4_3);}


IRQn_Type pin2irq(uint32_t pin)
{
    if (pin == P0_4)
    {
        return GPIO4_IRQn;
    }
    else if (pin == P0_5)
    {
        return GPIO5_IRQn;
    }
    else if ((pin == P0_3) || (pin == 7) || (pin == 11) || (pin == 15) || (pin == 19) || (pin == 23) ||
             (pin == 27) || (pin == 31) || (pin == P4_3) || (pin == H_2))
    {
        return (IRQn_Type)26;
    }
    else if ((pin == 2) || (pin == 6) || (pin == 10) || (pin == 14) || (pin == 18) || (pin == 22) ||
             (pin == 26) || (pin == 30) || (pin == P4_2) || (pin == H_1))
    {
        return (IRQn_Type)27;
    }
    else if ((pin == 1) || (pin == 9) || (pin == 13) || (pin == 17) || (pin == 21) || (pin == 25) ||
             (pin == 29) || (pin == P4_1) || (pin == H_0))
    {
        return (IRQn_Type)29;
    }
    else if ((pin == 0) || (pin == 8) || (pin == 12) || (pin == 16) || (pin == 20) || (pin == 24) ||
             (pin == 28) || (pin == P4_0))
    {
        return (IRQn_Type)30;
    }
    return (IRQn_Type)30;
}







