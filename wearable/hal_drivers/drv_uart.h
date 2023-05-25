/*
 * Copyright (c) 2022
 *
 *
 *
 *
 *
 *
 */

#ifndef __DRV_UART_H__
#define __DRV_UART_H__

#include "board.h"
#include "rtl_uart.h"
#include "rtl_rcc.h"
#include "rtl_nvic.h"
#include "rtl_pinmux.h"
#include "wsdef.h"
#include "wristband_sdk_config.h"
#include "drv_dlps.h"


#define _CONTACT_UART_INDEX(x) UART##x##_INDEX
#define CONTACT_UART_INDEX(x) _CONTACT_UART_INDEX(x)

#define _CONTACT_UART_HANDER(x) UART##x##_Handler
#define CONTACT_UART_HANDER(x) _CONTACT_UART_HANDER(x)

#if (WS_USING_LETTER_SHELL == 1)
#define LETTER_SHELL_UART_INDEX CONTACT_UART_INDEX(LETTER_SHELL_UART_CONFIG)
#define LETTER_SHELL_UART_HANDLE CONTACT_UART_HANDER(LETTER_SHELL_UART_CONFIG)
#endif

typedef enum
{
    BAUD_RATE_9600,
    BAUD_RATE_19200,
    BAUD_RATE_115200,
    BAUD_RATE_230400,
    BAUD_RATE_460800,
    BAUD_RATE_921600,
    BAUD_RATE_2000000,
    BAUD_RATE_3000000,
    BAUD_RATE_4000000,
    BAUD_RATE_6000000,
} UART_BaudRate_Def;

struct rtl_uart_config
{
    const char *name;
    UART_TypeDef *Instance;
    IRQn_Type irq_type;
    uint8_t tx_pin;
    uint8_t rx_pin;
    uint8_t tx_pin_func;
    uint8_t rx_pin_func;
    uint32_t periph;
    uint32_t periph_clock;
    UART_BaudRate_Def baudrate_index;
};

typedef struct
{
    uint16_t div;
    uint16_t ovsr;
    uint16_t ovsr_adj;
} UART_BaudRate_TypeDef;

#define UART0_CONFIG                                               \
    {                                                              \
        .name = "uart0",                                           \
                .Instance = UART0,                                         \
                            .irq_type = UART0_IRQn,                                    \
                                        .tx_pin = P3_0,                                            \
                                                  .rx_pin = P3_1,                                            \
                                                            .tx_pin_func = UART0_TX,                                   \
                                                                           .rx_pin_func = UART0_RX,                                   \
                                                                                   .periph = APBPeriph_UART0,                                 \
                                                                                           .periph_clock = APBPeriph_UART0_CLOCK,                     \
    }
#define UART1_CONFIG                                               \
    {                                                              \
        .name = "uart1",                                           \
                .Instance = UART1,                                         \
                            .irq_type = UART1_IRQn,                                    \
                                        .tx_pin = P3_0,                                            \
                                                  .rx_pin = P3_1,                                            \
                                                            .tx_pin_func = UART1_TX,                                   \
                                                                           .rx_pin_func = UART1_RX,                                   \
                                                                                   .periph = APBPeriph_UART1,                                 \
                                                                                           .periph_clock = APBPeriph_UART1_CLOCK,                     \
    }
#define UART2_CONFIG                                               \
    {                                                              \
        .name = "uart2",                                           \
                .Instance = UART2,                                         \
                            .irq_type = UART2_IRQn,                                    \
                                        .tx_pin = P3_0,                                            \
                                                  .rx_pin = P3_1,                                            \
                                                            .tx_pin_func = UART2_TX,                                   \
                                                                           .rx_pin_func = UART2_RX,                                   \
                                                                                   .periph = APBPeriph_UART2,                                 \
                                                                                           .periph_clock = APBPeriph_UART2_CLOCK,                     \
    }
#define UART3_CONFIG                                               \
    {                                                              \
        .name = "uart3",                                           \
                .Instance = UART3,                                         \
                            .irq_type = UART3_IRQn,                                    \
                                        .tx_pin = P3_0,                                            \
                                                  .rx_pin = P3_1,                                            \
                                                            .tx_pin_func = UART3_TX,                                   \
                                                                           .rx_pin_func = UART3_RX,                                   \
                                                                                   .periph = APBPeriph_UART3,                                 \
                                                                                           .periph_clock = APBPeriph_UART3_CLOCK,                     \
    }
#define UART4_CONFIG                                               \
    {                                                              \
        .name = "uart4",                                           \
                .Instance = UART4,                                         \
                            .irq_type = UART4_IRQn,                                    \
                                        .tx_pin = P3_0,                                            \
                                                  .rx_pin = P3_1,                                            \
                                                            .tx_pin_func = UART4_TX,                                   \
                                                                           .rx_pin_func = UART4_RX,                                   \
                                                                                   .periph = APBPeriph_UART4,                                 \
                                                                                           .periph_clock = APBPeriph_UART4_CLOCK,                     \
                                                                                                   .baudrate_index = UART4_CONFIG_BAUDRATE, \
    }
#define UART5_CONFIG                                               \
    {                                                              \
        .name = "uart5",                                           \
                .Instance = UART5,                                         \
                            .irq_type = UART5_IRQn,                                    \
                                        .tx_pin = P3_0,                                            \
                                                  .rx_pin = P3_1,                                            \
                                                            .tx_pin_func = UART5_TX,                                   \
                                                                           .rx_pin_func = UART5_RX,                                   \
                                                                                   .periph = APBPeriph_UART5,                                 \
                                                                                           .periph_clock = APBPeriph_UART5_CLOCK,                     \
                                                                                                   .baudrate_index = UART5_CONFIG_BAUDRATE,                             \
    }



extern struct rtl_uart_config uart_config[];


void drv_uart_dlps_init(struct rtl_uart_config *uart_cfg);
void hw_uart_init(void);


#endif  /* __DRV_UART_H__ */

/******************* end of file ***************/
