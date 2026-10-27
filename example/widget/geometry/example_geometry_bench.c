/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#include <math.h>
#include <stdio.h>
#include <string.h>
#include "guidef.h"
#include "gui_obj.h"
#include "gui_win.h"
#include "gui_text.h"
#include "gui_rect.h"
#include "gui_circle.h"
#include "gui_arc.h"
#include "gui_fb.h"
#include "gui_api_os.h"
#include "gui_message.h"
#include "gui_geometry_cache.h"
#include "gui_components_init.h"

#ifndef GEOMETRY_BENCH_FRAMES
#define GEOMETRY_BENCH_FRAMES 150u
#endif
#ifndef GEOMETRY_BENCH_ONLY_SCENE
#define GEOMETRY_BENCH_ONLY_SCENE (-1)
#endif
#define BENCH_WARMUP_FRAMES 30u
#define BENCH_SHAPES 9u
#define BENCH_HEADER_H 84
#define BENCH_FOOTER_H 74
#define BENCH_SAFE_MARGIN 60
#define BENCH_PERIOD 240u

typedef enum
{
    SCENE_ARC_SOLID,
    SCENE_ARC_ALPHA,
    SCENE_ARC_GRADIENT,
    SCENE_RECT_SOLID,
    SCENE_RECT_ALPHA,
    SCENE_RECT_GRADIENT,
    SCENE_CIRCLE_SOLID,
    SCENE_CIRCLE_ALPHA,
    SCENE_CIRCLE_GRADIENT,

    /* A progress ring advanced by end_angle only: the one shape the engine can
     * extend in place instead of rebuilding.  Paired with a full rebuild of
     * the same ring so the saving is measured rather than assumed. */
    SCENE_PROGRESS_STEP,
    SCENE_PROGRESS_REBUILD,
    SCENE_TOTAL
} bench_scene_t;

typedef enum
{
    WIDGET_ARC, WIDGET_RECT, WIDGET_CIRCLE, WIDGET_PROGRESS, WIDGET_TOTAL
} bench_widget_t;
typedef enum { STYLE_NONE = -1, STYLE_SOLID, STYLE_ALPHA, STYLE_GRADIENT, STYLE_TOTAL } bench_style_t;

typedef struct
{
    gui_obj_t *obj;
    void (*callback)(gui_obj_t *, T_OBJ_CB_TYPE);
    int x;
    int y;
} bench_shape_t;

typedef struct
{
    uint32_t frames;
    uint64_t period_us;
    uint64_t prepare_us;
    uint64_t draw_us;
    uint64_t post_us;
    uint64_t unaccounted_us;
    uint64_t clock_excess_us;
    uint32_t payload_peak;
    uint32_t capacity_peak;
    uint32_t cold_prepare_us;
    uint32_t cold_draw_us;
} bench_result_t;

typedef struct
{
    uint32_t scenes;
    uint32_t frames;
    uint64_t period_us;
    size_t peak_bytes;
    bool memory_valid;
    bool timing_valid;
} bench_score_t;

static const char *const scene_names[SCENE_TOTAL] =
{
    "Arc/solid", "Arc/opacity", "Arc/gradient",
    "Rect/solid", "Rect/opacity", "Rect/gradient",
    "Circle/solid", "Circle/opacity", "Circle/gradient",
    "Progress/step", "Progress/rebuild"
};
static const char *const widget_names[WIDGET_TOTAL] =
{
    "Arc", "Rect", "Circle", "Progress"
};
static gui_obj_t *page;
static gui_obj_t *layer;
static gui_obj_t *end_marker;
static gui_text_t *timing_label;
static gui_text_t *memory_label;
static bench_shape_t shapes[BENCH_SHAPES];
static bench_result_t result;
static bench_score_t scores[WIDGET_TOTAL];
static gui_mem_monitor_t memory_baseline;
static gui_mem_monitor_t memory_stats;
static size_t created_bytes;
static size_t resident_bytes;
static unsigned int scene;
static uint32_t cycle;
static uint32_t animation_frame;
static uint32_t warmup_frames;
static uint32_t frame_prepare;
static uint32_t frame_draw;
static uint32_t frame_post;
static uint32_t last_end_us;
static uint32_t next_hud_ms;
static uint32_t completed_ms;
static bool measuring;
static bool complete;
static bool memory_supported;
static char title_text[40];
static char timing_text[80];
static char memory_text[80];

