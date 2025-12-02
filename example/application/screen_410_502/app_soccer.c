/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "root_image/ui_resource.h"
#include "gui_img.h"
#include "gui_win.h"
#include "gui_text.h"
#include "time.h"
#include "tp_algo.h"
#include <math.h>
#include "cJSON.h"
#include "app_main_watch.h"
#include "gui_view.h"
#include "gui_soccer.h"


/*============================================================================*
 *                            Macros
 *============================================================================*/
#define CURRENT_VIEW_NAME "soccer_view"

/*============================================================================*
 *                           Function Declaration
 *============================================================================*/
static void soccer_app(gui_view_t *view);

/*============================================================================*
 *                            Variables
 *============================================================================*/
/* View Management */
static gui_view_t *current_view = NULL;
const static gui_view_descriptor_t *menu_view = NULL;
static gui_view_descriptor_t const descriptor =
{
    /* change Here for current view */
    .name = (const char *)CURRENT_VIEW_NAME,
    .pView = &current_view,
    .on_switch_in = soccer_app,
};

/* Soccer Management */
static uint32_t *gui_soccer_array[20];
// {
//     SOCCER_P0001_CALL_BIN,
//     SOCCER_P0002_SPORTS_BIN,
//     SOCCER_P0003_HEARTRATE_BIN,
//     SOCCER_P0004_SLEEP_BIN,
//     SOCCER_P0005_WEATHER_BIN,
//     SOCCER_P0006_ACTIVITIES_BIN,
//     SOCCER_P0007_STRESS_BIN,
//     SOCCER_P0008_SPO2_BIN,
//     SOCCER_P0009_MUSIC_BIN,
//     SOCCER_P0010_VOICE_BIN,
//     SOCCER_P0011_NOTIFICATION_BIN,
//     SOCCER_P0012_SETTINGS_BIN,
//     SOCCER_P0013_SPORT_CORECD_BIN,
//     SOCCER_P0014_MEASURE_BIN,
//     SOCCER_P0015_MOOD_BIN,
//     SOCCER_P0016_BREATHE_BIN,
//     SOCCER_P0017_ALARM_BIN,
//     SOCCER_P0018_PERIOD_BIN,
//     SOCCER_P0019_HOME_BIN,
//     SOCCER_P0020_MORE_BIN,
// };

/*============================================================================*
 *                           Private Functions
 *============================================================================*/
static int gui_view_descriptor_register_init(void)
{
    gui_view_descriptor_register(&descriptor);
    gui_log("File: %s, Function: %s\n", __FILE__, __func__);
    return 0;
}
static GUI_INIT_VIEW_DESCRIPTOR_REGISTER(gui_view_descriptor_register_init);

static int gui_view_get_other_view_descriptor_init(void)
{
    /* you can get other view descriptor point here */
    menu_view = gui_view_descriptor_get("menu_view");
    gui_log("File: %s, Function: %s\n", __FILE__, __func__);
    return 0;
}
static GUI_INIT_VIEW_DESCRIPTOR_GET(gui_view_get_other_view_descriptor_init);


static void soccer_app(gui_view_t *view)
{
    if (gui_soccer_array[0] == NULL)
    {
        uint32_t *soccer_array[20] =
        {
            SOCCER_P0001_CALL_BIN,
            SOCCER_P0002_SPORTS_BIN,
            SOCCER_P0003_HEARTRATE_BIN,
            SOCCER_P0004_SLEEP_BIN,
            SOCCER_P0005_WEATHER_BIN,
            SOCCER_P0006_ACTIVITIES_BIN,
            SOCCER_P0007_STRESS_BIN,
            SOCCER_P0008_SPO2_BIN,
            SOCCER_P0009_MUSIC_BIN,
            SOCCER_P0010_VOICE_BIN,
            SOCCER_P0011_NOTIFICATION_BIN,
            SOCCER_P0012_SETTINGS_BIN,
            SOCCER_P0013_SPORT_CORECD_BIN,
            SOCCER_P0014_MEASURE_BIN,
            SOCCER_P0015_MOOD_BIN,
            SOCCER_P0016_BREATHE_BIN,
            SOCCER_P0017_ALARM_BIN,
            SOCCER_P0018_PERIOD_BIN,
            SOCCER_P0019_HOME_BIN,
            SOCCER_P0020_MORE_BIN,
        };
        memcpy(gui_soccer_array, soccer_array, sizeof(soccer_array));
    }

    gui_obj_t *obj = GUI_BASE(view);
    gui_dispdev_t *dc = gui_get_dc();

    gui_soccer_t *soccer = gui_soccer_create(obj, "soccer", gui_soccer_array, 0, 0);
    gui_soccer_set_center(soccer, dc->screen_width / 2, dc->screen_height / 2);

    gui_view_switch_on_event(view, menu_view->name, SWITCH_OUT_ANIMATION_FADE,
                             SWITCH_IN_ANIMATION_FADE,
                             GUI_EVENT_KB_SHORT_CLICKED);

}