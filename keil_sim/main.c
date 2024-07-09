/*
 * Copyright (c) 2009-2020 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*============================ INCLUDES ======================================*/
#include <stdio.h>
#include "RTE_Components.h"
#ifdef RTE_Compiler_IO_STDOUT_EVR
#include "EventRecorder.h"
#endif
#include "Board_GLCD.h"
#include "GLCD_Config.h"
#include "gui_server.h"
#include "gui_components_init.h"
#include "time.h"
#include "cmsis_os2.h"                  // ARM::CMSIS:RTOS2:Keil RTX5



/*----------------------------------------------------------------------------
 *      Main: Initialize and start RTX Kernel
 *---------------------------------------------------------------------------*/
void app_main(void *argument)
{


//    GLCD_Initialize();                          /* Initialize the GLCD            */

//    /* display initial screen */
//    GLCD_SetBackgroundColor(GLCD_COLOR_RED);
//    GLCD_ClearScreen();
//    //GLCD_SetBackgroundColor(GLCD_COLOR_BLUE);
//    GLCD_SetForegroundColor(GLCD_COLOR_GREEN);

    gui_components_init();


    while (1)
    {
        //time_t now;
        //now = time(NULL);
        //printf("World Time: %.*s\n", 25, ctime(&now));
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
    printf("Realtek Bee GUI Welcome \r\n");
    // ...
    osKernelInitialize();                 // Initialize CMSIS-RTOS
    osThreadNew(app_main, NULL, NULL);    // Create application main thread
    if (osKernelGetState() == osKernelReady)
    {
        osKernelStart();                    // Start thread execution
    }

    while (1);

}

