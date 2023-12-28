#include "root_image_hongkong/ui_resource.h"
#include <gui_img.h>
#include "gui_win.h"

static gui_img_t *img_demo;
static uint8_t cnt_animate = 0;
static void img_loading_animate_callback(gui_img_t *img)
{
    // static uint8_t cnt = 0;
    uint16_t c_x = gui_img_get_width(img_demo) / 2;
    uint16_t c_y = gui_img_get_height(img_demo) / 2;

    gui_log("animate %d\n", cnt_animate++);
    gui_thread_mdelay(100);
    gui_img_scale(img_demo, 0.5f, 0.5f);
    gui_img_translate(img_demo, c_x, c_y);
    gui_img_rotation(img_demo, cnt_animate * 36, c_x, c_y);
}
void callback_img_demo(void *obj, gui_event_t e)
{
    gui_log("click\n");
    static uint8_t cnt = 0;

    cnt++;
    switch (cnt)
    {
    case 1:
        {
            gui_log("wid: %d, high: %d\n", gui_img_get_width(img_demo), gui_img_get_height(img_demo));
            break;
        }

    case 2:
        {
            gui_log("set_location\n");
            gui_img_set_location(img_demo, 100, 100);
            break;
        }

    case 3:
        {
            gui_log("set_location\n");
            gui_img_set_location(img_demo, 0, 0);
            break;
        }
    case 4:
        {
            gui_log("rotation\n");
            gui_img_rotation(img_demo, 45, 0, 0);
            break;
        }
    case 5:
        {
            gui_log("rotation\n");
            gui_img_rotation(img_demo, 0, 0, 0);
            break;
        }
    case 6:
        {
            gui_log("scale\n");
            gui_img_scale(img_demo, 0.5f, 0.5f);
            break;
        }
    case 7:
        {
            gui_log("scale\n");
            gui_img_scale(img_demo, 1.0f, 1.0f);
            break;
        }
    case 8:
        {
            gui_log("filepath\n");
            gui_img_set_attribute(img_demo, "HeartRate.bin", NULL, 100, 100);
            break;
        }
    case 9:
        {
            gui_log("filepath\n");
            // gui_img_set_attribute(img_demo, "Activity.bin", NULL, 0, 0);
            gui_img_set_attribute(img_demo, NULL, ACTIVITY_BIN, 0, 0);
            break;
        }
    case 10:
        {
            gui_log("translate\n");
            gui_img_translate(img_demo, 100, 100);
            break;
        }
    case 11:
        {
            gui_log("translate\n");
            gui_img_translate(img_demo, 0, 0);
            break;
        }
    case 12:
        {
            gui_log("opacity\n");
            gui_img_set_opacity(img_demo, 128);
            break;
        }
    case 13:
        {
            gui_log("opacity\n");
            gui_img_set_opacity(img_demo, 255);
            break;
        }
    case 14:
        {
            gui_log("animate\n");
            gui_img_set_animate(img_demo, 10, 10, img_loading_animate_callback, img_demo);
            break;
        }
    case 15:
        {
            gui_log("animate end\n");
            cnt_animate = 0;
            gui_img_translate(img_demo, 0, 0);
            gui_img_rotation(img_demo, 0, 0, 0);
            gui_img_scale(img_demo, 1.f, 1.f);
            break;
        }

    default:
        gui_log("reset\n");
        cnt = 0;

        break;
    }
}

void page_tb_img_fs_demo(void *parent)
{
    gui_img_t *img_act = gui_img_create_from_fs(parent, "Activity.bin", 0, 0);
    gui_win_t *win_function = gui_win_create(parent, "win_function", 0, 0, 368, 448);

    img_demo = img_act;
    gui_obj_add_event_cb(win_function, (gui_event_cb_t)callback_img_demo, GUI_EVENT_TOUCH_CLICKED,
                         NULL);

}