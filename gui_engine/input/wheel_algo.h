/**
 * @file wheel_algo.h
 * @author howie_wang (howie_wang@realtek.com.cn)
 * @brief
 * @version 0.1
 * @date 2024-02-22
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef __WHEEL_ALGO_H__
#define __WHEEL_ALGO_H__
#include <string.h>
#include "guidef.h"


wheel_info_t *wheel_get_info(void);
wheel_info_t *wheel_algo_process(gui_wheel_port_data_t *wheel_raw);


#endif
