#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include <nanovg.h>
#include <trace.h>
#include "vg_lite.h"

#define NANOVG_VGLITE_DEBUG
#ifdef NANOVG_VGLITE_DEBUG

#define NANOVG_LOG(format, ...) DBG_DIRECT(format, ##__VA_ARGS__)

#else

#define NANOVG_LOG(format, ...)

#endif

static int vglite_nvg_renderCreate(void *uptr)
{
    NANOVG_LOG(" %s %d\n", __func__, __LINE__);
    NVG_NOTUSED(uptr);
    NANOVG_LOG(" uptr = 0x%x\n", uptr);
    return 1;
}
static int vglite_nvg_renderCreateTexture(void *uptr, int type, int w, int h, int imageFlags,
                                          const unsigned char *data)
{
    NANOVG_LOG(" %s %d\n", __func__, __LINE__);
    NANOVG_LOG(" uptr = 0x%x, type = %d, w = %d, h = %d, imageFlags = %d, data = 0x%x\n", uptr, type, w,
               h, imageFlags, data);
    return 0;
}
static int vglite_nvg_renderDeleteTexture(void *uptr, int image)
{
    NANOVG_LOG(" %s %d\n", __func__, __LINE__);
    NANOVG_LOG(" uptr = 0x%x, image = %d\n", uptr, image);
    return 0;
}
static int vglite_nvg_renderUpdateTexture(void *uptr, int image, int x, int y, int w, int h,
                                          const unsigned char *data)
{
    NANOVG_LOG(" %s %d\n", __func__, __LINE__);
    NANOVG_LOG(" uptr = 0x%x, image = %d, x = %d, y = %d, w = %d, h = %d, data = 0x%x\n", uptr, image,
               x, y, w, h, data);
    return 0;
}
static void vglite_nvg_renderViewport(void *uptr, float width, float height, float devicePixelRatio)
{
    NANOVG_LOG(" %s %d\n", __func__, __LINE__);
    NANOVG_LOG(" uptr = 0x%x, width = %f, height = %f, devicePixelRatio = %f\n", uptr, width, height,
               devicePixelRatio);
    NVG_NOTUSED(uptr);
    NVG_NOTUSED(width);
    NVG_NOTUSED(height);
    NVG_NOTUSED(devicePixelRatio);
}
static int vglite_nvg_renderGetTextureSize(void *uptr, int image, int *w, int *h)
{
    NANOVG_LOG(" %s %d\n", __func__, __LINE__);
    NANOVG_LOG(" uptr = 0x%x, image = %d, w = 0x%x, h = 0x%x\n", uptr, image, w, h);
    return 0;
}
static void vglite_nvg_renderCancel(void *uptr)
{
    NVG_NOTUSED(uptr);
    NANOVG_LOG(" %s %d\n", __func__, __LINE__);
    NANOVG_LOG(" uptr = 0x%x\n", uptr);
}

static void vglite_nvg_renderFlush(void *uptr)
{
    NVG_NOTUSED(uptr);
    NANOVG_LOG(" %s %d\n", __func__, __LINE__);
    NANOVG_LOG(" uptr = 0x%x\n", uptr);

    vg_lite_finish();
}
static void vglite_nvg_renderDelete(void *uptr)
{
    NANOVG_LOG(" %s %d\n", __func__, __LINE__);
}

