#include "PX_3D.h"
#include "gui_api.h"
#include <nanovg.h>

extern NVGcontext *nvgCreateAGGE(uint32_t w, uint32_t h, uint32_t stride, enum NVGtexture format,
                                 uint8_t *data);
extern void nvgDeleteAGGE(NVGcontext *ctx);

px_void PX_3D_GPU_RenderListRasterization(px_surface *psurface, PX_3D_RenderList *pList,
                                          PX_3D_Vertex p0, PX_3D_Vertex p1, PX_3D_Vertex p2)
{
    gui_dispdev_t *dc = gui_get_dc();
    uint8_t *data = (uint8_t *)psurface->surfaceBuffer;


    NVGcontext *vg = nvgCreateAGGE(dc->fb_width, dc->fb_height, dc->fb_width * (dc->bit_depth >> 3),
                                   (dc->bit_depth >> 3) == 2 ? NVG_TEXTURE_BGR565 : NVG_TEXTURE_BGRA, data);


    nvgBeginFrame(vg, psurface->width, psurface->height, 1);

    nvgResetTransform(vg);

    nvgBeginPath(vg);

    nvgMoveTo(vg, p0.position.x, p0.position.y);
    nvgLineTo(vg, p1.position.x, p1.position.y);
    nvgLineTo(vg, p2.position.x, p2.position.y);
    nvgClosePath(vg);


    nvgFillColor(vg, nvgRGBA(0, 255, 0, 255));
    nvgFill(vg);


    nvgEndFrame(vg);
    nvgDeleteAGGE(vg);
}
