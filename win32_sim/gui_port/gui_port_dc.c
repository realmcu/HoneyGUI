
#include "guidef.h"
#include "gui_api.h"
#include "gui_port.h"
#include <SDL.h>
#include <pthread.h>
#include "unistd.h"
#include "tp_algo.h"
#include "kb_algo.h"

#ifndef DRV_LCD_WIDTH
#define DRV_LCD_WIDTH   480
#endif
#ifndef DRV_LCD_HIGHT
#define DRV_LCD_HIGHT   480
#endif
#ifndef DRV_PIXEL_BITS
#define DRV_PIXEL_BITS   16
#endif

// #define USE_DC_PFB

#ifdef USE_DC_PFB
#define LCD_SECTION_HEIGHT 20
#endif

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;

static SDL_Rect _rect = { 0, 0, DRV_LCD_WIDTH, DRV_LCD_HIGHT};
static SDL_Surface *surface;
static int bpp;                              /* texture bits per pixel */
static uint32_t Rmask, Gmask, Bmask, Amask;  /* masks for pixel format passed into OpenGL */
static pthread_mutex_t sdl_ok_mutex;
static pthread_cond_t sdl_ok_event;

static gui_touch_port_data_t tp_port_data = {0};

static gui_kb_port_data_t kb_port_data = {0};

static gui_wheel_port_data_t wheel_port_data = {0};


int sim_screen_width = DRV_LCD_WIDTH;
int sim_screen_hight = DRV_LCD_HIGHT;
int32_t sim_get_width(void)
{
    return sim_screen_width;
}
int32_t sim_get_hight(void)
{
    return sim_screen_hight;
}


static void lcd_update_window(uint8_t *input, uint8_t *output, uint16_t xStart, uint16_t yStart,
                              uint16_t w, uint16_t h)
{
#if (DRV_PIXEL_BITS == 32)
    uint32_t *read = (uint32_t *)input;
    uint32_t *write = (uint32_t *)output;
#else
    uint16_t *read = (uint16_t *)input;
    uint16_t *write = (uint16_t *)output;
#endif
    for (uint32_t i = yStart; i < (h + yStart); i++)
    {
        for (uint32_t j = xStart; j < (w + xStart); j++)
        {
            write[i * sim_get_width() + j] = *read;
            read++;
        }
    }
}

void port_direct_draw_bitmap_to_lcd(int16_t x, int16_t y, int16_t width, int16_t height,
                                    const uint8_t *bitmap)
{
    uint8_t *dst = surface->pixels;
    lcd_update_window((uint8_t *)bitmap, dst, x, y, width, height);
}

void port_gui_lcd_update(struct gui_dispdev *dc)
{
#ifdef USE_DC_PFB

    uint8_t *dst = surface->pixels;

    if (dc->section_count == dc->section_total - 1)
    {
        lcd_update_window(dc->frame_buf, dst, dc->section.x1, dc->section.y1,
                          dc->section.x2 - dc->section.x1 + 1, dc->section.y2 - dc->section.y1 + 1);
    }
    else
    {
        lcd_update_window(dc->frame_buf, dst, dc->section.x1, dc->section.y1,
                          dc->section.x2 - dc->section.x1 + 1, dc->section.y2 - dc->section.y1 + 1);

#if 0 //for debug
        SDL_Texture *texture;
        texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_NONE);
        SDL_RenderCopy(renderer, texture, NULL, &_rect);
        SDL_RenderPresent(renderer);
        SDL_DestroyTexture(texture);
#endif
    }
#else
    memcpy(surface->pixels, dc->frame_buf, sim_get_width() * sim_get_hight() * DRV_PIXEL_BITS / 8);
#endif

    return;
}

static struct gui_dispdev dc =
{
#ifdef USE_DC_PFB
    .bit_depth = DRV_PIXEL_BITS,
    // .fb_height = DRV_LCD_HIGHT,
    // .fb_width = LCD_SECTION_HEIGHT,
    .fb_height = LCD_SECTION_HEIGHT,
    .fb_width = DRV_LCD_WIDTH,
    .screen_width =  DRV_LCD_WIDTH,
    .screen_height = DRV_LCD_HIGHT,
    .driver_ic_fps = 30,
    .driver_ic_active_width = DRV_LCD_WIDTH,

    .section = {0, 0, 0, 0},
    .section_count = 0,
    .scale_x = 1,
    .scale_y = 1,
    .disp_buf_1 = NULL,
    .disp_buf_2 = NULL,
    .frame_buf = NULL,
    .type = DC_RAMLESS,
    .lcd_update = port_gui_lcd_update,
#else
    .bit_depth = DRV_PIXEL_BITS,
    .fb_height = DRV_LCD_HIGHT,
    .fb_width = DRV_LCD_WIDTH,
    .screen_width =  DRV_LCD_WIDTH,
    .screen_height = DRV_LCD_HIGHT,
    .driver_ic_fps = 30,
    .driver_ic_active_width = DRV_LCD_WIDTH,
    .scale_x = 1,
    .scale_y = 1,
    .type = DC_SINGLE,
    .lcd_update = port_gui_lcd_update,
#endif
    .direct_draw_bitmap_to_lcd = port_direct_draw_bitmap_to_lcd,

};

