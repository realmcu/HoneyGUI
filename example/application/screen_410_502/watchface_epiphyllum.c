#include "gui_win.h"
#include "gui_api.h"
#include "root_image_hongkong/ui_resource.h"
#include "guidef.h"
#include "gui_img.h"
#include "gui_video.h"
#include "gui_text.h"
#include <time.h>
#include "app_hongkong.h"
#include "tp_algo.h"

#define SCREEN_WIDTH  (int16_t)gui_get_width_height()
#define SCREEN_HEIGHT (int16_t)gui_get_screen_height()

#define CURRENT_VIEW_NAME "flower_clock_view"

static gui_view_t *current_view = NULL;
const static gui_view_descriptor_t *menu_view = NULL;

void create_watchface_flower(gui_view_t *view);
static const gui_view_descriptor_t descriptor =
{
    /* change Here for current view */
    .name = (const char *)CURRENT_VIEW_NAME,
    .pView = &current_view,
    .on_switch_in = create_watchface_flower,
    .keep = false,
};

static int gui_view_descriptor_register_init(void)
{
    gui_view_descriptor_register(&descriptor);
    gui_log("File: %s, Function: %s\n", __FILE__, __func__);
    return 0;
}
static GUI_INIT_VIEW_DESCRIPTOR_REGISTER(gui_view_descriptor_register_init);

static int gui_view_get_other_view_descriptor_init(void)
{
    menu_view = gui_view_descriptor_get("menu_view");
    gui_log("File: %s, Function: %s\n", __FILE__, __func__);
    return 0;
}
static GUI_INIT_VIEW_DESCRIPTOR_GET(gui_view_get_other_view_descriptor_init);

#define X_ORINGIN 50
#define Y_ORINGIN 180

static gui_video_t *video = NULL;
extern void *text_num_array[11];
extern char *day[7];

static char date_text_content[10];

// static void return_to_menu()
// {
//     gui_view_switch_direct(current_view, menu_view, SWITCH_OUT_ANIMATION_FADE,
//                            SWITCH_IN_ANIMATION_FADE);
// }

// static void return_timer_cb()
// {
//     touch_info_t *tp = tp_get_info();
//     GUI_RETURN_HELPER(tp, gui_get_dc()->screen_width, return_to_menu)
// }

unsigned char *flower[] = {FLOWER_MJPG, PEONY_RED_MJPG, PEONY_BLUE_MJPG};
static uint8_t flower_index = 0;

static void time_update_cb(void *p)
{
#ifdef __WIN32
    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
#else
    extern struct tm *timeinfo;
#endif

    GUI_WIDGET_POINTER_BY_NAME_ROOT(img_hour_decimal, "watch_hour_decimal", current_view);
    gui_img_set_image_data((gui_img_t *)img_hour_decimal, text_num_array[timeinfo->tm_hour / 10]);

    GUI_WIDGET_POINTER_BY_NAME_ROOT(img_hour_single, "watch_hour_single", current_view);
    gui_img_set_image_data((gui_img_t *)img_hour_single, text_num_array[timeinfo->tm_hour % 10]);

    GUI_WIDGET_POINTER_BY_NAME_ROOT(img_minute_decimal, "watch_minute_decimal", current_view);
    gui_img_set_image_data((gui_img_t *)img_minute_decimal, text_num_array[timeinfo->tm_min / 10]);

    GUI_WIDGET_POINTER_BY_NAME_ROOT(img_minute_single, "watch_minute_single", current_view);
    gui_img_set_image_data((gui_img_t *)img_minute_single, text_num_array[timeinfo->tm_min % 10]);
}


static void flower_change_cb(void *obj, gui_event_t e, void *param)
{
    {
        gui_obj_t *p = GUI_BASE(obj)->parent;
        if (obj)
        {
            gui_obj_tree_free(GUI_BASE(obj));
        }
        flower_index = (flower_index + 1) % 3;
        video = gui_video_create_from_mem(p, "flower",
                                          (unsigned char *)flower[flower_index],
                                          X_ORINGIN, Y_ORINGIN, 410, 502);
        gui_video_set_state(video, GUI_VIDEO_STATE_PLAYING);
        gui_video_set_repeat_count(video, 0);
        // gui_img_translate(video->img, X_ORINGIN, Y_ORINGIN);
        // gui_img_scale(video->img, 1.2f, 1.2f);
        gui_video_set_frame_rate(video, 60.f);
    }
    gui_obj_add_event_cb(video, flower_change_cb, GUI_EVENT_TOUCH_CLICKED, 0);
}

void create_watchface_flower(gui_view_t *view)
{
    gui_view_switch_on_event(view, menu_view, SWITCH_OUT_ANIMATION_FADE,
                             SWITCH_IN_ANIMATION_FADE,
                             GUI_EVENT_KB_SHORT_CLICKED);

    gui_obj_hidden(&(gui_view_get_current()->base), true);
    gui_win_t *win = gui_win_create(view, "win", 0, 0, 0, 0);

    video = gui_video_create_from_mem(win, "flower", (unsigned char *)flower[flower_index],
                                      X_ORINGIN, Y_ORINGIN, 410, 502);
    gui_video_set_state(video, GUI_VIDEO_STATE_PLAYING);
    gui_video_set_repeat_count(video, 0);
    // gui_img_translate(video->img, X_ORINGIN, Y_ORINGIN);
    // gui_img_scale(video->img, 1.5f, 1.5f);
    gui_video_set_frame_rate(video, 60.f);

    sprintf(date_text_content, "FRI 16");
    gui_text_t *text = gui_text_create(win, "date_text", -40, 33, 0, 0);
    gui_text_set(text, (void *)date_text_content, GUI_FONT_SRC_TTF, APP_COLOR_WHITE,
                 strlen(date_text_content), 48);
    gui_text_type_set(text, SOURCEHANSANSSC_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, RIGHT);
    gui_text_rendermode_set(text, 2);

    {
        int text_w = 35;
        gui_img_t *img = gui_img_create_from_mem(win, "watch_hour_decimal", text_num_array[0],
                                                 211, 88, 0, 0);
        gui_img_set_mode(img, IMG_SRC_OVER_MODE);
        img = gui_img_create_from_mem(win, "watch_hour_single", text_num_array[0],
                                      211 + text_w, 88, 0, 0);
        gui_img_set_mode(img, IMG_SRC_OVER_MODE);
        img = gui_img_create_from_mem(win, "colon", text_num_array[10],
                                      211 + text_w * 2 + 5, 88 + 5, 0, 0);
        gui_img_set_mode(img, IMG_SRC_OVER_MODE);
        img = gui_img_create_from_mem(win, "watch_minute_decimal", text_num_array[0],
                                      211 + text_w * 2 + 17, 88, 0, 0);
        gui_img_set_mode(img, IMG_SRC_OVER_MODE);
        img = gui_img_create_from_mem(win, "watch_minute_single", text_num_array[0],
                                      211 + text_w * 3 + 17, 88, 0, 0);
        gui_img_set_mode(img, IMG_SRC_OVER_MODE);
    }

    time_update_cb(NULL);

    // gui_obj_create_timer(, 17, true, flower_change_cb);
    gui_obj_add_event_cb(video, flower_change_cb, GUI_EVENT_TOUCH_CLICKED, 0);

    // gui_obj_create_timer(GUI_BASE(view), 17, true, return_timer_cb);
}