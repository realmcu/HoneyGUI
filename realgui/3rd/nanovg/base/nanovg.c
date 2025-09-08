//
// Copyright (c) 2013 Mikko Mononen memon@inside.org
//
// This software is provided 'as-is', without any express or implied
// warranty.  In no event will the authors be held liable for any damages
// arising from the use of this software.
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.
//

#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "nanovg.h"


#include "nanovg_port.h"

#define NANOVG_MALLOC nanovg_malloc
#define NANOVG_REALLOC nanovg_realloc
#define NANOVG_FREE nanovg_free

#ifdef _MSC_VER
#pragma warning(disable: 4100)  // unreferenced formal parameter
#pragma warning(disable: 4127)  // conditional expression is constant
#pragma warning(disable: 4204)  // nonstandard extension used : non-constant aggregate initializer
#pragma warning(disable: 4706)  // assignment within conditional expression
#endif

#define NVG_INIT_FONTIMAGE_SIZE  512
#define NVG_MAX_FONTIMAGE_SIZE   2048
#define NVG_MAX_FONTIMAGES       4

#define NVG_INIT_COMMANDS_SIZE 256
#define NVG_INIT_POINTS_SIZE 128
#define NVG_INIT_PATHS_SIZE 16
#define NVG_INIT_VERTS_SIZE 256
#define NVG_MAX_STATES 32

#define NVG_KAPPA90 0.5522847493f   // Length proportional to radius of a cubic bezier handle for 90deg arcs.

#define NVG_COUNTOF(arr) (sizeof(arr) / sizeof(0[arr]))


enum NVGcommands
{
    NVG_MOVETO = 0,
    NVG_LINETO = 1,
    NVG_BEZIERTO = 2,
    NVG_CLOSE = 3,
    NVG_WINDING = 4,
};

enum NVGpointFlags
{
    NVG_PT_CORNER = 0x01,
    NVG_PT_LEFT = 0x02,
    NVG_PT_BEVEL = 0x04,
    NVG_PR_INNERBEVEL = 0x08,
};

struct NVGstate
{
    NVGcompositeOperationState compositeOperation;
    int shapeAntiAlias;
    NVGpaint fill;
    NVGpaint stroke;
    float strokeWidth;
    float miterLimit;
    int lineJoin;
    int lineCap;
    float alpha;
    float xform[9];
    NVGscissor scissor;
    float fontSize;
    float letterSpacing;
    float lineHeight;
    float fontBlur;
    int textAlign;
    int fontId;
};
typedef struct NVGstate NVGstate;

struct NVGpoint
{
    float x, y;
    float dx, dy;
    float len;
    float dmx, dmy;
    unsigned char flags;
};
typedef struct NVGpoint NVGpoint;

struct NVGpathCache
{
    NVGpoint *points;
    int npoints;
    int cpoints;
    NVGpath *paths;
    int npaths;
    int cpaths;
    NVGvertex *verts;
    int nverts;
    int cverts;
    float bounds[4];
};
typedef struct NVGpathCache NVGpathCache;

struct NVGcontext
{
    NVGparams params;
    float *commands;
    int ccommands;
    int ncommands;
    float commandx, commandy;
    NVGstate states[NVG_MAX_STATES];
    int nstates;
    NVGpathCache *cache;
    float tessTol;
    float distTol;
    float fringeWidth;
    float devicePxRatio;
    struct FONScontext *fs;
    int fontImages[NVG_MAX_FONTIMAGES];
    int fontImageIdx;
    int drawCallCount;
    int fillTriCount;
    int strokeTriCount;
    int textTriCount;
};

float nvg__sqrtf(float a) { return sqrtf(a); }
float nvg__modf(float a, float b) { return fmodf(a, b); }
float nvg__sinf(float a) { return sinf(a); }
float nvg__cosf(float a) { return cosf(a); }
float nvg__tanf(float a) { return tanf(a); }
float nvg__atan2f(float a, float b) { return atan2f(a, b); }
float nvg__acosf(float a) { return acosf(a); }

static int nvg__mini(int a, int b) { return a < b ? a : b; }
static int nvg__maxi(int a, int b) { return a > b ? a : b; }
//static int nvg__clampi(int a, int mn, int mx) { return a < mn ? mn : (a > mx ? mx : a); }
static float nvg__minf(float a, float b) { return a < b ? a : b; }
static float nvg__maxf(float a, float b) { return a > b ? a : b; }
static float nvg__absf(float a) { return a >= 0.0f ? a : -a; }
static float nvg__signf(float a) { return a >= 0.0f ? 1.0f : -1.0f; }
static float nvg__clampf(float a, float mn, float mx) { return a < mn ? mn : (a > mx ? mx : a); }
static float nvg__cross(float dx0, float dy0, float dx1, float dy1) { return dx1 * dy0 - dx0 * dy1; }

static float nvg__normalize(float *x, float *y)
{
    float d = nvg__sqrtf((*x) * (*x) + (*y) * (*y));
    if (d > 1e-6f)
    {
        float id = 1.0f / d;
        *x *= id;
        *y *= id;
    }
    return d;
}


static void nvg__deletePathCache(NVGpathCache *c)
{
    if (c == NULL) { return; }
    if (c->points != NULL) { NANOVG_FREE(c->points); }
    if (c->paths != NULL) { NANOVG_FREE(c->paths); }
    if (c->verts != NULL) { NANOVG_FREE(c->verts); }
    NANOVG_FREE(c);
}

static NVGpathCache *nvg__allocPathCache(void)
{
    NVGpathCache *c = (NVGpathCache *)NANOVG_MALLOC(sizeof(NVGpathCache));
    if (c == NULL) { goto error; }
    memset(c, 0, sizeof(NVGpathCache));

    c->points = (NVGpoint *)NANOVG_MALLOC(sizeof(NVGpoint) * NVG_INIT_POINTS_SIZE);
    if (!c->points) { goto error; }
    c->npoints = 0;
    c->cpoints = NVG_INIT_POINTS_SIZE;

    c->paths = (NVGpath *)NANOVG_MALLOC(sizeof(NVGpath) * NVG_INIT_PATHS_SIZE);
    if (!c->paths) { goto error; }
    c->npaths = 0;
    c->cpaths = NVG_INIT_PATHS_SIZE;

    c->verts = (NVGvertex *)NANOVG_MALLOC(sizeof(NVGvertex) * NVG_INIT_VERTS_SIZE);
    if (!c->verts) { goto error; }
    c->nverts = 0;
    c->cverts = NVG_INIT_VERTS_SIZE;

    return c;
error:
    nvg__deletePathCache(c);
    return NULL;
}

static void nvg__setDevicePixelRatio(NVGcontext *ctx, float ratio)
{
    ctx->tessTol = 0.25f / ratio;
    ctx->distTol = 0.01f / ratio;
    ctx->fringeWidth = 1.0f / ratio;
    ctx->devicePxRatio = ratio;
}

static NVGcompositeOperationState nvg__compositeOperationState(int op)
{
    int sfactor, dfactor;

    if (op == NVG_SOURCE_OVER)
    {
        sfactor = NVG_ONE;
        dfactor = NVG_ONE_MINUS_SRC_ALPHA;
    }
    else if (op == NVG_SOURCE_IN)
    {
        sfactor = NVG_DST_ALPHA;
        dfactor = NVG_ZERO;
    }
    else if (op == NVG_SOURCE_OUT)
    {
        sfactor = NVG_ONE_MINUS_DST_ALPHA;
        dfactor = NVG_ZERO;
    }
    else if (op == NVG_ATOP)
    {
        sfactor = NVG_DST_ALPHA;
        dfactor = NVG_ONE_MINUS_SRC_ALPHA;
    }
    else if (op == NVG_DESTINATION_OVER)
    {
        sfactor = NVG_ONE_MINUS_DST_ALPHA;
        dfactor = NVG_ONE;
    }
    else if (op == NVG_DESTINATION_IN)
    {
        sfactor = NVG_ZERO;
        dfactor = NVG_SRC_ALPHA;
    }
    else if (op == NVG_DESTINATION_OUT)
    {
        sfactor = NVG_ZERO;
        dfactor = NVG_ONE_MINUS_SRC_ALPHA;
    }
    else if (op == NVG_DESTINATION_ATOP)
    {
        sfactor = NVG_ONE_MINUS_DST_ALPHA;
        dfactor = NVG_SRC_ALPHA;
    }
    else if (op == NVG_LIGHTER)
    {
        sfactor = NVG_ONE;
        dfactor = NVG_ONE;
    }
    else if (op == NVG_COPY)
    {
        sfactor = NVG_ONE;
        dfactor = NVG_ZERO;
    }
    else if (op == NVG_XOR)
    {
        sfactor = NVG_ONE_MINUS_DST_ALPHA;
        dfactor = NVG_ONE_MINUS_SRC_ALPHA;
    }
    else
    {
        sfactor = NVG_ONE;
        dfactor = NVG_ZERO;
    }

    NVGcompositeOperationState state;
    state.srcRGB = sfactor;
    state.dstRGB = dfactor;
    state.srcAlpha = sfactor;
    state.dstAlpha = dfactor;
    return state;
}

static NVGstate *nvg__getState(NVGcontext *ctx)
{
    return &ctx->states[ctx->nstates - 1];
}

NVGcontext *nvgCreateInternal(NVGparams *params)
{
    NVGcontext *ctx = (NVGcontext *)NANOVG_MALLOC(sizeof(NVGcontext));
    int i;
    if (ctx == NULL) { goto error; }
    memset(ctx, 0, sizeof(NVGcontext));

    ctx->params = *params;
    for (i = 0; i < NVG_MAX_FONTIMAGES; i++)
    {
        ctx->fontImages[i] = 0;
    }

    ctx->commands = (float *)NANOVG_MALLOC(sizeof(float) * NVG_INIT_COMMANDS_SIZE);
    if (!ctx->commands) { goto error; }
    ctx->ncommands = 0;
    ctx->ccommands = NVG_INIT_COMMANDS_SIZE;

    ctx->cache = nvg__allocPathCache();
    if (ctx->cache == NULL) { goto error; }

    nvgSave(ctx);
    nvgReset(ctx);

    nvg__setDevicePixelRatio(ctx, 1.0f);

    if (ctx->params.renderCreate(ctx->params.userPtr) == 0) { goto error; }

    return ctx;

error:
    nvgDeleteInternal(ctx);
    return 0;
}

