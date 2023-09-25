#include "root_image_hongkong/ui_resource.h"
#include <gui_magic_img.h>
#include "gui_canvas.h"
static void canvas_cb(gui_canvas_t *canvas)
{
    nvgRect(canvas->vg, 0, 0, 368, 448);
    nvgFillColor(canvas->vg, nvgRGBA(0, 0, 0, 150));
    nvgFill(canvas->vg);
}
void page_ct_sidebar(void *parent)
{
    gui_canvas_t *canvas = gui_canvas_create(parent, "canvas", 0, 0, 0, 368, 448);
    gui_canvas_set_canvas_cb(canvas, canvas_cb);
}