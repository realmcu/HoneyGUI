/*
 * Copyright (c) 2022
 *
 */

#include <string.h>
#include "drv_tim.h"
#include "trace.h"
#include "os_sync.h"


void (*tim5_indicate)(void) = NULL;


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

static void tim_isr(void (*tim_ind)(void), TIM_TypeDef *TIMx)
{
    TIM_ClearINT(TIMx);
    TIM_Cmd(TIMx, DISABLE);
    //Add user code here
    tim_ind();
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

bool drv_tim5_exit_dlps(void)
{
    /* Enable NVIC */
//    NVIC_InitTypeDef NVIC_InitStruct;
//    NVIC_InitStruct.NVIC_IRQChannel = Timer_B2_IRQn;
//    NVIC_InitStruct.NVIC_IRQChannelPriority = 3;
//    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
//    NVIC_SetIRQNonSecure(NVIC_InitStruct.NVIC_IRQChannel);
//    NVIC_Init(&NVIC_InitStruct);
    drv_tim5_init();
    DBG_DIRECT("drv_tim5_exit_dlps");
    return true;
}

void hw_tim_init(void)
{
    DBG_DIRECT("hw_tim_init");
    drv_dlps_exit_cbacks_register("tim5", drv_tim5_exit_dlps);
}

/************** end of file ********************/