/** The progress pair sits past the 3x3 grid and has no style axis. */
static bool is_progress_scene(void)
{
    return scene == SCENE_PROGRESS_STEP || scene == SCENE_PROGRESS_REBUILD;
}

static bench_widget_t widget_kind(void)
{
    return is_progress_scene() ? WIDGET_PROGRESS
           : (bench_widget_t)(scene / STYLE_TOTAL);
}

/** STYLE_NONE for the progress pair, which has no style axis. */
static bench_style_t style_kind(void)
{
    return is_progress_scene() ? STYLE_NONE : (bench_style_t)(scene % STYLE_TOTAL);
}

static uint32_t bench_us(void)
{
    /* The port supplies a microsecond clock on every target, so one call covers
     * the simulator and the boards alike. */
    return gui_us_get();
}

static uint32_t fps10(uint32_t frames, uint64_t period)
{
    return period ? (uint32_t)((uint64_t)frames * 10000000u / period) : 0u;
}

static float wave(uint32_t frame)
{
    float p = (float)(frame % BENCH_PERIOD) / (float)BENCH_PERIOD;
    return p < 0.5f ? p * 2.0f : (1.0f - p) * 2.0f;
}

static gui_color_t color(unsigned int i)
{
    static const uint8_t palette[][3] =
    {
        {255, 60, 105}, {155, 230, 55}, {30, 205, 230},
        {245, 190, 50}, {215, 115, 190}, {65, 185, 135}
    };
    unsigned int k = i % (sizeof(palette) / sizeof(palette[0]));
    return gui_rgba(palette[k][0], palette[k][1], palette[k][2], 255);
}

static gui_text_t *label(const char *name, const char *text, int y, int height)
{
    int width = (int)gui_get_dc()->screen_width;
    int inset = _UI_MIN(BENCH_SAFE_MARGIN, width / 8);
    gui_text_t *t = gui_text_create(page, name, inset, y, width - inset * 2, height);
    gui_text_set(t, (void *)text, GUI_FONT_SRC_BMP, gui_rgba(240, 243, 245, 255),
                 (uint16_t)strlen(text), 24);
    gui_text_mode_set(t, LEFT);
    return t;
}

static void shape_callback(gui_obj_t *obj, T_OBJ_CB_TYPE type)
{
    bench_shape_t *s = obj->user_data;
    uint32_t start = bench_us();
    s->callback(obj, type);
    uint32_t elapsed = bench_us() - start;
    if (type == OBJ_PRE_PROCESS) { frame_prepare += elapsed; }
    else if (type == OBJ_PROCESS) { frame_draw += elapsed; }
    else if (type == OBJ_POST_PROCESS) { frame_post += elapsed; }
}

static void update_hud(void)
{
    uint32_t fps = fps10(result.frames, result.period_us);
    uint32_t frame_ms10 = result.frames ?
                          (uint32_t)(result.period_us / result.frames / 100u) : 0u;
    snprintf(timing_text, sizeof(timing_text), "%u.%uFPS/%u.%ums",
             (unsigned int)(fps / 10u), (unsigned int)(fps % 10u),
             (unsigned int)(frame_ms10 / 10u), (unsigned int)(frame_ms10 % 10u));
    if (memory_supported)
    {
        size_t base = memory_baseline.main_used + memory_baseline.lower_used;
        size_t used = memory_stats.main_used + memory_stats.lower_used;
        snprintf(memory_text, sizeof(memory_text), "RAM%uKB/peak%uKB",
                 (unsigned int)((used >= base ? used - base : 0u) / 1024u),
                 (unsigned int)((memory_stats.total_peak >= base ?
                                 memory_stats.total_peak - base : 0u) / 1024u));
    }
    else { strcpy(memory_text, "RAM:N/A"); }
    gui_text_content_set(timing_label, timing_text, (uint16_t)strlen(timing_text));
    gui_text_content_set(memory_label, memory_text, (uint16_t)strlen(memory_text));
}

