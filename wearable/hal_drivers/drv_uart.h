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
#include "rtl_hal_peripheral.h"
#include "drv_dlps.h"



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


typedef struct
{
    uint16_t div;
    uint16_t ovsr;
    uint16_t ovsr_adj;
} UART_BaudRate_TypeDef;



void hw_uart_init(void);


void drv_uart2_init(uint8_t tx_pin, uint8_t rx_pin);
uint32_t drv_uart2_write(const void *buffer, uint32_t size);
void drv_uart2_set_rx_indicate(void (*rx_ind)(uint8_t ch));

void drv_uart3_init(uint8_t tx_pin, uint8_t rx_pin);
uint32_t drv_uart3_write(const void *buffer, uint32_t size);
void drv_uart3_set_rx_indicate(void (*rx_ind)(uint8_t ch));

void drv_uart4_init(uint8_t tx_pin, uint8_t rx_pin);
uint32_t drv_uart4_write(const void *buffer, uint32_t size);
void drv_uart4_set_rx_indicate(void (*rx_ind)(uint8_t ch));

void drv_uart5_init(uint8_t tx_pin, uint8_t rx_pin);
uint32_t drv_uart5_write(const void *buffer, uint32_t size);
void drv_uart5_set_rx_indicate(void (*rx_ind)(uint8_t ch));


#endif  /* __DRV_UART_H__ */

/******************* end of file ***************/
