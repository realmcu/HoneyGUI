/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "guidef.h"
#include "gui_img.h"
#include "gui_obj.h"
#include "app_main_watch.h"
#include "gui_components_init.h"
#include "gui_view.h"
#include "gui_win.h"
#include "gui_text.h"
#include "gui_canvas.h"
#include "root_image/ui_resource.h"
#include "watch_adapt.h"

/*============================================================================*
 *                            Macros
 *============================================================================*/
#define CURRENT_VIEW_NAME "call_dial_view"

#define SCREEN_WIDTH  (int16_t)gui_get_screen_width()
#define SCREEN_HEIGHT (int16_t)gui_get_screen_height()
#define DIAL_BUFFER_LENGTH 64
#define DIAL_LENGTH 16

#define TAB_START_X 15
#define TAB_START_Y 140
#define TAB_W 120
#define TAB_H 60
#define TAB_SPACE 10

/*============================================================================*
*                             Types
*============================================================================*/


/*============================================================================*
 *                           Function Declaration
 *============================================================================*/
static void app_call_dail_design(gui_view_t *view);
static void clear_mem(gui_view_t *view);

/*============================================================================*
 *                            Variables
 *============================================================================*/
/* View Management */
static gui_view_t *current_view = NULL;
const static gui_view_descriptor_t *recorder_view = NULL;
const static gui_view_descriptor_t *heartrate_view = NULL;
static const gui_view_descriptor_t descriptor =
{
    /* change Here for current view */
    .name = (const char *)CURRENT_VIEW_NAME,
    .pView = &current_view,

    .on_switch_in = app_call_dail_design,
    .on_switch_out = clear_mem,

    .keep = false,
};

static gui_win_t *win_call_dail = NULL;

static char dial_number[DIAL_BUFFER_LENGTH] = {0};
static char *dial_number_display = NULL;
static uint8_t index = 0;

static uint8_t *img_data = NULL;
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
    heartrate_view = gui_view_descriptor_get("heartrate_view");
    recorder_view = gui_view_descriptor_get("recorder_view");
    gui_log("File: %s, Function: %s\n", __FILE__, __func__);
    return 0;
}
static GUI_INIT_VIEW_DESCRIPTOR_GET(gui_view_get_other_view_descriptor_init);

static void clear_mem(gui_view_t *view)
{
    (void)view;
    if (img_data)
    {
        gui_lower_free(img_data);
        img_data = NULL;
    }
}

static void draw_tab_bg(NVGcontext *vg)
{
    nvgRoundedRect(vg, 0, 0, TAB_W, TAB_H, 10);
    nvgFillColor(vg, nvgRGB(60, 150, 255));
    nvgFill(vg);
}

static void click_call_end_cb(void *obj, gui_event_t e, void *param)
{
    (void)e;
    (void)param;
    gui_call_t *call = gui_get_call();
    if (call && call->call_end)
    {
        call->call_end();
    }
    gui_obj_tree_free_async(GUI_BASE(obj)->parent);
    gui_obj_hidden(GUI_BASE(win_call_dail), false);
}

static void call_wait_cb(void *p)
{
    gui_img_t *img = (gui_img_t *)p;
    static float scale = 1.0f;
    static float positive = 1.0f;
    if (scale >= 1.2f)
    {
        positive = -1.0f;
    }
    else if (scale <= 0.8f)
    {
        positive = 1.0f;
    }
    scale += 0.01f * positive;
    gui_img_scale(img, scale, scale);

    // if call taken
    gui_call_t *call = gui_get_call();
    if (call && call->call_get_dail_status)
    {
        if (call->call_get_dail_status())
        {
            extern void gui_update_call_number(char *number);
            gui_update_call_number(dial_number_display);

            extern void app_calling_design(gui_obj_t *parent);
            app_calling_design(GUI_BASE(current_view));
            gui_obj_tree_free_async(GUI_BASE(img)->parent);
        }
    }
}

static void win_scroll_cb(void *obj, gui_event_t e, void *param)
{
    (void)obj;
    (void)e;
    (void)param;
    // Disable view scroll
}