static void layer_callback(gui_obj_t *obj, T_OBJ_CB_TYPE type)
{
    GUI_UNUSED(obj);
    if (type != OBJ_PRE_PROCESS || complete) { return; }
    frame_prepare = frame_draw = frame_post = 0u;
    uint32_t f = animation_frame++;
    for (unsigned int i = 0; i < BENCH_SHAPES; i++)
    {
        if (is_progress_scene())
        {
            float span = 40.0f + 100.0f * wave(f + i * 17u);
            gui_arc_t *a = (gui_arc_t *)shapes[i].obj;
            if (scene == SCENE_PROGRESS_REBUILD)
            {
                gui_arc_set_start_angle(a, 270.0f);
            }
            gui_arc_set_end_angle(a, 270.0f + span);
        }
        else if (widget_kind() == WIDGET_ARC)
        {
            float end = 270.0f + 15.0f + 330.0f * wave(f + i * 17u);
            gui_arc_set_end_angle((gui_arc_t *)shapes[i].obj, end);
        }
        else
        {
            float angle = (float)(f % BENCH_PERIOD) * (2.0f * M_PI / (float)BENCH_PERIOD) +
                          (float)i;
            int dx = (int)(sinf(angle) * 8.0f);
            int dy = (int)(cosf(angle) * 8.0f);
            if (widget_kind() == WIDGET_RECT)
            {
                gui_rect_set_position((gui_rounded_rect_t *)shapes[i].obj,
                                      shapes[i].x + dx, shapes[i].y + dy);
            }
            else
            {
                gui_circle_set_position((gui_circle_t *)shapes[i].obj,
                                        shapes[i].x + dx, shapes[i].y + dy);
            }
        }
    }
    gui_fb_change();
}

/* Runs after the workload's draw and post callbacks, including all PFB bands. */
static void end_callback(gui_obj_t *obj, T_OBJ_CB_TYPE type)
{
    GUI_UNUSED(obj);
    if (type != OBJ_POST_PROCESS || complete) { return; }
    uint32_t end = bench_us();
    uint32_t period = end - last_end_us;
    last_end_us = end;
    if (warmup_frames == 0u && !measuring)
    {
        result.cold_prepare_us = frame_prepare;
        result.cold_draw_us = frame_draw;
    }
    if (memory_supported) { gui_mem_monitor_read(&memory_stats); }
    uint32_t payload = 0;
    gui_geometry_cache_stats(&payload, NULL);
    result.payload_peak = _UI_MAX(result.payload_peak, payload);
    result.capacity_peak = _UI_MAX(result.capacity_peak, gui_geometry_cache_capacity_bytes());
    if (!measuring)
    {
        if (++warmup_frames >= BENCH_WARMUP_FRAMES)
        {
            measuring = true;
            animation_frame = 0;
        }
        return;
    }
    /* Frame zero establishes the boundary; intervals 1..N use the same inputs. */
    if (animation_frame == 1u) { return; }
    result.frames++;
    result.period_us += period;
    result.prepare_us += frame_prepare;
    result.draw_us += frame_draw;
    result.post_us += frame_post;
    uint32_t geometry = frame_prepare + frame_draw + frame_post;
    if (period >= geometry) { result.unaccounted_us += period - geometry; }
    else { result.clock_excess_us += geometry - period; }
    resident_bytes = memory_stats.main_used + memory_stats.lower_used;
    complete = result.frames >= GEOMETRY_BENCH_FRAMES;
    uint32_t now = gui_ms_get();
    if (now - next_hud_ms < 0x80000000u)
    {
        update_hud();
        next_hud_ms = now + 500u;
    }
    if (complete)
    {
        completed_ms = now;
        bool widget_scored = is_progress_scene() ? (scene == SCENE_PROGRESS_REBUILD)
                             : (style_kind() == STYLE_GRADIENT);
        if (GEOMETRY_BENCH_ONLY_SCENE < 0 && widget_scored)
        {
            bench_score_t *score = &scores[widget_kind()];
            uint32_t combined = fps10(score->frames + result.frames,
                                      score->period_us + result.period_us);
            snprintf(timing_text, sizeof(timing_text), "%s/Score%u.%uFPS",
                     widget_names[widget_kind()], (unsigned int)(combined / 10u),
                     (unsigned int)(combined % 10u));
            gui_text_content_set(timing_label, timing_text, (uint16_t)strlen(timing_text));
            gui_fb_change();
        }
    }
}

