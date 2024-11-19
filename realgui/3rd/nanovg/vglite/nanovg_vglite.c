#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include <nanovg.h>
#include <trace.h>
#include "vg_lite.h"

//#define NANOVG_VGLITE_DEBUG
#ifdef NANOVG_VGLITE_DEBUG

#define NANOVG_LOG(format, ...) gui_log(format, ##__VA_ARGS__)

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
    return 0;
}
static void vglite_nvg_renderCancel(void *uptr)
{
    NVG_NOTUSED(uptr);
    NANOVG_LOG(" %s %d\n", __func__, __LINE__);
}

static void vglite_nvg_renderFlush(void *uptr)
{
    NVG_NOTUSED(uptr);
    NANOVG_LOG(" %s %d\n", __func__, __LINE__);
}
static void vglite_nvg_renderDelete(void *uptr)
{
    NVG_NOTUSED(uptr);
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
    NANOVG_LOG(" %s %d\n", __func__, __LINE__);
}


NVGcontext *nvgCreateAGGE(uint32_t w, uint32_t h, uint32_t stride, enum NVGtexture format,
                          uint8_t *data)
{
    NVGparams params;
    NVGcontext *ctx = NULL;
    static vg_lite_buffer_t target;
    memset(&target, 0x00, sizeof(vg_lite_buffer_t));
    target.width  = w;
    target.height = h;
    target.format = VG_LITE_BGRA8888;
    target.memory = (void *)data;
    target.address = (uint32_t)data;
    target.stride = target.width * 4;


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

    extern void patch_nvgStroke(NVGcontext * ctx);
    extern void (*patch_nvg_stroke)(NVGcontext * ctx);
    extern void patch_nvgFill(NVGcontext * ctx);
    extern void (*patch_nvg_fill)(NVGcontext * ctx);

    patch_nvg_stroke = patch_nvgStroke;
    patch_nvg_fill = patch_nvgFill;


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
