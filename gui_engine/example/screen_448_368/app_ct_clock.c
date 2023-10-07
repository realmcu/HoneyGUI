#include "root_image_hongkong/ui_resource.h"
#include <gui_magic_img.h>
#include "gui_win.h"
#include "gui_watch_gradient_spot.h"
gui_win_t *win_watch;
gui_magic_img_t *img;
gui_watch_gradient_spot_t *watch;
static gui_canvas_t *canvas;
static void callback_watchface()
{
    static char clock;
    if (clock == 1)
    {

        GET_BASE(img)->not_show = false;
        GET_BASE(watch)->not_show = true;
        GET_BASE(canvas)->not_show = true;
        clock = 2;
    }
    else if (clock == 2)
    {
        GET_BASE(img)->not_show = true;
        GET_BASE(watch)->not_show = false;
        GET_BASE(canvas)->not_show = true;
        clock = 0;
    }
    else if (clock == 0)
    {
        GET_BASE(img)->not_show = true;
        GET_BASE(watch)->not_show = true;
        GET_BASE(canvas)->not_show = false;
        clock = 1;
    }



}
static void canvas_design(gui_canvas_t *canvas)
{
    gui_dispdev_t *dc = gui_get_dc();
    nvgBeginPath(canvas->vg);
    nvgRect(canvas->vg, 0, 0, 368, 448);
    nvgFillColor(canvas->vg, nvgRGB(0x7C, 0x7A, 0xEB));
    nvgFill(canvas->vg);
    nvgBeginPath(canvas->vg);
    nvgCircle(canvas->vg, 368 / 2, 448 / 2, 296.52 / 2);
    nvgFillColor(canvas->vg, nvgRGBA(94, 92, 230, 0.5 * 0xff));
    nvgFill(canvas->vg);
    nvgBeginPath(canvas->vg);
    nvgCircle(canvas->vg, 368 / 2, 448 / 2, 192.17 / 2);
    nvgFillColor(canvas->vg, nvgRGBA(168, 167, 241, 0.5 * 0xff));
    nvgFill(canvas->vg);
    nvgBeginPath(canvas->vg);
    nvgCircle(canvas->vg, 368 / 2, 448 / 2, 87.83 / 2);
    nvgFillColor(canvas->vg, nvgRGBA(94, 92, 230, 0.5 * 0xff));
    nvgFill(canvas->vg);
    static float second;
    second++;
    float hour_degree = (second / 3600.0f) / 12.0f * 2 * NVG_PI;
    NVGcontext *vg = canvas->vg;
    gui_canvas_t *this = canvas;
    {
        //for hour point
        nvgResetTransform(vg);

        nvgBeginPath(vg);

        float width = 5;
        float height = 24;
        float x = width / 2;
        float y = height;

        nvgTranslate(vg, GET_BASE(this)->dx, GET_BASE(this)->dy);
        nvgTranslate(vg, GET_BASE(this)->tx, GET_BASE(this)->ty);
        nvgTranslate(vg, GET_BASE(this)->ax, GET_BASE(this)->ay);
        nvgTranslate(vg, dc->screen_width / 2, dc->screen_height / 2);
        nvgRotate(vg, hour_degree * 2.0f);
        nvgScale(vg, this->base.sx, this->base.sy);
        nvgTranslate(vg, -x, -y);

        nvgRoundedRect(vg, 0, 0, width, height, 0);
        nvgFillColor(vg, nvgRGBA(255, 255, 255, 1 * 255));
        nvgFill(vg);
    }
    {
        float r0, r1, ax, ay, bx, by, cx, cy, aeps, r;
        float a0, a1;
        cx = 368 / 2;
        cy = 448 / 2;
        r1 = 192.17F / 2.0F;
        r0 = 87.83F / 2.0F;
        a0 = hour_degree * 2.0f - NVG_PI / 2.0f;
        a1 = a0 + NVG_PI / 1.7f;
        nvgResetTransform(vg);
        nvgBeginPath(vg);
        nvgArc(vg, 368 / 2, 448 / 2, r1, a0, a1, NVG_CW);
        nvgArc(vg, 368 / 2, 448 / 2, r0, a1, a0, NVG_CCW);
        nvgClosePath(vg);
        ax = cx + cosf(a0) * (r0 + r1) * 0.5f;
        ay = cy + sinf(a0) * (r0 + r1) * 0.5f;
        bx = cx + cosf(a1) * (r0 + r1) * 0.5f;
        by = cy + sinf(a1) * (r0 + r1) * 0.5f;
        NVGpaint paint = nvgLinearGradient(vg, ax, ay, bx, by, nvgRGBA(255, 153, 102, 1 * 255), nvgRGBA(255,
                                           153, 102, 0));
        nvgFillPaint(vg, paint);
        nvgFill(vg);
    }

    {
        //for hour point
        nvgResetTransform(vg);

        nvgBeginPath(vg);

        float width = 15;
        float height = 73;
        float x = width / 2;
        float y = height + 20 + 3;

        nvgTranslate(vg, GET_BASE(this)->dx, GET_BASE(this)->dy);
        nvgTranslate(vg, GET_BASE(this)->tx, GET_BASE(this)->ty);
        nvgTranslate(vg, GET_BASE(this)->ax, GET_BASE(this)->ay);
        nvgTranslate(vg, dc->screen_width / 2, dc->screen_height / 2);
        nvgRotate(vg, hour_degree * 2.0f);
        nvgScale(vg, this->base.sx, this->base.sy);
        nvgTranslate(vg, -x, -y);

        nvgRoundedRect(vg, 0, 0, width, height, width / 2.0f);
        nvgFillColor(vg, nvgRGBA(255, 255, 255, 255));
        nvgFill(vg);
    }
    {
        //for hour point
        nvgResetTransform(vg);

        nvgBeginPath(vg);

        float width = 9;
        float height = 67;
        float x = width / 2;
        float y = height + 20 + 3 + 3;

        nvgTranslate(vg, GET_BASE(this)->dx, GET_BASE(this)->dy);
        nvgTranslate(vg, GET_BASE(this)->tx, GET_BASE(this)->ty);
        nvgTranslate(vg, GET_BASE(this)->ax, GET_BASE(this)->ay);
        nvgTranslate(vg, dc->screen_width / 2, dc->screen_height / 2);
        nvgRotate(vg, hour_degree * 2.0f);
        nvgScale(vg, this->base.sx, this->base.sy);
        nvgTranslate(vg, -x, -y);

        nvgRoundedRect(vg, 0, 0, width, height, width / 2.0f);
        nvgFillColor(vg, nvgRGBA(255, 153, 102, 1 * 255));
        nvgFill(vg);
    }



    {
        //for minuts point
        nvgResetTransform(vg);

        nvgBeginPath(vg);

        float width = 5;
        float height = 28;
        float x = width / 2;
        float y = height;

        nvgTranslate(vg, GET_BASE(this)->dx, GET_BASE(this)->dy);
        nvgTranslate(vg, GET_BASE(this)->tx, GET_BASE(this)->ty);
        nvgTranslate(vg, GET_BASE(this)->ax, GET_BASE(this)->ay);
        nvgTranslate(vg, dc->screen_width / 2, dc->screen_height / 2);
        nvgRotate(vg, hour_degree * 10.0F);
        nvgScale(vg, this->base.sx, this->base.sy);
        nvgTranslate(vg, -x, -y);

        nvgRoundedRect(vg, 0, 0, width, height, 0);
        nvgFillColor(vg, nvgRGBA(255, 255, 255, 1 * 255));
        nvgFill(vg);
    }
    {
        float r0, r1, ax, ay, bx, by, cx, cy, aeps, r;
        float a0, a1;
        cx = 368 / 2;
        cy = 448 / 2;
        r1 = 296.52 / 2;
        r0 = 192.17F / 2.0F;
        a0 = hour_degree * 10.0F - NVG_PI / 2.0f;
        a1 = a0 + NVG_PI / 1.7f;
        nvgResetTransform(vg);
        nvgBeginPath(vg);
        nvgArc(vg, 368 / 2, 448 / 2, r1, a0, a1, NVG_CW);
        nvgArc(vg, 368 / 2, 448 / 2, r0, a1, a0, NVG_CCW);
        nvgClosePath(vg);
        ax = cx + cosf(a0) * (r0 + r1) * 0.5f;
        ay = cy + sinf(a0) * (r0 + r1) * 0.5f;
        bx = cx + cosf(a1) * (r0 + r1) * 0.5f;
        by = cy + sinf(a1) * (r0 + r1) * 0.5f;
        NVGpaint paint = nvgLinearGradient(vg, ax, ay, bx, by, nvgRGBA(168, 167, 241, 1 * 255), nvgRGBA(94,
                                           92, 230, 0));
        nvgFillPaint(vg, paint);
        nvgFill(vg);
    }
    {
        //for minuts point
        nvgResetTransform(vg);

        nvgBeginPath(vg);

        float width = 15;
        float height = 123;
        float x = width / 2;
        float y = height + 20 + 5;

        nvgTranslate(vg, GET_BASE(this)->dx, GET_BASE(this)->dy);
        nvgTranslate(vg, GET_BASE(this)->tx, GET_BASE(this)->ty);
        nvgTranslate(vg, GET_BASE(this)->ax, GET_BASE(this)->ay);
        nvgTranslate(vg, dc->screen_width / 2, dc->screen_height / 2);
        nvgRotate(vg, hour_degree * 10.0F);
        nvgScale(vg, this->base.sx, this->base.sy);
        nvgTranslate(vg, -x, -y);

        nvgRoundedRect(vg, 0, 0, width, height, width / 2.0f);
        nvgFillColor(vg, nvgRGBA(255, 255, 255, 255));
        nvgFill(vg);
    }
    {
        //for minuts point
        nvgResetTransform(vg);

        nvgBeginPath(vg);

        float width = 9;
        float height = 116;
        float x = width / 2;
        float y = height + 20 + 5 + 3;

        nvgTranslate(vg, GET_BASE(this)->dx, GET_BASE(this)->dy);
        nvgTranslate(vg, GET_BASE(this)->tx, GET_BASE(this)->ty);
        nvgTranslate(vg, GET_BASE(this)->ax, GET_BASE(this)->ay);
        nvgTranslate(vg, dc->screen_width / 2, dc->screen_height / 2);
        nvgRotate(vg, hour_degree * 10.0F);
        nvgScale(vg, this->base.sx, this->base.sy);
        nvgTranslate(vg, -x, -y);

        nvgRoundedRect(vg, 0, 0, width, height, width / 2.0f);
        nvgFillColor(vg, nvgRGBA(94, 92, 230, 1 * 255));
        nvgFill(vg);
    }
    {
        //for sceond point
        float r0, r1, ax, ay, bx, by, cx, cy, aeps, r;
        float a0, a1;
        cx = 368 / 2;
        cy = 448 / 2;
        r1 = 448 / 2;
        r0 = 2 / 2.0F;
        a0 = hour_degree * 60.0f - NVG_PI / 2.0f;
        a1 = a0 + NVG_PI / 1.7f;
        nvgResetTransform(vg);
        nvgBeginPath(vg);
        nvgTranslate(vg, dc->screen_width / 2, dc->screen_height / 2);
        nvgRotate(vg, hour_degree * 60.0F);
        nvgTranslate(vg, -dc->screen_width / 2, -dc->screen_height / 2);
        nvgRect(vg, cx, cy, cx * 2, cy * 2);

        nvgClosePath(vg);
        NVGpaint paint = nvgLinearGradient(vg, cx * 2.0f, cy * 2.5f, cx * 2.0f, cy, nvgRGBA(255, 51, 119,
                                           1 * 255), nvgRGBA(255, 51, 119, 0));
        nvgFillPaint(vg, paint);

        nvgFill(vg);
    }
    {
        //for sceond point
        nvgResetTransform(vg);
        nvgTranslate(vg, GET_BASE(this)->dx, GET_BASE(this)->dy);
        nvgTranslate(vg, GET_BASE(this)->tx, GET_BASE(this)->ty);
        nvgTranslate(vg, GET_BASE(this)->ax, GET_BASE(this)->ay);
        nvgBeginPath(vg);
        nvgCircle(vg, 368 / 2, 448 / 2, 15.0f / 2.0f);
        nvgFillColor(vg, nvgRGBA(255, 255, 255, 1 * 255));
        nvgFill(vg);
    }
    {
        //for sceond point
        nvgResetTransform(vg);
        nvgTranslate(vg, GET_BASE(this)->dx, GET_BASE(this)->dy);
        nvgTranslate(vg, GET_BASE(this)->tx, GET_BASE(this)->ty);
        nvgTranslate(vg, GET_BASE(this)->ax, GET_BASE(this)->ay);
        nvgBeginPath(vg);
        nvgCircle(vg, 368 / 2, 448 / 2, 7.0f / 2.0f);
        nvgFillColor(vg, nvgRGBA(0, 0, 0, 1 * 255));
        nvgFill(vg);
    }

}
void page_ct_clock(void *parent)
{
    win_watch = gui_win_create(parent, "win", 0, 0, 368, 448);
    gui_obj_add_event_cb(win_watch, (gui_event_cb_t)callback_watchface, GUI_EVENT_TOUCH_CLICKED, NULL);
    img = gui_magic_img_create_from_mem(parent, "page0", CLOCKN_BIN, 0, 0, 0, 0);
    watch = gui_watch_gradient_spot_create(win_watch, "watchface", 0, 0, 0, 0);
    gui_watch_gradient_spot_set_center(watch, 368 / 2, 448 / 2);
    canvas = gui_canvas_create(parent, "canvas", 0, 0, 0, 368, 448);
    gui_canvas_set_canvas_cb(canvas, canvas_design);
    GET_BASE(watch)->not_show = true;
    GET_BASE(canvas)->not_show = true;
}