static gui_obj_t *make_shape(unsigned int i, int x, int y, int size)
{
    gui_obj_t *obj;
    if (is_progress_scene())
    {
        int radius = _UI_MAX(2, size / 2 - size / 10 - 3);
        int width = _UI_MAX(6, size / 6);
        gui_arc_t *a = gui_arc_create(layer, "progress", x, y, radius, 270.0f,
                                      270.0f + 40.0f, (float)width, color(i));
        obj = (gui_obj_t *)a;
    }
    else if (widget_kind() == WIDGET_ARC)
    {
        bool gradient = style_kind() == STYLE_GRADIENT;
        uint8_t opacity = style_kind() == STYLE_ALPHA ? 128u : 255u;
        int radius = _UI_MAX(2, size / 2 - size / 10 - 3);
        gui_arc_t *a = gui_arc_create(layer, "arc", x, y, radius, 270.0f, 450.0f,
                                      (float)_UI_MAX(2, size / 10), color(i));
        gui_arc_set_opacity(a, opacity);
        if (gradient)
        {
            gui_arc_set_angular_gradient(a, 270.0f, 630.0f);
            gui_arc_add_gradient_stop(a, 0.0f, color(i));
            gui_arc_add_gradient_stop(a, 1.0f, color(i + 1u));
        }
        obj = (gui_obj_t *)a;
    }
    else if (widget_kind() == WIDGET_RECT)
    {
        bool gradient = style_kind() == STYLE_GRADIENT;
        uint8_t opacity = style_kind() == STYLE_ALPHA ? 128u : 255u;
        x -= size / 2;
        y -= size / 2;
        gui_rounded_rect_t *r = gui_rect_create(layer, "rect", x, y, size, size,
                                                _UI_MAX(1, size / 5), color(i));
        gui_rect_set_opacity(r, opacity);
        if (gradient)
        {
            gui_rect_set_linear_gradient(r, RECT_GRADIENT_HORIZONTAL);
            gui_rect_add_gradient_stop(r, 0.0f, color(i));
            gui_rect_add_gradient_stop(r, 1.0f, color(i + 1u));
        }
        obj = (gui_obj_t *)r;
    }
    else
    {
        bool gradient = style_kind() == STYLE_GRADIENT;
        uint8_t opacity = style_kind() == STYLE_ALPHA ? 128u : 255u;
        gui_circle_t *c = gui_circle_create(layer, "circle", x, y, size / 2, color(i));
        gui_circle_set_opacity(c, opacity);
        if (gradient)
        {
            gui_circle_set_radial_gradient(c);
            gui_circle_add_gradient_stop(c, 0.0f, color(i));
            gui_circle_add_gradient_stop(c, 1.0f, color(i + 1u));
        }
        obj = (gui_obj_t *)c;
    }
    GUI_ASSERT(obj != NULL);
    shapes[i].obj = obj;
    shapes[i].callback = obj->obj_cb;
    shapes[i].x = x;
    shapes[i].y = y;
    obj->user_data = &shapes[i];
    obj->obj_cb = shape_callback;
    return obj;
}

