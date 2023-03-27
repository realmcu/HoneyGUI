
#include "guidef.h"
#include "gui_api.h"
#include "gui_port.h"
#include <SDL.h>
#include <pthread.h>
#include "unistd.h"
#include "time.h"
#include "tp_algo.h"

#define DRV_LCD_WIDTH   454
#define DRV_LCD_HIGHT   454
#define DRV_PIXEL_BITS  32



static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;

static SDL_Rect _rect = { 0, 0, DRV_LCD_WIDTH, DRV_LCD_HIGHT};
static SDL_Surface *surface;
static int bpp;                              /* texture bits per pixel */
static uint32_t Rmask, Gmask, Bmask, Amask;  /* masks for pixel format passed into OpenGL */
static pthread_mutex_t sdl_ok_mutex;
static pthread_cond_t sdl_ok_event;

static struct gui_touch_data raw_data = {0};

static gui_kb_port_data_t kb_port_data = {0};


void port_gui_lcd_update(struct gui_dispdev *dc)
{
    SDL_Texture *texture;
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_NONE);
    SDL_RenderCopy(renderer, texture, NULL, &_rect);
    SDL_RenderPresent(renderer);
    SDL_DestroyTexture(texture);

    return;
}

static struct gui_dispdev dc =
{

    .bit_depth = DRV_PIXEL_BITS,
    .fb_height = DRV_LCD_HIGHT,
    .fb_width = DRV_LCD_WIDTH,
    .screen_width =  DRV_LCD_WIDTH,
    .screen_height = DRV_LCD_HIGHT,
    .driver_ic_fps = 30,
    .driver_ic_active_width = DRV_LCD_WIDTH,
    .type = DC_SINGLE,

    .section = {0, 0, 0, 0},
    .section_count = 0,
    .adaption = false,
    .scale_x = 1,
    .scale_y = 1,
    .disp_buf_1 = NULL,
    .disp_buf_2 = NULL,
    .frame_buf = NULL,

    .lcd_update = port_gui_lcd_update,

};



void *rtk_gui_sdl(void *arg)
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
                raw_data.x_coordinate = event.button.x;
                raw_data.y_coordinate = event.button.y;
            }
            break;

        case SDL_MOUSEBUTTONDOWN:
            {
                SDL_MouseButtonEvent *mb;
                mb = (SDL_MouseButtonEvent *)&event;
                raw_data.event = GUI_TOUCH_EVENT_DOWN;
                raw_data.x_coordinate = event.button.x;
                raw_data.y_coordinate = event.button.y;
                //gui_log("mouse down:(%d,%d)\n", event.button.x, event.button.y);
            }
            break;

        case SDL_MOUSEBUTTONUP:
            {
                SDL_MouseButtonEvent *mb;
                mb = (SDL_MouseButtonEvent *)&event;
                raw_data.event = GUI_TOUCH_EVENT_UP;
                raw_data.x_coordinate = event.button.x;
                raw_data.y_coordinate = event.button.y;
                //gui_log("mouse down:(%d,%d)\n", event.button.x, event.button.y);
            }
            break;
        case SDL_MOUSEWHEEL:
            {
                gui_log("[SDL_MOUSEWHEEL]:(%d,%d)\n", event.button.x, event.button.y);
            }
            break;
        case SDL_KEYDOWN:
            {
                gui_log("[SDL_KEYDOWN]key %s down!\n", SDL_GetKeyName(event.key.keysym.sym));
                kb_port_data.flag = true;
                memset(kb_port_data.name, 0x00, 10);
                memcpy(kb_port_data.name, SDL_GetKeyName(event.key.keysym.sym),
                       strlen(SDL_GetKeyName(event.key.keysym.sym)));
            }
            break;
        case SDL_KEYUP:
            {
                gui_log("[SDL_KEYUP]key %s up!\n", SDL_GetKeyName(event.key.keysym.sym));
                kb_port_data.flag = false;
                memset(kb_port_data.name, 0x00, 10);
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

void gui_port_dc_init(void)
{
    pthread_mutex_init(&sdl_ok_mutex, NULL);
    pthread_cond_init(&sdl_ok_event, NULL);

    pthread_t thread;
    pthread_create(&thread, NULL, rtk_gui_sdl, NULL);


    pthread_mutex_lock(&sdl_ok_mutex);
    pthread_cond_wait(&sdl_ok_event, &sdl_ok_mutex);
    dc.frame_buf = surface->pixels;


    pthread_mutex_destroy(&sdl_ok_mutex);
    pthread_cond_destroy(&sdl_ok_event);

    gui_dc_info_register(&dc);
}



struct gui_touch_data *port_touchpad_get_data()
{
    unsigned long now = (unsigned long) time(NULL);
    struct timeval tv;
    mingw_gettimeofday(&tv, NULL);
    uint32_t tp_tick = (0xFFFF & tv.tv_sec) * 1000 + tv.tv_usec / 1000;


    raw_data.timestamp_ms = tp_tick;//todo


    return &raw_data;
}

gui_kb_port_data_t *port_kb_get_data(void)
{
    return &kb_port_data;
}


static struct gui_indev indev =
{
    .tp_get_data = port_touchpad_get_data,
    .kb_get_port_data = port_kb_get_data,
    .tp_height = 0,
    .tp_witdh = 0,
    .touch_timeout_ms = 110,
    .long_button_time_ms = 800,
    .short_button_time_ms = 300,
    .quick_slide_time_ms = 50,
};


void gui_port_indev_init(void)
{

    gui_indev_info_register(&indev);
}