NVGparams *nvgInternalParams(NVGcontext *ctx)
{
    return &ctx->params;
}

void nvgDeleteInternal(NVGcontext *ctx)
{
    int i;
    if (ctx == NULL) { return; }
    if (ctx->commands != NULL) { NANOVG_FREE(ctx->commands); }
    if (ctx->cache != NULL) { nvg__deletePathCache(ctx->cache); }

    (void)i;


    if (ctx->params.renderDelete != NULL)
    {
        ctx->params.renderDelete(ctx->params.userPtr);
    }

    NANOVG_FREE(ctx);
}

void nvgBeginFrame(NVGcontext *ctx, float windowWidth, float windowHeight, float devicePixelRatio)
{
    ctx->nstates = 0;
    nvgSave(ctx);
    nvgReset(ctx);

    nvg__setDevicePixelRatio(ctx, devicePixelRatio);

    ctx->params.renderViewport(ctx->params.userPtr, windowWidth, windowHeight, devicePixelRatio);

    ctx->drawCallCount = 0;
    ctx->fillTriCount = 0;
    ctx->strokeTriCount = 0;
    ctx->textTriCount = 0;
}

void nvgCancelFrame(NVGcontext *ctx)
{
    ctx->params.renderCancel(ctx->params.userPtr);
}

void nvgEndFrame(NVGcontext *ctx)
{
    ctx->params.renderFlush(ctx->params.userPtr);
    if (ctx->fontImageIdx != 0)
    {
        int fontImage = ctx->fontImages[ctx->fontImageIdx];
        int i, j, iw, ih;
        // delete images that smaller than current one
        if (fontImage == 0)
        {
            return;
        }
        nvgImageSize(ctx, fontImage, &iw, &ih);
        for (i = j = 0; i < ctx->fontImageIdx; i++)
        {
            if (ctx->fontImages[i] != 0)
            {
                int nw, nh;
                nvgImageSize(ctx, ctx->fontImages[i], &nw, &nh);
                if (nw < iw || nh < ih)
                {
                    nvgDeleteImage(ctx, ctx->fontImages[i]);
                }
                else
                {
                    ctx->fontImages[j++] = ctx->fontImages[i];
                }
            }
        }
        // make current font image to first
        ctx->fontImages[j++] = ctx->fontImages[0];
        ctx->fontImages[0] = fontImage;
        ctx->fontImageIdx = 0;
        // clear all images after j
        for (i = j; i < NVG_MAX_FONTIMAGES; i++)
        {
            ctx->fontImages[i] = 0;
        }
    }
}

NVGcolor nvgRGB(unsigned char r, unsigned char g, unsigned char b)
{
    return nvgRGBA(r, g, b, 255);
}

NVGcolor nvgRGBf(float r, float g, float b)
{
    return nvgRGBAf(r, g, b, 1.0f);
}

NVGcolor nvgRGBA(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
    NVGcolor color;
    // Use longer initialization to suppress warning.
    color.r = r / 255.0f;
    color.g = g / 255.0f;
    color.b = b / 255.0f;
    color.a = a / 255.0f;
    return color;
}

NVGcolor nvgRGBAf(float r, float g, float b, float a)
{
    NVGcolor color;
    // Use longer initialization to suppress warning.
    color.r = r;
    color.g = g;
    color.b = b;
    color.a = a;
    return color;
}

NVGcolor nvgTransRGBA(NVGcolor c, unsigned char a)
{
    c.a = a / 255.0f;
    return c;
}

NVGcolor nvgTransRGBAf(NVGcolor c, float a)
{
    c.a = a;
    return c;
}

NVGcolor nvgLerpRGBA(NVGcolor c0, NVGcolor c1, float u)
{
    int i;
    float oneminu;
    NVGcolor cint = {{{0}}};

    u = nvg__clampf(u, 0.0f, 1.0f);
    oneminu = 1.0f - u;
    for (i = 0; i < 4; i++)
    {
        cint.rgba[i] = c0.rgba[i] * oneminu + c1.rgba[i] * u;
    }

    return cint;
}

NVGcolor nvgHSL(float h, float s, float l)
{
    return nvgHSLA(h, s, l, 255);
}

static float nvg__hue(float h, float m1, float m2)
{
    if (h < 0) { h += 1; }
    if (h > 1) { h -= 1; }
    if (h < 1.0f / 6.0f)
    {
        return m1 + (m2 - m1) * h * 6.0f;
    }
    else if (h < 3.0f / 6.0f)
    {
        return m2;
    }
    else if (h < 4.0f / 6.0f)
    {
        return m1 + (m2 - m1) * (2.0f / 3.0f - h) * 6.0f;
    }
    return m1;
}

NVGcolor nvgHSLA(float h, float s, float l, unsigned char a)
{
    float m1, m2;
    NVGcolor col;
    h = nvg__modf(h, 1.0f);
    if (h < 0.0f) { h += 1.0f; }
    s = nvg__clampf(s, 0.0f, 1.0f);
    l = nvg__clampf(l, 0.0f, 1.0f);
    m2 = l <= 0.5f ? (l * (1 + s)) : (l + s - l * s);
    m1 = 2 * l - m2;
    col.r = nvg__clampf(nvg__hue(h + 1.0f / 3.0f, m1, m2), 0.0f, 1.0f);
    col.g = nvg__clampf(nvg__hue(h, m1, m2), 0.0f, 1.0f);
    col.b = nvg__clampf(nvg__hue(h - 1.0f / 3.0f, m1, m2), 0.0f, 1.0f);
    col.a = a / 255.0f;
    return col;
}

void nvgTransformIdentity(float *t)
{
    t[0] = 1.0f; t[1] = 0.0f;
    t[2] = 0.0f; t[3] = 1.0f;
    t[4] = 0.0f; t[5] = 0.0f;
    t[6] = 0.0f; t[7] = 0.0f; t[8] = 1.0f;
}

void nvgTransformTranslate(float *t, float tx, float ty)
{
    t[0] = 1.0f; t[1] = 0.0f;
    t[2] = 0.0f; t[3] = 1.0f;
    t[4] = tx; t[5] = ty;
    t[6] = 0.0f; t[7] = 0.0f; t[8] = 1.0f;
}

void nvgTransformScale(float *t, float sx, float sy)
{
    t[0] = sx; t[1] = 0.0f;
    t[2] = 0.0f; t[3] = sy;
    t[4] = 0.0f; t[5] = 0.0f;
    t[6] = 0.0f; t[7] = 0.0f; t[8] = 1.0f;
}

void nvgTransformRotate(float *t, float a)
{
    float cs = nvg__cosf(a), sn = nvg__sinf(a);
    t[0] = cs; t[1] = sn;
    t[2] = -sn; t[3] = cs;
    t[4] = 0.0f; t[5] = 0.0f;
    t[6] = 0.0f; t[7] = 0.0f; t[8] = 1.0f;
}

void nvgTransformSkewX(float *t, float a)
{
    t[0] = 1.0f; t[1] = 0.0f;
    t[2] = nvg__tanf(a); t[3] = 1.0f;
    t[4] = 0.0f; t[5] = 0.0f;
    t[6] = 0.0f; t[7] = 0.0f; t[8] = 1.0f;
}

void nvgTransformSkewY(float *t, float a)
{
    t[0] = 1.0f; t[1] = nvg__tanf(a);
    t[2] = 0.0f; t[3] = 1.0f;
    t[4] = 0.0f; t[5] = 0.0f;
    t[6] = 0.0f; t[7] = 0.0f; t[8] = 1.0f;
}

void nvgTransformMultiply(float *t, const float *s)
{
    float t0 = t[0] * s[0] + t[1] * s[2] + t[6] * s[4];
    float t2 = t[2] * s[0] + t[3] * s[2] + t[7] * s[4];
    float t4 = t[4] * s[0] + t[5] * s[2] + t[8] * s[4];
    float t1 = t[0] * s[1] + t[1] * s[3] + t[6] * s[5];
    float t3 = t[2] * s[1] + t[3] * s[3] + t[7] * s[5];
    float t5 = t[4] * s[1] + t[5] * s[3] + t[8] * s[5];

    t[6] = t[0] * s[6] + t[1] * s[7] + t[6] * s[8];
    t[7] = t[2] * s[6] + t[3] * s[7] + t[7] * s[8];
    t[8] = t[4] * s[6] + t[5] * s[7] + t[8] * s[8];

    t[0] = t0;
    t[1] = t1;
    t[2] = t2;
    t[3] = t3;
    t[4] = t4;
    t[5] = t5;
}

void nvgTransformPremultiply(float *t, const float *s)
{
    float s2[9];
    memcpy(s2, s, sizeof(float) * 9);
    nvgTransformMultiply(s2, t);
    memcpy(t, s2, sizeof(float) * 9);
}

int nvgTransformInverse(float *inv, const float *t)
{
    float invdet, det = t[0] * t[3] - t[2] * t[1];
    if (det > -1e-6f && det < 1e-6f)
    {
        nvgTransformIdentity(inv);
        return 0;
    }
    invdet = 1.0f / det;
    inv[0] = t[3] * invdet;
    inv[2] = -t[2] * invdet;
    inv[4] = (t[2] * t[5] - t[3] * t[4]) * invdet;
    inv[1] = -t[1] * invdet;
    inv[3] = t[0] * invdet;
    inv[5] = (t[1] * t[4] - t[0] * t[5]) * invdet;
    return 1;
}

void nvgTransformPoint(float *dx, float *dy, const float *t, float sx, float sy)
{
    float w = sx * t[6] + sy * t[7] + t[8];
    *dx = (sx * t[0] + sy * t[2] + t[4]) / w;
    *dy = (sx * t[1] + sy * t[3] + t[5]) / w;
}

float nvgDegToRad(float deg)
{
    return deg / 180.0f * NVG_PI;
}

float nvgRadToDeg(float rad)
{
    return rad / NVG_PI * 180.0f;
}

static void nvg__setPaintColor(NVGpaint *p, NVGcolor color)
{
    memset(p, 0, sizeof(*p));
    nvgTransformIdentity(p->xform);
    p->radius = 0.0f;
    p->feather = 1.0f;
    p->innerColor = color;
    p->outerColor = color;
}


// State handling
void nvgSave(NVGcontext *ctx)
{
    if (ctx->nstates >= NVG_MAX_STATES)
    {
        return;
    }
    if (ctx->nstates > 0)
    {
        memcpy(&ctx->states[ctx->nstates], &ctx->states[ctx->nstates - 1], sizeof(NVGstate));
    }
    ctx->nstates++;
}

