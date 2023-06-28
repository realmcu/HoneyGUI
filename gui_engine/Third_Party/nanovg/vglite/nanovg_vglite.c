#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include <nanovg.h>


static int vglite_nvg_renderCreate(void *uptr)
{
    printf(" %s %d\n", __func__, __LINE__);
    return 0;
}
static int vglite_nvg_renderCreateTexture(void *uptr, int type, int w, int h, int imageFlags,
                                          const unsigned char *data)
{
    printf(" %s %d\n", __func__, __LINE__);
    return 0;
}
static int vglite_nvg_renderDeleteTexture(void *uptr, int image)
{
    printf(" %s %d\n", __func__, __LINE__);
    return 0;
}
static int vglite_nvg_renderUpdateTexture(void *uptr, int image, int x, int y, int w, int h,
                                          const unsigned char *data)
{
    printf(" %s %d\n", __func__, __LINE__);
    return 0;
}
static void vglite_nvg_renderViewport(void *uptr, float width, float height, float devicePixelRatio)
{
    printf(" %s %d\n", __func__, __LINE__);
    return 0;
}
static int vglite_nvg_renderGetTextureSize(void *uptr, int image, int *w, int *h)
{
    printf(" %s %d\n", __func__, __LINE__);
    return 0;
}
static void vglite_nvg_renderCancel(void *uptr)
{
    NVG_NOTUSED(uptr);
    printf(" %s %d\n", __func__, __LINE__);
    return 0;
}

static void vglite_nvg_renderFlush(void *uptr)
{
    NVG_NOTUSED(uptr);
    printf(" %s %d\n", __func__, __LINE__);
    return 0;
}
static void vglite_nvg_renderDelete(void *uptr)
{
    printf(" %s %d\n", __func__, __LINE__);
    return 0;
}

void vglite_nvg_renderStroke(void *uptr, NVGpaint *paint,
                             NVGcompositeOperationState compositeOperation,
                             NVGscissor *scissor, float fringe, float strokeWidth, const NVGpath *paths,
                             int npaths)
{
    printf(" %s %d\n", __func__, __LINE__);
    return 0;
}
void vglite_nvg_renderFill(void *uptr, NVGpaint *paint,
                           NVGcompositeOperationState compositeOperation,
                           NVGscissor *scissor, float fringe, const float *bounds, const NVGpath *paths,
                           int npaths)
{
    printf(" %s %d\n", __func__, __LINE__);
    return 0;
}


NVGcontext *nvgCreateAGGE(uint32_t w, uint32_t h, uint32_t stride, enum NVGtexture format,
                          uint8_t *data)
{
    NVGparams params;
    NVGcontext *ctx = NULL;
    //AGGENVGcontext* agge = new AGGENVGcontext();

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
    params.renderStroke = vglite_nvg_renderStroke;
    params.renderFill = vglite_nvg_renderFill;
    params.userPtr = NULL;
    params.edgeAntiAlias = 1;

    //nvgInitAGGE(agge, &params, w, h, stride, format, data);

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
