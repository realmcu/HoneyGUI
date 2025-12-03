/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "root_image/ui_resource.h"
#include "app_main_watch.h"
#include "gui_img.h"
#include "gui_win.h"
#include "gui_text.h"
#include "tp_algo.h"
#include "gui_canvas.h"
#include "gui_canvas_rect.h"
#include "gui_canvas_round_rect.h"
#include "guidef.h"
#include "gui_list.h"
#include "app_main_watch.h"
#include "gui_view_instance.h"
#include "gui_lite_geometry_round_rect.h"
/*============================================================================*
 *                           Types
 *============================================================================*/

/*============================================================================*
 *                            Macros
 *============================================================================*/
#define SCREEN_WIDTH (int16_t)gui_get_screen_width()
#define SCREEN_HEIGHT (int16_t)gui_get_screen_height()
#define CURRENT_VIEW_NAME "battery_view"

/*============================================================================*
 *                           Function Declaration
 *============================================================================*/
static void battery_design(gui_view_t *view);

/*============================================================================*
 *                            Variables
 *============================================================================*/
// static gui_img_t *img_battery = NULL;
static uint8_t *img_data_battery = NULL;
// static gui_text_t *img_battery_text = NULL;
extern uint8_t battery_level;
/*============================================================================*
 *                           Private Functions
 *============================================================================*/
