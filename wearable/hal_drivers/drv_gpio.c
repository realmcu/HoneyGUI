/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author            Notes
 * 2018-11-06     howie        first version
 */

#include <board.h>
#include "drv_gpio.h"
#include "utils.h"
#include "trace.h"
#include "patch_header_check.h"
#if (IMG_IC_TYPE == 0xF)
#include "vector_table.h"
#endif
#if (IMG_IC_TYPE == 0xE)
#include "vector_table_ns.h"
#endif
#include "os_sched.h"




#define GPIO_NUM_MAX    128


typedef void (* pin_irq_cb)(void *args);

typedef struct
{
    pin_irq_cb gpio_cb;
    void *args;
} drv_gpio_irq;

static drv_gpio_irq gpio_cb[GPIO_NUM_MAX] = {{NULL, NULL}};

static void DRV_GPIO_HANDLER(uint32_t Pin_num)
{
    uint8_t num = GPIO_GetNum(Pin_num);
    GPIO_ClearINTPendingBit(GPIO_GetPort(Pin_num), GPIO_GetPin(Pin_num));

    if (gpio_cb[num].gpio_cb != NULL)
    {
        gpio_cb[num].gpio_cb(gpio_cb[num].args);
    }
}
#include "drv_gpio_ic.c"
void drv_pin_mode(uint32_t pin, uint32_t mode)
{

    drv_pin_clk_set(pin);


    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_StructInit(&GPIO_InitStruct);

    GPIO_InitStruct.GPIO_Pin = GPIO_GetPin(pin);

    Pad_Config(pin, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE,
               PAD_OUT_HIGH);
    Pinmux_Config(pin, DWGPIO);

    if (mode == PIN_MODE_OUTPUT)
    {
        /* output setting */
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    }
    else if (mode == PIN_MODE_INPUT)
    {
        /* input setting: not pull. */
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
    }
    else if (mode == PIN_MODE_OUTPUT_OD)
    {
        Pad_Config(pin, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE, PAD_OUT_LOW);
        Pinmux_Config(pin, DWGPIO);
    }
    GPIO_InitStruct.GPIO_ITCmd  = DISABLE;
    GPIO_Init(GPIO_GetPort(pin), &GPIO_InitStruct);
}

void drv_pin_write(uint32_t pin, uint32_t value)
{
    GPIO_WriteBit(GPIO_GetPort(pin), GPIO_GetPin(pin), (BitAction)(value));
}

uint8_t drv_pin_read(uint32_t pin)
{
    uint8_t value;

    value = PIN_LOW;

    value = GPIO_ReadInputDataBit(GPIO_GetPort(pin), GPIO_GetPin(pin));

    return value;
}

uint8_t drv_pin_attach_irq(uint32_t pin, uint32_t mode, void (*hdr)(void *args), void *args)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_StructInit(&GPIO_InitStruct);

    GPIO_InitStruct.GPIO_Pin = GPIO_GetPin(pin);

    Pad_Config(pin, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
    Pinmux_Config(pin, DWGPIO);

    DBG_DIRECT("drv gpio mcu attach irq pin num = %d!\n", pin);

    /* input setting: not pull. */
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStruct.GPIO_ITCmd  = ENABLE;
    if (mode == PIN_IRQ_MODE_FALLING)
    {
        GPIO_InitStruct.GPIO_ITTrigger = GPIO_INT_Trigger_EDGE;
        GPIO_InitStruct.GPIO_ITPolarity = GPIO_INT_POLARITY_ACTIVE_LOW;
    }
    else if (mode == PIN_IRQ_MODE_RISING)
    {
        GPIO_InitStruct.GPIO_ITTrigger = GPIO_INT_Trigger_EDGE;
        GPIO_InitStruct.GPIO_ITPolarity = GPIO_INT_POLARITY_ACTIVE_HIGH;
    }
    else if (mode == PIN_IRQ_MODE_RISING_FALLING)
    {
        GPIO_InitStruct.GPIO_ITTrigger = GPIO_INT_Trigger_EDGE;
        GPIO_InitStruct.GPIO_ITPolarity = GPIO_INT_POLARITY_ACTIVE_LOW;
    }
    else if (mode == PIN_IRQ_MODE_HIGH_LEVEL)
    {
        GPIO_InitStruct.GPIO_ITTrigger = GPIO_INT_Trigger_LEVEL;
        GPIO_InitStruct.GPIO_ITPolarity = GPIO_INT_POLARITY_ACTIVE_HIGH;
    }
    else if (mode == PIN_IRQ_MODE_LOW_LEVEL)
    {
        GPIO_InitStruct.GPIO_ITTrigger = GPIO_INT_Trigger_LEVEL;
        GPIO_InitStruct.GPIO_ITPolarity = GPIO_INT_POLARITY_ACTIVE_LOW;
    }

    GPIO_Init(GPIO_GetPort(pin), &GPIO_InitStruct);

    gpio_cb[GPIO_GetNum(pin)].gpio_cb = hdr;
    gpio_cb[GPIO_GetNum(pin)].args = args;
    RamVectorTableUpdate_ns_ext(table_vector[GPIO_GetNum(pin)], table_func[GPIO_GetNum(pin)]);

    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = table_irq[GPIO_GetNum(pin)];
    NVIC_InitStruct.NVIC_IRQChannelPriority = 3;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);
    return 0;
}

uint8_t drv_pin_dettach_irq(uint32_t pin)
{
    gpio_cb[GPIO_GetNum(pin)].gpio_cb = NULL;
    gpio_cb[GPIO_GetNum(pin)].args = NULL;
    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = table_irq[GPIO_GetNum(pin)];
    NVIC_InitStruct.NVIC_IRQChannelPriority = 3;
    NVIC_InitStruct.NVIC_IRQChannelCmd = DISABLE;
    NVIC_Init(&NVIC_InitStruct);
    return 0;
}

uint8_t drv_pin_irq_enable(uint32_t pin, uint32_t enabled)
{
    if (PIN_IRQ_ENABLE == enabled)
    {
        GPIO_MaskINTConfig(GPIO_GetPort(pin), GPIO_GetPin(pin), ENABLE);
        GPIO_INTConfig(GPIO_GetPort(pin), GPIO_GetPin(pin), ENABLE);
        GPIO_ClearINTPendingBit(GPIO_GetPort(pin), GPIO_GetPin(pin));
        //platform_delay_us(10);
        os_delay(2);
        GPIO_MaskINTConfig(GPIO_GetPort(pin), GPIO_GetPin(pin), DISABLE);
    }
    else
    {
        GPIO_INTConfig(GPIO_GetPort(pin), GPIO_GetPin(pin), DISABLE);
        GPIO_ClearINTPendingBit(GPIO_GetPort(pin), GPIO_GetPin(pin));
        GPIO_MaskINTConfig(GPIO_GetPort(pin), GPIO_GetPin(pin), DISABLE);
    }
    return 0;
}

void hw_pin_init(void)
{

}

