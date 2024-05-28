#include "gui_obj.h"
#include "gui_api.h"
#include "gui_win.h"
#include "benchmark_common.h"
#include "watchface_mgr.h"
#include "player_mgr.h"
#include "call_mgr.h"
#include "record_mgr.h"
#include "contacts_mgr.h"
#include "benchmark_mgr.h"

static uint32_t scenario_count = 0;
static void scenario_benchmark_manager_frame_start_cb(void);
static void scenario_benchmark_manager_frame_input_prepare_cb(void);
static void scenario_benchmark_manager_frame_draw_prepare_cb(void);
static void scenario_benchmark_manager_frame_draw_cb(void);
static void scenario_benchmark_manager_frame_end_cb(void);

gui_frame_monitor_t frame_monitor_benchmark =
{
    .start_cb = scenario_benchmark_manager_frame_start_cb,
    .input_prepare_cb = scenario_benchmark_manager_frame_input_prepare_cb,
    .draw_prepare_cb = scenario_benchmark_manager_frame_draw_prepare_cb,
    .draw_cb = scenario_benchmark_manager_frame_draw_cb,
    .end_cb = scenario_benchmark_manager_frame_end_cb,
};

const static benchmark_info_t benchmark_infos[BENCHMARK_SCENARIO_MAX] =
{
    {"BENCHMARK_SINGLE_IMG",            BENCHMARK_SCENARIO_SINGLE_IMG,           benchmark_single_img_design,            NULL},
    {"BENCHMARK_SINGLE_IMG_ROTATION",   BENCHMARK_SCENARIO_SINGLE_IMG_ROTATION,  benchmark_single_img_rotation_design,   NULL},
    {"BENCHMARK_SINGLE_IMG_CUBE",       BENCHMARK_SCENARIO_SINGLE_IMG_CUBE,      benchmark_single_img_cube_design,       NULL},
    {"BENCHMARK_SINGLE_IMG_REDUCTION",  BENCHMARK_SCENARIO_SINGLE_IMG_REDUCTION, benchmark_single_img_reduction_design,  NULL},
    {"BENCHMARK_DOUBLE_IMG",            BENCHMARK_SCENARIO_DOUBLE_IMG,           benchmark_double_img_design,            NULL},
    {"BENCHMARK_CUBE",                  BENCHMARK_SCENARIO_CUBE,                 benchmark_cube,                         NULL},
    {"BENCHMARK_PERSPECTIVE",           BENCHMARK_SCENARIO_PERSPECTIVE,          benchmark_perspective,                  NULL},
    {"BENCHMARK_TEXT",                  BENCHMARK_SCENARIO_TEXT,                 benchmark_single_text,                  NULL},
    {"BENCHMARK_SCROLL_TEXT",           BENCHMARK_SCENARIO_SCROLL_TEXT,          benchmark_single_scroll_text,           NULL},
};

benchmark_scenario_t benchmark_scenarios[BENCHMARK_SCENARIO_MAX] =
{
    {NULL, &benchmark_infos[BENCHMARK_SCENARIO_SINGLE_IMG]},
    {NULL, &benchmark_infos[BENCHMARK_SCENARIO_SINGLE_IMG_ROTATION]},
    {NULL, &benchmark_infos[BENCHMARK_SCENARIO_SINGLE_IMG_CUBE]},
    {NULL, &benchmark_infos[BENCHMARK_SCENARIO_SINGLE_IMG_REDUCTION]},
    {NULL, &benchmark_infos[BENCHMARK_SCENARIO_DOUBLE_IMG]},
    {NULL, &benchmark_infos[BENCHMARK_SCENARIO_CUBE]},
    {NULL, &benchmark_infos[BENCHMARK_SCENARIO_PERSPECTIVE]},
    {NULL, &benchmark_infos[BENCHMARK_SCENARIO_TEXT]},
    {NULL, &benchmark_infos[BENCHMARK_SCENARIO_SCROLL_TEXT]},
};

benchmark_scenario_t *get_current_benchmark_scenario(void)
{
    return &benchmark_scenarios[scenario_count];
}

bool scenario_benchmark_manager_ctor(void *parent, benchmark_scenario_t *benchmark)
{
    if (benchmark->benchmark_info->ctor == NULL)
    {
        return false;
    }
    gui_win_t *win = gui_win_create(parent, "win_benchmark_scenario", 0, 0, 0, 0);
    benchmark->obj = (gui_obj_t *)win;
    benchmark->benchmark_info->ctor(win);
    return true;
}

bool scenario_benchmark_manager_dtor(benchmark_scenario_t *benchmark)
{
    if (benchmark->benchmark_info->dtor == NULL)
    {
        //gui_log("app_benchmark_manager_dtor 0x%x", (uint32_t *)benchmark->obj);
        gui_obj_tree_free(benchmark->obj);
        benchmark->obj = NULL;
    }
    else
    {
        benchmark->benchmark_info->dtor();
    }
    return true;
}

