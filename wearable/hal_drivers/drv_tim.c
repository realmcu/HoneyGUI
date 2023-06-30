/*
 * Copyright (c) 2022
 *
 */

#include <string.h>
#include "drv_tim.h"
#include "trace.h"
#include "os_sync.h"


void (*tim4_indicate)(void) = NULL;


void drv_tim4_start(void)
{
    TIM_Cmd(TIM4, ENABLE);
}

void drv_tim4_stop(void)
{
    TIM_Cmd(TIM4, DISABLE);
}

void drv_tim4_init(void)
{
    /* Enable TIMER peripheral clock */
#ifdef RTL8772F
    RCC_PeriphClockCmd(APBPeriph_TIMERB, APBPeriph_TIMERB_CLOCK, DISABLE);
    RCC_PeriphClockCmd(APBPeriph_TIMERB, APBPeriph_TIMERB_CLOCK, ENABLE);
#endif
    /* Configure TIMER init parameters */
    TIM_TimeBaseInitTypeDef TIM_InitStruct;
    TIM_StructInit(&TIM_InitStruct);
    TIM_InitStruct.TIM_PWM_En = DISABLE;
    TIM_InitStruct.TIM_Period = 40 * 10000 - 1;
    TIM_InitStruct.TIM_Mode = TIM_Mode_UserDefine;
    TIM_TimeBaseInit(TIM4, &TIM_InitStruct);

    /* Enable NVIC */
    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = Timer_B1_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 3;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_SetIRQNonSecure(NVIC_InitStruct.NVIC_IRQChannel);
    NVIC_Init(&NVIC_InitStruct);

    TIM_ClearINT(TIM4);
    TIM_INTConfig(TIM4, ENABLE);
}

static void tim_isr(void (*tim_ind)(void), TIM_TypeDef *TIMx)
{
    TIM_ClearINT(TIMx);
    TIM_Cmd(TIMx, DISABLE);
    //Add user code here
    tim_ind();
    TIM_Cmd(TIMx, ENABLE);
}

void Timer_B1_Handler(void)
{
    tim_isr(tim4_indicate, TIM4);
}

void drv_tim4_set_indicate(void (*tim_ind)(void))
{
    tim4_indicate = tim_ind;
}

bool drv_tim4_exit_dlps(void)
{
    /* Enable NVIC */
//    NVIC_InitTypeDef NVIC_InitStruct;
//    NVIC_InitStruct.NVIC_IRQChannel = Timer_B1_IRQn;
//    NVIC_InitStruct.NVIC_IRQChannelPriority = 3;
//    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
//    NVIC_SetIRQNonSecure(NVIC_InitStruct.NVIC_IRQChannel);
//    NVIC_Init(&NVIC_InitStruct);
    drv_tim4_init();
    DBG_DIRECT("drv_tim4_exit_dlps");
    return true;
}

void hw_tim_init(void)
{
    DBG_DIRECT("hw_tim_init");
    drv_dlps_exit_cbacks_register("tim4", drv_tim4_exit_dlps);
}

/************** end of file ********************/
