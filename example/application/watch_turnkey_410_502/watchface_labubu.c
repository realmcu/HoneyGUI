/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <time.h>
#include "gui_win.h"
#include "gui_api.h"
#include "root_image/ui_resource.h"
#include "guidef.h"
#include "gui_img.h"
#include "gui_video.h"
#include "gui_text.h"
#include "app_main_watch.h"

/*============================================================================*
 *                            Macros
 *============================================================================*/


/*============================================================================*
 *                           Function Declaration
 *============================================================================*/


/*============================================================================*
 *                            Variables
 *============================================================================*/
static char time_content[6] = {0};
static char date_content[10] = {0};

/*============================================================================*
 *                           Private Functions
 *============================================================================*/
static void time_update_cb(void *p)
{
    gui_obj_t *obj = (gui_obj_t *)p;
    if (!timeinfo)
    {
        return;
    }

    sprintf(time_content, "%02d:%02d", timeinfo->tm_hour, timeinfo->tm_min);
    sprintf(date_content, "%d %s", timeinfo->tm_mday, day[timeinfo->tm_wday]);
    GUI_WIDGET_POINTER_BY_NAME_ROOT(t_time, "t_time", obj);
    GUI_WIDGET_POINTER_BY_NAME_ROOT(t_date, "t_date", obj);
    gui_text_content_set((gui_text_t *)t_time, time_content, strlen(time_content));
    gui_text_content_set((gui_text_t *)t_date, date_content, strlen(date_content));
}

void create_watchface_labubu(gui_obj_t *parent)
{
    gui_win_t *win = gui_win_create(parent, "win", 0, 0, 0, 0);
    gui_video_t *video = gui_video_create_from_mem(win, "labubu", LABUBU_MJPG, 0, 0, 410,
                                                   502);
    gui_video_set_state(video, GUI_VIDEO_STATE_PLAYING);
    gui_video_set_repeat_count(video, 0);

    sprintf(time_content, "%02d:%02d", timeinfo->tm_hour, timeinfo->tm_min);
    sprintf(date_content, "%d %s", timeinfo->tm_mday, day[timeinfo->tm_wday]);
    gui_text_t *t_time = gui_text_create(win, "t_time", 128, 84, 159, 84);
    gui_text_set(t_time, time_content, GUI_FONT_SRC_TTF, gui_rgb(0, 0, 0), strlen(time_content), 70);
    gui_text_type_set(t_time, SF_COMPACT_TEXT_MEDIUM_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(t_time, MID_CENTER);
    gui_text_rendermode_set(t_time, 2);

    gui_text_t *t_date = gui_text_create(win, "t_date", 155, 51, 105, 38);
    gui_text_set(t_date, date_content, GUI_FONT_SRC_TTF, gui_rgb(0, 0, 0), strlen(date_content), 32);
    gui_text_type_set(t_date, SF_COMPACT_TEXT_MEDIUM_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(t_date, MID_CENTER);
    gui_text_rendermode_set(t_date, 2);

    gui_obj_create_timer(GUI_BASE(win), 30000, true, time_update_cb);
}