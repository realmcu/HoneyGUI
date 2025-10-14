/**
 * @file knob_algo.h
 * @author howie_wang (howie_wang@realtek.com.cn)
 * @brief
 * @version 0.1
 * @date 2024-02-22
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef __KNOB_ALGO_H__
#define __KNOB_ALGO_H__
#include <string.h>
#include "guidef.h"

#ifdef __cplusplus
extern "C" {
#endif

knob_info_t *knob_get_info(void);
knob_info_t *knob_algo_process(gui_knob_port_data_t *knob_raw);

#ifdef __cplusplus
}
#endif

#endif