void nvgRestore(NVGcontext *ctx)
{
    if (ctx->nstates <= 1)
    {
        return;
    }
    ctx->nstates--;
}

void nvgReset(NVGcontext *ctx)
{
    NVGstate *state = nvg__getState(ctx);
    memset(state, 0, sizeof(*state));

    nvg__setPaintColor(&state->fill, nvgRGBA(255, 255, 255, 255));
    nvg__setPaintColor(&state->stroke, nvgRGBA(0, 0, 0, 255));
    state->compositeOperation = nvg__compositeOperationState(NVG_SOURCE_OVER);
    state->shapeAntiAlias = 1;
    state->strokeWidth = 1.0f;
    state->miterLimit = 10.0f;
    state->lineCap = NVG_BUTT;
    state->lineJoin = NVG_MITER;
    state->alpha = 1.0f;
    nvgTransformIdentity(state->xform);

    state->scissor.extent[0] = -1.0f;
    state->scissor.extent[1] = -1.0f;

    state->fontSize = 16.0f;
    state->letterSpacing = 0.0f;
    state->lineHeight = 1.0f;
    state->fontBlur = 0.0f;
    state->textAlign = NVG_ALIGN_LEFT | NVG_ALIGN_BASELINE;
    state->fontId = 0;
}

// State setting
void nvgShapeAntiAlias(NVGcontext *ctx, int enabled)
{
    NVGstate *state = nvg__getState(ctx);
    state->shapeAntiAlias = enabled;
}

void nvgStrokeWidth(NVGcontext *ctx, float width)
{
    NVGstate *state = nvg__getState(ctx);
    state->strokeWidth = width;
}

void nvgMiterLimit(NVGcontext *ctx, float limit)
{
    NVGstate *state = nvg__getState(ctx);
    state->miterLimit = limit;
}

void nvgLineCap(NVGcontext *ctx, int cap)
{
    NVGstate *state = nvg__getState(ctx);
    state->lineCap = cap;
    if (ctx->params.setLineCap != NULL)
    {
        ctx->params.setLineCap(ctx->params.userPtr, cap);
    }
}

void nvgLineJoin(NVGcontext *ctx, int join)
{
    NVGstate *state = nvg__getState(ctx);
    state->lineJoin = join;
}

void nvgGlobalAlpha(NVGcontext *ctx, float alpha)
{
    NVGstate *state = nvg__getState(ctx);
    state->alpha = alpha;
}

void nvgTransformxyz(NVGcontext *ctx, float a, float b, float c, float d, float e, float f, float g,
                     float h, float i)
{
    NVGstate *state = nvg__getState(ctx);
    float t[9] = { a, b, c, d, e, f, g, h, i};
    nvgTransformPremultiply(state->xform, t);
}

void nvgTransform(NVGcontext *ctx, float a, float b, float c, float d, float e, float f)
{
    NVGstate *state = nvg__getState(ctx);
    float t[9] = { a, b, c, d, e, f, 0, 0, 1};
    nvgTransformPremultiply(state->xform, t);
}

void nvgResetTransform(NVGcontext *ctx)
{
    NVGstate *state = nvg__getState(ctx);
    nvgTransformIdentity(state->xform);
}

void nvgTranslate(NVGcontext *ctx, float x, float y)
{
    NVGstate *state = nvg__getState(ctx);
    float t[9];
    nvgTransformTranslate(t, x, y);
    nvgTransformPremultiply(state->xform, t);
}

void nvgRotate(NVGcontext *ctx, float angle)
{
    NVGstate *state = nvg__getState(ctx);
    float t[9];
    nvgTransformRotate(t, angle);
    nvgTransformPremultiply(state->xform, t);
}

void nvgSkewX(NVGcontext *ctx, float angle)
{
    NVGstate *state = nvg__getState(ctx);
    float t[9];
    nvgTransformSkewX(t, angle);
    nvgTransformPremultiply(state->xform, t);
}

void nvgSkewY(NVGcontext *ctx, float angle)
{
    NVGstate *state = nvg__getState(ctx);
    float t[9];
    nvgTransformSkewY(t, angle);
    nvgTransformPremultiply(state->xform, t);
}

void nvgScale(NVGcontext *ctx, float x, float y)
{
    NVGstate *state = nvg__getState(ctx);
    float t[9];
    nvgTransformScale(t, x, y);
    nvgTransformPremultiply(state->xform, t);
}

void nvgCurrentTransform(NVGcontext *ctx, float *xform)
{
    NVGstate *state = nvg__getState(ctx);
    if (xform == NULL) { return; }
    memcpy(xform, state->xform, sizeof(float) * 6);
}

void nvgStrokeColor(NVGcontext *ctx, NVGcolor color)
{
    NVGstate *state = nvg__getState(ctx);
    nvg__setPaintColor(&state->stroke, color);
}

void nvgStrokePaint(NVGcontext *ctx, NVGpaint paint)
{
    NVGstate *state = nvg__getState(ctx);
    state->stroke = paint;
    nvgTransformMultiply(state->stroke.xform, state->xform);
}

void nvgFillColor(NVGcontext *ctx, NVGcolor color)
{
    NVGstate *state = nvg__getState(ctx);
    nvg__setPaintColor(&state->fill, color);
}

void nvgFillPaint(NVGcontext *ctx, NVGpaint paint)
{
    NVGstate *state = nvg__getState(ctx);
    state->fill = paint;
    nvgTransformMultiply(state->fill.xform, state->xform);
}

int nvgCreateImage(NVGcontext *ctx, const char *filename, int imageFlags)
{
    (void)ctx; (void)filename; (void)imageFlags;
    return -1;
}

int nvgCreateImageMem(NVGcontext *ctx, int imageFlags, unsigned char *data, int ndata)
{
    (void)ctx; (void)data; (void)ndata; (void)imageFlags;
    return -1;
}


int nvgCreateImageRGBA(NVGcontext *ctx, int w, int h, int imageFlags, const unsigned char *data)
{
    return ctx->params.renderCreateTexture(ctx->params.userPtr, NVG_TEXTURE_RGBA, w, h, imageFlags,
                                           data);
}

void nvgUpdateImage(NVGcontext *ctx, int image, const unsigned char *data)
{
    int w, h;
    ctx->params.renderGetTextureSize(ctx->params.userPtr, image, &w, &h);
    ctx->params.renderUpdateTexture(ctx->params.userPtr, image, 0, 0, w, h, data);
}

void nvgImageSize(NVGcontext *ctx, int image, int *w, int *h)
{
    ctx->params.renderGetTextureSize(ctx->params.userPtr, image, w, h);
}

void nvgDeleteImage(NVGcontext *ctx, int image)
{
    ctx->params.renderDeleteTexture(ctx->params.userPtr, image);
}

NVGpaint nvgLinearGradient(NVGcontext *ctx,
                           float sx, float sy, float ex, float ey,
                           NVGcolor icol, NVGcolor ocol)
{
    NVGpaint p;
    float dx, dy, d;
    const float large = 1e5;
    NVG_NOTUSED(ctx);
    memset(&p, 0, sizeof(p));

    // Calculate transform aligned to the line
    dx = ex - sx;
    dy = ey - sy;
    d = sqrtf(dx * dx + dy * dy);
    if (d > 0.0001f)
    {
        dx /= d;
        dy /= d;
    }
    else
    {
        dx = 0;
        dy = 1;
    }
    nvgTransformIdentity(p.xform);
    p.xform[0] = dy; p.xform[1] = -dx;
    p.xform[2] = dx; p.xform[3] = dy;
    p.xform[4] = sx - dx * large; p.xform[5] = sy - dy * large;

    p.extent[0] = large;
    p.extent[1] = large + d * 0.5f;

    p.radius = 0.0f;

    p.feather = nvg__maxf(1.0f, d);

    p.innerColor = icol;
    p.outerColor = ocol;

    return p;
}

NVGpaint nvgRadialGradient(NVGcontext *ctx,
                           float cx, float cy, float inr, float outr,
                           NVGcolor icol, NVGcolor ocol)
{
    NVGpaint p;
    float r = (inr + outr) * 0.5f;
    float f = (outr - inr);
    NVG_NOTUSED(ctx);
    memset(&p, 0, sizeof(p));

    nvgTransformIdentity(p.xform);
    p.xform[4] = cx;
    p.xform[5] = cy;

    p.extent[0] = r;
    p.extent[1] = r;

    p.radius = r;

    p.feather = nvg__maxf(1.0f, f);

    p.innerColor = icol;
    p.outerColor = ocol;

    return p;
}

NVGpaint nvgBoxGradient(NVGcontext *ctx,
                        float x, float y, float w, float h, float r, float f,
                        NVGcolor icol, NVGcolor ocol)
{
    NVGpaint p;
    NVG_NOTUSED(ctx);
    memset(&p, 0, sizeof(p));

    nvgTransformIdentity(p.xform);
    p.xform[4] = x + w * 0.5f;
    p.xform[5] = y + h * 0.5f;

    p.extent[0] = w * 0.5f;
    p.extent[1] = h * 0.5f;

    p.radius = r;

    p.feather = nvg__maxf(1.0f, f);

    p.innerColor = icol;
    p.outerColor = ocol;

    return p;
}


NVGpaint nvgImagePattern(NVGcontext *ctx,
                         float cx, float cy, float w, float h, float angle,
                         int image, float alpha)
{
    NVGpaint p;
    NVG_NOTUSED(ctx);
    memset(&p, 0, sizeof(p));

    nvgTransformRotate(p.xform, angle);
    p.xform[4] = cx;
    p.xform[5] = cy;

    p.extent[0] = w;
    p.extent[1] = h;

    p.image = image;

    p.innerColor = p.outerColor = nvgRGBAf(1, 1, 1, alpha);

    return p;
}

// Scissoring
void nvgScissor(NVGcontext *ctx, float x, float y, float w, float h)
{
    NVGstate *state = nvg__getState(ctx);

    w = nvg__maxf(0.0f, w);
    h = nvg__maxf(0.0f, h);

    nvgTransformIdentity(state->scissor.xform);
    state->scissor.xform[4] = x + w * 0.5f;
    state->scissor.xform[5] = y + h * 0.5f;
    nvgTransformMultiply(state->scissor.xform, state->xform);

    state->scissor.extent[0] = w * 0.5f;
    state->scissor.extent[1] = h * 0.5f;
}

