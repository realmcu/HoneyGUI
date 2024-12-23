#include "gui_app.h"
#include "gui_win.h"
#include "gui_page.h"
#include "gui_api.h"
#include "root_image_hongkong/ui_resource.h"
#include "guidef.h"
#include "gui_img.h"
#include <time.h>

#define SCREEN_WIDTH 410
#define SCREEN_HEIGHT 502

static gui_img_t *h_hand, *m_hand, *s_hand;


static void win_cb()
{

    int millisecond = 0;
#ifdef __WIN32
    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    uint16_t seconds = timeinfo->tm_sec;
    uint16_t minute = timeinfo->tm_min;
    uint16_t hour = timeinfo->tm_hour;
    struct timespec spec;
    clock_gettime(CLOCK_REALTIME, &spec);
    millisecond = spec.tv_nsec / 1000000;
#else
    // extern struct tm watch_clock_get(void);
    // extern uint16_t millisecond_cnt;
    // struct tm watch_time = watch_clock_get();
    // millisecond = millisecond_cnt;
    // uint16_t seconds = watch_time.tm_sec;
    // uint16_t minute = watch_time.tm_min;
    // uint16_t hour = watch_time.tm_hour;

    uint16_t seconds = 0;
    uint16_t minute = 0;
    uint16_t hour = 0;
#endif
    const float angle_per_second = 360.0f / 60.0f;
    gui_img_t *hand;
    {
        hand = h_hand;
        float angle_hour = (hour % 12) * M_PI / 6 + minute * M_PI / 360;
        gui_img_translate(hand, hand->base.w / 2, SCREEN_HEIGHT / 2 - hand->base.y);
        gui_img_rotation(hand, angle_hour * 180 / M_PI,  hand->base.w / 2,
                         SCREEN_HEIGHT / 2 - hand->base.y);
    }
    {
        hand = m_hand;
        float angle_min  = minute * M_PI / 30 + seconds * M_PI / 1800;
        gui_img_translate(hand, hand->base.w / 2, SCREEN_HEIGHT / 2 - hand->base.y);
        gui_img_rotation(hand, angle_min * 180 / M_PI, hand->base.w / 2,
                         SCREEN_HEIGHT / 2 - hand->base.y);
    }
    {
        hand = s_hand;
        float angle_sec = (seconds + millisecond / 1000.0f) * angle_per_second;
        gui_img_translate(hand, hand->base.w / 2, SCREEN_HEIGHT / 2 - hand->base.y);
        gui_img_rotation(hand, angle_sec,  hand->base.w / 2,
                         SCREEN_HEIGHT / 2 - hand->base.y);
        // gui_log("seconds: %d, millisecond: %d \n", seconds, millisecond);
    }
}

void create_watchface_ring(void *parent)
{
    gui_obj_t *obj = GUI_BASE(parent);
    gui_win_t *win = gui_win_create(obj, "win_wf_ring", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    extern void app_box2d_ring_ui_design(gui_obj_t *obj);
    app_box2d_ring_ui_design((gui_obj_t *)win);

    gui_img_create_from_mem(win, "mask", WATCH_MASK_BIN, 200, 242, 0, 0);
    h_hand = gui_img_create_from_mem(win, "h_hand", WATCH_HOUR_HAND_BIN, 200, 163, 0, 0);
    m_hand = gui_img_create_from_mem(win, "m_hand", WATCH_MINUTE_HAND_BIN, 200, 87, 0, 0);
    s_hand = gui_img_create_from_mem(win, "s_hand", WATCH_SECOND_HAND_BIN, 206, 123, 0, 0);

    gui_win_set_animate(win, 2000, -1, (gui_animate_callback_t)win_cb, NULL);
}