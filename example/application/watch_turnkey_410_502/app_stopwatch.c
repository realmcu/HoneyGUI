/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <time.h>
#include "gui_win.h"
#include "gui_api.h"
#include "root_image/ui_resource.h"
#include "guidef.h"
#include "gui_img.h"
#include "gui_text.h"
#include "app_main_watch.h"
#include "gui_view_instance.h"

/*============================================================================*
 *                            Macros
 *============================================================================*/
#define SCREEN_WIDTH  (int16_t)gui_get_width_height()
#define SCREEN_HEIGHT (int16_t)gui_get_screen_height()
#define CURRENT_VIEW_NAME "stopwatch_view"
#define X_TARGET  37
#define Y_TARGET  42
#define X_ORIGIN -300
#define Y_ORIGIN 50
#define SCALE_ORINGIN 2.f
#define COUNT_MAX 30

/*============================================================================*
 *                           Function Declaration
 *============================================================================*/


/*============================================================================*
 *                            Variables
 *============================================================================*/


/*============================================================================*
 *                           Private Functions
 *============================================================================*/
void stopwatch_design(gui_view_t *view)
{
    gui_win_t *win_main = gui_win_create(view, "win_main", 0, 0, 0, 0);

    // time text init
    gui_text_t *t_time = gui_text_create(win_main, "t_time", -40, 60, 0, 0);
    gui_text_set(t_time, time_str, GUI_FONT_SRC_TTF, gui_rgb(255, 255, 255),
                 strlen(time_str), 80);
    gui_text_type_set(t_time, SF_COMPACT_TEXT_MEDIUM_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(t_time, RIGHT);
    gui_text_rendermode_set(t_time, 2);
}

/*============================================================================*
 *                           GUI_VIEW_INSTANCE
 *============================================================================*/
GUI_VIEW_INSTANCE(CURRENT_VIEW_NAME, false, stopwatch_design, NULL);