static void nvg__isectRects(float *dst,
                            float ax, float ay, float aw, float ah,
                            float bx, float by, float bw, float bh)
{
    float minx = nvg__maxf(ax, bx);
    float miny = nvg__maxf(ay, by);
    float maxx = nvg__minf(ax + aw, bx + bw);
    float maxy = nvg__minf(ay + ah, by + bh);
    dst[0] = minx;
    dst[1] = miny;
    dst[2] = nvg__maxf(0.0f, maxx - minx);
    dst[3] = nvg__maxf(0.0f, maxy - miny);
}

void nvgIntersectScissor(NVGcontext *ctx, float x, float y, float w, float h)
{
    NVGstate *state = nvg__getState(ctx);
    float pxform[9], invxorm[9];
    float rect[4];
    float ex, ey, tex, tey;

    // If no previous scissor has been set, set the scissor as current scissor.
    if (state->scissor.extent[0] < 0)
    {
        nvgScissor(ctx, x, y, w, h);
        return;
    }

    // Transform the current scissor rect into current transform space.
    // If there is difference in rotation, this will be approximation.
    memcpy(pxform, state->scissor.xform, sizeof(float) * 9);
    ex = state->scissor.extent[0];
    ey = state->scissor.extent[1];
    nvgTransformInverse(invxorm, state->xform);
    nvgTransformMultiply(pxform, invxorm);
    tex = ex * nvg__absf(pxform[0]) + ey * nvg__absf(pxform[2]);
    tey = ex * nvg__absf(pxform[1]) + ey * nvg__absf(pxform[3]);

    // Intersect rects.
    nvg__isectRects(rect, pxform[4] - tex, pxform[5] - tey, tex * 2, tey * 2, x, y, w, h);

    nvgScissor(ctx, rect[0], rect[1], rect[2], rect[3]);
}

void nvgResetScissor(NVGcontext *ctx)
{
    NVGstate *state = nvg__getState(ctx);
    memset(state->scissor.xform, 0, sizeof(state->scissor.xform));
    state->scissor.extent[0] = -1.0f;
    state->scissor.extent[1] = -1.0f;
}

// Global composite operation.
void nvgGlobalCompositeOperation(NVGcontext *ctx, int op)
{
    NVGstate *state = nvg__getState(ctx);
    state->compositeOperation = nvg__compositeOperationState(op);
}

void nvgGlobalCompositeBlendFunc(NVGcontext *ctx, int sfactor, int dfactor)
{
    nvgGlobalCompositeBlendFuncSeparate(ctx, sfactor, dfactor, sfactor, dfactor);
}

void nvgGlobalCompositeBlendFuncSeparate(NVGcontext *ctx, int srcRGB, int dstRGB, int srcAlpha,
                                         int dstAlpha)
{
    NVGcompositeOperationState op;
    op.srcRGB = srcRGB;
    op.dstRGB = dstRGB;
    op.srcAlpha = srcAlpha;
    op.dstAlpha = dstAlpha;

    NVGstate *state = nvg__getState(ctx);
    state->compositeOperation = op;
}

static int nvg__ptEquals(float x1, float y1, float x2, float y2, float tol)
{
    float dx = x2 - x1;
    float dy = y2 - y1;
    return dx * dx + dy * dy < tol * tol;
}

static float nvg__distPtSeg(float x, float y, float px, float py, float qx, float qy)
{
    float pqx, pqy, dx, dy, d, t;
    pqx = qx - px;
    pqy = qy - py;
    dx = x - px;
    dy = y - py;
    d = pqx * pqx + pqy * pqy;
    t = pqx * dx + pqy * dy;
    if (d > 0) { t /= d; }
    if (t < 0) { t = 0; }
    else if (t > 1) { t = 1; }
    dx = px + t * pqx - x;
    dy = py + t * pqy - y;
    return dx * dx + dy * dy;
}

static void nvg__appendCommands(NVGcontext *ctx, float *vals, int nvals)
{
    NVGstate *state = nvg__getState(ctx);
    int i;

    if (ctx->ncommands + nvals > ctx->ccommands)
    {
        float *commands;
        int ccommands = ctx->ncommands + nvals + ctx->ccommands / 2;
        commands = (float *)NANOVG_REALLOC(ctx->commands, sizeof(float) * ccommands);
        if (commands == NULL) { return; }
        ctx->commands = commands;
        ctx->ccommands = ccommands;
    }

    if ((int)vals[0] != NVG_CLOSE && (int)vals[0] != NVG_WINDING)
    {
        ctx->commandx = vals[nvals - 2];
        ctx->commandy = vals[nvals - 1];
    }

    // transform commands
    i = 0;
    while (i < nvals)
    {
        int cmd = (int)vals[i];
        switch (cmd)
        {
        case NVG_MOVETO:
            nvgTransformPoint(&vals[i + 1], &vals[i + 2], state->xform, vals[i + 1], vals[i + 2]);
            i += 3;
            break;
        case NVG_LINETO:
            nvgTransformPoint(&vals[i + 1], &vals[i + 2], state->xform, vals[i + 1], vals[i + 2]);
            i += 3;
            break;
        case NVG_BEZIERTO:
            nvgTransformPoint(&vals[i + 1], &vals[i + 2], state->xform, vals[i + 1], vals[i + 2]);
            nvgTransformPoint(&vals[i + 3], &vals[i + 4], state->xform, vals[i + 3], vals[i + 4]);
            nvgTransformPoint(&vals[i + 5], &vals[i + 6], state->xform, vals[i + 5], vals[i + 6]);
            i += 7;
            break;
        case NVG_CLOSE:
            i++;
            break;
        case NVG_WINDING:
            i += 2;
            break;
        default:
            i++;
        }
    }

    memcpy(&ctx->commands[ctx->ncommands], vals, nvals * sizeof(float));

    ctx->ncommands += nvals;
}


static void nvg__clearPathCache(NVGcontext *ctx)
{
    ctx->cache->npoints = 0;
    ctx->cache->npaths = 0;
}

static NVGpath *nvg__lastPath(NVGcontext *ctx)
{
    if (ctx->cache->npaths > 0)
    {
        return &ctx->cache->paths[ctx->cache->npaths - 1];
    }
    return NULL;
}

static void nvg__addPath(NVGcontext *ctx)
{
    NVGpath *path;
    if (ctx->cache->npaths + 1 > ctx->cache->cpaths)
    {
        NVGpath *paths;
        int cpaths = ctx->cache->npaths + 1 + ctx->cache->cpaths / 2;
        paths = (NVGpath *)NANOVG_REALLOC(ctx->cache->paths, sizeof(NVGpath) * cpaths);
        if (paths == NULL) { return; }
        ctx->cache->paths = paths;
        ctx->cache->cpaths = cpaths;
    }
    path = &ctx->cache->paths[ctx->cache->npaths];
    memset(path, 0, sizeof(*path));
    path->first = ctx->cache->npoints;
    path->winding = NVG_CCW;

    ctx->cache->npaths++;
}

static NVGpoint *nvg__lastPoint(NVGcontext *ctx)
{
    if (ctx->cache->npoints > 0)
    {
        return &ctx->cache->points[ctx->cache->npoints - 1];
    }
    return NULL;
}

static void nvg__addPoint(NVGcontext *ctx, float x, float y, int flags)
{
    NVGpath *path = nvg__lastPath(ctx);
    NVGpoint *pt;
    if (path == NULL) { return; }

    if (path->count > 0 && ctx->cache->npoints > 0)
    {
        pt = nvg__lastPoint(ctx);
        if (nvg__ptEquals(pt->x, pt->y, x, y, ctx->distTol))
        {
            pt->flags |= flags;
            return;
        }
    }

    if (ctx->cache->npoints + 1 > ctx->cache->cpoints)
    {
        NVGpoint *points;
        int cpoints = ctx->cache->npoints + 1 + ctx->cache->cpoints / 2;
        points = (NVGpoint *)NANOVG_REALLOC(ctx->cache->points, sizeof(NVGpoint) * cpoints);
        if (points == NULL) { return; }
        ctx->cache->points = points;
        ctx->cache->cpoints = cpoints;
    }

    pt = &ctx->cache->points[ctx->cache->npoints];
    memset(pt, 0, sizeof(*pt));
    pt->x = x;
    pt->y = y;
    pt->flags = (unsigned char)flags;

    ctx->cache->npoints++;
    path->count++;
}

static void nvg__closePath(NVGcontext *ctx)
{
    NVGpath *path = nvg__lastPath(ctx);
    if (path == NULL) { return; }
    path->closed = 1;
}

static void nvg__pathWinding(NVGcontext *ctx, int winding)
{
    NVGpath *path = nvg__lastPath(ctx);
    if (path == NULL) { return; }
    path->winding = winding;
}

static float nvg__getAverageScale(float *t)
{
    float sx = sqrtf(t[0] * t[0] + t[2] * t[2]);
    float sy = sqrtf(t[1] * t[1] + t[3] * t[3]);
    return (sx + sy) * 0.5f;
}

static NVGvertex *nvg__allocTempVerts(NVGcontext *ctx, int nverts)
{
    if (nverts > ctx->cache->cverts)
    {
        NVGvertex *verts;
        int cverts = (nverts + 0xff) &
                     ~0xff; // Round up to prevent allocations when things change just slightly.
        verts = (NVGvertex *)NANOVG_REALLOC(ctx->cache->verts, sizeof(NVGvertex) * cverts);
        if (verts == NULL) { return NULL; }
        ctx->cache->verts = verts;
        ctx->cache->cverts = cverts;
    }

    return ctx->cache->verts;
}

static float nvg__triarea2(float ax, float ay, float bx, float by, float cx, float cy)
{
    float abx = bx - ax;
    float aby = by - ay;
    float acx = cx - ax;
    float acy = cy - ay;
    return acx * aby - abx * acy;
}

static float nvg__polyArea(NVGpoint *pts, int npts)
{
    int i;
    float area = 0;
    for (i = 2; i < npts; i++)
    {
        NVGpoint *a = &pts[0];
        NVGpoint *b = &pts[i - 1];
        NVGpoint *c = &pts[i];
        area += nvg__triarea2(a->x, a->y, b->x, b->y, c->x, c->y);
    }
    return area * 0.5f;
}