GUI_VIEW_INSTANCE(CURRENT_VIEW_NAME, false, battery_design, NULL);
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
            GUI_WIDGET_POINTER_BY_NAME_ROOT(t_time, "time_b", gui_view_get_current());
            gui_text_content_set((gui_text_t *)t_time, time_str, strlen(time_str));
        }

    }
}
static void draw_battery_arc(NVGcontext *vg)
{
    float cx = SCREEN_WIDTH / 2.0f;
    float cy = SCREEN_HEIGHT / 2.0f;
    float outer_radius = 100.0f;
    // float inner_radius = 32.4f;
    float stroke_width = 24.0f;
    battery_level = 75;

    nvgBeginPath(vg);
    nvgCircle(vg, cx, cy, outer_radius);
    nvgFillColor(vg, nvgRGBA(50, 50, 50, 255));
    nvgFill(vg);

    nvgBeginPath(vg);
    nvgCircle(vg, cx, cy, outer_radius);
    nvgStrokeColor(vg, nvgRGBA(0, 0, 0, 255));
    nvgStrokeWidth(vg, stroke_width);
    nvgStroke(vg);

    nvgBeginPath(vg);
    nvgArc(vg, cx, cy, outer_radius - stroke_width / 2, 0, 2 * M_PI_F, NVG_CW);

    nvgStrokeColor(vg, battery_level < 10 ? nvgRGBA(255, 0, 0, 80) : nvgRGBA(0, 200, 0, 80));
    nvgStrokeWidth(vg, stroke_width);
    nvgStroke(vg);

    float start_angle = -M_PI_F / 2;
    float sweep_angle = (battery_level / 100.0f) * 2 * M_PI_F;

    nvgBeginPath(vg);
    nvgArc(vg, cx, cy, outer_radius - stroke_width / 2, start_angle, start_angle + sweep_angle, NVG_CW);


    if (battery_level < 10.0f)
    {
        nvgStrokeColor(vg, nvgRGBA(200, 0, 0, 255));
    }
    else
    {
        nvgStrokeColor(vg, nvgRGBA(0, 200, 0, 255));
    }

    nvgStrokeWidth(vg, stroke_width);
    nvgStroke(vg);
}
// static void battery_press_cb(void *p)
// {
//     GUI_UNUSED(p);
//     gui_view_switch_direct(gui_view_get_current(), "_view",
//                            SWITCH_IN_ANIMATION_FADE, SWITCH_OUT_ANIMATION_FADE);
// }
static void battery_return_press_cb(void *p)
{
    GUI_UNUSED(p);
    gui_view_switch_direct(gui_view_get_current(), "bottom_view",
                           SWITCH_IN_ANIMATION_FADE, SWITCH_OUT_ANIMATION_FADE);
}
static void battery_design(gui_view_t *view)
{
    const char *obj_name = gui_view_get_current()->descriptor->name;
    if (strcmp(obj_name, "bottom_view") == 0)
    {
        gui_view_set_animate_step(gui_view_get_current(), 60);
        gui_view_switch_on_event(gui_view_get_current(), "bottom_view",
                                 SWITCH_OUT_ANIMATION_FADE,
                                 SWITCH_IN_ANIMATION_FADE,
                                 GUI_EVENT_KB_SHORT_CLICKED);
    }
    gui_obj_t *parent = GUI_BASE(view);

    // ----------time-----------------//
    gui_text_t *t_time = gui_text_create(parent, "time_b", -40, 20, 0, 0);
    gui_text_set(t_time, time_str, GUI_FONT_SRC_TTF, gui_rgb(255, 255, 255),
                 strlen(time_str), 42);
    gui_text_type_set(t_time, SF_COMPACT_TEXT_MEDIUM_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(t_time, RIGHT);
    gui_text_rendermode_set(t_time, 2);
    gui_obj_create_timer(GUI_BASE(view), 30000, true, time_update_cb);

    gui_text_t *sport_text = gui_text_create(parent, "ac_text1",  -40, 60, 0, 0);
    gui_text_set(sport_text, "Battery", GUI_FONT_SRC_TTF, gui_rgb(0, 200, 0),
                 strlen("Battery"), 42);
    if (battery_level < 10)
    {
        gui_text_set(sport_text, "Battery", GUI_FONT_SRC_TTF, gui_rgb(200, 0, 0),
                     strlen("Battery"), 42);
    }
    gui_text_type_set(sport_text, SF_COMPACT_TEXT_MEDIUM_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(sport_text, RIGHT);

    {
        int image_h = 400;
        int image_w = 350;
        int pixel_bytes = 4;
        size_t buffer_size = image_h * image_w * pixel_bytes + sizeof(gui_rgb_data_head_t);

        if (!img_data_battery)
        {
            img_data_battery = gui_lower_malloc(buffer_size);
        }
        memset(img_data_battery, 0, buffer_size);

        gui_canvas_render_to_image_buffer(GUI_CANVAS_OUTPUT_RGBA, 0, image_w, image_h, draw_battery_arc,
                                          img_data_battery);
        gui_img_t *img_battery = gui_img_create_from_mem(parent, "BATTERY", img_data_battery, 0,
                                                         0, 0, 0);
        gui_img_set_mode(img_battery, IMG_SRC_OVER_MODE);

        // char battery_content[2];

        // sprintf(battery_content, "%d", battery_level);

        gui_text_t *battery_remain_text = gui_text_create(parent, "battery_remain_text", 150, 210, 0, 0);
        gui_text_set(battery_remain_text, "75%", GUI_FONT_SRC_TTF, APP_COLOR_WHITE, strlen("75%"), 65);
        gui_text_type_set(battery_remain_text, SF_COMPACT_TEXT_BOLD_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(battery_remain_text, LEFT);
    }
    {
        gui_lite_round_rect_t *battery_rect = gui_lite_round_rect_create(parent, "battery_rect", 35, 400,
                                                                         340, 80, 40, gui_rgba(200, 200, 200, 255 * 0.7));
        gui_text_t *battery_text = gui_text_create(battery_rect, "battery_text", 35, 20, 0, 0);
        gui_text_set(battery_text, "Low Power Mode", GUI_FONT_SRC_TTF, APP_COLOR_WHITE,
                     strlen("Low Power Mode"), 36);
        gui_text_type_set(battery_text, SF_COMPACT_TEXT_BOLD_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(battery_text, LEFT);
    }

    gui_img_t *battery_return = gui_img_create_from_mem(parent, "battery_return", DELETE_ICON_BIN,
                                                        30, 30, 0, 0);
    gui_obj_add_event_cb(GUI_BASE(battery_return), (gui_event_cb_t)battery_return_press_cb,
                         GUI_EVENT_TOUCH_CLICKED, NULL);

}


