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

#define LOG_VERSION_NUM                "x.x.x"
#define COMPILE_TIME                    __DATE__", "__TIME__
#define PROJECT_NAME                   "GUI Simulator"
#define COMPANY_NAME                    "Realtek Semiconductor Corporation"

char *defaultPath = "gui_engine\\example\\screen_480_480\\root\\";
// char *defaultPath = "gui_engine\\example\\screen_480_480\\Visual Disigner Prj\\480x480\\Export\\root\\";
int main(int argc, char **argv)
{
    printf("\n\n\t************** %s **************\t\n   \t <%s> \t Build Time: %s\n\n", COMPANY_NAME,
           PROJECT_NAME, COMPILE_TIME);

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

    // TODO: SASA auto get screen size

    gui_components_init();



    while (1)
    {
        time_t now;
        now = time(NULL);
        printf("World Time: %.*s\n", 25, ctime(&now));
        // int age;
        // scanf("%d", &age);
        // printf("Your age is %d \n", age);
        sleep(1000);
    }

    return 0;
}