static void nvg__polyReverse(NVGpoint *pts, int npts)
{
    NVGpoint tmp;
    int i = 0, j = npts - 1;
    while (i < j)
    {
        tmp = pts[i];
        pts[i] = pts[j];
        pts[j] = tmp;
        i++;
        j--;
    }
}


static void nvg__vset(NVGvertex *vtx, float x, float y, float u, float v)
{
    vtx->x = x;
    vtx->y = y;
    vtx->u = u;
    vtx->v = v;
}

static void nvg__tesselateBezier(NVGcontext *ctx,
                                 float x1, float y1, float x2, float y2,
                                 float x3, float y3, float x4, float y4,
                                 int level, int type)
{
    float x12, y12, x23, y23, x34, y34, x123, y123, x234, y234, x1234, y1234;
    float dx, dy, d2, d3;

    if (level > 10) { return; }

    x12 = (x1 + x2) * 0.5f;
    y12 = (y1 + y2) * 0.5f;
    x23 = (x2 + x3) * 0.5f;
    y23 = (y2 + y3) * 0.5f;
    x34 = (x3 + x4) * 0.5f;
    y34 = (y3 + y4) * 0.5f;
    x123 = (x12 + x23) * 0.5f;
    y123 = (y12 + y23) * 0.5f;

    dx = x4 - x1;
    dy = y4 - y1;
    d2 = nvg__absf(((x2 - x4) * dy - (y2 - y4) * dx));
    d3 = nvg__absf(((x3 - x4) * dy - (y3 - y4) * dx));

    if ((d2 + d3) * (d2 + d3) < ctx->tessTol * (dx * dx + dy * dy))
    {
        nvg__addPoint(ctx, x4, y4, type);
        return;
    }

    /*  if (nvg__absf(x1+x3-x2-x2) + nvg__absf(y1+y3-y2-y2) + nvg__absf(x2+x4-x3-x3) + nvg__absf(y2+y4-y3-y3) < ctx->tessTol) {
            nvg__addPoint(ctx, x4, y4, type);
            return;
        }*/

    x234 = (x23 + x34) * 0.5f;
    y234 = (y23 + y34) * 0.5f;
    x1234 = (x123 + x234) * 0.5f;
    y1234 = (y123 + y234) * 0.5f;

    nvg__tesselateBezier(ctx, x1, y1, x12, y12, x123, y123, x1234, y1234, level + 1, 0);
    nvg__tesselateBezier(ctx, x1234, y1234, x234, y234, x34, y34, x4, y4, level + 1, type);
}

static void nvg__flattenPaths(NVGcontext *ctx)
{
    NVGpathCache *cache = ctx->cache;
//  NVGstate* state = nvg__getState(ctx);
    NVGpoint *last;
    NVGpoint *p0;
    NVGpoint *p1;
    NVGpoint *pts;
    NVGpath *path;
    int i, j;
    float *cp1;
    float *cp2;
    float *p;
    float area;

    if (cache->npaths > 0)
    {
        return;
    }

    // Flatten
    i = 0;
    while (i < ctx->ncommands)
    {
        int cmd = (int)ctx->commands[i];
        switch (cmd)
        {
        case NVG_MOVETO:
            nvg__addPath(ctx);
            p = &ctx->commands[i + 1];
            nvg__addPoint(ctx, p[0], p[1], NVG_PT_CORNER);
            i += 3;
            break;
        case NVG_LINETO:
            p = &ctx->commands[i + 1];
            nvg__addPoint(ctx, p[0], p[1], NVG_PT_CORNER);
            i += 3;
            break;
        case NVG_BEZIERTO:
            last = nvg__lastPoint(ctx);
            if (last != NULL)
            {
                cp1 = &ctx->commands[i + 1];
                cp2 = &ctx->commands[i + 3];
                p = &ctx->commands[i + 5];
                nvg__tesselateBezier(ctx, last->x, last->y, cp1[0], cp1[1], cp2[0], cp2[1], p[0], p[1], 0,
                                     NVG_PT_CORNER);
            }
            i += 7;
            break;
        case NVG_CLOSE:
            nvg__closePath(ctx);
            i++;
            break;
        case NVG_WINDING:
            nvg__pathWinding(ctx, (int)ctx->commands[i + 1]);
            i += 2;
            break;
        default:
            i++;
        }
    }

    cache->bounds[0] = cache->bounds[1] = 1e6f;
    cache->bounds[2] = cache->bounds[3] = -1e6f;

    // Calculate the direction and length of line segments.
    for (j = 0; j < cache->npaths; j++)
    {
        path = &cache->paths[j];
        pts = &cache->points[path->first];

        // If the first and last points are the same, remove the last, mark as closed path.
        p0 = &pts[path->count - 1];
        p1 = &pts[0];
        if (nvg__ptEquals(p0->x, p0->y, p1->x, p1->y, ctx->distTol))
        {
            path->count--;
            p0 = &pts[path->count - 1];
            path->closed = 1;
        }

        // Enforce winding.
        if (path->count > 2)
        {
            area = nvg__polyArea(pts, path->count);
            if (path->winding == NVG_CCW && area < 0.0f)
            {
                nvg__polyReverse(pts, path->count);
            }
            if (path->winding == NVG_CW && area > 0.0f)
            {
                nvg__polyReverse(pts, path->count);
            }
        }

        for (i = 0; i < path->count; i++)
        {
            // Calculate segment direction and length
            p0->dx = p1->x - p0->x;
            p0->dy = p1->y - p0->y;
            p0->len = nvg__normalize(&p0->dx, &p0->dy);
            // Update bounds
            cache->bounds[0] = nvg__minf(cache->bounds[0], p0->x);
            cache->bounds[1] = nvg__minf(cache->bounds[1], p0->y);
            cache->bounds[2] = nvg__maxf(cache->bounds[2], p0->x);
            cache->bounds[3] = nvg__maxf(cache->bounds[3], p0->y);
            // Advance
            p0 = p1++;
        }
    }
}

static int nvg__curveDivs(float r, float arc, float tol)
{
    float da = acosf(r / (r + tol)) * 2.0f;
    return nvg__maxi(2, (int)ceilf(arc / da));
}



static int nvg__expandStroke(NVGcontext *ctx, float w, float fringe, int lineCap, int lineJoin,
                             float miterLimit)
{
    (void)lineJoin;//not used
    (void)miterLimit;
    NVGpathCache *cache = ctx->cache;
    NVGvertex *verts;
    NVGvertex *dst;
    int cverts, i, j;
    float aa = fringe;//ctx->fringeWidth;
    float u0 = 0.0f, u1 = 1.0f;
    int ncap = nvg__curveDivs(w, NVG_PI, ctx->tessTol); // Calculate divisions per half circle.

    w += aa * 0.5f;

    // Disable the gradient used for antialiasing when antialiasing is not used.
    if (aa == 0.0f)
    {
        u0 = 0.5f;
        u1 = 0.5f;
    }

    // Calculate max vertex usage.
    cverts = 0;
    for (i = 0; i < cache->npaths; i++)
    {
        NVGpath *path = &cache->paths[i];
        int loop = (path->closed == 0) ? 0 : 1;
        cverts += (path->count + path->nbevel * 5 + 1) * 2; // plus one for loop
        if (loop == 0)
        {
            // space for caps
            if (lineCap == NVG_ROUND)
            {
                cverts += (ncap * 2 + 2) * 2;
            }
            else
            {
                cverts += (3 + 3) * 2;
            }
        }
    }

    verts = nvg__allocTempVerts(ctx, cverts);
    if (verts == NULL) { return 0; }

    for (i = 0; i < cache->npaths; i++)
    {
        NVGpath *path = &cache->paths[i];
        NVGpoint *pts = &cache->points[path->first];
        NVGpoint *p0;
        NVGpoint *p1;
        int s, e, loop;
        float dx = 0.0f;
        float dy = 0.0f;
        dx = dy;
        dy = dx;
        path->fill = 0;
        path->nfill = 0;

        // Calculate fringe or stroke
        loop = (path->closed == 0) ? 0 : 1;
        dst = verts;
        path->stroke = dst;

        if (loop)
        {
            // Looping
            p0 = &pts[path->count - 1];
            p1 = &pts[0];
            s = 0;
            e = path->count;
        }
        else
        {
            // Add cap
            p0 = &pts[0];
            p1 = &pts[1];
            s = 1;
            e = path->count - 1;
        }

        if (loop == 0)
        {
            // Add cap
            nvg__vset(dst, p0->x, p0->y, u0, 1); dst++;
        }

        for (j = s; j < e; ++j)
        {
            nvg__vset(dst, p1->x + (p1->dmx * w), p1->y + (p1->dmy * w), u0, 1); dst++;
            nvg__vset(dst, p1->x - (p1->dmx * w), p1->y - (p1->dmy * w), u1, 1); dst++;
            p0 = p1++;
        }

        if (loop)
        {
            // Loop it
            nvg__vset(dst, verts[0].x, verts[0].y, u0, 1); dst++;
            nvg__vset(dst, verts[1].x, verts[1].y, u1, 1); dst++;
        }
        else
        {
            // Add cap
            nvg__vset(dst, p1->x, p1->y, u0, 1); dst++;
        }

        path->nstroke = (int)(dst - verts);

        verts = dst;
    }

    return 1;
}

