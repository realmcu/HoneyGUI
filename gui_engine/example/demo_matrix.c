#include <gui_fb.h>
//#include "from_mem_image_addr.h"

#include <gui_tabview.h>
#include <gui_img.h>
#include <gui_win.h>
#include <gui_obj.h>
#include <gui_text.h>
#include <time.h>
#include "string.h"
#include "rtk_gui_resource.h"

static gui_obj_t test_screen =
{
    .name = "app_launcher",
    .x    = 0,
    .y    = 0,
//    .mutex = &app_launcher_screen_mutex,
};

int demo_matrix(gui_obj_t *parent)
{
    gui_list_init(&test_screen.child_list);
    gui_list_init(&test_screen.brother_list);
    gui_win_t *win = gui_win_create(&test_screen, "clock_win", 0, 0, 0, 0);
    //gui_img_t *image = gui_img_create_from_fs(win, "test_image", "/rgb/rgb888/Wch6_Bg.bin", 200, 200, 0, 0);

    //gui_img_create_from_fs(win, "test_image", "/rgb/rgb565/Wch6_Bg.bin", 0, 0, 0, 0);


    //gui_img_t *image = gui_img_create_from_mem(win, "test_image", (void *)WCH6_BG_BIN, 0, 0, 0, 0);
//    gui_img_t *image = gui_img_create_from_mem(win, "test_image", (void *)BIRD_BIN, 100, 100, 0, 0);


    //image->rotation(image, 45, 0, 0);
    //gui_text_t *text_icon_0 = gui_text_create(win, "text_icon_0", 110, 20, 240, 40);
    //char *string = "123456";
    //text_icon_0->set(text_icon_0, string, "rtk_font_fs32", 0xffff, strlen(string));
    //gui_exec(&test_screen);
    gui_fb_disp(&test_screen);
    return 0;
}

//MSH_CMD_EXPORT(demo_matrix, lcd display test);

