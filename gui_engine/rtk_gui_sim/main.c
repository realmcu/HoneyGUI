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
#include "menu_config.h"
#include "signal.h"
#include "time.h"




uint8_t resource_root[1024 * 1024 * 20];

char *defaultPath = "example/root_image/root_image_sim_vscode/root/";
int main(int argc, char **argv)
{
    for (int count = 1; count < argc; count++)
    {
        if (!strcmp(argv[count], "-p") && count + 1 < argc)
        {
            defaultPath = argv[count + 1];
        }
    }

    printf("defaultPath path: %s\n", defaultPath);
    //extern int nanosv_main(void);
    //nanosv_main();
#ifndef RTK_GUI_SCRIPT_AS_A_APP
    int fd;
    fd = open("./example/root_image/root_image_sim_vscode/root(0x4400000).bin", 0);
    if (fd > 0)
    {
        printf("open root(0x4400000).bin Successful!\n");
        read(fd, resource_root, 1024 * 1024 * 20);
    }
    else
    {
        printf("open root(0x4400000).bin Fail!\n");
        return 0;
    }

#endif


    rtgui_server_init();
//#ifdef MODULE_USING_RTK_GUI_DEMO
#if 1
#ifndef RTK_GUI_SCRIPT_AS_A_APP
    extern gui_app_t *get_app_launcher(void);
    gui_app_startup(get_app_launcher());
#endif
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
        sleep(10);
    }

    return 0;
}