static int nvg__expandFill(NVGcontext *ctx, float w, int lineJoin, float miterLimit)
{
    (void)lineJoin;//not used
    (void)miterLimit;
    NVGpathCache *cache = ctx->cache;
    NVGvertex *verts;
    NVGvertex *dst;
    int cverts, convex, i, j;
    float aa = ctx->fringeWidth;
    int fringe = w > 0.0f;

    // Calculate max vertex usage.
    cverts = 0;
    for (i = 0; i < cache->npaths; i++)
    {
        NVGpath *path = &cache->paths[i];
        cverts += path->count + path->nbevel + 1;
        if (fringe)
        {
            cverts += (path->count + path->nbevel * 5 + 1) * 2;    // plus one for loop
        }
    }

    verts = nvg__allocTempVerts(ctx, cverts);
    if (verts == NULL) { return 0; }

    convex = cache->npaths == 1 && cache->paths[0].convex;

    for (i = 0; i < cache->npaths; i++)
    {
        NVGpath *path = &cache->paths[i];
        NVGpoint *pts = &cache->points[path->first];
        NVGpoint *p0 = NULL;
        NVGpoint *p1 = NULL;
        p0 = p1;
        p1 = p0;
        float rw, lw, woff;
        float ru, lu;

        // Calculate shape vertices.
        woff = 0.5f * aa;
        dst = verts;
        path->fill = dst;

        if (fringe)
        {
            // Looping
            p0 = &pts[path->count - 1];
            p1 = &pts[0];
            for (j = 0; j < path->count; ++j)
            {
                nvg__vset(dst, p1->x + (p1->dmx * woff), p1->y + (p1->dmy * woff), 0.5f, 1); dst++;
                p0 = p1++;
            }
        }
        else
        {
            for (j = 0; j < path->count; ++j)
            {
                nvg__vset(dst, pts[j].x, pts[j].y, 0.5f, 1);
                dst++;
            }
        }

        path->nfill = (int)(dst - verts);
        verts = dst;

        // Calculate fringe
        if (fringe)
        {
            lw = w + woff;
            rw = w - woff;
            lu = 0;
            ru = 1;
            dst = verts;
            path->stroke = dst;

            // Create only half a fringe for convex shapes so that
            // the shape can be rendered without stenciling.
            if (convex)
            {
                lw = woff;  // This should generate the same vertex as fill inset above.
                lu = 0.5f;  // Set outline fade at middle.
            }

            // Looping
            p0 = &pts[path->count - 1];
            p1 = &pts[0];

            for (j = 0; j < path->count; ++j)
            {
                nvg__vset(dst, p1->x + (p1->dmx * lw), p1->y + (p1->dmy * lw), lu, 1); dst++;
                nvg__vset(dst, p1->x - (p1->dmx * rw), p1->y - (p1->dmy * rw), ru, 1); dst++;
                p0 = p1++;
            }

            // Loop it
            nvg__vset(dst, verts[0].x, verts[0].y, lu, 1); dst++;
            nvg__vset(dst, verts[1].x, verts[1].y, ru, 1); dst++;

            path->nstroke = (int)(dst - verts);
            verts = dst;
        }
        else
        {
            path->stroke = NULL;
            path->nstroke = 0;
        }
    }

    return 1;
}


// Draw
void nvgBeginPath(NVGcontext *ctx)
{
    ctx->ncommands = 0;
    nvg__clearPathCache(ctx);
}

void nvgMoveTo(NVGcontext *ctx, float x, float y)
{
    float vals[] = { NVG_MOVETO, x, y };
    nvg__appendCommands(ctx, vals, NVG_COUNTOF(vals));
}

void nvgLineTo(NVGcontext *ctx, float x, float y)
{
    float vals[] = { NVG_LINETO, x, y };
    nvg__appendCommands(ctx, vals, NVG_COUNTOF(vals));
}

void nvgBezierTo(NVGcontext *ctx, float c1x, float c1y, float c2x, float c2y, float x, float y)
{
    float vals[] = { NVG_BEZIERTO, c1x, c1y, c2x, c2y, x, y };
    nvg__appendCommands(ctx, vals, NVG_COUNTOF(vals));
}

void nvgQuadTo(NVGcontext *ctx, float cx, float cy, float x, float y)
{
    float x0 = ctx->commandx;
    float y0 = ctx->commandy;
    float vals[] = { NVG_BEZIERTO,
                     x0 + 2.0f / 3.0f * (cx - x0), y0 + 2.0f / 3.0f * (cy - y0),
                     x + 2.0f / 3.0f * (cx - x), y + 2.0f / 3.0f * (cy - y),
                     x, y
                   };
    nvg__appendCommands(ctx, vals, NVG_COUNTOF(vals));
}

void nvgArcTo(NVGcontext *ctx, float x1, float y1, float x2, float y2, float radius)
{
    float x0 = ctx->commandx;
    float y0 = ctx->commandy;
    float dx0, dy0, dx1, dy1, a, d, cx, cy, a0, a1;
    int dir;

    if (ctx->ncommands == 0)
    {
        return;
    }

    // Handle degenerate cases.
    if (nvg__ptEquals(x0, y0, x1, y1, ctx->distTol) ||
        nvg__ptEquals(x1, y1, x2, y2, ctx->distTol) ||
        nvg__distPtSeg(x1, y1, x0, y0, x2, y2) < ctx->distTol * ctx->distTol ||
        radius < ctx->distTol)
    {
        nvgLineTo(ctx, x1, y1);
        return;
    }

    // Calculate tangential circle to lines (x0,y0)-(x1,y1) and (x1,y1)-(x2,y2).
    dx0 = x0 - x1;
    dy0 = y0 - y1;
    dx1 = x2 - x1;
    dy1 = y2 - y1;
    nvg__normalize(&dx0, &dy0);
    nvg__normalize(&dx1, &dy1);
    a = nvg__acosf(dx0 * dx1 + dy0 * dy1);
    d = radius / nvg__tanf(a / 2.0f);


    if (d > 10000.0f)
    {
        nvgLineTo(ctx, x1, y1);
        return;
    }

    if (nvg__cross(dx0, dy0, dx1, dy1) > 0.0f)
    {
        cx = x1 + dx0 * d + dy0 * radius;
        cy = y1 + dy0 * d + -dx0 * radius;
        a0 = nvg__atan2f(dx0, -dy0);
        a1 = nvg__atan2f(-dx1, dy1);
        dir = NVG_CW;
    }
    else
    {
        cx = x1 + dx0 * d + -dy0 * radius;
        cy = y1 + dy0 * d + dx0 * radius;
        a0 = nvg__atan2f(-dx0, dy0);
        a1 = nvg__atan2f(dx1, -dy1);
        dir = NVG_CCW;
    }

    nvgArc(ctx, cx, cy, radius, a0, a1, dir);
}

void nvgClosePath(NVGcontext *ctx)
{
    float vals[] = { NVG_CLOSE };
    nvg__appendCommands(ctx, vals, NVG_COUNTOF(vals));
}

void nvgPathWinding(NVGcontext *ctx, int dir)
{
    float vals[] = { NVG_WINDING, (float)dir };
    nvg__appendCommands(ctx, vals, NVG_COUNTOF(vals));
}

void nvgArc(NVGcontext *ctx, float cx, float cy, float r, float a0, float a1, int dir)
{
    float a = 0, da = 0, hda = 0, kappa = 0;
    float dx = 0, dy = 0, x = 0, y = 0, tanx = 0, tany = 0;
    float px = 0, py = 0, ptanx = 0, ptany = 0;
    float vals[3 + 5 * 7 + 100];
    int i, ndivs, nvals;
    int move = ctx->ncommands > 0 ? NVG_LINETO : NVG_MOVETO;

    // Clamp angles
    da = a1 - a0;
    if (dir == NVG_CW)
    {
        if (nvg__absf(da) >= NVG_PI * 2)
        {
            da = NVG_PI * 2;
        }
        else
        {
            while (da < 0.0f) { da += NVG_PI * 2; }
        }
    }
    else
    {
        if (nvg__absf(da) >= NVG_PI * 2)
        {
            da = -NVG_PI * 2;
        }
        else
        {
            while (da > 0.0f) { da -= NVG_PI * 2; }
        }
    }

    // Split arc into max 90 degree segments.
    ndivs = nvg__maxi(1, nvg__mini((int)(nvg__absf(da) / (NVG_PI * 0.5f) + 0.5f), 5));
    hda = (da / (float)ndivs) / 2.0f;
    kappa = nvg__absf(4.0f / 3.0f * (1.0f - nvg__cosf(hda)) / nvg__sinf(hda));

    if (dir == NVG_CCW)
    {
        kappa = -kappa;
    }

    nvals = 0;
    for (i = 0; i <= ndivs; i++)
    {
        a = a0 + da * (i / (float)ndivs);
        dx = nvg__cosf(a);
        dy = nvg__sinf(a);
        x = cx + dx * r;
        y = cy + dy * r;
        tanx = -dy * r * kappa;
        tany = dx * r * kappa;

        if (i == 0)
        {
            vals[nvals++] = (float)move;
            vals[nvals++] = x;
            vals[nvals++] = y;
        }
        else
        {
            vals[nvals++] = NVG_BEZIERTO;
            vals[nvals++] = px + ptanx;
            vals[nvals++] = py + ptany;
            vals[nvals++] = x - tanx;
            vals[nvals++] = y - tany;
            vals[nvals++] = x;
            vals[nvals++] = y;
        }
        px = x;
        py = y;
        ptanx = tanx;
        ptany = tany;
    }

    nvg__appendCommands(ctx, vals, nvals);
}

void nvgRect(NVGcontext *ctx, float x, float y, float w, float h)
{
    float vals[] =
    {
        NVG_MOVETO, x, y,
        NVG_LINETO, x, y + h,
        NVG_LINETO, x + w, y + h,
        NVG_LINETO, x + w, y,
        NVG_CLOSE
    };
    nvg__appendCommands(ctx, vals, NVG_COUNTOF(vals));
}

void nvgRoundedRect(NVGcontext *ctx, float x, float y, float w, float h, float r)
{
    nvgRoundedRectVarying(ctx, x, y, w, h, r, r, r, r);
}

void nvgRoundedRectVarying(NVGcontext *ctx, float x, float y, float w, float h, float radTopLeft,
                           float radTopRight, float radBottomRight, float radBottomLeft)
{
    if (radTopLeft < 0.1f && radTopRight < 0.1f && radBottomRight < 0.1f && radBottomLeft < 0.1f)
    {
        nvgRect(ctx, x, y, w, h);
        return;
    }
    else
    {
        float halfw = nvg__absf(w) * 0.5f;
        float halfh = nvg__absf(h) * 0.5f;
        float rxBL = nvg__minf(radBottomLeft, halfw) * nvg__signf(w), ryBL = nvg__minf(radBottomLeft,
                                                                                       halfh) * nvg__signf(h);
        float rxBR = nvg__minf(radBottomRight, halfw) * nvg__signf(w), ryBR = nvg__minf(radBottomRight,
                                                                                        halfh) * nvg__signf(h);
        float rxTR = nvg__minf(radTopRight, halfw) * nvg__signf(w), ryTR = nvg__minf(radTopRight,
                                                                                     halfh) * nvg__signf(h);
        float rxTL = nvg__minf(radTopLeft, halfw) * nvg__signf(w), ryTL = nvg__minf(radTopLeft,
                                                                                    halfh) * nvg__signf(h);
        float vals[] =
        {
            NVG_MOVETO, x, y + ryTL,
            NVG_LINETO, x, y + h - ryBL,
            NVG_BEZIERTO, x, y + h - ryBL *(1 - NVG_KAPPA90), x + rxBL *(1 - NVG_KAPPA90), y + h, x + rxBL, y + h,
            NVG_LINETO, x + w - rxBR, y + h,
            NVG_BEZIERTO, x + w - rxBR *(1 - NVG_KAPPA90), y + h, x + w, y + h - ryBR *(1 - NVG_KAPPA90), x + w, y + h - ryBR,
            NVG_LINETO, x + w, y + ryTR,
            NVG_BEZIERTO, x + w, y + ryTR *(1 - NVG_KAPPA90), x + w - rxTR *(1 - NVG_KAPPA90), y, x + w - rxTR, y,
            NVG_LINETO, x + rxTL, y,
            NVG_BEZIERTO, x + rxTL *(1 - NVG_KAPPA90), y, x, y + ryTL *(1 - NVG_KAPPA90), x, y + ryTL,
            NVG_CLOSE
        };
        nvg__appendCommands(ctx, vals, NVG_COUNTOF(vals));
    }
}

