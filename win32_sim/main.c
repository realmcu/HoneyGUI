/*
  Copyright (C) 1997-2022 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely.
*/


#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "gui_server.h"
#include "unistd.h"
#include "time.h"
#include "gui_components_init.h"
#include "rvd.h"
//#define RVD_APP_STARTUP
#define LOG_VERSION_NUM                "x.x.x"
#define COMPILE_TIME                    __DATE__", "__TIME__
#define PROJECT_NAME                   "HoneyGUI Simulator"
#define COMPANY_NAME                   "Realtek Semiconductor Corporation"


int main(int argc, char **argv)
{
    printf("\n\n\t************** %s **************\t\n   \t <%s> \t Build Time: %s\n\n", \
           COMPANY_NAME, \
           PROJECT_NAME, \
           COMPILE_TIME);

    gui_change_default_path(argc, argv);
    gui_components_init();
#ifdef ENABLE_RTK_GUI_SCRIPT_AS_A_APP
    extern gui_app_t *get_app_launcher_frontend(void);
    gui_app_install(get_app_launcher_frontend(), get_app_launcher_frontend()->ui_design, NULL);
#ifdef RVD_APP_STARTUP
    /*Startup RVD APP (index 0)*/
    int rst = rvd_startup_app(0);
    gui_log("RVD_APP_STARTUP:%d\n", rst);
#else
    /*Startup RVD launcher*/

    gui_app_startup(get_app_launcher_frontend());
#endif
#endif
//gui_update_screen_force(true);
    while (1)
    {
        time_t now;
        now = time(NULL);
        printf("World Time: %.*s\n", 25, ctime(&now));
        sleep(1000);
    }

    return 0;
}


