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



char *defaultPath = "gui_engine\\example\\screen_480_480\\root\\";
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
    gui_components_init();
    while (1)
    {
        time_t now;
        now = time(NULL);
        printf("World Time: %.*s\n", 25, ctime(&now));
        sleep(10);
    }

    return 0;
}


