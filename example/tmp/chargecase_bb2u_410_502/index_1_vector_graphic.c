#include "gui_view_macro.h"
#include "gui_canvas.h"

#ifdef _WIN32
unsigned char __attribute__((aligned(8))) resource_array[1024 * 1024 * 10] ;
unsigned char *resource_root = resource_array;
#endif
#include "root_image/ui_resource.h"


static void canvas_rect_cb(gui_canvas_t *canvas)
{
    NVGcontext *vg = canvas->vg;
    nvgBeginPath(vg);
    nvgRoundedRect(vg, 150, 150, 200, 180, 20);

    nvgStrokeWidth(vg, 8.0f);
    nvgStrokeColor(vg, nvgRGB(255, 0, 0));
    nvgStroke(vg);

    NVGpaint gradient = nvgLinearGradient(vg, 150, 150, 350, 330, nvgRGB(255, 0, 0), nvgRGBA(0, 255, 0,
                                          255));
    nvgFillPaint(vg, gradient);
    nvgFill(vg);
}

static void vector_graphic_switch_in(gui_view_t *view)
{
    GUI_UNUSED(view);
    gui_log("vector_graphic_view switch in\n");
    gui_canvas_t *canvas = gui_canvas_create(view, "canvas", 0, 0, 0, 480, 480);
    gui_canvas_set_canvas_cb(canvas, canvas_rect_cb);

    gui_view_switch_on_event(view, gui_view_descriptor_get("fps_show_view"),
                             SWITCH_OUT_TO_LEFT_USE_TRANSLATION,
                             SWITCH_IN_FROM_RIGHT_USE_TRANSLATION,
                             GUI_EVENT_TOUCH_MOVE_LEFT);
}

static void vector_graphic_switch_out(gui_view_t *view)
{
    GUI_UNUSED(view);
    gui_log("vector_graphic_view switch out\n");
}

GUI_VIEW_INSTANCE("vector_graphic_view", true, vector_graphic_switch_in, vector_graphic_switch_out);



static int chargecase_demo_entry(void)
{
#ifdef _WIN32
    const char *filename =
        "./example/tmp/chargecase_bb2u_410_502/root_image/root_0x704D1400.bin";

    int fd;
    fd = gui_fs_open(filename, 0);
    if (fd > 0)
    {
        printf("open root.bin Successful!\n");
        gui_fs_read(fd, resource_root, 1024 * 1024 * 10);
    }
    else
    {
        printf("open root.bin Fail!\n");
    }
#endif

    gui_view_create(gui_obj_get_root(), gui_view_descriptor_get("vector_graphic_view"), 0, 0, 0, 0);
    return 0;
}

GUI_INIT_APP_EXPORT(chargecase_demo_entry);

