/**
 * @file auto_test_file.h
 * @author luke_sun (luke_sun@realtek.com.cn)
 * @brief
 * @version 1.0
 * @date 2024-08-01
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef __GUI_AUTO_TEST_FILE_H__
#define __GUI_AUTO_TEST_FILE_H__
#ifdef __cplusplus
extern "C" {
#endif

#include "auto_test_cmd.h"

char *test_case_name;

void gui_test_save_dc(void);
void gui_test_compare_dc(void);


#ifdef __cplusplus
}
#endif

#endif

