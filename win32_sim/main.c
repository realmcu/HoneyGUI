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

    while (1)
    {
        time_t now;
        now = time(NULL);
        printf("World Time: %.*s\n", 25, ctime(&now));
        sleep(1000);
    }

    return 0;
}


