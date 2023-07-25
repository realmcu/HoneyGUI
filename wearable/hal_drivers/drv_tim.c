/*
 * Copyright (c) 2022
 *
 */

#include <string.h>
#include "drv_tim.h"
#include "trace.h"
#include "os_sync.h"


void (*tim5_indicate)(void) = NULL;
void (*tim6_indicate)(void) = NULL;


void drv_tim5_start(void)
{
    TIM_Cmd(TIM5, ENABLE);
}

void drv_tim5_stop(void)
{
    TIM_Cmd(TIM5, DISABLE);
}

void drv_tim5_init(void)
{
    /* Enable TIMER peripheral clock */
#ifdef RTL8772F
    RCC_PeriphClockCmd(APBPeriph_TIMERB, APBPeriph_TIMERB_CLOCK, ENABLE);
#endif
    /* Configure TIMER init parameters */
    TIM_TimeBaseInitTypeDef TIM_InitStruct;
    TIM_StructInit(&TIM_InitStruct);
    TIM_InitStruct.TIM_PWM_En = DISABLE;
    TIM_InitStruct.TIM_ClockSrc = CK_40M_VCORE4;
    //TIM_InitStruct.TIM_ClockSrc = CK_32K_TIMER;
    TIM_InitStruct.TIM_Period = 40 * 10000 - 1;
    TIM_InitStruct.TIM_Mode = TIM_Mode_UserDefine;
    TIM_TimeBaseInit(TIM5, &TIM_InitStruct);

    /* Enable NVIC */
    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = Timer_B2_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 3;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_SetIRQNonSecure(NVIC_InitStruct.NVIC_IRQChannel);
    NVIC_Init(&NVIC_InitStruct);

    TIM_ClearINT(TIM5);
    TIM_INTConfig(TIM5, ENABLE);
}

void drv_tim6_start(void)
{
    TIM_Cmd(TIM6, ENABLE);
}

void drv_tim6_stop(void)
{
    TIM_Cmd(TIM6, DISABLE);
}

uint32_t drv_tim6_read(void)// unit: 1us
{
    //return TIM_GetCurrentValue(TIM6);
    return (~TIM_GetCurrentValue(TIM6));
}

void drv_tim6_init(void)
{
    /* Enable TIMER peripheral clock */
#ifdef RTL8772F
    RCC_PeriphClockCmd(APBPeriph_TIMERB, APBPeriph_TIMERB_CLOCK, ENABLE);
#endif
    /* Configure TIMER init parameters */
    TIM_TimeBaseInitTypeDef TIM_InitStruct;
    TIM_StructInit(&TIM_InitStruct);
    TIM_InitStruct.TIM_PWM_En = DISABLE;
    TIM_InitStruct.TIM_ClockSrc = CK_40M_VCORE4;
    //TIM_InitStruct.TIM_ClockSrc = CK_32K_TIMER;
    TIM_InitStruct.TIM_ClockSrcDiv_En = ENABLE;
    TIM_InitStruct.TIM_ClockSrcDiv = TIM_CLOCK_DIVIDER_40;
    TIM_InitStruct.TIM_Mode = TIM_Mode_FreeRun;
    TIM_TimeBaseInit(TIM6, &TIM_InitStruct);

}

static void tim_isr(void (*tim_ind)(void), TIM_TypeDef *TIMx)
{
    TIM_ClearINT(TIMx);
    TIM_Cmd(TIMx, DISABLE);
    if (tim_ind != NULL)
    {
        //Add user code here
        tim_ind();
    }
    TIM_Cmd(TIMx, ENABLE);
}

void Timer_B2_Handler(void)
{
    tim_isr(tim5_indicate, TIM5);
}

void drv_tim5_set_indicate(void (*tim_ind)(void))
{
    tim5_indicate = tim_ind;
}

void Timer_B3_Handler(void)
{
    tim_isr(tim6_indicate, TIM6);
}

void drv_tim6_set_indicate(void (*tim_ind)(void))
{
    tim6_indicate = tim_ind;
}

void hw_tim_init(void)
{
    DBG_DIRECT("hw_tim_init");
}

/************** end of file ********************/
