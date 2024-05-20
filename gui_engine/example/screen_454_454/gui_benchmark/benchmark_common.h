#ifndef __BENCHMARK_COMMON_H__
#define __BENCHMARK_COMMON_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "gui_obj.h"

typedef void (*pConstruct)(void *parent);
typedef void (*pDesturct)();

typedef enum
{
    BENCHMARK_SCENARIO_SINGLE_IMG,
    BENCHMARK_SCENARIO_SINGLE_IMG_ROTATION,
    BENCHMARK_SCENARIO_SINGLE_IMG_CUBE,
    BENCHMARK_SCENARIO_SINGLE_IMG_REDUCTION,
    BENCHMARK_SCENARIO_DOUBLE_IMG,
    BENCHMARK_SCENARIO_CUBE,
    BENCHMARK_SCENARIO_PERSPECTIVE,
    BENCHMARK_SCENARIO_TEXT,
    BENCHMARK_SCENARIO_SCROLL_TEXT,
    BENCHMARK_SCENARIO_MAX
} T_GUI_APP_BENCHMARK_SCENARIO;

typedef struct
{
    uint32_t time_max;
    uint32_t time_max_frame;
    uint32_t time_min;
    uint32_t time_min_frame;
    uint32_t time_avg;
} benchmark_cost_t;

typedef struct
{
    const char *name;
    uint32_t scenario;
    pConstruct ctor;
    pDesturct dtor;
} benchmark_info_t;

typedef struct
{
    gui_obj_t *obj;
    const benchmark_info_t *benchmark_info;
    uint32_t current_frame;
    benchmark_cost_t input_prepare_cost_us;
    benchmark_cost_t draw_prepare_cost_us;
    benchmark_cost_t draw_cost_us;
    benchmark_cost_t total_cost_us;
} benchmark_scenario_t;

extern benchmark_scenario_t benchmark_scenarios[BENCHMARK_SCENARIO_MAX];
extern gui_frame_monitor_t frame_monitor_benchmark;

bool scenario_benchmark_manager_ctor(void *parent, benchmark_scenario_t *benchmark);
bool scenario_benchmark_manager_dtor(benchmark_scenario_t *benchmark);
benchmark_scenario_t *get_current_benchmark_scenario(void);
int scenario_benchmark_manager_switch_next(void *parent);
void scenario_benchmark_manager_current_cost(void);

void benchmark_single_img_design(void *parent);
void benchmark_single_img_rotation_design(void *parent);
void benchmark_single_img_cube_design(void *parent);
void benchmark_single_img_reduction_design(void *parent);
void benchmark_double_img_design(void *parent);
void benchmark_cube(void *parent);
void benchmark_perspective(void *parent);
void benchmark_single_text(void *parent);
void benchmark_single_scroll_text(void *parent);

void app_benchmark_start(void);
void app_benchmark_finish(void);

void benchmark_free_all_other_mgrs(void);


#ifdef __cplusplus
}
#endif
#endif