void nvgEllipse(NVGcontext *ctx, float cx, float cy, float rx, float ry)
{
    float vals[] =
    {
        NVG_MOVETO, cx - rx, cy,
        NVG_BEZIERTO, cx - rx, cy + ry * NVG_KAPPA90, cx - rx * NVG_KAPPA90, cy + ry, cx, cy + ry,
        NVG_BEZIERTO, cx + rx * NVG_KAPPA90, cy + ry, cx + rx, cy + ry * NVG_KAPPA90, cx + rx, cy,
        NVG_BEZIERTO, cx + rx, cy - ry * NVG_KAPPA90, cx + rx * NVG_KAPPA90, cy - ry, cx, cy - ry,
        NVG_BEZIERTO, cx - rx * NVG_KAPPA90, cy - ry, cx - rx, cy - ry * NVG_KAPPA90, cx - rx, cy,
        NVG_CLOSE
    };
    nvg__appendCommands(ctx, vals, NVG_COUNTOF(vals));
}

void nvgCircle(NVGcontext *ctx, float cx, float cy, float r)
{
    nvgEllipse(ctx, cx, cy, r, r);
}



void (*patch_nvg_fill)(NVGcontext *ctx) = NULL;
void (*patch_nvg_stroke)(NVGcontext *ctx) = NULL;

void nvgFill(NVGcontext *ctx)
{
    if (patch_nvg_fill != NULL)
    {
        patch_nvg_fill(ctx);
        return;
    }
    NVGstate *state = nvg__getState(ctx);
    const NVGpath *path;
    NVGpaint fillPaint = state->fill;
    int i;

    nvg__flattenPaths(ctx);
    if (ctx->params.edgeAntiAlias && state->shapeAntiAlias)
    {
        nvg__expandFill(ctx, ctx->fringeWidth, NVG_MITER, 2.4f);
    }
    else
    {
        nvg__expandFill(ctx, 0.0f, NVG_MITER, 2.4f);
    }

    // Apply global alpha
    fillPaint.innerColor.a *= state->alpha;
    fillPaint.outerColor.a *= state->alpha;

    ctx->params.renderFill(ctx->params.userPtr, &fillPaint, state->compositeOperation, &state->scissor,
                           ctx->fringeWidth,
                           ctx->cache->bounds, ctx->cache->paths, ctx->cache->npaths);

    // Count triangles
    for (i = 0; i < ctx->cache->npaths; i++)
    {
        path = &ctx->cache->paths[i];
        ctx->fillTriCount += path->nfill - 2;
        ctx->fillTriCount += path->nstroke - 2;
        ctx->drawCallCount += 2;
    }
}



void nvgStroke(NVGcontext *ctx)
{
    if (patch_nvg_stroke != NULL)
    {
        patch_nvg_stroke(ctx);
        return;
    }

    NVGstate *state = nvg__getState(ctx);
    float scale = nvg__getAverageScale(state->xform);
    float strokeWidth = nvg__clampf(state->strokeWidth * scale, 0.0f, 200.0f);
    NVGpaint strokePaint = state->stroke;
    const NVGpath *path;
    int i;


    if (strokeWidth < ctx->fringeWidth)
    {
        // If the stroke width is less than pixel size, use alpha to emulate coverage.
        // Since coverage is area, scale by alpha*alpha.
        float alpha = nvg__clampf(strokeWidth / ctx->fringeWidth, 0.0f, 1.0f);
        strokePaint.innerColor.a *= alpha * alpha;
        strokePaint.outerColor.a *= alpha * alpha;
        strokeWidth = ctx->fringeWidth;
    }

    // Apply global alpha
    strokePaint.innerColor.a *= state->alpha;
    strokePaint.outerColor.a *= state->alpha;

    nvg__flattenPaths(ctx);

    if (ctx->params.edgeAntiAlias && state->shapeAntiAlias)
    {
        nvg__expandStroke(ctx, strokeWidth * 0.5f, ctx->fringeWidth, state->lineCap, state->lineJoin,
                          state->miterLimit);
    }
    else
    {
        nvg__expandStroke(ctx, strokeWidth * 0.5f, 0.0f, state->lineCap, state->lineJoin,
                          state->miterLimit);
    }

    ctx->params.renderStroke(ctx->params.userPtr, &strokePaint, state->compositeOperation,
                             &state->scissor, ctx->fringeWidth,
                             strokeWidth, ctx->cache->paths, ctx->cache->npaths);

    // Count triangles
    for (i = 0; i < ctx->cache->npaths; i++)
    {
        path = &ctx->cache->paths[i];
        ctx->strokeTriCount += path->nstroke - 2;
        ctx->drawCallCount++;
    }
}



// State setting
void nvgFontSize(NVGcontext *ctx, float size)
{
    NVGstate *state = nvg__getState(ctx);
    state->fontSize = size;
}

void nvgFontBlur(NVGcontext *ctx, float blur)
{
    (void)ctx;//not used
    (void)blur;

}

void nvgTextLetterSpacing(NVGcontext *ctx, float spacing)
{
    (void)ctx;//not used
    (void)spacing;
}

void nvgTextLineHeight(NVGcontext *ctx, float lineHeight)
{
    (void)ctx;//not used
    (void)lineHeight;
}

void nvgTextAlign(NVGcontext *ctx, int align)
{
    (void)ctx;//not used
    (void)align;
}

void nvgFontFaceId(NVGcontext *ctx, int font)
{
    (void)ctx;//not used
    (void)font;
}

void nvgFontFace(NVGcontext *ctx, const char *font)
{
    (void)ctx;//not used
    (void)font;
}

int nvgCreateFontMem(NVGcontext *ctx, const char *name, unsigned char *data, int ndata,
                     int freeData)
{
    (void)ctx;//not used
    (void)name;
    (void)data;
    (void)ndata;
    (void)freeData;
    return 0;
}

int nvgFindFont(NVGcontext *ctx, const char *name)
{
    (void)ctx;//not used
    (void)name;
    return 0;
}

float nvgText(NVGcontext *ctx, float x, float y, const char *string, const char *end)
{
    (void)ctx;//not used
    (void)x;
    (void)y;
    (void)string;
    (void)end;
    return 0;
}

float nvgTextBounds(NVGcontext *ctx, float x, float y, const char *string, const char *end,
                    float *bounds)
{
    (void)ctx;//not used
    (void)x;
    (void)y;
    (void)string;
    (void)end;
    (void)bounds;
    return 0;
}



NVGparams *nvgGetParams(NVGcontext *ctx)
{
    (void)ctx;//not used
    return &(ctx->params);
}

int nvgCreateImageRaw(NVGcontext *ctx, int w, int h, int format, int imageFlags,
                      const unsigned char *data)
{
    return ctx->params.renderCreateTexture(ctx->params.userPtr, format, w, h, imageFlags, data);
}

#ifdef RTK_MODULE_VG_LITE

#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include "nanovg.h"
#include "vg_lite.h"
#include "trace.h"

#define PATH_CMD_LEN 512
uint8_t path_cmd[PATH_CMD_LEN] = {0};
vg_lite_float_t path_data[PATH_CMD_LEN * 2] = {0};


static void multiply(vg_lite_matrix_t *matrix, vg_lite_matrix_t *mult)
{
    vg_lite_matrix_t temp;
    int row, column;

    /* Process all rows. */
    for (row = 0; row < 3; row++)
    {
        /* Process all columns. */
        for (column = 0; column < 3; column++)
        {
            /* Compute matrix entry. */
            temp.m[row][column] = (matrix->m[row][0] * mult->m[0][column])
                                  + (matrix->m[row][1] * mult->m[1][column])
                                  + (matrix->m[row][2] * mult->m[2][column]);
        }
    }

    /* Copy temporary matrix into result. */
    memcpy(matrix, &temp, sizeof(temp));
}

