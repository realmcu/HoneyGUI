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

    rtgui_server_init();
#ifdef BUILD_USING_RTK_GUI_DEMO
    extern gui_app_t *get_launcher_app(void);
    gui_app_startup(get_launcher_app());
#else
    extern gui_app_t *get_rtk_gui_demo(void);
    gui_app_install(get_rtk_gui_demo(), get_rtk_gui_demo()->ui_design, NULL);
    gui_app_startup(get_rtk_gui_demo());
#endif

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

