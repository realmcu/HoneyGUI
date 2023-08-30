/*
  Copyright (C) 1997-2022 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely.
*/
#include <SDL.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdbool.h>
#include "lvgl.h"

#define DRV_LCD_WIDTH   MY_DISP_HOR_RES
#define DRV_LCD_HIGHT   MY_DISP_VER_RES
#define DRV_PIXEL_BITS  LV_COLOR_DEPTH



static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;

static SDL_Rect _rect = { 0, 0, DRV_LCD_WIDTH, DRV_LCD_HIGHT};
static SDL_Surface *surface;
static int bpp;                              /* texture bits per pixel */
static uint32_t Rmask, Gmask, Bmask, Amask;  /* masks for pixel format passed into OpenGL */
static pthread_mutex_t sdl_ok_mutex;
static pthread_cond_t sdl_ok_event;




static void *loop_sdl(void *arg)
{
    int quit = 0;
    /* Enable standard application logging */
    SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO);

    /* Load the SDL library */
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s\n", SDL_GetError());
        return NULL;
    }
    window = SDL_CreateWindow("Bee GUI Simulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              DRV_LCD_WIDTH,
                              DRV_LCD_HIGHT, 0);
    renderer = SDL_CreateRenderer(window, -1, 0);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);


    /* Grab info about format that will be passed into OpenGL */
    if (DRV_PIXEL_BITS == 16)
    {
        SDL_PixelFormatEnumToMasks(SDL_PIXELFORMAT_RGB565, &bpp, &Rmask, &Gmask, &Bmask, &Amask);
    }
    else if (DRV_PIXEL_BITS == 32)
    {
        SDL_PixelFormatEnumToMasks(SDL_PIXELFORMAT_ARGB8888, &bpp, &Rmask, &Gmask, &Bmask, &Amask);
    }

    surface = SDL_CreateRGBSurface(0, DRV_LCD_WIDTH, DRV_LCD_HIGHT, bpp, Rmask, Gmask, Bmask, Amask);
    SDL_RenderPresent(renderer);

    pthread_cond_signal(&sdl_ok_event);
    pthread_mutex_unlock(&sdl_ok_mutex);

    SDL_Event event;
    while (true)
    {
        SDL_WaitEventTimeout(&event, 0xffffffff);
        switch (event.type)
        {
        case SDL_MOUSEMOTION:
            {
                /* save to (x,y) in the motion */
                //gui_log("line = %d \n",__LINE__);
                //gui_log("mouse motion:(%d,%d)\n",event.button.x,event.button.y);
            }
            break;

        case SDL_MOUSEBUTTONDOWN:
            {
                SDL_MouseButtonEvent *mb;
                mb = (SDL_MouseButtonEvent *)&event;
                //gui_log("mouse down:(%d,%d)\n", event.button.x, event.button.y);
            }
            break;

        case SDL_MOUSEBUTTONUP:
            {
                SDL_MouseButtonEvent *mb;
                mb = (SDL_MouseButtonEvent *)&event;
                //gui_log("mouse down:(%d,%d)\n", event.button.x, event.button.y);
            }
            break;
        case SDL_MOUSEWHEEL:
            {
                //gui_log("[SDL_MOUSEWHEEL]:(%d,%d)\n", event.button.x, event.button.y);
            }
            break;
        case SDL_KEYDOWN:
            {
                //gui_log("[SDL_KEYDOWN]key %s down!\n", SDL_GetKeyName(event.key.keysym.sym));
            }
            break;
        case SDL_KEYUP:
            {
                //gui_log("[SDL_KEYUP]key %s up!\n", SDL_GetKeyName(event.key.keysym.sym));
            }
            break;
        case SDL_QUIT:
            {
                SDL_Quit();
                quit = 1;
            }
            break;

        default:
            break;
        }
    }
}


void sdl_dc_init(void)
{
    pthread_mutex_init(&sdl_ok_mutex, NULL);
    pthread_cond_init(&sdl_ok_event, NULL);

    pthread_t thread;
    pthread_create(&thread, NULL, loop_sdl, NULL);


    pthread_mutex_lock(&sdl_ok_mutex);
    pthread_cond_wait(&sdl_ok_event, &sdl_ok_mutex);
    //dc.frame_buf = surface->pixels;


    pthread_mutex_destroy(&sdl_ok_mutex);
    pthread_cond_destroy(&sdl_ok_event);
}

void sdl_dc_update(uint8_t *framebuffer, uint16_t xStart, uint16_t yStart, uint16_t w,
                   uint16_t h)
{
    uint16_t xEnd = xStart + w - 1;
    uint16_t yEnd = yStart + h - 1;
    if (DRV_PIXEL_BITS == 16)
    {
        uint16_t *pixel = (uint16_t *)framebuffer;
        uint16_t *write = surface->pixels;
        for (uint32_t i = yStart; i <= yEnd; i++)
        {
            for (uint32_t j = xStart; j <= xEnd; j++)
            {
                *(write + i * w + j) = *pixel;
                pixel++;
            }
        }
    }
    else if (DRV_PIXEL_BITS == 32)
    {
        uint32_t *pixel = (uint32_t *)framebuffer;
        uint32_t *write = surface->pixels;
        for (uint32_t i = yStart; i <= yEnd; i++)
        {
            for (uint32_t j = xStart; j <= xEnd; j++)
            {
                *(write + i * w + j) = *pixel;
                pixel++;
            }
        }
    }
    SDL_Texture *texture;

    texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_NONE);
    SDL_RenderCopy(renderer, texture, NULL, &_rect);
    SDL_RenderPresent(renderer);
    SDL_DestroyTexture(texture);
}




