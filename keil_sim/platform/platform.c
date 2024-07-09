/****************************************************************************
*  Copyright 2022 Gorgon Meducer (Email:embedded_zhuoran@hotmail.com)       *
*                                                                           *
*  Licensed under the Apache License, Version 2.0 (the "License");          *
*  you may not use this file except in compliance with the License.         *
*  You may obtain a copy of the License at                                  *
*                                                                           *
*     http://www.apache.org/licenses/LICENSE-2.0                            *
*                                                                           *
*  Unless required by applicable law or agreed to in writing, software      *
*  distributed under the License is distributed on an "AS IS" BASIS,        *
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
*  See the License for the specific language governing permissions and      *
*  limitations under the License.                                           *
*                                                                           *
****************************************************************************/

/*============================ INCLUDES ======================================*/

#include "app_cfg.h"
#include "platform.h"
#include "cmsis_compiler.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>

#include "SSE300MPS3.h"

#if defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wunknown-warning-option"
#   pragma clang diagnostic ignored "-Wreserved-identifier"
#   pragma clang diagnostic ignored "-Wformat-nonliteral"
#   pragma clang diagnostic ignored "-Wsign-compare"
#   pragma clang diagnostic ignored "-Wmissing-prototypes"
#   pragma clang diagnostic ignored "-Wcast-qual"
#   pragma clang diagnostic ignored "-Wsign-conversion"
#   pragma clang diagnostic ignored "-Wgnu-statement-expression"
#elif defined(__IS_COMPILER_GCC__)
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wpedantic"
#endif

/*============================ MACROS ========================================*/
#ifndef MIN
#define MIN(__A, __B)        ((__A) < (__B) ? (__A) : (__B))
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

//void SysTick_Handler(void)
//{

//}

__WEAK
bool device_specific_init(void)
{
    //! every 5ms
    //SysTick_Config(SystemCoreClock / 1000);

    return false;
}

__attribute__((used, constructor(101)))
void app_platform_init(void)
{
    init_cycle_counter(device_specific_init());

    //serial_init();

    GLCD_Initialize();                          /* Initialize the GLCD      */

    /* display initial screen */
    GLCD_SetFont(&GLCD_Font_6x8);
    GLCD_SetBackgroundColor(GLCD_COLOR_BLACK);
    GLCD_ClearScreen();
    //GLCD_SetBackgroundColor(GLCD_COLOR_BLUE);
    GLCD_SetForegroundColor(GLCD_COLOR_GREEN);
}


int lcd_printf(int16_t x, int16_t y, const char *format, ...)
{
    int real_size;
    static char s_chBuffer[64];
    __va_list ap;
    va_start(ap, format);
    real_size = vsnprintf(s_chBuffer, sizeof(s_chBuffer) - 1, format, ap);
    va_end(ap);
    real_size = MIN(sizeof(s_chBuffer) - 1, real_size);
    s_chBuffer[real_size] = '\0';
    GLCD_DrawString(x, y, s_chBuffer);
    return real_size;
}

/*
 * Semihosting is a mechanism that enables code running on an ARM target
 * to communicate and use the Input/Output facilities of a host computer
 * that is running a debugger.
 * There is an issue where if you use armclang at -O0 optimisation with
 * no parameters specified in the main function, the initialisation code
 * contains a breakpoint for semihosting by default. This will stop the
 * code from running before main is reached.
 * Semihosting can be disabled by defining __ARM_use_no_argv symbol
 * (or using higher optimization level).
 */
#if __IS_COMPILER_ARM_COMPILER_6__
__asm(".global __use_no_semihosting\n\t");
#   ifndef __MICROLIB
__asm(".global __ARM_use_no_argv\n\t");
#   endif

__NO_RETURN
void _sys_exit(int ret)
{
    // UNUSED_PARAM(ret);
    while (1) {}
}

#endif

#if defined(__MICROLIB)
_ARMABI_NORETURN
ARM_NONNULL(1, 2)
void __aeabi_assert(const char *chCond, const char *chLine, int wErrCode)
{
    ARM_2D_UNUSED(chCond);
    ARM_2D_UNUSED(chLine);
    ARM_2D_UNUSED(wErrCode);
    while (1)
    {
        __NOP();
    }
}
#endif

#if !defined(__IS_COMPILER_GCC__)
void _ttywrch(int ch)
{
    // UNUSED_PARAM(ch);
}
#endif


#if defined(__clang__)
#   pragma clang diagnostic pop
#elif defined(__IS_COMPILER_GCC__)
#   pragma GCC diagnostic pop
#endif