static void app_call_wait_design(void)
{
    gui_win_t *win_call_wait = gui_win_create(current_view, "win_call_wait", 0, 0, 0, 0);
    gui_obj_add_event_cb(GUI_BASE(win_call_wait), win_scroll_cb, GUI_EVENT_TOUCH_SCROLL_HORIZONTAL,
                         NULL); // Disable view horizontal scroll
    gui_obj_add_event_cb(GUI_BASE(win_call_wait), win_scroll_cb, GUI_EVENT_TOUCH_SCROLL_VERTICAL,
                         NULL); // Disable view vertical scroll

    gui_text_t *text = gui_text_create(win_call_wait, 0, 0, 50, SCREEN_WIDTH, 60);
    gui_text_set(text, "Call", GUI_FONT_SRC_TTF, APP_COLOR_WHITE, strlen("Call"), 48);
    gui_text_type_set(text, SOURCEHANSANSSC_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, CENTER);
    gui_text_rendermode_set(text, 2);

    text = gui_text_create(win_call_wait, 0, 0, 110, SCREEN_WIDTH, 50);
    gui_text_set(text, dial_number_display, GUI_FONT_SRC_TTF, APP_COLOR_WHITE,
                 strlen(dial_number_display), 48);
    gui_text_type_set(text, SOURCEHANSANSSC_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, CENTER);
    gui_text_rendermode_set(text, 2);

    gui_img_t *img_person = gui_img_create_from_mem(win_call_wait, 0, CALL_PERSON_ICON_BIN, 155 + 50,
                                                    230 + 50, 0, 0);
    gui_img_set_focus(img_person, 50, 50);
    gui_img_set_quality(img_person, true);
    gui_obj_create_timer(GUI_BASE(img_person), 20, true, call_wait_cb);

    gui_img_t *img_call_reject = gui_img_create_from_mem(win_call_wait, 0, CALL_REJECT_ICON_BIN, 175,
                                                         380, 0, 0);
    gui_img_set_quality(img_call_reject, true);
    gui_obj_add_event_cb(img_call_reject, click_call_end_cb, GUI_EVENT_TOUCH_CLICKED, NULL);
}

static void click_call_dail_cb(void *obj, gui_event_t e, void *param)
{
    // TODO: Add functionality to turn on CALL
    (void)obj;
    (void)e;
    (void)param;
    if (strlen(dial_number_display) == 0) { return; }
    gui_call_t *call = gui_get_call();
    if (call && call->call_dail)
    {
        call->call_dail(dial_number_display);
    }
    gui_obj_hidden(GUI_BASE(win_call_dail), true);
    app_call_wait_design();
}

static void click_number_tab_cb(void *o, gui_event_t e, void *param)
{
    (void)e;
    (void)param;
    gui_obj_t *obj = GUI_BASE(o);

    if (index < DIAL_BUFFER_LENGTH - 1)
    {
        dial_number[index++] = obj->name[0]; // obj->name is the number character
        dial_number[index] = '\0';
    }
    if (index > DIAL_LENGTH && index < DIAL_BUFFER_LENGTH - 1)
    {
        dial_number_display++;
    }
    GUI_WIDGET_POINTER_BY_NAME_ROOT(text, "dial_number", current_view)
    gui_text_content_set((gui_text_t *)text, dial_number_display, strlen(dial_number_display));
}

static void click_back_cb(void *o, gui_event_t e, void *param)
{
    (void)o;
    (void)e;
    (void)param;
    if (index > 0)
    {
        dial_number[--index] = '\0';
    }
    else
    {
        return; // No number to delete
    }
    if (index > DIAL_LENGTH && index < DIAL_BUFFER_LENGTH - 1)
    {
        dial_number_display--;
    }
    GUI_WIDGET_POINTER_BY_NAME_ROOT(text, "dial_number", current_view)
    gui_text_content_set((gui_text_t *)text, dial_number_display, strlen(dial_number_display));
}

static void number_tab_design(char *number, int x, int y)
{
    gui_img_t *rect = gui_img_create_from_mem(GUI_BASE(win_call_dail), number, img_data, x, y, 0, 0);
    gui_text_t *text = gui_text_create(rect, 0, 0, 0, 120, 60);
    gui_text_set(text, number, GUI_FONT_SRC_TTF, APP_COLOR_WHITE, strlen(number), 48);
    gui_text_type_set(text, SOURCEHANSANSSC_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, MID_CENTER);
    gui_text_rendermode_set(text, 2);
    gui_obj_add_event_cb(rect, click_number_tab_cb, GUI_EVENT_TOUCH_CLICKED, NULL);
}

