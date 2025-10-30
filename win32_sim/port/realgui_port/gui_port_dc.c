
#include "guidef.h"
#include "gui_api.h"
#include "gui_port.h"
#include <SDL.h>
#include <pthread.h>
#include "unistd.h"
#include "tp_algo.h"
#include "kb_algo.h"
#include "gui_message.h"
#ifndef DRV_LCD_WIDTH
#define DRV_LCD_WIDTH   480
#endif
#ifndef DRV_LCD_HEIGHT
#define DRV_LCD_HEIGHT   480
#endif
#ifndef DRV_PIXEL_BITS
#define DRV_PIXEL_BITS   16
#endif

#define USE_DC_PFB

#ifdef USE_DC_PFB
#define LCD_SECTION_HEIGHT 20
#endif

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;

static SDL_Rect _rect = { 0, 0, DRV_LCD_WIDTH, DRV_LCD_HEIGHT};
static SDL_Surface *surface;
static int bpp;                              /* texture bits per pixel */
static uint32_t Rmask, Gmask, Bmask, Amask;  /* masks for pixel format passed into OpenGL */
static pthread_mutex_t sdl_ok_mutex;
static pthread_cond_t sdl_ok_event;

static gui_touch_port_data_t tp_port_data = {0};

static gui_kb_port_data_t kb_port_data = {0};

static gui_wheel_port_data_t wheel_port_data = {0};


int sim_screen_width = DRV_LCD_WIDTH;
int sim_screen_height = DRV_LCD_HEIGHT;
int32_t sim_get_width(void)
{
    return sim_screen_width;
}
int32_t sim_get_height(void)
{
    return sim_screen_height;
}


static void lcd_update_window(uint8_t *input, uint8_t *output, uint16_t xStart, uint16_t yStart,
                              uint16_t w, uint16_t h)
{
    uint32_t x1 = xStart;
    uint32_t y1 = yStart;
    uint32_t x2 = xStart + w;
    uint32_t y2 = yStart + h;
#if (DRV_PIXEL_BITS == 32)
    uint32_t *read = (uint32_t *)input;
    uint32_t *write = (uint32_t *)output;
    for (uint32_t i = y1; i < y2; i++)
    {
        for (uint32_t j = x1; j < x2; j++)
        {
            write[i * sim_get_width() + j] = *read;
            read++;
        }
    }
#elif (DRV_PIXEL_BITS == 16)
    uint16_t *read = (uint16_t *)input;
    uint16_t *write = (uint16_t *)output;
    for (uint32_t i = y1; i < y2; i++)
    {
        for (uint32_t j = x1; j < x2; j++)
        {
            write[i * sim_get_width() + j] = *read;
            read++;
        }
    }
#elif (DRV_PIXEL_BITS == 8)
    uint8_t *read = (uint8_t *)input;
    uint8_t *write = (uint8_t *)output;
    for (uint32_t i = y1; i < y2; i++)
    {
        for (uint32_t j = x1; j < x2; j++)
        {
            write[i * sim_get_width() + j] = *read;
            read++;
        }
    }
#elif (DRV_PIXEL_BITS == 4)
    uint8_t *read = (uint8_t *)input;
    uint8_t *write = (uint8_t *)output;

    uint16_t byte_x_start = x1 / 2;
    uint16_t byte_x_end = (x2 + 1) / 2;
    uint16_t x_offset = x1 % 2;

    for (uint32_t y = y1; y < y2; y++)
    {
        uint32_t src_row_offset = (y - y1) * w / 2;
        uint32_t dst_row_offset = y * (sim_get_width() / 2);

        if (x_offset)
        {
            uint8_t src_byte = read[src_row_offset];
            uint8_t dst_byte = write[dst_row_offset + byte_x_start];

            write[dst_row_offset + byte_x_start] = (dst_byte & 0x0F) | (src_byte << 4);
            byte_x_start++;
            src_row_offset++;
        }

        if (byte_x_start < byte_x_end)
        {
            uint32_t byte_count = byte_x_end - byte_x_start;
            memcpy(&write[dst_row_offset + byte_x_start],
                   &read[src_row_offset],
                   byte_count);
        }
    }
#else
    uint32_t *read = (uint32_t *)input;
    uint32_t *write = (uint32_t *)output;
    for (uint32_t i = y1; i < y2; i++)
    {
        for (uint32_t j = x1; j < x2; j++)
        {
            write[i * sim_get_width() + j] = *read;
            read++;
        }
    }
#endif
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
    memcpy(surface->pixels, dc->frame_buf, sim_get_width() * sim_get_height() * DRV_PIXEL_BITS / 8);
#endif

    return;
}

static struct gui_dispdev dc =
{
#ifdef USE_DC_PFB
    .bit_depth = DRV_PIXEL_BITS,
    // .fb_height = DRV_LCD_HEIGHT,
    // .fb_width = LCD_SECTION_HEIGHT,
    .fb_height = LCD_SECTION_HEIGHT,
    .fb_width = DRV_LCD_WIDTH,
    .screen_width =  DRV_LCD_WIDTH,
    .screen_height = DRV_LCD_HEIGHT,