static void build_scene(void)
{
    gui_dispdev_t *dc = gui_get_dc();
    int w = (int)dc->screen_width;
    int h = (int)dc->screen_height - BENCH_HEADER_H - BENCH_FOOTER_H;
    GUI_ASSERT(w >= 120 && h >= 120);
    int size = _UI_MAX(12, _UI_MIN(w / 3, h / 3) - 24);
    memset(&result, 0, sizeof(result));
    memset(&memory_baseline, 0, sizeof(memory_baseline));
    memset(&memory_stats, 0, sizeof(memory_stats));
    memory_supported = gui_mem_monitor_begin(&memory_baseline);
    page = (gui_obj_t *)gui_win_create(gui_obj_get_root(), "geometry_bench", 0, 0,
                                       w, (int)dc->screen_height);
    gui_rect_create(page, "background", 0, 0, w, (int)dc->screen_height, 0,
                    gui_rgba(18, 20, 24, 255));
    /* Three cheap bands provide the same visible background for all styles. */
    for (int i = 0; i < 3; i++)
    {
        gui_rect_create(page, "band", 0, BENCH_HEADER_H + i * h / 3, w,
                        (i + 1) * h / 3 - i * h / 3, 0,
                        gui_rgba((uint8_t)(24 + i * 10), (uint8_t)(32 + i * 8),
                                 (uint8_t)(40 + i * 5), 255));
    }
    layer = gui_obj_create(page, "workload", 0, BENCH_HEADER_H, w, h);
    layer->obj_cb = layer_callback;
    layer->has_pre_process_cb = true;
    for (unsigned int i = 0; i < BENCH_SHAPES; i++)
    {
        int x = (int)(i % 3u) * w / 3 + w / 6;
        int y = (int)(i / 3u) * h / 3 + h / 6;
        make_shape(i, x, y, size);
    }
    snprintf(title_text, sizeof(title_text), "%u/%u/%s", scene + 1u,
             (unsigned int)SCENE_TOTAL, scene_names[scene]);
    label("title", title_text, 8, 30);
    strcpy(timing_text, "Warmingup");
    strcpy(memory_text, "RAM:N/A");
    timing_label = label("timing", timing_text, (int)dc->screen_height - 62, 28);
    memory_label = label("memory", memory_text, (int)dc->screen_height - 32, 28);
    end_marker = gui_obj_create(gui_obj_get_root(), "benchmark_end", 0, 0, 1, 1);
    end_marker->obj_cb = end_callback;
    end_marker->has_post_process_cb = true;
    if (memory_supported) { gui_mem_monitor_read(&memory_stats); }
    created_bytes = memory_stats.main_used + memory_stats.lower_used;
    resident_bytes = 0;
    animation_frame = warmup_frames = last_end_us = 0u;
    measuring = complete = false;
    next_hud_ms = gui_ms_get() + 500u;
    gui_log("GEOM_SCENE,%u,%s,%u,%u,%d\n", (unsigned int)cycle, scene_names[scene],
            (unsigned int)GEOMETRY_BENCH_FRAMES, (unsigned int)BENCH_SHAPES, size);
    gui_fb_change();
}

static void finish_scene(void)
{
    uint32_t n = result.frames;
    uint32_t fps = fps10(n, result.period_us);
    gui_log("GEOM_V3,%u,%s,%u,%u.%u,%u,%u,%u,%u,%u,%u,%u\n",
            (unsigned int)cycle, scene_names[scene], (unsigned int)n,
            (unsigned int)(fps / 10u), (unsigned int)(fps % 10u),
            n ? (unsigned int)(result.period_us / n) : 0u,
            n ? (unsigned int)(result.prepare_us / n) : 0u,
            n ? (unsigned int)(result.draw_us / n) : 0u,
            n ? (unsigned int)(result.post_us / n) : 0u,
            n ? (unsigned int)(result.unaccounted_us / n) : 0u,
            (unsigned int)result.payload_peak, (unsigned int)result.capacity_peak);
    gui_log("GEOM_COLD,%u,%s,%u,%u\n", (unsigned int)cycle, scene_names[scene],
            (unsigned int)result.cold_prepare_us, (unsigned int)result.cold_draw_us);
    gui_obj_tree_free(end_marker);
    gui_obj_tree_free(page);
    page = end_marker = NULL;
    if (memory_supported) { gui_mem_monitor_end(&memory_stats); }
    size_t base = memory_baseline.main_used + memory_baseline.lower_used;
    size_t after = memory_stats.main_used + memory_stats.lower_used;
    size_t peak = memory_stats.total_peak >= base ? memory_stats.total_peak - base : 0u;
    gui_log("MEM_V3,%u,%s,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u\n",
            (unsigned int)cycle, scene_names[scene], (unsigned int)memory_supported,
            (unsigned int)base,
            (unsigned int)(created_bytes >= base ? created_bytes - base : 0u),
            (unsigned int)(resident_bytes >= base ? resident_bytes - base : 0u),
            (unsigned int)peak, (unsigned int)after,
            (unsigned int)(after >= base ? after - base : 0u),
            (unsigned int)memory_stats.consistent, (unsigned int)memory_stats.allocations,
            (unsigned int)memory_stats.frees, (unsigned int)memory_stats.resizes);
    bench_score_t *score = &scores[widget_kind()];
    score->scenes++;
    score->frames += n;
    score->period_us += result.period_us;
    score->peak_bytes = _UI_MAX(score->peak_bytes, peak);
    score->memory_valid = score->memory_valid && memory_supported && memory_stats.consistent;
    score->timing_valid = score->timing_valid && result.clock_excess_us == 0u;
    /* The 3x3 widgets score across their three styles; the progress pair has
     * two scenes, so it scores when both have run. */
    if (score->scenes == (is_progress_scene() ? 2u : (uint32_t)STYLE_TOTAL))
    {
        fps = fps10(score->frames, score->period_us);
        gui_log("WIDGET_SCORE,%u,%s,%u,%u.%u,%u,%u,%u\n",
                (unsigned int)cycle, widget_names[widget_kind()],
                (unsigned int)score->frames, (unsigned int)(fps / 10u),
                (unsigned int)(fps % 10u), (unsigned int)score->peak_bytes,
                (unsigned int)score->memory_valid, (unsigned int)score->timing_valid);
    }
    if (result.clock_excess_us != 0u)
    {
        gui_log("[geometry] clock warning: %u us geometry excess\n",
                (unsigned int)result.clock_excess_us);
    }
}

