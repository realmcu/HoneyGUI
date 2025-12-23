/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "root_image/ui_resource.h"
#include "guidef.h"
#include "gui_img.h"
#include "gui_obj.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "app_main_watch.h"
#include "gui_view_instance.h"
#include "gui_list.h"
#include "gui_text.h"
#include "gui_arc.h"
#include "gui_rect.h"

/*============================================================================*
 *                            Macros
 *============================================================================*/
#define CURRENT_VIEW_NAME "compass_info_view"
#define SCREEN_WIDTH (int16_t)gui_get_screen_width()
#define SCREEN_HEIGHT (int16_t)gui_get_screen_height()
#define ICON_DEFAULT_ANGLE 315  // Icon's default direction is NW 315°

/*============================================================================*
 *                           External Variables
 *============================================================================*/
extern int16_t current_angle;  // From app_compass.c
extern const char *direction_names[];
extern const char *get_direction_name(int16_t angle);
/*============================================================================*
 *                           Function Declaration
 *============================================================================*/
static void compass_info_design(gui_view_t *view);
static void compass_info_clean(gui_view_t *view);

extern int16_t g_selected_workout_index;

/*============================================================================*
 *                           GUI_VIEW_INSTANCE
 *============================================================================*/
GUI_VIEW_INSTANCE(CURRENT_VIEW_NAME, false, compass_info_design, NULL);

/*============================================================================*
 *                            Variables
 *============================================================================*/
static gui_view_t *current_view = NULL;
static gui_img_t *compass_info_img = NULL;
static gui_text_t *direction_text = NULL;
static gui_text_t *altitude_label_text = NULL;
static gui_text_t *altitude_value_text = NULL;
static gui_text_t *accuracy_text = NULL;

static char direction_str[32];
static char angle_str[32];
static char altitude_label_str[32] = "Altitude";
static char altitude_value_str[32];
static char accuracy_str[32];
static int16_t altitude = 11;  // Simulated altitude in meters
static int16_t accuracy = 10;  // Simulated accuracy in meters




/*============================================================================*
 *                           Private Functions
 *============================================================================*/
// Clean up compass_info_info resources
static void compass_info_clean(gui_view_t *view)
{
    GUI_UNUSED(view);
}

static void time_update_cb(void *p)
{
    GUI_UNUSED(p);
    if (!timeinfo)
    {
        return;
    }
    else
    {
        {
            GUI_WIDGET_POINTER_BY_NAME_ROOT(t_time, "time_b", current_view);
            gui_text_content_set((gui_text_t *)t_time, time_str, strlen(time_str));
        }

    }
}

