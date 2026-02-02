/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#include <SDL.h>
#include <pthread.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "unistd.h"
#include "tp_algo.h"
#include "kb_algo.h"
#include "sdl_driver.h"
#include "sdl_driver_panel.h"
#include "gui_api.h"

static uint32_t simulator_width = 480;
static uint32_t simulator_height = 480;
static uint32_t simulator_pixel_bits = 32;

SDL_Surface *windowSurface = NULL;
SDL_Surface *customSurface = NULL;

static float scale_x = 1.0f;
static float scale_y = 1.0f;

/* Canvas mode configuration */
static bool canvas_mode = false;
static int canvas_width = 0;
static int canvas_height = 0;
static int screen_offset_x = 0;
static int screen_offset_y = 0;
static int screen_corner_radius = 0;  /* 0=rect, -1=circle, >0=rounded corner radius */
static SDL_Surface *frameSurface = NULL;
static SDL_Surface *maskSurface = NULL;

static pthread_mutex_t sdl_init_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t sdl_init_cond = PTHREAD_COND_INITIALIZER;
static bool sdl_initialized = false;

extern gui_touch_port_data_t tp_port_data;
extern gui_wheel_port_data_t wheel_port_data;

static void create_screen_mask(int w, int h, int radius)
{
    maskSurface = SDL_CreateRGBSurfaceWithFormat(0, w, h, 32, SDL_PIXELFORMAT_ARGB8888);
    uint32_t *pixels = (uint32_t *)maskSurface->pixels;
    int cx = w / 2, cy = h / 2;
    int circle_r = (w < h ? w : h) / 2;

    for (int y = 0; y < h; y++)
    {
        for (int x = 0; x < w; x++)
        {
            bool visible = true;
            if (radius < 0)
            {
                /* Circle */
                int dx = x - cx, dy = y - cy;
                visible = (dx * dx + dy * dy <= circle_r * circle_r);
            }
            else if (radius > 0)
            {
                /* Rounded rectangle - check four corners */
                int r = radius;
                int dx, dy;
                if (x < r && y < r)
                {
                    dx = x - r; dy = y - r;
                    visible = (dx * dx + dy * dy <= r * r);
                }
                else if (x >= w - r && y < r)
                {
                    dx = x - (w - r - 1); dy = y - r;
                    visible = (dx * dx + dy * dy <= r * r);
                }
                else if (x < r && y >= h - r)
                {
                    dx = x - r; dy = y - (h - r - 1);
                    visible = (dx * dx + dy * dy <= r * r);
                }
                else if (x >= w - r && y >= h - r)
                {
                    dx = x - (w - r - 1); dy = y - (h - r - 1);
                    visible = (dx * dx + dy * dy <= r * r);
                }
            }
            pixels[y * w + x] = visible ? 0x00000000 : 0xFF282828;
        }
    }
}

static void update_touch_coordinates(int mouse_x, int mouse_y)
{
    if (canvas_mode)
    {
        tp_port_data.x_coordinate = (int)((mouse_x / scale_x) - screen_offset_x);
        tp_port_data.y_coordinate = (int)((mouse_y / scale_y) - screen_offset_y);
    }
    else
    {
        tp_port_data.x_coordinate = (int)(mouse_x / scale_x);
        tp_port_data.y_coordinate = (int)(mouse_y / scale_y);
    }
}

void sdl_driver_update_window(uint8_t *input, uint16_t x, uint16_t y,
                              uint16_t w, uint16_t h)
{
    uint8_t *gram = (uint8_t *)customSurface->pixels;
    uint32_t x1 = x, y1 = y;
    uint32_t x2 = x + w - 1, y2 = y + h - 1;

    if (simulator_pixel_bits == 32)
    {
        BLIT_RECT_EX(uint32_t, gram, input, simulator_width, x1, x2, y1, y2);
    }
    else if (simulator_pixel_bits == 16)
    {
        BLIT_RECT_EX(uint16_t, gram, input, simulator_width, x1, x2, y1, y2);
    }
    else if (simulator_pixel_bits == 8)
    {
        BLIT_RECT_EX(uint8_t, gram, input, simulator_width, x1, x2, y1, y2);
    }

    SDL_Event ev;
    SDL_memset(&ev, 0, sizeof(ev));
    ev.type = SDL_USEREVENT_LCD_UPDATE;
    SDL_PushEvent(&ev);
}

