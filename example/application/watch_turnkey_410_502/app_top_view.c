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
/*============================================================================*
 *                           Types
 *============================================================================*/

/*============================================================================*
 *                            Macros
 *============================================================================*/
#define SCREEN_WIDTH (int16_t)gui_get_screen_width()
#define SCREEN_HEIGHT (int16_t)gui_get_screen_height()
#define NOTE_HEIGHT 220
#define NOTE_INTERVAL 20
#define NOTE_START 140
#define INFOR_NUM_MAX 4
#define CURRENT_VIEW_NAME "app_top_view"

/*============================================================================*
 *                           Function Declaration
 *============================================================================*/
static void top_view_design(gui_view_t *view);

/*============================================================================*
 *                            Variables
 *============================================================================*/
static gui_view_t *current_view = NULL;

static gui_list_t *list;
static gui_canvas_round_rect_t *canvas_clear;
static bool is_cleared  = false;
// static gui_text_t *no_notification_text = NULL;


static uint8_t *img_data_activity = NULL;
/*============================================================================*
 *                           Private Functions
 *============================================================================*/
GUI_VIEW_INSTANCE(CURRENT_VIEW_NAME, false, top_view_design, NULL);

static void clear_all_note_cb(gui_event_t *e)
{
    (void)e;
    is_cleared = true;

    // if (no_notification_text == NULL) {
    //     no_notification_text = gui_text_create(GUI_BASE(list), "no_notification",
    //                                           0, 0, 0, 0);
    // }
    // gui_text_set(no_notification_text, "No notification", GUI_FONT_SRC_TTF,
    //              gui_rgb(200, 200, 200), strlen("No notification"), 36);
    // gui_text_type_set(no_notification_text, SF_COMPACT_TEXT_BOLD_BIN, FONT_SRC_MEMADDR);
    // gui_text_mode_set(no_notification_text, MID_CENTER);

}
static void create_clear_note(void *parent)
{
    gui_text_t *clear_text;
    canvas_clear = gui_canvas_round_rect_create(GUI_BASE(parent), "canvas_clear",
                                                52, 40, 305, 80, 40, gui_rgb(39, 43, 44));
    gui_obj_add_event_cb(GUI_BASE(canvas_clear), (gui_event_cb_t)clear_all_note_cb,
                         GUI_EVENT_TOUCH_CLICKED, NULL);
    // text
    char *clear_content = "clear all";
    clear_text = gui_text_create(canvas_clear, "clear all", 0, 0, 0, 0);
    gui_text_set(clear_text, (void *)clear_content, GUI_FONT_SRC_TTF, APP_COLOR_WHITE,
                 strlen(clear_content),
                 32);
    gui_text_type_set(clear_text, SF_COMPACT_TEXT_MEDIUM_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(clear_text, MID_CENTER);
}
static void arc_activity_cb(NVGcontext *vg)
{
    nvgBeginPath(vg);
    nvgArc(vg, 100 / 2, 100 / 2, 50 - 8, 3 * M_PI_F / 2, M_PI_F * 3.5f, NVG_CW);
    nvgStrokeWidth(vg, 8);
    nvgStrokeColor(vg, nvgRGB(230, 67, 79));
    nvgStroke(vg);

    nvgBeginPath(vg);
    nvgArc(vg, 100 / 2, 100 / 2, 50 - 18, 3 * M_PI_F / 2, M_PI_F * 3.5f, NVG_CW);
    nvgStrokeWidth(vg, 8);
    nvgStrokeColor(vg, nvgRGB(186, 253, 79));
    nvgStroke(vg);

    nvgBeginPath(vg);
    nvgArc(vg, 100 / 2, 100 / 2, 50 - 28, 3 * M_PI_F / 2, M_PI_F * 3.5f, NVG_CW);
    nvgStrokeWidth(vg, 8);
    nvgStrokeColor(vg, nvgRGB(117, 230, 229));
    nvgStroke(vg);
}
static void note_design(gui_obj_t *obj, void *p)
{
    (void)p;
    uint16_t index = ((gui_list_note_t *)obj)->index;
    int16_t offset_X = 29;

    if (is_cleared && index > 0)
    {
        return;
    }
    if (index == 0)
    {
        create_clear_note(obj);

    }
    else if (index == 1)
    {
        // note_activity
        {
            gui_img_t *canvas = (gui_img_t *)gui_canvas_round_rect_create(obj, "note_ac", offset_X, 0, 352, 120,
                                                                          30,
                                                                          gui_rgba(98, 101, 98, 255 * 0.7));
            // gui_img_set_mode(canvas, IMG_SRC_OVER_MODE);


            gui_text_t *sport_text0 = gui_text_create(canvas, "ac_text0", 230, 10, 0, 0);
            gui_text_set(sport_text0, "1 hour ago", GUI_FONT_SRC_TTF, gui_rgb(200, 200, 200),
                         strlen("1 hour ago"), 24);
            gui_text_type_set(sport_text0, SF_COMPACT_TEXT_BOLD_BIN, FONT_SRC_MEMADDR);
            gui_text_mode_set(sport_text0, LEFT);

            gui_text_t *sport_text = gui_text_create(canvas, "ac_text1", 30, 50, 0, 0);
            gui_text_set(sport_text, "come on", GUI_FONT_SRC_TTF, gui_rgb(255, 255, 255),
                         strlen("come on"), 48);
            gui_text_type_set(sport_text, SF_COMPACT_TEXT_BOLD_BIN, FONT_SRC_MEMADDR);
            gui_text_mode_set(sport_text, LEFT);
            // activity icon
            {
                int image_h = 100;
                int image_w = 100;
                int pixel_bytes = 4;
                size_t buffer_size = image_h * image_w * pixel_bytes + sizeof(gui_rgb_data_head_t);
                if (!img_data_activity)
                {
                    img_data_activity = gui_lower_malloc(buffer_size);
                }
                memset(img_data_activity, 0, buffer_size);
                gui_img_t *img = gui_img_create_from_mem(canvas, "activity", img_data_activity, 20, -50, 0, 0);
                img->base.w = image_w;
                img->base.h = image_h;
                gui_img_set_mode(img, IMG_SRC_OVER_MODE);
                gui_canvas_render_to_image_buffer(GUI_CANVAS_OUTPUT_RGBA, 0, 100, 100, arc_activity_cb,
                                                  img_data_activity);
            }
        }
    }
    else if (index == 2)
    {
        // note_music
        gui_img_t *canvas = (gui_img_t *)gui_canvas_round_rect_create(obj, "note_ac", offset_X, 0, 352, 120,
                                                                      30, gui_rgba(98, 101, 98, 255 * 0.7));
        // gui_img_set_mode(canvas, IMG_SRC_OVER_MODE);
        gui_img_t *img = gui_img_create_from_mem(canvas, "music", APP_MUSIC_ICON_BIN, 20,
                                                 -50, 0, 0);
        gui_img_scale(img, 0.85, 0.85);
        gui_text_t *sport_text0 = gui_text_create(canvas, "music_text0", 184, 10, 0, 0);
        gui_text_set(sport_text0, "45 mimute ago", GUI_FONT_SRC_TTF, gui_rgb(200, 200, 200),
                     strlen("45 mimute ago"), 24);
        gui_text_type_set(sport_text0, SF_COMPACT_TEXT_BOLD_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(sport_text0, LEFT);

        gui_text_t *sport_text = gui_text_create(canvas, "music_text1", 30, 50, 0, 0);
        gui_text_set(sport_text, "music", GUI_FONT_SRC_TTF, gui_rgb(255, 255, 255),
                     strlen("music"), 48);
        gui_text_type_set(sport_text, SF_COMPACT_TEXT_BOLD_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(sport_text, LEFT);
    }
}
static void top_view_design(gui_view_t *view)
{
    current_view = view;
    gui_view_switch_on_event(view, gui_view_descriptor_get("watchface_view"),
                             SWITCH_OUT_TO_TOP_USE_TRANSLATION,
                             SWITCH_IN_STILL_USE_BLUR,
                             GUI_EVENT_TOUCH_MOVE_UP);
    gui_view_set_opacity(view, 128);
    gui_obj_t *parent = GUI_BASE(view);
    // draw background
    gui_canvas_rect_create(GUI_BASE(parent), "background", 0, 0,
                           SCREEN_WIDTH, SCREEN_HEIGHT, gui_rgb(76, 76, 76));

    // create_clear_note(view);

    int space = 20;
    int length = 157;
    list = gui_list_create(parent, "list", 0, 0, 0, 0, length, space, VERTICAL, note_design, NULL,
                           false);
    gui_list_set_style(list, LIST_CLASSIC);
    gui_list_set_note_num(list, 3);
}


