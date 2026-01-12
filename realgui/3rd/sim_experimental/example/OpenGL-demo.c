/*
 * OpenGL Demo Skeleton
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include "fb_sdl.h"

static int opengl_example(void)
{
    printf("OpenGL demo skeleton\n");
    return 0;
}

__attribute__((constructor(1001))) static int opengl_demo(void)
{
    return opengl_example();
}