void *sdl_driver_thread(void *arg)
{
    (void)arg;
    SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO);

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s\n", SDL_GetError());
        return NULL;
    }

    char str[50];
    sprintf(str, "RTKIOT GUI Simulator %u x %u", simulator_width, simulator_height);

    int win_w = canvas_mode ? canvas_width : (int)simulator_width;
    int win_h = canvas_mode ? canvas_height : (int)simulator_height;

    SDL_Window *window = SDL_CreateWindow(str, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          win_w, win_h, SDL_WINDOW_RESIZABLE);
    windowSurface = SDL_GetWindowSurface(window);

    if (canvas_mode)
    {
        SDL_FillRect(windowSurface, NULL, SDL_MapRGB(windowSurface->format, 40, 40, 40));
    }

    customSurface = SDL_CreateRGBSurfaceWithFormat(0, simulator_width, simulator_height,
                                                   simulator_pixel_bits,
                                                   simulator_pixel_bits == 32 ? SDL_PIXELFORMAT_ARGB8888 :
                                                   simulator_pixel_bits == 16 ? SDL_PIXELFORMAT_RGB565 :
                                                   SDL_PIXELFORMAT_INDEX8);

    if (screen_corner_radius != 0)
    {
        create_screen_mask(simulator_width, simulator_height, screen_corner_radius);
    }

    /* Notify main thread that initialization is complete */
    pthread_mutex_lock(&sdl_init_mutex);
    sdl_initialized = true;
    pthread_cond_signal(&sdl_init_cond);
    pthread_mutex_unlock(&sdl_init_mutex);

    SDL_Event event;
    while (true)
    {
        SDL_WaitEventTimeout(&event, 0xffffffff);
        switch (event.type)
        {
        case SDL_USEREVENT_LCD_UPDATE:
            {
                /* Flush pending LCD_UPDATE events to prevent event queue buildup */
                SDL_Event next;
                while (SDL_PeepEvents(&next, 1, SDL_PEEKEVENT, SDL_USEREVENT_LCD_UPDATE,
                                      SDL_USEREVENT_LCD_UPDATE) > 0)
                {
                    SDL_PeepEvents(&next, 1, SDL_GETEVENT, SDL_USEREVENT_LCD_UPDATE,
                                   SDL_USEREVENT_LCD_UPDATE);
                }

                if (canvas_mode)
                {
                    SDL_FillRect(windowSurface, NULL, SDL_MapRGB(windowSurface->format, 40, 40, 40));
                    SDL_Rect dstRect =
                    {
                        (int)(screen_offset_x * scale_x),
                        (int)(screen_offset_y * scale_y),
                        (int)(simulator_width * scale_x),
                        (int)(simulator_height * scale_y)
                    };
                    SDL_BlitScaled(customSurface, NULL, windowSurface, &dstRect);
                    if (maskSurface)
                    {
                        SDL_BlitScaled(maskSurface, NULL, windowSurface, &dstRect);
                    }
                    if (frameSurface)
                    {
                        SDL_Rect frameRect = {0, 0, windowSurface->w, windowSurface->h};
                        SDL_BlitScaled(frameSurface, NULL, windowSurface, &frameRect);
                    }
                    sdl_panel_draw(windowSurface);
                }
                else if (scale_x != 1.0f || scale_y != 1.0f)
                {
                    SDL_Rect dstRect = {0, 0, windowSurface->w, windowSurface->h};
                    SDL_BlitScaled(customSurface, NULL, windowSurface, &dstRect);
                    sdl_panel_draw(windowSurface);
                }
                else
                {
                    SDL_BlitSurface(customSurface, NULL, windowSurface, NULL);
                    sdl_panel_draw(windowSurface);
                }
                SDL_UpdateWindowSurface(window);
            }
            break;

        case SDL_MOUSEMOTION:
            update_touch_coordinates(event.button.x, event.button.y);
            break;

        case SDL_MOUSEBUTTONDOWN:
            if (!sdl_panel_check_click(event.button.x, event.button.y, true))
            {
                tp_port_data.event = GUI_TOUCH_EVENT_DOWN;
                update_touch_coordinates(event.button.x, event.button.y);
            }
            if (event.button.button == SDL_BUTTON_MIDDLE)
            {
                wheel_port_data.event = GUI_WHEEL_BUTTON_DOWN;
            }
            break;

        case SDL_MOUSEBUTTONUP:
            if (!sdl_panel_check_click(event.button.x, event.button.y, false))
            {
                tp_port_data.event = GUI_TOUCH_EVENT_UP;
                update_touch_coordinates(event.button.x, event.button.y);
            }
            if (event.button.button == SDL_BUTTON_MIDDLE)
            {
                wheel_port_data.event = GUI_WHEEL_BUTTON_UP;
            }
            break;

        case SDL_MOUSEWHEEL:
            wheel_port_data.delta = event.wheel.y;
            wheel_port_data.event = GUI_WHEEL_SCROLL;
            break;

        case SDL_WINDOWEVENT:
            if (event.window.event == SDL_WINDOWEVENT_RESIZED ||
                event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
            {
                int base_w = canvas_mode ? canvas_width : (int)simulator_width;
                int base_h = canvas_mode ? canvas_height : (int)simulator_height;
                windowSurface = SDL_GetWindowSurface(window);
                scale_x = (float)event.window.data1 / base_w;
                scale_y = (float)event.window.data2 / base_h;
            }
            break;

        case SDL_QUIT:
            SDL_Quit();
            exit(0);
            break;

        default:
            break;
        }
    }
}

void sdl_driver_set_canvas(int cw, int ch, const char *frame_path)
{
    canvas_mode = true;
    canvas_width = cw;
    canvas_height = ch;

    if (frame_path)
    {
        frameSurface = SDL_LoadBMP(frame_path);
    }
}

void sdl_driver_set_screen(int offset_x, int offset_y, int corner_radius)
{
    screen_offset_x = offset_x;
    screen_offset_y = offset_y;
    screen_corner_radius = corner_radius;
}

void sdl_driver_init(uint32_t width, uint32_t height, uint32_t pixel_bits)
{
    simulator_width = width;
    simulator_height = height;
    simulator_pixel_bits = pixel_bits;

    pthread_t thread;
    pthread_create(&thread, NULL, sdl_driver_thread, NULL);

    /* Wait for SDL initialization to complete */
    pthread_mutex_lock(&sdl_init_mutex);
    while (!sdl_initialized)
    {
        pthread_cond_wait(&sdl_init_cond, &sdl_init_mutex);
    }
    pthread_mutex_unlock(&sdl_init_mutex);
}
