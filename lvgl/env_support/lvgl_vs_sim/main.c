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
#include <stdint.h>
#include <unistd.h>
#include "unistd.h"
#include "menu_config.h"
#include "signal.h"
#include "time.h"
#include "sdl_init.h"



int main(int argc, char **argv)
{

    sdl_dc_init();

    while (1)
    {
        time_t now;
        now = time(NULL);
        printf("World Time: %.*s\n", 25, ctime(&now));
        sleep(10);
    }

    return 0;
}

