/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "unistd.h"
#include "time.h"

#define LOG_VERSION_NUM                "x.x.x"
#define COMPILE_TIME                    __DATE__", "__TIME__
#define PROJECT_NAME                   "RealTek Display Simulator"
#define COMPANY_NAME                   "Realtek Semiconductor Corporation"


int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    printf("\n\n\t************** %s **************\t\n   \t <%s> \t Build Time: %s\n\n", \
           COMPANY_NAME, \
           PROJECT_NAME, \
           COMPILE_TIME);

    while (1)
    {
        time_t now;
        now = time(NULL);
        printf("World Time: %.*s\n", 25, ctime(&now));
        sleep(1000);
    }

    return 0;
}


