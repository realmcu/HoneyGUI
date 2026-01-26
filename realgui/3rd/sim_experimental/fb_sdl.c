/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#include <SDL.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include "fb_sdl.h"

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static SDL_Texture *texture = NULL;
static SDL_Surface *surface = NULL;
static pthread_mutex_t sdl_ok_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t sdl_ok_event = PTHREAD_COND_INITIALIZER;

int sim_screen_width = DRV_LCD_WIDTH;
int sim_screen_height = DRV_LCD_HEIGHT;

// Mouse interaction state
float mouse_rot_x = 0.0f;
float mouse_rot_y = 0.0f;
float mouse_zoom = 3.0f;
static int mouse_down = 0;
static int last_mouse_x = 0;
static int last_mouse_y = 0;

int32_t sim_get_width(void)  { return sim_screen_width; }
int32_t sim_get_height(void) { return sim_screen_height; }

void port_direct_draw_bitmap_to_lcd(int16_t x, int16_t y, int16_t width, int16_t height,
                                    const uint8_t *bitmap)
{
#if (DRV_PIXEL_BITS == 32)
    uint32_t *src = (uint32_t *)bitmap;
    uint32_t *dst = (uint32_t *)surface->pixels;
#else
    uint16_t *src = (uint16_t *)bitmap;
    uint16_t *dst = (uint16_t *)surface->pixels;
#endif
    for (int i = y; i < y + height; i++)
    {
        for (int j = x; j < x + width; j++)
        {
            dst[i * sim_screen_width + j] = *src++;
        }
    }
}

static void *rtk_gui_sdl(void *arg)
{
    (void)arg;

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        fprintf(stderr, "SDL init failed: %s\n", SDL_GetError());
        return NULL;
    }

    char title[64];
    sprintf(title, "HoneyGUI Simulator %d x %d", sim_screen_width, sim_screen_height);

    window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              sim_screen_width, sim_screen_height, 0);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    Uint32 format = (DRV_PIXEL_BITS == 16) ? SDL_PIXELFORMAT_RGB565 : SDL_PIXELFORMAT_ARGB8888;
    texture = SDL_CreateTexture(renderer, format, SDL_TEXTUREACCESS_STREAMING,
                                sim_screen_width, sim_screen_height);

    int bpp;
    Uint32 Rmask, Gmask, Bmask, Amask;
    SDL_PixelFormatEnumToMasks(format, &bpp, &Rmask, &Gmask, &Bmask, &Amask);
    surface = SDL_CreateRGBSurface(0, sim_screen_width, sim_screen_height, bpp,
                                   Rmask, Gmask, Bmask, Amask);

    pthread_mutex_lock(&sdl_ok_mutex);
    pthread_cond_signal(&sdl_ok_event);
    pthread_mutex_unlock(&sdl_ok_mutex);

    SDL_Event event;
    while (true)
    {
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
                SDL_DestroyTexture(texture);
                SDL_DestroyRenderer(renderer);
                SDL_DestroyWindow(window);
                SDL_Quit();
                exit(0);
            case SDL_MOUSEBUTTONDOWN:
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    mouse_down = 1;
                    last_mouse_x = event.button.x;
                    last_mouse_y = event.button.y;
                }
                break;
            case SDL_MOUSEBUTTONUP:
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    mouse_down = 0;
                }
                break;
            case SDL_MOUSEMOTION:
                if (mouse_down)
                {
                    mouse_rot_y += (event.motion.x - last_mouse_x) * 0.5f;
                    mouse_rot_x += (event.motion.y - last_mouse_y) * 0.5f;
                    last_mouse_x = event.motion.x;
                    last_mouse_y = event.motion.y;
                }
                break;
            case SDL_MOUSEWHEEL:
                mouse_zoom -= event.wheel.y * 0.3f;
                if (mouse_zoom < 1.0f) { mouse_zoom = 1.0f; }
                if (mouse_zoom > 10.0f) { mouse_zoom = 10.0f; }
                break;
            }
        }

        SDL_UpdateTexture(texture, NULL, surface->pixels, surface->pitch);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
        usleep(16000);
    }
}

__attribute__((constructor(1000))) void gui_port_dc_init(void)
{
    printf("gui_port_dc_init\n");

    pthread_t thread;
    pthread_create(&thread, NULL, rtk_gui_sdl, NULL);

    pthread_mutex_lock(&sdl_ok_mutex);
    pthread_cond_wait(&sdl_ok_event, &sdl_ok_mutex);
    pthread_mutex_unlock(&sdl_ok_mutex);
}