static void app_call_dail_design(gui_view_t *view)
{
    // view layout
    gui_view_switch_on_event(view, recorder_view->name, SWITCH_OUT_TO_RIGHT_USE_ROTATE,
                             SWITCH_IN_FROM_LEFT_USE_ROTATE,
                             GUI_EVENT_TOUCH_MOVE_RIGHT);
    gui_view_switch_on_event(view, heartrate_view->name, SWITCH_OUT_TO_LEFT_USE_ROTATE,
                             SWITCH_IN_FROM_RIGHT_USE_ROTATE,
                             GUI_EVENT_TOUCH_MOVE_LEFT);

    // call
    win_call_dail = gui_win_create(view, "win_call_dail", 0, 0, 0, 0);

    gui_text_t *text = gui_text_create(win_call_dail, 0, 0, 20, SCREEN_WIDTH, 60);
    gui_text_set(text, "Call", GUI_FONT_SRC_TTF, APP_COLOR_WHITE, strlen("Call"), 48);
    gui_text_type_set(text, SOURCEHANSANSSC_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, CENTER);
    gui_text_rendermode_set(text, 2);

    index = 0;
    dial_number[index] = '\0';
    dial_number_display = dial_number;
    text = gui_text_create(win_call_dail, "dial_number", 0, 80, SCREEN_WIDTH, 50);
    gui_text_set(text, dial_number_display, GUI_FONT_SRC_TTF, APP_COLOR_WHITE,
                 strlen(dial_number_display), 48);
    gui_text_type_set(text, SOURCEHANSANSSC_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, CENTER);
    gui_text_rendermode_set(text, 2);

    gui_img_t *img_back = gui_img_create_from_mem(win_call_dail, 0, UI_RETURN_W_ICON_BIN, 360,
                                                  80, 0, 0);
    gui_img_set_quality(img_back, true);
    gui_obj_add_event_cb(img_back, click_back_cb, GUI_EVENT_TOUCH_CLICKED, NULL);

    // Draw number tab bg
    int image_h = TAB_H;
    int image_w = TAB_W;
    int pixel_bytes = 3;
    size_t buffer_size = image_h * image_w * pixel_bytes + sizeof(gui_rgb_data_head_t);
    if (!img_data)
    {
        img_data = gui_lower_malloc(buffer_size);
    }
    memset(img_data, 0, buffer_size);
    gui_canvas_render_to_image_buffer(GUI_CANVAS_OUTPUT_RGB565, 0, image_w, image_h, draw_tab_bg,
                                      img_data);

    number_tab_design("1", TAB_START_X, TAB_START_Y);
    number_tab_design("2", TAB_START_X + (TAB_W + TAB_SPACE) * 1, TAB_START_Y);
    number_tab_design("3", TAB_START_X + (TAB_W + TAB_SPACE) * 2, TAB_START_Y);
    number_tab_design("4", TAB_START_X, TAB_START_Y + (TAB_H + TAB_SPACE) * 1);
    number_tab_design("5", TAB_START_X + (TAB_W + TAB_SPACE) * 1,
                      TAB_START_Y + (TAB_H + TAB_SPACE) * 1);
    number_tab_design("6", TAB_START_X + (TAB_W + TAB_SPACE) * 2,
                      TAB_START_Y + (TAB_H + TAB_SPACE) * 1);
    number_tab_design("7", TAB_START_X, TAB_START_Y + (TAB_H + TAB_SPACE) * 2);
    number_tab_design("8", TAB_START_X + (TAB_W + TAB_SPACE) * 1,
                      TAB_START_Y + (TAB_H + TAB_SPACE) * 2);
    number_tab_design("9", TAB_START_X + (TAB_W + TAB_SPACE) * 2,
                      TAB_START_Y + (TAB_H + TAB_SPACE) * 2);
    number_tab_design("*", TAB_START_X, TAB_START_Y + (TAB_H + TAB_SPACE) * 3);
    number_tab_design("0", TAB_START_X + (TAB_W + TAB_SPACE) * 1,
                      TAB_START_Y + (TAB_H + TAB_SPACE) * 3);
    number_tab_design("#", TAB_START_X + (TAB_W + TAB_SPACE) * 2,
                      TAB_START_Y + (TAB_H + TAB_SPACE) * 3);

    gui_img_t *img_call = gui_img_create_from_mem(win_call_dail, 0, CALL_ICON_BIN, 175,
                                                  420, 0, 0);
    gui_img_set_quality(img_call, true);
    gui_obj_add_event_cb(img_call, click_call_dail_cb, GUI_EVENT_TOUCH_CLICKED, NULL);
}
