#include <SDL.h>
#include <pthread.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "unistd.h"
#include "tp_algo.h"
#include "kb_algo.h"
#include "sdl_driver.h"

static uint32_t sdl_driver_width = 480;
static uint32_t sdl_driver_height = 480;
static uint32_t sdl_driver_pixel_bits = 32;

SDL_Surface *windowSurface = NULL;
SDL_Surface *customSurface = NULL;

static pthread_mutex_t sdl_init_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t sdl_init_cond = PTHREAD_COND_INITIALIZER;
static bool sdl_initialized = false;

extern gui_touch_port_data_t tp_port_data;
extern gui_kb_port_data_t kb_port_data;
extern gui_wheel_port_data_t wheel_port_data;



void sdl_driver_update_window(uint8_t *input, uint16_t x, uint16_t y,
                              uint16_t w, uint16_t h)
{
    uint8_t *gram = (uint8_t *)customSurface->pixels;
    uint32_t x1 = x;
    uint32_t y1 = y;
    uint32_t x2 = x + w - 1;
    uint32_t y2 = y + h - 1;
    if (sdl_driver_pixel_bits == 32)
    {
        BLIT_RECT_EX(uint32_t, gram, input, sdl_driver_width, x1, x2, y1, y2);
    }
    else if (sdl_driver_pixel_bits == 16)
    {
        BLIT_RECT_EX(uint16_t, gram, input, sdl_driver_width, x1, x2, y1, y2);
    }
    else if (sdl_driver_pixel_bits == 8)
    {
        BLIT_RECT_EX(uint8_t,  gram, input, sdl_driver_width, x1, x2, y1, y2);
    }

    SDL_Event ev;
    SDL_memset(&ev, 0, sizeof(ev));
    ev.type = SDL_USEREVENT_LCD_UPDATE;
    SDL_PushEvent(&ev);
}

void *sdl_driver_thread(void *arg)
{
    (void)arg;
    /* Enable standard application logging */
    SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO);

    /* Load the SDL library */
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s\n", SDL_GetError());
        return NULL;
    }

    char str[50];
    sprintf(str, "RTKIOT GUI Simulator %u x %u", sdl_driver_width, sdl_driver_height);

    SDL_Window *window = SDL_CreateWindow(str, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          sdl_driver_width, sdl_driver_height, 0);


    windowSurface = SDL_GetWindowSurface(window);


    customSurface = SDL_CreateRGBSurfaceWithFormat(0, sdl_driver_width, sdl_driver_height,
                                                   sdl_driver_pixel_bits, SDL_PIXELFORMAT_ARGB8888);

    customSurface = SDL_CreateRGBSurfaceWithFormat(0, sdl_driver_width, sdl_driver_height,
                                                   sdl_driver_pixel_bits, \
                                                   sdl_driver_pixel_bits == 32 ? SDL_PIXELFORMAT_ARGB8888 : \
                                                   sdl_driver_pixel_bits == 16 ? SDL_PIXELFORMAT_RGB565 : \
                                                   SDL_PIXELFORMAT_INDEX8);

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
                SDL_BlitSurface(customSurface, NULL, windowSurface, NULL);
                SDL_UpdateWindowSurface(window);
                break;
            }
        case SDL_MOUSEMOTION:
            tp_port_data.x_coordinate = event.button.x;
            tp_port_data.y_coordinate = event.button.y;
            break;
        case SDL_MOUSEBUTTONDOWN:
            {
                SDL_MouseButtonEvent *mb;
                mb = (SDL_MouseButtonEvent *)&event;
                tp_port_data.event = GUI_TOUCH_EVENT_DOWN;
                tp_port_data.x_coordinate = event.button.x;
                tp_port_data.y_coordinate = event.button.y;
                //gui_log("mouse down:(%d,%d)\n", event.button.x, event.button.y);
                if (event.button.button == SDL_BUTTON_MIDDLE)
                {
                    wheel_port_data.event = GUI_WHEEL_BUTTON_DOWN;
                }
            }
            break;
        case SDL_MOUSEBUTTONUP:
            {
                SDL_MouseButtonEvent *mb;
                mb = (SDL_MouseButtonEvent *)&event;
                tp_port_data.event = GUI_TOUCH_EVENT_UP;
                tp_port_data.x_coordinate = event.button.x;
                tp_port_data.y_coordinate = event.button.y;
                //gui_log("mouse up:(%d,%d)\n", event.button.x, event.button.y);
                if (event.button.button == SDL_BUTTON_MIDDLE)
                {
                    wheel_port_data.event = GUI_WHEEL_BUTTON_UP;
                }
            }
            break;
        case SDL_MOUSEWHEEL:
            {

                wheel_port_data.delta = event.wheel.y;
                wheel_port_data.event = GUI_WHEEL_SCROLL;
            }
            break;
        case SDL_KEYDOWN:
            {
                // gui_log("[SDL_KEYDOWN]key %s down!\n", SDL_GetKeyName(event.key.keysym.sym));
                kb_port_data.event = GUI_KB_EVENT_DOWN;
                memset(kb_port_data.name, 0x00, sizeof(kb_port_data.name));

                const char *key_name = SDL_GetKeyName(event.key.keysym.sym);
                strncpy(kb_port_data.name, key_name, sizeof(kb_port_data.name) - 1);
                kb_port_data.name[sizeof(kb_port_data.name) - 1] = '\0';
            }
            break;
        case SDL_KEYUP:
            {
                // gui_log("[SDL_KEYUP]key %s up!\n", SDL_GetKeyName(event.key.keysym.sym));
                kb_port_data.event = GUI_KB_EVENT_UP;
                memset(kb_port_data.name, 0x00, sizeof(kb_port_data.name));

                const char *key_name = SDL_GetKeyName(event.key.keysym.sym);
                strncpy(kb_port_data.name, key_name, sizeof(kb_port_data.name) - 1);
                kb_port_data.name[sizeof(kb_port_data.name) - 1] = '\0';
            }
            break;
        case SDL_QUIT:
            {
                SDL_Quit();
            }
            break;

        default:
            break;
        }
    }
}

void sdl_driver_init(uint32_t width, uint32_t height, uint32_t pixel_bits)
{
    /* Intentionally left empty */
    sdl_driver_width = width;
    sdl_driver_height = height;
    sdl_driver_pixel_bits = pixel_bits;
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






