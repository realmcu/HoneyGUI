/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "gui_img.h"
#include "tp_algo.h"
#include "gui_view.h"
#include "gui_canvas.h"
#include "root_image/ui_resource.h"
#include "gui_view_macro.h"

/*============================================================================*
 *                            Macros
 *============================================================================*/
#define TAB_START_POS_X 160
#define TAB_START_POS_Y 102
#define TAB_STOP_POS_Y  310
#define TAB_SIZE        80


/*============================================================================*
 *                           Function Declaration
 *============================================================================*/
void lock_view_design(gui_view_t *view);

/*============================================================================*
 *                            Variables
 *============================================================================*/
static void *press_bg = NULL;
static void *pressing_bg = NULL;
static void *bar_bg = NULL;

/*============================================================================*
 *                           Private Functions
 *============================================================================*/
static void render_press(NVGcontext *ctx)
{
    nvgShapeAntiAlias(ctx, 1);

    // Main gradient fill: from transparent in the center to cyan at the edges
    nvgBeginPath(ctx);
    NVGpaint gradient = nvgRadialGradient(ctx, 45, 45, 2, 36,
                                          nvgRGBA(0, 0, 0, 0),
                                          nvgRGBA(0, 0xFF, 0xF6, 0xFF));
    nvgCircle(ctx, 45, 45, 36);
    nvgFillPaint(ctx, gradient);
    nvgFill(ctx);

    // This does not affect the center color and only adds a softening effect to the edges.
    nvgBeginPath(ctx);
    nvgCircle(ctx, 45, 45, 37);
    nvgStrokeWidth(ctx, 3.0f);

    NVGpaint strokeGradient = nvgRadialGradient(ctx, 45, 45, 35, 39,
                                                nvgRGBA(0, 0xFF, 0xF6, 200),
                                                nvgRGBA(0, 0xFF, 0xF6, 0));
    nvgStrokePaint(ctx, strokeGradient);
    nvgStroke(ctx);
}

static void render_pressing(NVGcontext *ctx)
{
    nvgBeginPath(ctx);

    nvgCircle(ctx, 45, 45, 40);

    nvgFillColor(ctx, nvgRGBA(0xFF, 0xFF, 0xFF, 0xFF));

    nvgFill(ctx);

    nvgBeginPath(ctx);
    nvgCircle(ctx, 45, 45, 40);

    nvgStrokeWidth(ctx, 5.0f);
    nvgStrokeColor(ctx, nvgRGBA(0xFF, 0xFF, 0xFF, 100));

    nvgStroke(ctx);
}

static void pressing_tab(void *obj)
{
    GUI_UNUSED(obj);

    touch_info_t *tp = tp_get_info();
    gui_img_t *img = (gui_img_t *)obj;
    if (tp->pressing && tp->x >= TAB_START_POS_X && tp->x <= (TAB_START_POS_X + TAB_SIZE)
        && tp->y >= TAB_START_POS_Y && tp->y <= (TAB_START_POS_Y + TAB_SIZE))
    {
        int16_t y = tp->deltaY;
        if (y < 0)
        {
            y = 0;
        }
        else if (y > TAB_STOP_POS_Y - TAB_START_POS_Y)
        {
            y = TAB_STOP_POS_Y - TAB_START_POS_Y;
        }
        gui_img_translate(img, 0, (float)y);
    }
    else if (img->t_y != 0)
    {
        int16_t y = (int16_t)img->t_y;
        const int16_t step = 20;
        if (y >= TAB_STOP_POS_Y - TAB_START_POS_Y)
        {
            gui_view_switch_direct(gui_view_get_current(), "charging", SWITCH_OUT_NONE_ANIMATION,
                                   SWITCH_IN_ANIMATION_FADE);
            gui_obj_stop_timer(GUI_BASE(img));
            return;
        }
        else if (y >= (TAB_STOP_POS_Y - TAB_START_POS_Y - 40))
        {
            y += step;
        }
        else
        {
            y -= step;
        }
        if (y <= 0)
        {
            y = 0;
            gui_img_set_image_data(img, press_bg);
            gui_obj_t *icon = gui_list_entry(img->base.child_list.next, gui_obj_t, brother_list);
            gui_obj_hidden(icon, false);
            gui_obj_stop_timer(GUI_BASE(img));
        }
        else if (y >= TAB_STOP_POS_Y - TAB_START_POS_Y)
        {
            y = TAB_STOP_POS_Y - TAB_START_POS_Y;
        }
        gui_img_translate(img, 0, (float)y);
    }
    else
    {
        gui_img_set_image_data(img, press_bg);
        gui_obj_t *icon = gui_list_entry(img->base.child_list.next, gui_obj_t, brother_list);
        gui_obj_hidden(icon, false);
        gui_obj_stop_timer(GUI_BASE(img));
    }
}

