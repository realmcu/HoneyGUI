/*
 * Copyright (c) 2022
 *
 */

#include <string.h>
#include "drv_uart.h"
#include "trace.h"
#include "os_sync.h"
#include "drv_dlps.h"
#include "os_timer.h"
#include "board.h"

void (*uart0_rx_indicate)(uint8_t ch) = NULL;
void (*uart1_rx_indicate)(uint8_t ch) = NULL;
void (*uart2_rx_indicate)(uint8_t ch) = NULL;
void (*uart3_rx_indicate)(uint8_t ch) = NULL;
void (*uart4_rx_indicate)(uint8_t ch) = NULL;
void (*uart5_rx_indicate)(uint8_t ch) = NULL;

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

static void uart_isr(void (*rx_ind)(uint8_t ch), UART_TypeDef *UARTx)
{
    uint8_t ch;
    /* Read interrupt type */
    uint32_t int_status = UART_GetIID(UARTx);

    /* Disable interrupt */

    UART_INTConfig(UARTx, UART_INT_RD_AVA | UART_INT_RX_LINE_STS, DISABLE);

    /* Interrupt handle */
    switch (int_status & 0x0E)
    {
    case UART_INT_ID_RX_DATA_TIMEOUT:
        {
            while (UART_GetFlagStatus(UARTx, UART_FLAG_RX_DATA_AVA) == SET)
            {
                //HAVE RECIVE TIMEOUT THE LEFT
                ch = UART_ReceiveByte(UARTx);
                if (rx_ind != NULL)
                {
                    rx_ind(ch);
                }
            }
            break;
        }
    case UART_INT_ID_LINE_STATUS:
        {
            UART_GetFlagStatus(UARTx, UART_FLAG_RX_OVERRUN);
            UART_ClearRxFIFO(UARTx);
            UART_ClearTxFIFO(UARTx);
            /* Enable interrupt */
            UART_INTConfig(UARTx, UART_INT_RD_AVA | UART_INT_RX_LINE_STS, ENABLE);
            return ;
        }
    case UART_INT_ID_RX_LEVEL_REACH:
        {
            while (UART_GetFlagStatus(UARTx, UART_FLAG_RX_DATA_AVA) == SET)
            {
                ch = UART_ReceiveByte(UARTx);
                if (rx_ind != NULL)
                {
                    rx_ind(ch);
                }
            }
            break;
        }
    case UART_INT_ID_TX_EMPTY:
        {
            break;
        }
    default:
        {
            break;
        }
    }

    /* Enable interrupt */
    UART_INTConfig(UARTx, UART_INT_RD_AVA |  UART_INT_RX_LINE_STS, ENABLE);
}

void UART2_Handler(void)
{
    uart_isr(uart2_rx_indicate, UART2);
}
#if defined RTL8772F || defined RTL8762G
void UART3_Handler(void)
{
    uart_isr(uart3_rx_indicate, UART3);
}
void UART4_Handler(void)
{
#ifdef RTK_RT_USING_CONSOLE
    UART4_RT_Handler();
#else
    uart_isr(uart4_rx_indicate, UART4);
#endif
}
void UART5_Handler(void)
{
    uart_isr(uart5_rx_indicate, UART5);
}
#endif

static uint32_t drv_uart_write(UART_TypeDef *UARTx, const void *buffer, uint32_t size)
{
    if (size == 0)
    {
        return 0;
    }

    uint8_t *buf = (uint8_t *)buffer;
    uint32_t i = 0;

    for (i = 0; i < (size / UART_TX_FIFO_SIZE); i++)
    {
        UART_SendData(UARTx, buf, UART_TX_FIFO_SIZE);
        while (UART_GetFlagStatus(UARTx, UART_FLAG_TX_EMPTY) != SET);
        buf += UART_TX_FIFO_SIZE;
    }

    UART_SendData(UARTx, buf, size % UART_TX_FIFO_SIZE);
    while (UART_GetFlagStatus(UARTx, UART_FLAG_TX_EMPTY) != SET);

    return size;
}



