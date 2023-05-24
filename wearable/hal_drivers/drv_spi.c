/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-07-14     tyustli   first version
 */

#include "drv_spi.h"
#include "board.h"

#ifdef RT_USING_SPI
#include "rtl_gpio.h"
#include "rtl_rcc.h"
#include "rtl_spi.h"
#include "rtl_pinmux.h"
#include "string.h"

#define DRV_SPI_CLK_PIN   P4_0
#define DRV_SPI_MOSI_PIN  P4_1
#define DRV_SPI_MISO_PIN  P4_2
#define DRV_SPI_CS_PIN    P4_3


static void spi_write(uint32_t *pBuf, uint16_t len)
{
    uint8_t tmp = 0;
    /* clear Rx FIFO first */
    uint32_t dummy_len = SPI_GetRxFIFOLen(SPI0);
    while (dummy_len--)
    {
        tmp = SPI_ReceiveData(SPI0);
    }

    while (SPI_GetFlagState(SPI0, SPI_FLAG_TFE) == RESET);
    while (len--)
    {
        SPI_SendData(SPI0, *(pBuf++));
        while (SPI_GetFlagState(SPI0, SPI_FLAG_RFNE) == RESET);
        SPI_ReceiveData(SPI0);
    }
}

static void spi_read(uint32_t *pBuf, uint16_t len)
{
    uint32_t tmp = 0;
    /* clear Rx FIFO first */
    uint32_t dummy_len = SPI_GetRxFIFOLen(SPI0);
    while (dummy_len--)
    {
        tmp = SPI_ReceiveData(SPI0);
    }

    while (SPI_GetFlagState(SPI0, SPI_FLAG_TFE) == RESET);
    while (len--)
    {

        SPI_SendData(SPI0, 0);
        while (SPI_GetFlagState(SPI0, SPI_FLAG_RFNE) == RESET);
        tmp = SPI_ReceiveData(SPI0);
        *pBuf++ = tmp;

    }
}
static void spi_write_and_read(uint32_t *pWrite, uint32_t *pRead, uint16_t len)
{
    uint32_t tmp = 0;
    /* clear Rx FIFO first */
    uint32_t dummy_len = SPI_GetRxFIFOLen(SPI0);
    while (dummy_len--)
    {
        tmp = SPI_ReceiveData(SPI0);
    }

    /* wait Tx FIFO empty */
    while (SPI_GetFlagState(SPI0, SPI_FLAG_TFE) == RESET);
    while (len--)
    {
        SPI_SendData(SPI0, *(pWrite++));
        while (SPI_GetFlagState(SPI0, SPI_FLAG_RFNE) == RESET);
        *pRead++ = SPI_ReceiveData(SPI0);
    }
}

static rt_err_t spi_configure(struct rt_spi_device *device,
                              struct rt_spi_configuration *configuration)
{
    rt_kprintf("spi_drv spi_configure\n");
    device->config = *configuration;
    //RCC_PeriphClockCmd(APBPeriph_SPI0, APBPeriph_SPI0_CLOCK, ENABLE);
    SPI_Cmd(SPI0, DISABLE);
    SPI_InitTypeDef  SPI_InitStructure;
    SPI_StructInit(&SPI_InitStructure);
    SPI_InitStructure.SPI_Direction         = SPI_Direction_FullDuplex;
    SPI_InitStructure.SPI_DataSize          = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL              = SPI_CPOL_High;
    SPI_InitStructure.SPI_CPHA              = SPI_CPHA_2Edge;
    SPI_InitStructure.SPI_FrameFormat       = SPI_Frame_Motorola;


    if (configuration->data_width == 8)
    {
        SPI_InitStructure.SPI_DataSize          = SPI_DataSize_8b;
    }
    else if (configuration->data_width == 9)
    {
        SPI_InitStructure.SPI_DataSize          = SPI_DataSize_9b;
    }
    else if (configuration->data_width == 16)
    {
        SPI_InitStructure.SPI_DataSize          = SPI_DataSize_16b;
    }
    if (configuration->mode & RT_SPI_CPHA)
    {
        SPI_InitStructure.SPI_CPHA              = SPI_CPHA_2Edge;
    }
    else
    {
        SPI_InitStructure.SPI_CPHA              = SPI_CPHA_2Edge;
    }

    if (configuration->mode & RT_SPI_CPOL)
    {
        SPI_InitStructure.SPI_CPOL              = SPI_CPOL_High;
    }
    else
    {
        SPI_InitStructure.SPI_CPOL              = SPI_CPOL_High;
    }

    if (configuration->mode & RT_SPI_NO_CS)
    {

    }
    else
    {

    }


