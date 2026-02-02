#include "guidef.h"
#include "gui_img.h"
#include "gui_win.h"
#include "gui_img.h"
#include "gui_obj.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "gui_server.h"
#include "gui_components_init.h"
#include "gui_video.h"
#include "gui_obj_event.h"



#ifdef _HONEYGUI_SIMULATOR_
extern const unsigned char _binary_earth_420_410_502_40_lq_mjpg_start[];
extern const unsigned char _binary_earth_420_410_502_40_lq_mjpg_end[];
extern const unsigned char _binary_earth_420_410_502_40_lq_mjpg_size[];
#else
#define EARTH_420_410_502_40_LQ_MJPG     0x7004D100
#endif

/* gui video widget example start*/
static void video_click_cb(void *obj)
{
    gui_video_t *this = (gui_video_t *)obj;
    GUI_VIDEO_STATE state = gui_video_get_state(this);
    if (state == GUI_VIDEO_STATE_PLAYING)
    {
        gui_video_set_state(this, GUI_VIDEO_STATE_PAUSE);
    }
    else
    {
        gui_video_set_state(this, GUI_VIDEO_STATE_PLAYING);
    }

}


static void video_pressing_cb(void *obj)
{
    gui_video_t *this = (gui_video_t *)obj;
    static uint32_t cnt = 0;
    uint32_t frame_time = gui_video_get_frame_time(this);

    if (frame_time > 30)
    {
        cnt = 0;
        gui_video_set_frame_rate(this, 50.f);
    }
    else if (frame_time > 15)
    {
        cnt++;
        if (cnt >= 80)
        {
            cnt = 80;
            gui_video_set_frame_step(this, 2);
        }
    }

}

static void video_release_cb(void *obj)
{
    gui_video_t *this = (gui_video_t *)obj;

    gui_video_set_frame_rate(this, 30.f);
    gui_video_set_frame_step(this, 1);
}

static int app_init(void)
{
#ifdef _HONEYGUI_SIMULATOR_
    unsigned char *resource_root = (unsigned char *)_binary_earth_420_410_502_40_lq_mjpg_start;
#else
    unsigned char *resource_root = (unsigned char *)EARTH_420_410_502_40_LQ_MJPG;
#endif
    gui_log("GUI Video Widget Example Start\n");
    gui_video_t *video = gui_video_create_from_mem(gui_obj_get_root(), "earth",
                                                   (void *)resource_root,
                                                   0, 0, 410,
                                                   502);
    /* Set default frame rate to 30 FPS */
    gui_video_set_frame_rate(video, 30.f);
    gui_video_set_state(video, GUI_VIDEO_STATE_PLAYING);      // Set initial state to Playing
    gui_video_set_repeat_count(video, GUI_VIDEO_REPEAT_INFINITE); // Enable infinite loop

    /*
     * Register CLICK event callback.
     * Logic: Toggle between Play and Pause states.
     */
    gui_obj_add_event_cb(video,
                         (gui_event_cb_t)video_click_cb,
                         GUI_EVENT_TOUCH_CLICKED,
                         NULL);

    /*
     * Register PRESSING (Long Press) event callback.
     * Logic: Handle 2-stage Fast Forward:
     *   1. Initial long press: Increase frame rate (FPS) for smooth fast forward.
     *   2. Extended long press: Increase seek step for faster fast forward.
     */
    gui_obj_add_event_cb(video,
                         (gui_event_cb_t)video_pressing_cb,
                         GUI_EVENT_TOUCH_PRESSING,
                         NULL);

    /*
     * Register RELEASE event callback.
     * Logic: Restore initial playback settings (Default FPS and Step) when finger is lifted.
     */
    gui_obj_add_event_cb(video,
                         (gui_event_cb_t)video_release_cb,
                         GUI_EVENT_TOUCH_RELEASED,
                         NULL);

    return 0;
}
GUI_INIT_APP_EXPORT(app_init);
/* gui video widget example end*/

