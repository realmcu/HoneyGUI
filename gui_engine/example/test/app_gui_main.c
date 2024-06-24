#include "gui_img.h"
#include "gui_app.h"
#include "root_image\ui_resource.h"
#include "gui_grid.h"
#include "gui_canvas_round_rect.h"
#include "gui_canvas_rect.h"
#include "gui_text.h"
#include "gui_api.h"
#define APP_TEST
#define SCREEN_W ((int)gui_get_screen_width())
#define SCREEN_H ((int)gui_get_screen_height())
/**
 * @link https://www.rapidtables.com/web/color/RGB_Color.html
*/
#define COLOR_RED gui_rgb(255, 0, 0)
#define COLOR_CRIMSON gui_rgb(220,20,60)
#define COLOR_FIREBRICK gui_rgb(178,34,34)
#define COLOR_WHITE gui_rgb(255,255,255)
#define COLOR_WHITE_OPACITY gui_rgba(255,255,255,150)
#define COLOR_SILVER gui_rgb(192,192,192)
#define COLOR_SILVER_OPACITY(opacity) gui_rgba(192,192,192, opacity)
GUI_APP_DEFINE(APP_TEST, test)  // cppcheck-suppress syntaxError
static void test(gui_app_t *app)
{
    gui_canvas_rect_create(GUI_APP_ROOT_SCREEN, "canvas_rect", 0, 0, SCREEN_W, SCREEN_H, COLOR_SILVER);
    {
        char *text = "565:";
        int font_size = 16;
        gui_text_t *t = gui_text_create(GUI_APP_ROOT_SCREEN, "txt", 0, 0, gui_get_screen_width(),
                                        font_size);
        {
            gui_text_set(t, text, GUI_FONT_SRC_BMP, gui_rgb(0, 255, 255), strlen(text), font_size);
        }
        void *addr1 = ARIALBD_SIZE16_BITS4_FONT_BIN;
        gui_font_mem_init(addr1);
        gui_text_type_set(t, addr1, FONT_SRC_MEMADDR);
    }
    {
        gui_img_t *img = gui_img_create_from_mem(GUI_APP_ROOT_SCREEN, 0, IMAGE565_BIN, 0, 16, 0, 0);
        gui_img_set_mode(img, IMG_BYPASS_MODE);
        char *text = "BYPASS";
        int font_size = 16;
        gui_text_t *t = gui_text_create(img, "txt", 0, 80, gui_get_screen_width(), font_size);
        {
            gui_text_set(t, text, GUI_FONT_SRC_BMP, gui_rgb(0, 255, 0), strlen(text), font_size);
        }
        void *addr1 = ARIALBD_SIZE16_BITS4_FONT_BIN;
        gui_font_mem_init(addr1);
        gui_text_type_set(t, addr1, FONT_SRC_MEMADDR);
    }
    {
        gui_img_t *img = gui_img_create_from_mem(GUI_APP_ROOT_SCREEN, 0, COMPRESS_565_BIN, 0, 16 + 100, 0,
                                                 0);
        gui_img_set_mode(img, IMG_BYPASS_MODE);
        char *text = "BYPASS ZIP";
        int font_size = 16;
        gui_text_t *t = gui_text_create(img, "txt", 0, 80, gui_get_screen_width(), font_size);
        {
            gui_text_set(t, text, GUI_FONT_SRC_BMP, gui_rgb(0, 255, 0), strlen(text), font_size);
        }
        void *addr1 = ARIALBD_SIZE16_BITS4_FONT_BIN;
        gui_font_mem_init(addr1);
        gui_text_type_set(t, addr1, FONT_SRC_MEMADDR);
    }
    {
        gui_img_t *img = gui_img_create_from_mem(GUI_APP_ROOT_SCREEN, 0, MIX565_BIN, 0, 16 + 100 * 2, 0, 0);
        gui_img_set_mode(img, IMG_FILTER_BLACK);
        char *text = "FILTER MIX";
        int font_size = 16;
        gui_text_t *t = gui_text_create(img, "txt", 0, 80, gui_get_screen_width(), font_size);
        {
            gui_text_set(t, text, GUI_FONT_SRC_BMP, gui_rgb(0, 255, 0), strlen(text), font_size);
        }
        void *addr1 = ARIALBD_SIZE16_BITS4_FONT_BIN;
        gui_font_mem_init(addr1);
        gui_text_type_set(t, addr1, FONT_SRC_MEMADDR);
    }
    {
        gui_img_t *img = gui_img_create_from_mem(GUI_APP_ROOT_SCREEN, 0, COMPRESS_MIX_565_BIN, 0,
                                                 16 + 100 * 3, 0, 0);
        gui_img_set_mode(img, IMG_FILTER_BLACK);
        char *text = "F MIX ZIP";
        int font_size = 16;
        gui_text_t *t = gui_text_create(img, "txt", 0, 80, gui_get_screen_width(), font_size);
        {
            gui_text_set(t, text, GUI_FONT_SRC_BMP, gui_rgb(0, 255, 0), strlen(text), font_size);
        }
        void *addr1 = ARIALBD_SIZE16_BITS4_FONT_BIN;
        gui_font_mem_init(addr1);
        gui_text_type_set(t, addr1, FONT_SRC_MEMADDR);
    }
    {
        gui_img_t *img = gui_img_create_from_mem(GUI_APP_ROOT_SCREEN, 0, IMAGE888_BIN, 100, 16 + 100 * 0, 0,
                                                 0);
        gui_img_set_mode(img, IMG_BYPASS_MODE);
        char *text = "888";
        int font_size = 16;
        gui_text_t *t = gui_text_create(img, "txt", 0, 80, gui_get_screen_width(), font_size);
        {
            gui_text_set(t, text, GUI_FONT_SRC_BMP, gui_rgb(0, 255, 0), strlen(text), font_size);
        }
        void *addr1 = ARIALBD_SIZE16_BITS4_FONT_BIN;
        gui_font_mem_init(addr1);
        gui_text_type_set(t, addr1, FONT_SRC_MEMADDR);
    }
    {
        gui_img_t *img = gui_img_create_from_mem(GUI_APP_ROOT_SCREEN, 0, COMPRESS888_BIN, 100, 16 + 100 * 1,
                                                 0, 0);
        gui_img_set_mode(img, IMG_BYPASS_MODE);
        char *text = "888 ZIP";
        int font_size = 16;
        gui_text_t *t = gui_text_create(img, "txt", 0, 80, gui_get_screen_width(), font_size);
        {
            gui_text_set(t, text, GUI_FONT_SRC_BMP, gui_rgb(0, 255, 0), strlen(text), font_size);
        }
        void *addr1 = ARIALBD_SIZE16_BITS4_FONT_BIN;
        gui_font_mem_init(addr1);
        gui_text_type_set(t, addr1, FONT_SRC_MEMADDR);
    }
    {
        gui_img_t *img = gui_img_create_from_mem(GUI_APP_ROOT_SCREEN, 0, IMAGE8565_BIN, 100, 16 + 100 * 2,
                                                 0, 0);
        gui_img_set_mode(img, IMG_SRC_OVER_MODE);
        char *text = "8565";
        int font_size = 16;
        gui_text_t *t = gui_text_create(img, "txt", 0, 80, gui_get_screen_width(), font_size);
        {
            gui_text_set(t, text, GUI_FONT_SRC_BMP, gui_rgb(0, 255, 0), strlen(text), font_size);
        }
        void *addr1 = ARIALBD_SIZE16_BITS4_FONT_BIN;
        gui_font_mem_init(addr1);
        gui_text_type_set(t, addr1, FONT_SRC_MEMADDR);
    }
    {
        gui_img_t *img = gui_img_create_from_mem(GUI_APP_ROOT_SCREEN, 0, COMPRESS8565_BIN, 100,
                                                 16 + 100 * 3, 0, 0);
        gui_img_set_mode(img, IMG_SRC_OVER_MODE);
        char *text = "8565 ZIP";
        int font_size = 16;
        gui_text_t *t = gui_text_create(img, "txt", 0, 80, gui_get_screen_width(), font_size);
        {
            gui_text_set(t, text, GUI_FONT_SRC_BMP, gui_rgb(0, 255, 0), strlen(text), font_size);
        }
        void *addr1 = ARIALBD_SIZE16_BITS4_FONT_BIN;
        gui_font_mem_init(addr1);
        gui_text_type_set(t, addr1, FONT_SRC_MEMADDR);
    }
    {
        char *text = "RECT; RECT ROUND";
        int font_size = 16;
        gui_text_t *t = gui_text_create(GUI_APP_ROOT_SCREEN, "txt", 200 + 10, 0, gui_get_screen_width(),
                                        font_size);
        {
            gui_text_set(t, text, GUI_FONT_SRC_BMP, gui_rgb(0, 255, 0), strlen(text), font_size);
        }
        void *addr1 = ARIALBD_SIZE16_BITS4_FONT_BIN;
        gui_font_mem_init(addr1);
        gui_text_type_set(t, addr1, FONT_SRC_MEMADDR);
        gui_canvas_rect_t *rect = gui_canvas_rect_create(GUI_APP_ROOT_SCREEN,
                                                         "canvas_rect", 200 + 10, 16, 200, 120, COLOR_FIREBRICK);
        gui_canvas_round_rect_create(rect, "r_r", 100, 10, 6, 100, 3, gui_rgba(163, 163, 163,
                                                                               255));

        gui_canvas_round_rect_create(rect, "r_r", 50, 10, 30, 100, 3, gui_rgba(163, 163, 163,
                                                                               255));
    }
}

#if defined _WIN32
uint8_t resource_root[1024 * 1024 * 20];
#endif
static int watch_app_init(void)
{
#if defined _WIN32
    int fd;
    fd = open("./gui_engine/example/test/root_image/root(0x4400000).bin", 0);
    if (fd > 0)
    {
        printf("open root(0x4400000).bin Successful!\n");
        read(fd, resource_root, 1024 * 1024 * 20);
    }
    else
    {
        printf("open root(0x4400000).bin Fail!\n");
        printf("open root(0x4400000).bin Fail!\n");
        printf("open root(0x4400000).bin Fail!\n");
        return 0;
    }
#endif
    extern int gui_server_init(void);
    gui_server_init();
    GUI_APP_STARTUP(APP_TEST);
    return 0;
}
#include "gui_components_init.h"
GUI_INIT_APP_EXPORT(watch_app_init);