static void *sdl_flush(void *arg)
{
    while (true)
    {
        usleep((1000 / 60) * 1000);
        //todo
        SDL_Texture *texture;
        texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_NONE);
        SDL_RenderCopy(renderer, texture, NULL, &_rect);
        SDL_RenderPresent(renderer);
        SDL_DestroyTexture(texture);
    }
}

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

    char str[50];
    sprintf(str, "RTKIOT GUI Simulator %d x %d", sim_get_width(), sim_get_hight());

    window = SDL_CreateWindow(str, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              sim_get_width(),
                              sim_get_hight(), 0);
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

    surface = SDL_CreateRGBSurface(0, sim_get_width(), sim_get_hight(), bpp, Rmask, Gmask, Bmask,
                                   Amask);
    SDL_RenderPresent(renderer);

    pthread_cond_signal(&sdl_ok_event);
    pthread_mutex_unlock(&sdl_ok_mutex);

    pthread_t thread;
    pthread_create(&thread, NULL, sdl_flush, NULL);

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
                tp_port_data.x_coordinate = event.button.x;
                tp_port_data.y_coordinate = event.button.y;
            }
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
                    wheel_port_data.timestamp_ms = gui_ms_get();
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
                    wheel_port_data.timestamp_ms = gui_ms_get();
                }
            }
            break;
        case SDL_MOUSEWHEEL:
            {

                wheel_port_data.delta = event.wheel.y;
                wheel_port_data.timestamp_ms = gui_ms_get();
                wheel_port_data.event = GUI_WHEEL_SCROLL;
            }
            break;
        case SDL_KEYDOWN:
            {
                // gui_log("[SDL_KEYDOWN]key %s down!\n", SDL_GetKeyName(event.key.keysym.sym));
                kb_port_data.event = GUI_KB_EVENT_DOWN;
                kb_port_data.timestamp_ms_press = gui_ms_get();
                kb_port_data.timestamp_ms_pressing = gui_ms_get();
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
                kb_port_data.timestamp_ms_release = gui_ms_get();
                memset(kb_port_data.name, 0x00, sizeof(kb_port_data.name));

                const char *key_name = SDL_GetKeyName(event.key.keysym.sym);
                strncpy(kb_port_data.name, key_name, sizeof(kb_port_data.name) - 1);
                kb_port_data.name[sizeof(kb_port_data.name) - 1] = '\0';
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
#ifdef ENABLE_RTK_GUI_SCRIPT_AS_A_APP
    char *apppath = "app";
    char *path = gui_malloc(strlen(apppath) + strlen(GUI_ROOT_FOLDER) + 1);
    sprintf(path, "%s%s", GUI_ROOT_FOLDER, apppath);

    extern void xml_get_screen(char *dirPath, char *xml_file, int *width, int *hight);
    extern int sim_screen_width;
    extern int sim_screen_hight;
    xml_get_screen(path, 0, &sim_screen_width, &sim_screen_hight);
    _rect.w = sim_screen_width;
    _rect.h = sim_screen_hight;
    dc.fb_width = sim_screen_width;
    dc.screen_width = sim_screen_width;
    dc.screen_height = sim_screen_hight;
#ifndef USE_DC_PFB
    dc.fb_height = sim_screen_hight;
#endif
#endif
    pthread_mutex_init(&sdl_ok_mutex, NULL);
    pthread_cond_init(&sdl_ok_event, NULL);

    pthread_t thread;
    pthread_create(&thread, NULL, rtk_gui_sdl, NULL);


    pthread_mutex_lock(&sdl_ok_mutex);
    pthread_cond_wait(&sdl_ok_event, &sdl_ok_mutex);


    pthread_mutex_destroy(&sdl_ok_mutex);
    pthread_cond_destroy(&sdl_ok_event);

    gui_dc_info_register(&dc);
#ifdef USE_DC_PFB
    dc.disp_buf_1 =  malloc(dc.fb_height * dc.fb_width * DRV_PIXEL_BITS / 8);
    dc.disp_buf_2 =  malloc(dc.fb_height * dc.fb_width * DRV_PIXEL_BITS / 8);
#else
    dc.frame_buf = malloc(dc.fb_height * dc.fb_width * DRV_PIXEL_BITS / 8);
#endif

    dc.lcd_gram = surface->pixels;

}



struct gui_touch_port_data *port_touchpad_get_data()
{
    tp_port_data.timestamp_ms = gui_ms_get();//todo
#ifdef ENABLE_MONKEY_TEST
    static uint32_t live_time = 0;
    if (gui_ms_get() >= live_time + 5000)
    {
        live_time += 5000;
        gui_log("win_32 sim get tp data, GUI live \n");
    }
#endif
    return &tp_port_data;
}

gui_kb_port_data_t *port_kb_get_data(void)
{
    return &kb_port_data;
}

gui_wheel_port_data_t *port_wheel_get_data(void)
{
    return &wheel_port_data;
}


static struct gui_indev indev =
{
    .tp_get_data = port_touchpad_get_data,
    .kb_get_port_data = port_kb_get_data,
    .wheel_get_port_data = port_wheel_get_data,
    .tp_height = 0,
    .tp_witdh = 0,
    .touch_timeout_ms = 110,
    .long_button_time_ms = 800,
    .short_button_time_ms = 800,
    .quick_slide_time_ms = 50,

    .kb_short_button_time_ms = 30,
    .kb_long_button_time_ms = 800,
};


void gui_port_indev_init(void)
{
    gui_indev_info_register(&indev);
}




