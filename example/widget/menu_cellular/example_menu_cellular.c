/* gui menu cellular widget example start*/
#include "guidef.h"
#include "gui_img.h"
#include "gui_win.h"
#include "gui_obj.h"
#include "gui_canvas.h"
#include "gui_menu_cellular.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "gui_server.h"
#include "gui_components_init.h"

#define ICON_NUM 56
#define ICON_SIZE gui_get_screen_width() / 4

static size_t buffer_size = 0;
static NVGcolor color[4];
static uint8_t color_index = 0;

static void init_colors(void)
{
    color[0] = nvgRGBAf(1.0f, 1.0f, 1.0f, 1.0f); //white
    color[1] = nvgRGBAf(1.0f, 0, 0, 1.0f);       //red
    color[2] = nvgRGBAf(0, 1.0f, 0, 1.0f);       //green
    color[3] = nvgRGBAf(0, 0, 1.0f, 1.0f);       //blue
}

static void draw_circle_cb(NVGcontext *vg)
{
    nvgBeginPath(vg);
    nvgCircle(vg, ICON_SIZE / 2.0f, ICON_SIZE / 2.0f, ICON_SIZE / 2.0f);
    nvgFillColor(vg, color[color_index++]);
    nvgFill(vg);
    color_index %= 4;
}

static int app_init(void)
{
    init_colors();
    uint32_t *img_data_array[ICON_NUM] = {0};
    for (int i = 0; i < ICON_NUM; i++)
    {
        int image_h = ICON_SIZE;
        int image_w = ICON_SIZE;
        int pixel_bytes = 4;
        buffer_size = image_h * image_w * pixel_bytes + sizeof(gui_rgb_data_head_t);
        img_data_array[i] = gui_lower_malloc(buffer_size);
        memset(img_data_array[i], 0, buffer_size);
        gui_canvas_render_to_image_buffer(GUI_CANVAS_OUTPUT_RGBA, 0, image_w, image_h, draw_circle_cb,
                                          (uint8_t *)img_data_array[i]);

    }
    int array_size = sizeof(img_data_array) / sizeof(img_data_array[0]);
    gui_menu_cellular_t *menu = gui_menu_cellular_create(gui_obj_get_root(), ICON_SIZE, img_data_array,
                                                         array_size);
    GUI_UNUSED(menu);

    return 0;
}

GUI_INIT_APP_EXPORT(app_init);
/* gui menu cellular widget example end*/