    .section = {0, 0, 0, 0},
    .section_count = 0,
    .disp_buf_1 = NULL,
    .disp_buf_2 = NULL,
    .frame_buf = NULL,
    .type = DC_RAMLESS,
    .lcd_update = port_gui_lcd_update,
#else
    .bit_depth = DRV_PIXEL_BITS,
    .fb_height = DRV_LCD_HEIGHT,
    .fb_width = DRV_LCD_WIDTH,
    .screen_width =  DRV_LCD_WIDTH,
    .screen_height = DRV_LCD_HEIGHT,
    .type = DC_SINGLE,
    .lcd_update = port_gui_lcd_update,
#endif
    .direct_draw_bitmap_to_lcd = port_direct_draw_bitmap_to_lcd,

};

static void *sdl_flush(void *arg)
{
    (void)arg;
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
    (void)arg;
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
    sprintf(str, "RTKIOT GUI Simulator %d x %d", sim_get_width(), sim_get_height());

    window = SDL_CreateWindow(str, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              sim_get_width(),
                              sim_get_height(), 0);
    renderer = SDL_CreateRenderer(window, -1, 0);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);


    /* Grab info about format that will be passed into OpenGL */
    if (DRV_PIXEL_BITS == 16)
    {
        SDL_PixelFormatEnumToMasks(SDL_PIXELFORMAT_RGB565, &bpp, &Rmask, &Gmask, &Bmask, &Amask);
        surface = SDL_CreateRGBSurface(0, sim_get_width(), sim_get_height(), bpp, Rmask, Gmask, Bmask,
                                       Amask);
    }
    else if (DRV_PIXEL_BITS == 32)
    {
        SDL_PixelFormatEnumToMasks(SDL_PIXELFORMAT_ARGB8888, &bpp, &Rmask, &Gmask, &Bmask, &Amask);
        surface = SDL_CreateRGBSurface(0, sim_get_width(), sim_get_height(), bpp, Rmask, Gmask, Bmask,
                                       Amask);
    }
    else if (DRV_PIXEL_BITS == 4)
    {
        SDL_PixelFormatEnumToMasks(SDL_PIXELFORMAT_INDEX4LSB, &bpp, &Rmask, &Gmask, &Bmask, &Amask);

        surface = SDL_CreateRGBSurface(0, sim_get_width(), sim_get_height(), bpp, Rmask, Gmask, Bmask,
                                       Amask);

        // SDL_Color palette[16] = {
        //     {0, 0, 0, 255},       // Black
        //     {255, 0, 0, 255},     // Red
        //     {0, 255, 0, 255},     // Green
        //     {0, 0, 255, 255},     // Blue
        //     {255, 255, 0, 255},   // Yellow
        //     {255, 0, 255, 255},   // Magenta
        //     {0, 255, 255, 255},   // Cyan
        //     {255, 255, 255, 255}, // White
        //     {128, 128, 128, 255}, // Gray
        //     {128, 0, 0, 255},     // Dark Red
        //     {0, 128, 0, 255},     // Dark Green
        //     {0, 0, 128, 255},     // Dark Blue
        //     {128, 128, 0, 255},   // Olive
        //     {128, 0, 128, 255},   // Purple
        //     {0, 128, 128, 255},   // Teal
        //     {192, 192, 192, 255}  // Silver
        // };
        SDL_Color greenPalette[16] =
        {
            {0,   0,   0, 255},  // Level 0: Black (no green)
            {0,  17,   0, 255},  // Level 1
            {0,  34,   0, 255},  // Level 2
            {0,  51,   0, 255},  // Level 3
            {0,  68,   0, 255},  // Level 4
            {0,  85,   0, 255},  // Level 5
            {0, 102,   0, 255},  // Level 6
            {0, 119,   0, 255},  // Level 7
            {0, 136,   0, 255},  // Level 8
            {0, 153,   0, 255},  // Level 9
            {0, 170,   0, 255},  // Level 10
            {0, 187,   0, 255},  // Level 11
            {0, 204,   0, 255},  // Level 12
            {0, 221,   0, 255},  // Level 13
            {0, 238,   0, 255},  // Level 14
            {0, 255,   0, 255}   // Level 15: Full Green
        };

        SDL_SetPaletteColors(surface->format->palette, greenPalette, 0, 16);
    }
    else if (DRV_PIXEL_BITS == 8)
    {
        SDL_PixelFormatEnumToMasks(SDL_PIXELFORMAT_INDEX8, &bpp, &Rmask, &Gmask, &Bmask, &Amask);

        surface = SDL_CreateRGBSurface(0, sim_get_width(), sim_get_height(), bpp, Rmask, Gmask, Bmask,
                                       Amask);

        SDL_Color greenPalette[256];
        for (uint32_t i = 0; i < 256; i++)
        {
            greenPalette[i].a = 255;
            greenPalette[i].r = 0;
            greenPalette[i].g = i;
            greenPalette[i].b = 0;
        }
        SDL_SetPaletteColors(surface->format->palette, greenPalette, 0, 256);
    }


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



struct gui_touch_port_data *port_touchpad_get_data(void)
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