void vglite_nvg_renderStroke(void *uptr, NVGpaint *paint,
                             NVGcompositeOperationState compositeOperation,
                             NVGscissor *scissor, float fringe, float strokeWidth, const NVGpath *paths,
                             int npaths)
{
    NANOVG_LOG(" %s %d\n", __func__, __LINE__);
}
void vglite_nvg_renderFill(void *uptr, NVGpaint *paint,
                           NVGcompositeOperationState compositeOperation,
                           NVGscissor *scissor, float fringe, const float *bounds, const NVGpath *paths,
                           int npaths)
{
    vg_lite_buffer_t *target = (vg_lite_buffer_t *)uptr;
    NANOVG_LOG("[%s] paint->radius = %f\n", __func__, paint->radius);
    NANOVG_LOG("[%s] paint->feather = %f\n", __func__, paint->feather);
    NANOVG_LOG("[%s] paint->image = %d\n", __func__, paint->image);
    NANOVG_LOG("[%s] paint->innerColor A[%f], R[%f], G[%f], B[%f]\n", __func__, paint->innerColor.a,
               paint->innerColor.r, paint->innerColor.g, paint->innerColor.b);
    NANOVG_LOG("[%s] paint->outerColor A[%f], R[%f], G[%f], B[%f]\n", __func__, paint->outerColor.a,
               paint->outerColor.r, paint->outerColor.g, paint->outerColor.b);
    NANOVG_LOG("[%s] paint->xform = [%f][%f][%f][%f][%f][%f]\n", __func__, paint->xform[0],
               paint->xform[1], paint->xform[2], paint->xform[3], paint->xform[4], paint->xform[5]);
    NANOVG_LOG("[%s] paint->extent = [%f][%f]\n", __func__, paint->extent[0], paint->extent[1]);

    for (int i = 0; i < npaths; i++)
    {
        const NVGpath *p = paths + i;
        for (int j = 0; j < p->nfill; j++)
        {
            const NVGvertex *v = p->fill + j;
            if (j == 0)
            {
                //ras.move_to(v->x, v->y);
                NANOVG_LOG("[J = 0]v->x[%f], v->y[%f]\n", v->x, v->y);
            }
            else
            {
                //ras.line_to(v->x, v->y);
                NANOVG_LOG("[J = 1]v->x[%f], v->y[%f]\n", v->x, v->y);
            }
        }
        NANOVG_LOG("[CLOSE]\n");
    }
}


NVGcontext *nvgCreateAGGE(uint32_t w, uint32_t h, uint32_t stride, enum NVGtexture format,
                          uint8_t *data)
{
    NVGparams params;
    NVGcontext *ctx = NULL;
    //AGGENVGcontext* agge = new AGGENVGcontext();
    static vg_lite_buffer_t target;
    memset(&target, 0x00, sizeof(vg_lite_buffer_t));
    target.width  = w;
    target.height = h;
    target.format = VG_LITE_RGBA8888;
    target.memory = (void *)data;
    target.address = (uint32_t)data;
    target.stride = target.width * 4;

    //if (agge == NULL) goto error;

    memset(&params, 0, sizeof(params));
    params.renderCreate = vglite_nvg_renderCreate;
    params.renderCreateTexture = vglite_nvg_renderCreateTexture;
    params.renderDeleteTexture = vglite_nvg_renderDeleteTexture;
    params.renderUpdateTexture = vglite_nvg_renderUpdateTexture;
    params.renderGetTextureSize = vglite_nvg_renderGetTextureSize;
    params.renderViewport = vglite_nvg_renderViewport;
    params.renderCancel = vglite_nvg_renderCancel;
    params.renderFlush = vglite_nvg_renderFlush;
    params.renderDelete = vglite_nvg_renderDelete;
    params.renderTriangles = NULL;
    params.renderStroke = vglite_nvg_renderStroke;
    params.renderFill = vglite_nvg_renderFill;
    params.userPtr = (void *)&target;
    params.edgeAntiAlias = 1;

    //nvgInitAGGE(agge, &params, w, h, stride, format, data);


    //vg_lite_clear(&target, NULL, 0xFFFFAAAA);
    //vg_lite_finish();

    ctx = nvgCreateInternal(&params);
    if (ctx == NULL) { goto error; }

    return ctx;

error:
    if (ctx != NULL) { nvgDeleteInternal(ctx); }
    return NULL;
}

void nvgDeleteAGGE(NVGcontext *ctx)
{
    nvgDeleteInternal(ctx);
}
