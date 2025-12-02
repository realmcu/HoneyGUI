#include "gui_view_macro.h"
#include "gui_canvas.h"
#include "gui_img.h"

#include <math.h>

typedef struct
{
    float time;
    float scale;
    float angle;
    float corner_radius;
    float alpha;
    float offset_x;
    float offset_y;
} AnimState;

static AnimState anim = {0};


static void update_animation(float dt)
{
    anim.time += dt;

    anim.scale = 1.0f + 0.3f * (0.5f + 0.5f * sinf(anim.time * 1.8f));

    anim.angle = anim.time * 30.0f;

    anim.corner_radius = 5.0f + 55.0f * (0.5f + 0.5f * sinf(anim.time * 2.5f));

    anim.alpha = 0.6f + 0.4f * (0.5f + 0.5f * sinf(anim.time * 1.2f));

    anim.offset_x = 8.0f * sinf(anim.time * 1.1f);
    anim.offset_y = 6.0f * cosf(anim.time * 1.4f);
}
static void render(NVGcontext *vg)
{
    float dt = 0.01f;
    if (dt <= 0) { dt = 0.016f; }

    update_animation(dt);

    float base_x = 150.0f + anim.offset_x;
    float base_y = 150.0f + anim.offset_y;
    float w = 200.0f * anim.scale;
    float h = 180.0f * anim.scale;

    nvgSave(vg);

    nvgTranslate(vg, base_x + w / 2, base_y + h / 2);
#define    NVG_DEG_TO_RAD(angle) ((angle) * M_PI_F / 180.0f)
    nvgRotate(vg, NVG_DEG_TO_RAD(anim.angle));
    nvgTranslate(vg, -w / 2, -h / 2);

    nvgBeginPath(vg);
    nvgRoundedRect(vg, 0, 0, w, h, anim.corner_radius);

    nvgStrokeWidth(vg, 8.0f * anim.scale);
    NVGcolor stroke_col = nvgRGB(255, 0, 0);
    stroke_col.a = anim.alpha;
    nvgStrokeColor(vg, stroke_col);
    nvgStroke(vg);

    float hue = fmodf(anim.time * 40.0f, 360.0f);
    NVGcolor col1 = nvgHSLA(hue / 360.0f, 0.8f, 0.6f, (unsigned char)(255 * anim.alpha));
    NVGcolor col2 = nvgHSLA((hue + 90) / 360.0f, 0.8f, 0.6f, (unsigned char)(255 * anim.alpha));

    NVGpaint gradient = nvgLinearGradient(vg, 0, 0, w, h, col1, col2);
    nvgFillPaint(vg, gradient);
    nvgFill(vg);

    nvgRestore(vg);
}
static void canvas_rect_cb(gui_canvas_t *canvas)
{

    NVGcontext *vg = canvas->vg;
    render(vg);
    canvas->render = 1;
}

#define CANVAS_W 480
#define CANVAS_H 480
#define CANVAS_BUFFER_LENGTH CANVAS_W*CANVAS_H*4
static uint8_t *buffer = 0;
static void vector_graphic_switch_in_timer(void *p)
{
    GUI_UNUSED(p); gui_log("111\n");
    memset(buffer, 0,  CANVAS_BUFFER_LENGTH);
    gui_canvas_render_to_image_buffer(GUI_CANVAS_OUTPUT_RGBA,
                                      0, 480, 480, render, buffer);
}
static int resource(void)
{
#ifdef _WIN32
    extern const unsigned char _binary_root_0x00950000_bin_start[];
    resource_root = (unsigned char *)_binary_root_0x00950000_bin_start;
#else
    resource_root = 0x704D1400;
#endif
    return 0;
}
static void vector_graphic_switch_in(gui_view_t *view)
{


    GUI_UNUSED(view);
    gui_log("vector_graphic_view switch in\n");
    // gui_canvas_t *canvas = gui_canvas_create(view, "canvas", 0, 0, 0, CANVAS_W, CANVAS_H);
    // gui_canvas_set_canvas_cb(canvas, canvas_rect_cb111);

    buffer = gui_lower_malloc(CANVAS_W * CANVAS_H * 4);
    memset(buffer, 0,  CANVAS_BUFFER_LENGTH);
    gui_canvas_render_to_image_buffer(GUI_CANVAS_OUTPUT_RGBA,
                                      0, 480, 480, render, buffer);
    gui_img_t *img = gui_img_create_from_mem(view, 0, buffer, 0, 0, 0, 0);
    gui_img_set_mode(img, IMG_SRC_OVER_MODE);
//gui_obj_create_timer(img, 10, 1, vector_graphic_switch_in_timer);






    gui_view_switch_on_event(view, "transparent_gradient_animation_view",
                             SWITCH_OUT_TO_RIGHT_USE_TRANSLATION,
                             SWITCH_IN_FROM_LEFT_USE_TRANSLATION,
                             GUI_EVENT_TOUCH_MOVE_RIGHT);
    gui_view_switch_on_event(view, "fps_show_view",
                             SWITCH_OUT_TO_LEFT_USE_TRANSLATION,
                             SWITCH_IN_FROM_RIGHT_USE_TRANSLATION,
                             GUI_EVENT_TOUCH_MOVE_LEFT);
}

static void vector_graphic_switch_out(gui_view_t *view)
{
    GUI_UNUSED(view);
    gui_log("vector_graphic_view switch out\n");
    if (buffer)
    {
        gui_lower_free(buffer);
    }

}
#include "gui_win.h"
GUI_VIEW_INSTANCE("vector_graphic_view", 1, vector_graphic_switch_in, vector_graphic_switch_out);
unsigned char *resource_root = 0;
void fps_create(void *parent);
static int chargecase_demo_entry(void)
{

    gui_win_t *win = gui_win_create(gui_obj_get_root(), 0, 0, 0, 0, 0);
    gui_view_create(win, "vector_graphic_view", 0, 0, 0, 0);
    fps_create(gui_obj_get_root());
    return 0;
}
GUI_INIT_VIEW_DESCRIPTOR_REGISTER(resource);
GUI_INIT_APP_EXPORT(chargecase_demo_entry);