    if (configuration->max_hz == 400 * 1000)
    {
        SPI_InitStructure.SPI_BaudRatePrescaler = 142;
    }
    else
    {
        //SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
        SPI_InitStructure.SPI_BaudRatePrescaler = 4;
    }

    if (configuration->mode & RT_SPI_MSB)
    {

    }
    else if (configuration->mode & RT_SPI_LSB)
    {

    }
    SPI_InitStructure.SPI_Direction         = SPI_Direction_FullDuplex;

    SPI_InitStructure.SPI_CPOL              = SPI_CPOL_High;
    SPI_InitStructure.SPI_CPHA              = SPI_CPHA_2Edge;
    SPI_InitStructure.SPI_BaudRatePrescaler = 8;
    SPI_InitStructure.SPI_FrameFormat       = SPI_Frame_Motorola;
    SPI_Init(SPI0, &SPI_InitStructure);
    SPI_Cmd(SPI0, ENABLE);
    return RT_EOK;
}

static rt_uint32_t spixfer(struct rt_spi_device *device, struct rt_spi_message *message)
{
    rt_size_t message_length;
    uint32_t *recv_buf;
    uint32_t *send_buf;

    RT_ASSERT(device != RT_NULL);
    RT_ASSERT(device->bus != RT_NULL);
    //RT_ASSERT(device->bus->parent.user_data != RT_NULL);
    RT_ASSERT(message != RT_NULL);

    message_length = message->length;
    recv_buf = message->recv_buf;
    send_buf = (uint32_t *)message->send_buf;

    if (message->cs_take == 1)
    {
        rt_pin_write(DRV_SPI_CS_PIN, 0);
    }

    if (message->send_buf && message->recv_buf)
    {
        spi_write_and_read(send_buf, recv_buf, message_length);
    }
    else if (message->send_buf)
    {
        spi_write(send_buf, message_length);
    }
    else if (message->recv_buf)
    {
        memset((uint8_t *)recv_buf, 0xff, message_length);
        spi_read(recv_buf, message_length);
    }

    if (message->cs_release == 1)
    {
        rt_pin_write(DRV_SPI_CS_PIN, 1);
    }

    return message->length;
}

static const struct rt_spi_ops rtk_spi_bus_ops =
{
    .configure = spi_configure,
    .xfer = spixfer,
};

static struct rt_spi_device owner;
static struct rt_spi_bus   rtl_spi_bus =
{
    .owner = &owner,
    .ops = &rtk_spi_bus_ops,
};

int rt_hw_spi_init(void)
{
    rt_err_t result;
    /* init gpio */
    Pad_Config(DRV_SPI_CLK_PIN, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE,
               PAD_OUT_HIGH); //CLK
    Pad_Config(DRV_SPI_MOSI_PIN, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE,
               PAD_OUT_HIGH); //MOSI
    Pad_Config(DRV_SPI_MISO_PIN, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE,
               PAD_OUT_HIGH); //MISO
    Pad_Config(DRV_SPI_CS_PIN, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE,
               PAD_OUT_HIGH); //CS

    Pinmux_Config(DRV_SPI_CLK_PIN, SPI0_CLK_MASTER);
    Pinmux_Config(DRV_SPI_MOSI_PIN, SPI0_MO_MASTER);
    Pinmux_Config(DRV_SPI_MISO_PIN, SPI0_MI_MASTER);
    //Pinmux_Config(P4_3, SPI0_SS_N_0_MASTER);


    SPI_DeInit(SPI0);
    RCC_PeriphClockCmd(APBPeriph_SPI0, APBPeriph_SPI0_CLOCK, ENABLE);
    SPI_InitTypeDef  SPI_InitStructure;
    SPI_StructInit(&SPI_InitStructure);
    SPI_InitStructure.SPI_Direction         = SPI_Direction_FullDuplex;
    SPI_InitStructure.SPI_DataSize          = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL              = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA              = SPI_CPHA_2Edge;
    SPI_InitStructure.SPI_BaudRatePrescaler = 4;
    SPI_InitStructure.SPI_FrameFormat       = SPI_Frame_Motorola;
    SPI_Init(SPI0, &SPI_InitStructure);
    SPI_Cmd(SPI0, ENABLE);
    rt_pin_mode(DRV_SPI_CS_PIN, PIN_MODE_OUTPUT);
    rt_pin_write(DRV_SPI_CS_PIN, 1);

    result = rt_spi_bus_register(&rtl_spi_bus, "spi0", &rtk_spi_bus_ops);

    return result;
}
//INIT_DEVICE_EXPORT(rt_hw_spi_init);


#endif /* RT_USING_SPI */

/********************** end of file *****************/
