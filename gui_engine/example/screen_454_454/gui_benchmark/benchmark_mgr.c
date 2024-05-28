#include "app_gui_main.h"
#include "gui_common.h"
#include "gui_event.h"
#include "benchmark_mgr.h"

gui_tabview_t *tabview_benchmark_mgr = NULL;

const tab_info_t benchmark_tab_infos[BENCHMARK_MAX] =
{
    {BENCHMARK_MAIN, 0, 0, "benchmark_main", design_tab_benchmark_main, NULL},
};

app_tab_t benchmark_tabs[BENCHMARK_MAX] =
{
    {NULL, &benchmark_tab_infos[BENCHMARK_MAIN]},
};

void design_benchmark_mgr(void *parent)
{
    tabview_benchmark_mgr = gui_tabview_create(parent, "tabview benchmark mgr", 0, 0, 0, 0);
    gui_tabview_tp_disable(tabview_benchmark_mgr, true);

    app_tab_manager_ctor(tabview_benchmark_mgr, &benchmark_tabs[BENCHMARK_MAIN]);
}

void free_benchmark_mgr(void)
{
    for (uint16_t i = 0; i < BENCHMARK_MAX; i++)
    {
        app_tab_manager_free(&benchmark_tabs[i]);
    }

    gui_obj_tree_free(tabview_benchmark_mgr);
}