static void info_img_press_cb(void *p)
{
    GUI_UNUSED(p);
    gui_view_switch_direct(gui_view_get_current(), "compass_view", SWITCH_OUT_ANIMATION_FADE,
                           SWITCH_IN_ANIMATION_FADE);
}
static void compass_info_design(gui_view_t *view)
{
    current_view = view;
    const char *obj_name = gui_view_get_current()->descriptor->name;
    VIEW_SWITCH_STYLE swtich_in = SWITCH_IN_ANIMATION_FADE;
    VIEW_SWITCH_STYLE swtich_out = SWITCH_OUT_ANIMATION_FADE;
    {
        gui_view_switch_on_event(current_view, "watchface_view",
                                 swtich_out,
                                 swtich_in,
                                 GUI_EVENT_KB_SHORT_CLICKED);
    }
    gui_obj_t *parent = GUI_BASE(view);

    // Background
    gui_rect_create(parent, "compass_info_bg", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, gui_rgb(0x64, 0x64,
                    0x64));

    // Close button (top left)
    gui_img_t *info_delete_img = gui_img_create_from_mem(parent, "info_delete_img", DELETE_ICON_BIN, 30,
                                                         23, 0, 0);
    gui_obj_add_event_cb(GUI_BASE(info_delete_img), (gui_event_cb_t)info_img_press_cb,
                         GUI_EVENT_TOUCH_CLICKED, NULL);

    // Left side - Direction info
    const char *dir = get_direction_name(current_angle);
    sprintf(direction_str, "%s", dir);
    direction_text = gui_text_create(parent, "direction_text", 40, 200, 150, 80);
    gui_text_set(direction_text, direction_str, GUI_FONT_SRC_TTF, gui_rgb(0xFF, 0x66, 0x00),
                 strlen(direction_str), 60);
    gui_text_type_set(direction_text, SF_COMPACT_TEXT_MEDIUM_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(direction_text, LEFT);
    gui_text_rendermode_set(direction_text, 2);

    // Angle text (e.g., "148")
    gui_text_t *angle_text = gui_text_create(parent, "angle_text", 40, 260, 150, 60);
    sprintf(angle_str, "~%d°", current_angle);
    gui_text_set(angle_text, angle_str, GUI_FONT_SRC_TTF, gui_rgb(0xFF, 0xFF, 0xFF),
                 strlen(angle_str), 48);
    gui_text_type_set(angle_text, SF_COMPACT_TEXT_MEDIUM_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(angle_text, LEFT);
    gui_text_rendermode_set(angle_text, 2);

    // Altitude label (e.g., "altitude")
    altitude_label_text = gui_text_create(parent, "altitude_label", 40, 340, 150, 40);
    gui_text_set(altitude_label_text, altitude_label_str, GUI_FONT_SRC_TTF, gui_rgb(0xFF, 0x66, 0x00),
                 strlen(altitude_label_str), 28);
    gui_text_type_set(altitude_label_text, SF_COMPACT_TEXT_MEDIUM_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(altitude_label_text, LEFT);
    gui_text_rendermode_set(altitude_label_text, 2);

    // Altitude value (e.g., "11 M")
    sprintf(altitude_value_str, "%d M", altitude);
    altitude_value_text = gui_text_create(parent, "altitude_value", 40, 380, 150, 60);
    gui_text_set(altitude_value_text, altitude_value_str, GUI_FONT_SRC_TTF, gui_rgb(0xFF, 0xFF, 0xFF),
                 strlen(altitude_value_str), 48);
    gui_text_type_set(altitude_value_text, SF_COMPACT_TEXT_MEDIUM_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(altitude_value_text, LEFT);
    gui_text_rendermode_set(altitude_value_text, 2);

    // Accuracy text (e.g., "+/- 10 meter")
    sprintf(accuracy_str, "+/- %d M", accuracy);
    accuracy_text = gui_text_create(parent, "accuracy_text", 40, 440, 150, 40);
    gui_text_set(accuracy_text, accuracy_str, GUI_FONT_SRC_TTF, gui_rgb(0xCC, 0xCC, 0xCC),
                 strlen(accuracy_str), 24);
    gui_text_type_set(accuracy_text, SF_COMPACT_TEXT_MEDIUM_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(accuracy_text, LEFT);
    gui_text_rendermode_set(accuracy_text, 2);

    // Right side - Compass icon
    compass_info_img = gui_img_create_from_mem(parent, "compass_img", COMPASS_ICON_BIN, 260, 220, 0, 0);

    // Set rotation center and rotate to current angle
    uint16_t img_width = gui_img_get_width(compass_info_img);
    uint16_t img_height = gui_img_get_height(compass_info_img);
    gui_img_set_focus(compass_info_img, img_width / 2.0f, img_height / 2.0f);
    gui_img_scale(compass_info_img, 0.7f, 0.7f);

    // Rotate icon to match current angle
    int16_t rotation_angle = ICON_DEFAULT_ANGLE - current_angle;
    gui_img_rotation(compass_info_img, rotation_angle);

    // Time display (top right)
    gui_text_t *t_time = gui_text_create(parent, "time_b", 0, 0, 380, 80);
    gui_text_set(t_time, time_str, GUI_FONT_SRC_TTF, gui_rgb(255, 255, 255),
                 strlen(time_str), 34);
    gui_text_type_set(t_time, SF_COMPACT_TEXT_MEDIUM_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(t_time, MID_RIGHT);
    gui_text_rendermode_set(t_time, 2);

    gui_obj_create_timer(GUI_BASE(view), 30000, true, time_update_cb);
}
