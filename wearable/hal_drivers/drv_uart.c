/*
 * Copyright (c) 2022
 *
 */

#include <string.h>
#include "drv_uart.h"
#include "trace.h"
#include "os_sync.h"
#include "drv_dlps.h"


static const UART_BaudRate_TypeDef BaudRate_Table[10] =
{
    {271, 10, 0x24A}, // BAUD_RATE_9600
    {165, 7,  0x5AD}, // BAUD_RATE_19200
    {20,  12, 0x252}, // BAUD_RATE_115200
    {10,  12, 0x252}, // BAUD_RATE_230400
    {5,   12, 0x252}, // BAUD_RATE_460800
    {4,   5,  0x3F7}, // BAUD_RATE_921600
    {2,   5,  0},     // BAUD_RATE_2000000
    {1,   8,  0x492}, // BAUD_RATE_3000000
    {1,   5,  0},     // BAUD_RATE_4000000
    {1,   1,  0x36D}, // BAUD_RATE_6000000
};

struct rtl_uart_config uart_config[] =
{
    UART0_CONFIG,
    UART1_CONFIG,
    UART2_CONFIG,
    UART3_CONFIG,
    UART4_CONFIG,
    UART5_CONFIG,
};



static void mcu_uart_init(struct rtl_uart_config *uart_cfg)
{
    Pinmux_Deinit(uart_cfg->tx_pin);
    Pinmux_Deinit(uart_cfg->rx_pin);
    Pad_Config(uart_cfg->tx_pin, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE,
               PAD_OUT_LOW);
    Pad_Config(uart_cfg->rx_pin, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE,
               PAD_OUT_LOW);
    Pinmux_Config(uart_cfg->tx_pin, uart_cfg->tx_pin_func);
    Pinmux_Config(uart_cfg->rx_pin, uart_cfg->rx_pin_func);

    RCC_PeriphClockCmd(uart_cfg->periph, uart_cfg->periph_clock, ENABLE);
    UART_InitTypeDef UART_InitStruct;
    UART_StructInit(&UART_InitStruct);

    UART_InitStruct.UART_Div            = BaudRate_Table[uart_cfg->baudrate_index].div;
    UART_InitStruct.UART_Ovsr           = BaudRate_Table[uart_cfg->baudrate_index].ovsr;
    UART_InitStruct.UART_OvsrAdj        = BaudRate_Table[uart_cfg->baudrate_index].ovsr_adj;

    UART_Init(uart_cfg->Instance, &UART_InitStruct);
}

static void mcu_uart_irq_init(struct rtl_uart_config *uart_cfg)
{
    UART_INTConfig(uart_cfg->Instance, UART_INT_RD_AVA | UART_INT_RX_LINE_STS, ENABLE);
    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = uart_cfg->irq_type;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 3;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);
}




void hw_uart_init(void)
{

}


/************** end of file ********************/
