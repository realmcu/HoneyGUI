/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*============================ INCLUDES ======================================*/
#include <stdio.h>
#include "RTE_Components.h"
#ifdef RTE_Compiler_IO_STDOUT_EVR
#include "EventRecorder.h"
#endif
#include "cmsis_os2.h"                  // ARM::CMSIS:RTOS2:Keil RTX5
#include "Board_GLCD.h"
#include "GLCD_Config.h"
#include "gui_server.h"
#include "time.h"



/*----------------------------------------------------------------------------
 *      Main: Initialize and start RTX Kernel
 *---------------------------------------------------------------------------*/
void app_main(void *argument)
{


    GLCD_Initialize();                          /* Initialize the GLCD            */

    /* display initial screen */
    GLCD_SetBackgroundColor(GLCD_COLOR_RED);
    GLCD_ClearScreen();
    //GLCD_SetBackgroundColor(GLCD_COLOR_BLUE);
    GLCD_SetForegroundColor(GLCD_COLOR_GREEN);

    gui_server_init();


    while (1)
    {
        time_t now;
        now = time(NULL);
        printf("World Time: %.*s\n", 25, ctime(&now));
        osDelay(10000);
    }
}
/*----------------------------------------------------------------------------
  Main function
 *----------------------------------------------------------------------------*/
int main(void)
{
#ifdef RTE_Compiler_IO_STDOUT_EVR
    EventRecorderInitialize(0, 1);
#endif
    printf("Realtek GUI Welcome \r\n");
    // ...
    osKernelInitialize();                 // Initialize CMSIS-RTOS
    osThreadNew(app_main, NULL, NULL);    // Create application main thread
    if (osKernelGetState() == osKernelReady)
    {
        osKernelStart();                    // Start thread execution
    }

    while (1);

}

