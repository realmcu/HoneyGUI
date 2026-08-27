#include "guidef.h"
#include "gui_img.h"
#include "gui_win.h"
#include "gui_img.h"
#include "gui_obj.h"
#include "string.h"
#include "stdio.h"
#include "gui_server.h"
#include "gui_components_init.h"
#include "gui_video.h"
#include "gui_obj_event.h"

#ifdef _HONEYGUI_SIMULATOR_
#include <stdlib.h>
#include "gui_api_os.h"
#include "gui_vfs.h"

static unsigned char *resource_root;

static unsigned char *load_resource_file(const char *path)
{
    gui_vfs_stat_t stat;
    if (gui_vfs_stat(path, &stat) != 0 ||
        stat.type != GUI_VFS_TYPE_FILE ||
        stat.size == 0)
    {
        gui_log("Failed to get simulator resource: %s\n", path);
        return NULL;
    }

    gui_vfs_file_t *file = gui_vfs_open(path, GUI_VFS_READ);
    if (file == NULL)
    {
        gui_log("Failed to open simulator resource: %s\n", path);
        return NULL;
    }

    unsigned char *data = (unsigned char *)malloc(stat.size);
    if (data == NULL)
    {
        gui_log("Failed to allocate simulator resource: %s\n", path);
        gui_vfs_close(file);
        return NULL;
    }

    size_t total = 0;
    while (total < stat.size)
    {
        int read_size = gui_vfs_read(file, data + total, stat.size - total);
        if (read_size <= 0)
        {
            gui_log("Failed to read simulator resource: %s\n", path);
            free(data);
            gui_vfs_close(file);
            return NULL;
        }
        total += (size_t)read_size;
    }

    gui_vfs_close(file);
    return data;
}
#else
#include "flash_map.h"
#define EARTH_420_410_502_40_LQ_MJPG     (USER_DATA1_ADDR)
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
    resource_root = load_resource_file(
                        "/pc/example/widget/video/root_image/earth_420_410_502_40_lq.mjpg");
    if (resource_root == NULL)
    {
        return -1;
    }
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