void patch_nvgFill(NVGcontext *ctx)
{
    NVGstate *state = nvg__getState(ctx);
    NVGpaint fillPaint = state->fill;


    nvg__flattenPaths(ctx);
    if (ctx->params.edgeAntiAlias && state->shapeAntiAlias)
    {
        nvg__expandFill(ctx, ctx->fringeWidth, NVG_MITER, 2.4f);
    }
    else
    {
        nvg__expandFill(ctx, 0.0f, NVG_MITER, 2.4f);
    }

    // Apply global alpha
    fillPaint.innerColor.a *= state->alpha;
    fillPaint.outerColor.a *= state->alpha;

    uint32_t total_cnt = 0;
    uint32_t cmd_cnt = 0;
    uint32_t data_cnt = 0;
    vg_lite_matrix_t matrix;
    vg_lite_identity(&matrix);

    while (total_cnt < ctx->ncommands)
    {
        float *cp1;
        float *cp2;
        float *p;
        int cmd = (int)ctx->commands[total_cnt];
        switch (cmd)
        {
        case NVG_MOVETO:
            p = &ctx->commands[total_cnt + 1];
            path_cmd[cmd_cnt++] = VLC_OP_MOVE;
            path_data[data_cnt++] = p[0];
            path_data[data_cnt++] = p[1];
            total_cnt += 3;
            break;
        case NVG_LINETO:
            p = &ctx->commands[total_cnt + 1];
            path_cmd[cmd_cnt++] = VLC_OP_LINE;
            path_data[data_cnt++] = p[0];
            path_data[data_cnt++] = p[1];
            total_cnt += 3;
            break;
        case NVG_BEZIERTO:
            p = &ctx->commands[total_cnt + 1];
            path_cmd[cmd_cnt++] = VLC_OP_CUBIC;
            path_data[data_cnt++] = p[0];
            path_data[data_cnt++] = p[1];
            path_data[data_cnt++] = p[2];
            path_data[data_cnt++] = p[3];
            path_data[data_cnt++] = p[4];
            path_data[data_cnt++] = p[5];
            total_cnt += 7;
            break;
        case NVG_CLOSE:
            path_cmd[cmd_cnt++] = VLC_OP_END;
            total_cnt++;
            break;
        case NVG_WINDING:
            total_cnt += 2;
            break;
        default:
            total_cnt++;
        }
        if (total_cnt >= PATH_CMD_LEN)
        {
            DBG_DIRECT(" ERROR !%s %d, cmd_cnt >= PATH_CMD_LEN\n", __func__, __LINE__);
            *(uint32_t *)0xFFFFFFFF = 0;
            while (1);
        }
    }
    vg_lite_path_t path;
    memset(&path, 0, sizeof(vg_lite_path_t));
    uint32_t path_data_len = vg_lite_path_calc_length(path_cmd, sizeof(path_cmd), VG_LITE_FP32);
    vg_lite_init_path(&path, VG_LITE_FP32, VG_LITE_HIGH, path_data_len, NULL, 0, 0, 0, 0);
    vg_lite_path_append(&path, path_cmd, path_data, cmd_cnt);

    vg_lite_linear_gradient_t grad;
    memset(&grad, 0, sizeof(vg_lite_linear_gradient_t));
    vg_lite_init_grad(&grad);



    vg_lite_buffer_t *target = (vg_lite_buffer_t *)ctx->params.userPtr;

    NVGpaint *paint = &fillPaint;

    uint8_t inner_r = paint->innerColor.r * 0xff;
    uint8_t inner_g = paint->innerColor.g * 0xff;
    uint8_t inner_b = paint->innerColor.b * 0xff;
    uint8_t inner_a = paint->innerColor.a * 0xff;
    uint8_t outer_r = paint->outerColor.r * 0xff;
    uint8_t outer_g = paint->outerColor.g * 0xff;
    uint8_t outer_b = paint->outerColor.b * 0xff;
    uint8_t outer_a = paint->outerColor.a * 0xff;



    if (memcmp(&(paint->innerColor), &(paint->outerColor), sizeof(paint->outerColor)) == 0)
    {
        uint32_t inner_color = (inner_a << 24) | (inner_r << 0) | (inner_g << 8) | (inner_b << 16);
        uint32_t outer_color = (outer_a << 24) | (outer_r << 0) | (outer_g << 8) | (outer_b << 16);
        vg_lite_draw(target, &path, VG_LITE_FILL_NON_ZERO, &matrix, VG_LITE_BLEND_SRC_OVER, inner_color);
    }
    else if (paint->radius == 0)
    {
        const float large = 1e5;
        float dx = paint->xform[2];
        float dy = paint->xform[3];
        float d = (paint->extent[1] - large) * 2;
        float sx = paint->xform[4] + dx * large;
        float sy = paint->xform[5] + dy * large;
        float ex = sx + d * dx;
        float ey = sy + d * dy;
        uint32_t inner_color = (inner_a << 24) | (inner_b << 0) | (inner_g << 8) | (inner_r << 16);
        uint32_t outer_color = (outer_a << 24) | (outer_b << 0) | (outer_g << 8) |
                               (outer_r << 16);//VG_LITE_RGBA8888

        uint32_t colors[] = {inner_color, outer_color};
        uint32_t stops[] = {0, VLC_GRADIENT_BUFFER_WIDTH - 1};
        vg_lite_set_grad(&grad, 2, colors, stops);
        vg_lite_update_grad(&grad);
        vg_lite_matrix_t *gradMatrix = vg_lite_get_grad_matrix(&grad);
        vg_lite_identity(gradMatrix);
        vg_lite_translate(sx, sy, gradMatrix);


        vg_lite_float_t cos_angle = dx;
        vg_lite_float_t sin_angle = dy;
        /* Set rotation matrix. */
        vg_lite_matrix_t r = { { {cos_angle, -sin_angle, 0.0f},
                {sin_angle, cos_angle, 0.0f},
                {0.0f, 0.0f, 1.0f}
            }
        };
        multiply(gradMatrix, &r);

        vg_lite_scale(d / VLC_GRADIENT_BUFFER_WIDTH, 1, gradMatrix);

        vg_lite_draw_gradient(target, &path, VG_LITE_FILL_EVEN_ODD, &matrix, &grad, VG_LITE_BLEND_SRC_OVER);
    }
    else
    {
        float cx = paint->xform[4];
        float cy = paint->xform[5];
        float inr = paint->radius - paint->feather / 2;
        float outr = paint->radius + paint->feather / 2;
    }
    vg_lite_finish();
    vg_lite_clear_path(&path);
    vg_lite_clear_grad(&grad);



    // Count triangles
    for (uint32_t i = 0; i < ctx->cache->npaths; i++)
    {
        const NVGpath *path;
        path = &ctx->cache->paths[i];
        ctx->fillTriCount += path->nfill - 2;
        ctx->fillTriCount += path->nstroke - 2;
        ctx->drawCallCount += 2;
    }

}

void patch_nvgStroke(NVGcontext *ctx)
{
    NVGstate *state = nvg__getState(ctx);
    float scale = nvg__getAverageScale(state->xform);
    float strokeWidth = nvg__clampf(state->strokeWidth * scale, 0.0f, 200.0f);
    NVGpaint strokePaint = state->stroke;

    if (strokeWidth < ctx->fringeWidth)
    {
        // If the stroke width is less than pixel size, use alpha to emulate coverage.
        // Since coverage is area, scale by alpha*alpha.
        float alpha = nvg__clampf(strokeWidth / ctx->fringeWidth, 0.0f, 1.0f);
        strokePaint.innerColor.a *= alpha * alpha;
        strokePaint.outerColor.a *= alpha * alpha;
        strokeWidth = ctx->fringeWidth;
    }

    // Apply global alpha
    strokePaint.innerColor.a *= state->alpha;
    strokePaint.outerColor.a *= state->alpha;

    if (ctx->params.edgeAntiAlias && state->shapeAntiAlias)
    {
        nvg__expandStroke(ctx, strokeWidth * 0.5f, ctx->fringeWidth, state->lineCap, state->lineJoin,
                          state->miterLimit);
    }
    else
    {
        nvg__expandStroke(ctx, strokeWidth * 0.5f, 0.0f, state->lineCap, state->lineJoin,
                          state->miterLimit);
    }
    nvg__flattenPaths(ctx);
    if (ctx->params.edgeAntiAlias && state->shapeAntiAlias)
    {
        nvg__expandStroke(ctx, strokeWidth * 0.5f, ctx->fringeWidth, state->lineCap, state->lineJoin,
                          state->miterLimit);
    }
    else
    {
        nvg__expandStroke(ctx, strokeWidth * 0.5f, 0.0f, state->lineCap, state->lineJoin,
                          state->miterLimit);
    }


    vg_lite_buffer_t *target = (vg_lite_buffer_t *)ctx->params.userPtr;
    NVGpaint *paint = &strokePaint;
    NVGpath *paths = ctx->cache->paths;
    int npaths = ctx->cache->npaths;


    uint8_t a = paint->innerColor.a * 0xff;
    uint8_t r = paint->innerColor.r * 0xff * paint->innerColor.a;
    uint8_t g = paint->innerColor.g * 0xff * paint->innerColor.a;
    uint8_t b = paint->innerColor.b * 0xff * paint->innerColor.a;
    vg_lite_color_t color = (a << 24) | (r << 0) | (g << 8) | (b << 16);


    vg_lite_matrix_t matrix;
    vg_lite_identity(&matrix);

    vg_lite_path_t *path_stroke = (vg_lite_path_t *)vg_lite_os_malloc(sizeof(vg_lite_path_t) * npaths);

    for (int i = 0; i < npaths; i++)
    {
        const NVGpath *p = paths + i;
        uint32_t cmd_cnt = 0;
        uint32_t data_cnt = 0;
        for (int j = 0; j < p->nstroke; j++)
        {
            const NVGvertex *v = p->stroke + j;
            if (j == 0)
            {
                path_cmd[cmd_cnt++] = VLC_OP_MOVE;
                path_data[data_cnt++] = v->x;
                path_data[data_cnt++] = v->y;
            }
            else
            {
                path_cmd[cmd_cnt++] = VLC_OP_LINE;
                path_data[data_cnt++] = v->x;
                path_data[data_cnt++] = v->y;
            }
        }
        if (cmd_cnt >= PATH_CMD_LEN)
        {
            *(uint32_t *)0xFFFFFFFF = 0;
            while (1);
        }
        //vg_lite_path_t path;
        memset(path_stroke + i, 0, sizeof(vg_lite_path_t));
        uint32_t path_data_len = vg_lite_path_calc_length(path_cmd, sizeof(path_cmd), VG_LITE_FP32);
        vg_lite_init_path(path_stroke + i, VG_LITE_FP32, VG_LITE_HIGH, path_data_len, NULL, 0, 0, 0, 0);
        vg_lite_path_append(path_stroke + i, path_cmd, path_data, cmd_cnt);

        vg_lite_set_stroke(path_stroke + i, VG_LITE_CAP_ROUND, VG_LITE_JOIN_ROUND, strokeWidth, 60, NULL, 0,
                           0, color);
        vg_lite_update_stroke(path_stroke + i);
        vg_lite_set_draw_path_type(path_stroke + i, VG_LITE_DRAW_STROKE_PATH);
        vg_lite_draw(target, path_stroke + i, VG_LITE_FILL_NON_ZERO, &matrix, VG_LITE_BLEND_SRC_OVER,
                     color);
    }
    vg_lite_finish();
    for (int i = 0; i < npaths; i++)
    {
        vg_lite_clear_path(path_stroke + i);
    }
    vg_lite_os_free(path_stroke);



    // Count triangles
    for (uint32_t i = 0; i < ctx->cache->npaths; i++)
    {
        const NVGpath *path;
        path = &ctx->cache->paths[i];
        ctx->strokeTriCount += path->nstroke - 2;
        ctx->drawCallCount++;
    }
}

#endif


// vim: ft=c nu noet ts=4
