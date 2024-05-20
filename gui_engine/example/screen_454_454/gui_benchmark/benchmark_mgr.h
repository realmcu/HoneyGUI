#ifndef __BENCHMARK_MGR_H__
#define __BENCHMARK_MGR_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "gui_tabview.h"

typedef enum
{
    BENCHMARK_MAIN,
    BENCHMARK_MAX
} benchmark_ids;

//player main apis
void design_tab_benchmark_main(void *parent);
void free_benchmark_mgr(void);

#ifdef __cplusplus
}
#endif
#endif