void drv_uart_init(UART_TypeDef *UARTx, uint8_t tx_pin, uint8_t rx_pin)
{
    IRQn_Type irq_type = UART2_IRQn;
    uint8_t tx_pin_func = UART2_TX;
    uint8_t rx_pin_func = UART2_RX;
    uint32_t periph = APBPeriph_UART2;
    uint32_t periph_clock = APBPeriph_UART2_CLOCK;
    if (UARTx == UART2)
    {
        irq_type = UART2_IRQn;
        tx_pin_func = UART2_TX;
        rx_pin_func = UART2_RX;
        periph = APBPeriph_UART2;
        periph_clock = APBPeriph_UART2_CLOCK;
    }
#if defined RTL8772F || defined RTL8762G
    else if (UARTx == UART3)
    {
        irq_type = UART3_IRQn;
        tx_pin_func = UART3_TX;
        rx_pin_func = UART3_RX;
        periph = APBPeriph_UART3;
        periph_clock = APBPeriph_UART3_CLOCK;
    }
    else if (UARTx == UART4)
    {
        irq_type = UART4_IRQn;
        tx_pin_func = UART4_TX;
        rx_pin_func = UART4_RX;
        periph = APBPeriph_UART4;
        periph_clock = APBPeriph_UART4_CLOCK;
    }
    else if (UARTx == UART5)
    {
        irq_type = UART5_IRQn;
        tx_pin_func = UART5_TX;
        rx_pin_func = UART5_RX;
        periph = APBPeriph_UART5;
        periph_clock = APBPeriph_UART5_CLOCK;
    }
#endif
    Pinmux_Deinit(tx_pin);
    Pinmux_Deinit(rx_pin);
    Pad_Config(tx_pin, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(rx_pin, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pinmux_Config(tx_pin, tx_pin_func);
    Pinmux_Config(rx_pin, rx_pin_func);

    RCC_PeriphClockCmd(periph, periph_clock, ENABLE);
    UART_InitTypeDef UART_InitStruct;
    UART_StructInit(&UART_InitStruct);

    UART_InitStruct.UART_Div            = BaudRate_Table[BAUD_RATE_2000000].div;
    UART_InitStruct.UART_Ovsr           = BaudRate_Table[BAUD_RATE_2000000].ovsr;
    UART_InitStruct.UART_OvsrAdj        = BaudRate_Table[BAUD_RATE_2000000].ovsr_adj;

    UART_Init(UARTx, &UART_InitStruct);

    UART_INTConfig(UARTx, UART_INT_RD_AVA | UART_INT_RX_LINE_STS, ENABLE);
    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = irq_type;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 3;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
#if defined RTL8772F || defined RTL8762G
    NVIC_SetIRQNonSecure(NVIC_InitStruct.NVIC_IRQChannel);
#endif
    NVIC_Init(&NVIC_InitStruct);
}

void drv_uart2_init(uint8_t tx_pin, uint8_t rx_pin)
{
    drv_uart_init(UART2, tx_pin, rx_pin);
}
#if defined RTL8772F || defined RTL8762G
void drv_uart3_init(uint8_t tx_pin, uint8_t rx_pin)
{
    drv_uart_init(UART3, tx_pin, rx_pin);
}
void drv_uart4_init(uint8_t tx_pin, uint8_t rx_pin)
{
    drv_uart_init(UART4, tx_pin, rx_pin);
}
void drv_uart5_init(uint8_t tx_pin, uint8_t rx_pin)
{
    drv_uart_init(UART5, tx_pin, rx_pin);
}
#endif

uint32_t drv_uart2_write(const void *buffer, uint32_t size)
{
    return drv_uart_write(UART2, buffer, size);
}
#if defined RTL8772F || defined RTL8762G
uint32_t drv_uart3_write(const void *buffer, uint32_t size)
{
    return drv_uart_write(UART3, buffer, size);
}
uint32_t drv_uart4_write(const void *buffer, uint32_t size)
{
    return drv_uart_write(UART4, buffer, size);
}
uint32_t drv_uart5_write(const void *buffer, uint32_t size)
{
    return drv_uart_write(UART5, buffer, size);
}
#endif

void drv_uart2_set_rx_indicate(void (*rx_ind)(uint8_t ch))
{
    uart2_rx_indicate = rx_ind;
}
void drv_uart3_set_rx_indicate(void (*rx_ind)(uint8_t ch))
{
    uart3_rx_indicate = rx_ind;
}
void drv_uart4_set_rx_indicate(void (*rx_ind)(uint8_t ch))
{
    uart4_rx_indicate = rx_ind;
}
void drv_uart5_set_rx_indicate(void (*rx_ind)(uint8_t ch))
{
    uart5_rx_indicate = rx_ind;
}



void hw_uart_init(void)
{
    return;
}


/************** end of file ********************/