static void reset_scores(void)
{
    memset(scores, 0, sizeof(scores));
    for (unsigned int i = 0; i < WIDGET_TOTAL; i++)
    {
        scores[i].memory_valid = true;
        scores[i].timing_valid = true;
    }
}

static void next_scene(void *obj)
{
    GUI_UNUSED(obj);
    if (!complete) { return; }
    /* Hold the finished scene for a second so the combined score can be read;
     * the same test as the HUD, see end_callback(). */
    bool widget_scored = is_progress_scene() ? (scene == SCENE_PROGRESS_REBUILD)
                         : (style_kind() == STYLE_GRADIENT);
    if (GEOMETRY_BENCH_ONLY_SCENE < 0 && widget_scored &&
        gui_ms_get() - completed_ms < 1000u)
    {
        return;
    }
    /* Root timer runs before traversal; the old scene can be freed here. */
    finish_scene();
    if (GEOMETRY_BENCH_ONLY_SCENE >= 0)
    {
        cycle++;
        reset_scores();
    }
    else
    {
        scene = (scene + 1u) % SCENE_TOTAL;
        if (scene == 0u)
        {
            cycle++;
            reset_scores();
        }
    }
    build_scene();
}

void geometry_benchmark_run(void)
{
    if (page != NULL) { return; }
    GUI_ASSERT(GEOMETRY_BENCH_ONLY_SCENE >= -1 && GEOMETRY_BENCH_ONLY_SCENE < SCENE_TOTAL);
    GUI_ASSERT(GEOMETRY_BENCH_FRAMES > 0u);
    scene = GEOMETRY_BENCH_ONLY_SCENE >= 0 ? (unsigned int)GEOMETRY_BENCH_ONLY_SCENE : 0u;
    cycle = 1;
    reset_scores();
    gui_log("GEOM_V3,cycle,scene,frames,fps,period_us,prepare_us,draw_us,post_us,"
            "unaccounted_us,cache_payload_peak_B,cache_capacity_peak_B\n");
    gui_log("MEM_V3,cycle,scene,supported,baseline_B,created_delta_B,resident_delta_B,"
            "peak_delta_B,after_free_B,residual_delta_B,consistent,allocs,frees,resizes\n");
    gui_log("WIDGET_SCORE,cycle,widget,frames,combined_fps,peak_delta_B,memory_valid,timing_valid\n");
    gui_log("[geometry] score=total measured frames/total frame time across three styles;"
            " memory and callback instrumentation overhead included\n");
    gui_dispdev_t *dc = gui_get_dc();
    uint32_t buffers = dc->disp_buf_1 != NULL ? 1u : 0u;
    if (dc->disp_buf_2 != NULL && dc->disp_buf_2 != dc->disp_buf_1) { buffers++; }
    gui_log("DISPLAY,%u,%u,%u,%u,%u,%u,%u\n", (unsigned int)dc->screen_width,
            (unsigned int)dc->screen_height, (unsigned int)dc->bit_depth,
            (unsigned int)dc->fb_width, (unsigned int)dc->fb_height, (unsigned int)buffers,
            (unsigned int)((uint64_t)dc->fb_width * dc->fb_height *
                           ((dc->bit_depth + 7u) / 8u) * buffers));
    gui_obj_create_timer(gui_obj_get_root(), 1u, true, next_scene);
    build_scene();
    gui_obj_start_timer(gui_obj_get_root());
}

static int geometry_benchmark_init(void)
{
    gui_set_keep_active_time(0xFFFFFFFF);
    geometry_benchmark_run();
    return 0;
}

GUI_INIT_APP_EXPORT(geometry_benchmark_init);