static void render_bar_bg(NVGcontext *ctx)
{
    // Draw rounded rectangle with width 80 and height 288, corner radius 40
    nvgBeginPath(ctx);
    nvgRoundedRect(ctx, 0, 0, 80, 288, 40);

    // Create vertical gradient from top to bottom
    // Start color: ARGB 0xFF00C8FF = RGBA(0, 200, 255, 255)
    // End color: same RGB but with 20% opacity = RGBA(0, 200, 255, 51)
    NVGpaint gradient = nvgLinearGradient(ctx,
                                          0, 0,           // Start point (top)
                                          0, 288,         // End point (bottom)
                                          nvgRGBA(0x0C, 0x83, 0xA4, 255),  // Start color: 0C83A4
                                          nvgRGBA(0x1A, 0x31, 0x38, 255)    // End color: 1A3138
                                         );

    // Apply gradient fill
    nvgFillPaint(ctx, gradient);
    nvgFill(ctx);
}

static void tab_scale(void *obj)
{
    static uint8_t cnt = 0;
    cnt++;
    uint8_t cnt_max = 5;
    gui_img_t *img = (gui_img_t *)obj;
    float scale = 0.8f + 0.2f * cnt / (float)cnt_max;
    gui_img_scale(img, scale, scale);
    if (cnt >= cnt_max)
    {
        cnt = 0;
        gui_obj_create_timer(GUI_BASE(img), 10, true, pressing_tab);
    }
}

static void press_tab(void *obj, gui_event_t e, void *param)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);
    GUI_UNUSED(param);

    gui_img_t *img = (gui_img_t *)obj;
    gui_img_set_image_data(img, (void *)FILE_POINTER(ICON_PRESSING_BIN));
    gui_img_scale(img, 0.8f, 0.8f);
    gui_obj_create_timer(GUI_BASE(img), 10, true, tab_scale);
    gui_obj_start_timer(GUI_BASE(img));

    gui_obj_t *icon = gui_list_entry(img->base.child_list.next, gui_obj_t, brother_list);
    gui_obj_hidden(icon, true);
}

void lock_view_design(gui_view_t *view)
{
    gui_obj_t *parent = GUI_BASE(view);

    if (!press_bg)
    {
        uint32_t size = 90 * 90 * 4 + sizeof(gui_rgb_data_head_t);
        press_bg = gui_malloc(size);
        memset(press_bg, 0, size);
        gui_canvas_render_to_image_buffer(GUI_CANVAS_OUTPUT_RGBA_NOMIX,
                                          0, 90, 90, render_press, press_bg);

        pressing_bg = gui_malloc(size);
        memset(pressing_bg, 0, size);
        gui_canvas_render_to_image_buffer(GUI_CANVAS_OUTPUT_RGBA_NOMIX,
                                          0, 90, 90, render_pressing, pressing_bg);

        size = 80 * 288 * 4 + sizeof(gui_rgb_data_head_t);
        bar_bg = gui_malloc(size);
        memset(bar_bg, 0, size);
        gui_canvas_render_to_image_buffer(GUI_CANVAS_OUTPUT_RGBA_NOMIX,
                                          0, 80, 288, render_bar_bg, bar_bg);
    }
    gui_img_t *bg = gui_img_create_from_mem(parent, "bg", bar_bg,
                                            165, 107, 0, 0);
    gui_img_set_mode(bg, IMG_SRC_OVER_MODE);

    int16_t focus = 45;
    gui_img_t *tab = gui_img_create_from_mem(parent, "tab_bg", press_bg,
                                             TAB_START_POS_X + focus,
                                             TAB_START_POS_Y + focus, 0, 0);
    gui_img_set_focus(tab, 0.1f + focus, 0.1f + focus);
    gui_img_set_mode(tab, IMG_SRC_OVER_MODE);
    gui_img_set_quality(tab, true);
    gui_obj_add_event_cb(tab, press_tab, GUI_EVENT_TOUCH_PRESSED, NULL);

    gui_img_create_from_mem(tab, 0, (void *)FILE_POINTER(ICON_TAB_BIN), 27, 27, 0, 0);

    gui_img_create_from_mem(parent, 0, (void *)FILE_POINTER(LOCK_OPEN_BIN), 187, 339, 0, 0);
}