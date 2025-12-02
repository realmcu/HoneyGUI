#include "gui_view_macro.h"
#include "gui_video.h"
#include "gui_obj_event.h"





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

static void video_support_switch_in(gui_view_t *view)
{
    GUI_UNUSED(view);
    gui_log("video_support_view switch in\n");


    gui_video_t *video = gui_video_create_from_mem(view, "video",
                                                   (void *)FILE_POINTER(EARTH_420_410_502_40_LQ_MJPG), 0, 0,
                                                   410,
                                                   502);
    gui_video_set_frame_rate(video, 30.f);
    gui_video_set_state(video, GUI_VIDEO_STATE_PLAYING);
    gui_video_set_repeat_count(video, GUI_VIDEO_REPEAT_INFINITE);

    gui_obj_add_event_cb(video,
                         (gui_event_cb_t)video_click_cb,
                         GUI_EVENT_TOUCH_CLICKED,
                         NULL);
    gui_obj_add_event_cb(video,
                         (gui_event_cb_t)video_pressing_cb,
                         GUI_EVENT_TOUCH_PRESSING,
                         NULL);
    gui_obj_add_event_cb(video,
                         (gui_event_cb_t)video_release_cb,
                         GUI_EVENT_TOUCH_RELEASED,
                         NULL);

    gui_view_switch_on_event(view, "gif_apng_support_view",
                             SWITCH_OUT_TO_LEFT_USE_TRANSLATION,
                             SWITCH_IN_FROM_RIGHT_USE_TRANSLATION,
                             GUI_EVENT_TOUCH_MOVE_LEFT);

    gui_view_switch_on_event(view, "anti_aliasing_view",
                             SWITCH_OUT_TO_RIGHT_USE_TRANSLATION,
                             SWITCH_IN_FROM_LEFT_USE_TRANSLATION,
                             GUI_EVENT_TOUCH_MOVE_RIGHT);
}

static void video_support_switch_out(gui_view_t *view)
{
    GUI_UNUSED(view);
    gui_log("video_support_view switch out\n");
}

GUI_VIEW_INSTANCE("video_support_view", 1, video_support_switch_in, video_support_switch_out);