int scenario_benchmark_manager_switch_next(void *parent)
{
    if (scenario_count >= BENCHMARK_SCENARIO_MAX - 1)
    {
        scenario_benchmark_manager_dtor(&benchmark_scenarios[scenario_count]);
        scenario_count = 0;
        return -1;
    }
    scenario_benchmark_manager_dtor(&benchmark_scenarios[scenario_count]);
    scenario_count ++;
    gui_log("scenario benchmark next: %d\n", scenario_count);
    if (scenario_benchmark_manager_ctor(parent, &benchmark_scenarios[scenario_count]))
    {
        return 1;
    }
    else
    {
        gui_log("scenario benchmark create fail!");
        return 0;
    }

}

void scenario_benchmark_manager_current_cost(void)
{
    benchmark_scenario_t *benchmark = get_current_benchmark_scenario();
    benchmark_cost_t *p_cost_ms = NULL;
    gui_log("scenario %d: %s\n", benchmark->benchmark_info->scenario, benchmark->benchmark_info->name);
    p_cost_ms = &benchmark->input_prepare_cost_us;
    gui_log("input prepare time ms, max: %d.%d at %d frame, min: %d.%d at %d frame, avg: %d.%d\n",
            p_cost_ms->time_max / 1000, p_cost_ms->time_max % 1000, p_cost_ms->time_max_frame,
            p_cost_ms->time_min / 1000, p_cost_ms->time_min % 1000, p_cost_ms->time_min_frame,
            p_cost_ms->time_avg / 1000, p_cost_ms->time_avg % 1000);

    p_cost_ms = &benchmark->draw_prepare_cost_us;
    gui_log("draw prepare time ms, max: %d.%d at %d frame, min: %d.%d at %d frame, avg: %d.%d\n",
            p_cost_ms->time_max / 1000, p_cost_ms->time_max % 1000, p_cost_ms->time_max_frame,
            p_cost_ms->time_min / 1000, p_cost_ms->time_min % 1000, p_cost_ms->time_min_frame,
            p_cost_ms->time_avg / 1000, p_cost_ms->time_avg % 1000);

    p_cost_ms = &benchmark->draw_cost_us;
    gui_log("draw time ms, max: %d.%d at %d frame, min: %d.%d at %d frame, avg: %d.%d\n",
            p_cost_ms->time_max / 1000, p_cost_ms->time_max % 1000, p_cost_ms->time_max_frame,
            p_cost_ms->time_min / 1000, p_cost_ms->time_min % 1000, p_cost_ms->time_min_frame,
            p_cost_ms->time_avg / 1000, p_cost_ms->time_avg % 1000);

    p_cost_ms = &benchmark->total_cost_us;
    gui_log("total time ms, max: %d.%d at %d frame, min: %d.%d at %d frame, avg: %d.%d\n",
            p_cost_ms->time_max / 1000, p_cost_ms->time_max % 1000, p_cost_ms->time_max_frame,
            p_cost_ms->time_min / 1000, p_cost_ms->time_min % 1000, p_cost_ms->time_min_frame,
            p_cost_ms->time_avg / 1000, p_cost_ms->time_avg % 1000);
}

static void scenario_benchmark_manager_frame_start_cb(void)
{
    frame_monitor_benchmark.start = gui_us_get();
}

static void scenario_benchmark_manager_frame_input_prepare_cb(void)
{
    frame_monitor_benchmark.input_prepare = gui_us_get();
}

static void scenario_benchmark_manager_frame_draw_prepare_cb(void)
{
    frame_monitor_benchmark.draw_prepare = gui_us_get();
}

static void scenario_benchmark_manager_frame_draw_cb(void)
{
    frame_monitor_benchmark.draw = gui_us_get();
}

static void scenario_benchmark_manager_frame_end_cb(void)
{
    frame_monitor_benchmark.end = gui_us_get();
    /*print each frame cost*/
    uint32_t input_prepare_time = frame_monitor_benchmark.input_prepare - frame_monitor_benchmark.start;
    uint32_t draw_prepare_time = frame_monitor_benchmark.draw_prepare -
                                 frame_monitor_benchmark.input_prepare;
    uint32_t draw_time = frame_monitor_benchmark.draw - frame_monitor_benchmark.draw_prepare;
    uint32_t total_time = frame_monitor_benchmark.end - frame_monitor_benchmark.start;

    gui_log("frame monitor time ms: input prepare %d.%d, draw prepare %d.%d, draw %d.%d, total %d.%d\n",
            input_prepare_time / 1000, input_prepare_time % 1000,
            draw_prepare_time / 1000, draw_prepare_time % 1000,
            draw_time / 1000, draw_time % 1000,
            total_time / 1000, total_time % 1000);
}

void benchmark_free_all_other_mgrs(void)
{
    free_watchface_mgr();
    free_call_mgr();
    free_player_mgr();
    free_record_mgr();
    free_contacts_mgr();
    free_benchmark_mgr();
}

