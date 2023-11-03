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


#define BEE3_PRO       0
#define BEE3_PRO_HK    1
#define BEE4_HK        2

#ifdef BUILD_USING_RTK_GUI_454_454_DEMO
#define UI_DEMO        BEE3_PRO
#endif
#ifdef BUILD_USING_RTK_GUI_448_368_DEMO
#define UI_DEMO        BEE3_PRO_HK
#endif
#ifdef BUILD_USING_RTK_GUI_320_384_DEMO
#define UI_DEMO        BEE4_HK
#endif



uint8_t resource_root[1024 * 1024 * 200];

char *defaultPath = "./gui_engine/example/screen_454_454/root_image/root/";
int main(int argc, char **argv)
{

    for (int count = 1; count < argc; count++)
    {
        if (!strcmp(argv[count], "-p") && count + 1 < argc)
        {
            char *path = malloc(strlen(argv[count + 1]) + strlen("\\") + 1);
            sprintf(path, "%s%s", argv[count + 1], "\\");
            defaultPath = path;
        }
    }

    printf("defaultPath path: %s\n", defaultPath);
    //extern int nanosv_main(void);
    //nanosv_main();
#ifndef RTK_GUI_SCRIPT_AS_A_APP
    int fd;
#if UI_DEMO == BEE3_PRO
    fd = open("./example/screen_454_454/root_image/root(0x4400000).bin", 0);
#endif
#if UI_DEMO == BEE3_PRO_HK
    fd = open("./gui_engine/example/screen_448_368/root_image_hongkong/root(0x4400000).bin", 0);
#endif
#if UI_DEMO == BEE4_HK
    fd = open("./example/screen_320_384/root_image_hongkong_bee4/root(0x4400000).bin", 0);
#endif
    if (fd > 0)
    {
        printf("open root(0x4400000).bin Successful!\n");
        read(fd, resource_root, 1024 * 1024 * 200);
    }
    else
    {
        printf("open root(0x4400000).bin Fail!\n");
        return 0;
    }

#endif


    rtgui_server_init();
//#ifdef BUILD_USING_RTK_GUI_DEMO
#if 1
#ifndef RTK_GUI_SCRIPT_AS_A_APP
#if UI_DEMO == BEE3_PRO
    extern gui_app_t *get_app_launcher(void);
    gui_app_startup(get_app_launcher());
#endif
#if UI_DEMO == BEE3_PRO_HK
    extern void *get_app_hongkong(void);
    extern void *get_app_lvgl(void);
    //gui_app_startup(get_app_hongkong());
    gui_app_startup(get_app_lvgl());
#endif
#if UI_DEMO == BEE4_HK
    extern void gui_load_bee4_hk(void);
    gui_load_bee4_hk();
#endif
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

