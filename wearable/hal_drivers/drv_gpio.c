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
#include "trace.h"
#if defined RTL8762G
#include "vector_table.h"
#endif
#if defined RTL8772F
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

static void DRV_GPIO_HANDLER(uint32_t pin)
{
    uint8_t num = GPIO_GetNum(pin);
#if defined RTL8772F || defined RTL8762G
    GPIO_ClearINTPendingBit(GPIO_GetPort(pin), GPIO_GetPin(pin));
#elif defined RTL8762D
    GPIO_ClearINTPendingBit(GPIO_GetPin(pin));
#endif

    if (gpio_cb[num].gpio_cb != NULL)
    {
        gpio_cb[num].gpio_cb(gpio_cb[num].args);
    }
}

#if defined RTL8772F
#include "drv_gpio_8772f.c"
#elif defined RTL8762D
#include "drv_gpio_8762d.c"
#elif defined RTL8762G
#include "drv_gpio_8762g.c"
#endif

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

#if defined RTL8772F || defined RTL8762G
    GPIO_Init(GPIO_GetPort(pin), &GPIO_InitStruct);
#elif defined RTL8762D
    GPIO_Init(&GPIO_InitStruct);
#endif
}

void drv_pin_write(uint32_t pin, uint32_t value)
{
#if defined RTL8772F || defined RTL8762G
    GPIO_WriteBit(GPIO_GetPort(pin), GPIO_GetPin(pin), (BitAction)(value));
#elif defined RTL8762D
    GPIO_WriteBit(GPIO_GetPin(pin), (BitAction)(value));
#endif
}

uint8_t drv_pin_read(uint32_t pin)
{
    uint8_t value;

    value = PIN_LOW;

#if defined RTL8772F || defined RTL8762G
    value = GPIO_ReadInputDataBit(GPIO_GetPort(pin), GPIO_GetPin(pin));
#elif defined RTL8762D
    value = GPIO_ReadInputDataBit(GPIO_GetPin(pin));
#endif

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

#if defined RTL8772F || defined RTL8762G
    GPIO_Init(GPIO_GetPort(pin), &GPIO_InitStruct);
#elif defined RTL8762D
    GPIO_Init(&GPIO_InitStruct);
#endif

    gpio_cb[GPIO_GetNum(pin)].gpio_cb = hdr;
    gpio_cb[GPIO_GetNum(pin)].args = args;
#if defined RTL8772F || defined RTL8762G
    RamVectorTableUpdate_ns_ext(table_vector[GPIO_GetNum(pin)], table_func[GPIO_GetNum(pin)]);
#elif defined RTL8762D
    //extern bool RamVectorTableUpdate(VECTORn_Type v_num, IRQ_Fun isr_handler);
    //RamVectorTableUpdate(table_vector[GPIO_GetNum(pin)], table_func[GPIO_GetNum(pin)]);
#endif

    NVIC_InitTypeDef NVIC_InitStruct;
#if defined RTL8772F || defined RTL8762G
    NVIC_InitStruct.NVIC_IRQChannel = table_irq[GPIO_GetNum(pin)];
#elif defined RTL8762D
    NVIC_InitStruct.NVIC_IRQChannel = pin2irq(pin);
#endif
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
#if defined RTL8772F || defined RTL8762G
    NVIC_InitStruct.NVIC_IRQChannel = table_irq[GPIO_GetNum(pin)];
#elif defined RTL8762D
    NVIC_InitStruct.NVIC_IRQChannel = pin2irq(pin);
#endif
    NVIC_InitStruct.NVIC_IRQChannelPriority = 3;
    NVIC_InitStruct.NVIC_IRQChannelCmd = DISABLE;
    NVIC_Init(&NVIC_InitStruct);
    return 0;
}

uint8_t drv_pin_irq_enable(uint32_t pin, uint32_t enabled)
{

#if defined RTL8772F || defined RTL8762G
    if (PIN_IRQ_ENABLE == enabled)
    {
        GPIO_MaskINTConfig(GPIO_GetPort(pin), GPIO_GetPin(pin), ENABLE);
        GPIO_INTConfig(GPIO_GetPort(pin), GPIO_GetPin(pin), ENABLE);
        GPIO_ClearINTPendingBit(GPIO_GetPort(pin), GPIO_GetPin(pin));
        platform_delay_us(10);
        GPIO_MaskINTConfig(GPIO_GetPort(pin), GPIO_GetPin(pin), DISABLE);
    }
    else
    {
        GPIO_INTConfig(GPIO_GetPort(pin), GPIO_GetPin(pin), DISABLE);
        GPIO_ClearINTPendingBit(GPIO_GetPort(pin), GPIO_GetPin(pin));
        GPIO_MaskINTConfig(GPIO_GetPort(pin), GPIO_GetPin(pin), DISABLE);
    }
#elif defined RTL8762D
    if (PIN_IRQ_ENABLE == enabled)
    {
        GPIO_MaskINTConfig(GPIO_GetPin(pin), ENABLE);
        GPIO_INTConfig(GPIO_GetPin(pin), ENABLE);
        GPIO_ClearINTPendingBit(GPIO_GetPin(pin));
        platform_delay_us(100);
        GPIO_MaskINTConfig(GPIO_GetPin(pin), DISABLE);
    }
    else
    {
        GPIO_INTConfig(GPIO_GetPin(pin), DISABLE);
        GPIO_ClearINTPendingBit(GPIO_GetPin(pin));
        GPIO_MaskINTConfig(GPIO_GetPin(pin), DISABLE);
    }
#endif

    return 0;
}

void hw_pin_init(void)
{

}

