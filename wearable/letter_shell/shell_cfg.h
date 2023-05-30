/**
 * @file shell_cfg.h
 * @author Letter (nevermindzzt@gmail.com)
 * @brief shell config
 * @version 3.0.0
 * @date 2019-12-31
 *
 * @copyright (c) 2019 Letter
 *
 */

#ifndef __SHELL_CFG_H__
#define __SHELL_CFG_H__

#include "os_sched.h"
/**
 * @brief
 *
 *
 */
#define     SHELL_TASK_WHILE            1

/**
 * @brief
 *
 *
 */
#define     SHELL_USING_CMD_EXPORT      1

/**
 * @brief
 *
 */
#define     SHELL_USING_COMPANION       0

/**
 * @brief
 */
#define     SHELL_SUPPORT_END_LINE      0

/**
 * @brief
 */
#define     SHELL_HELP_LIST_USER        0

/**
 * @brief
 */
#define     SHELL_HELP_LIST_VAR         0

/**
 * @brief
 */
#define     SHELL_HELP_LIST_KEY         0

/**
 * @brief
 */
#define     SHELL_HELP_SHOW_PERMISSION  1

/**
 * @brief
 *
 */
#define     SHELL_ENTER_LF              1

/**
 * @brief
 *
 */
#define     SHELL_ENTER_CR              1

/**
 * @brief
 *
 */
#define     SHELL_ENTER_CRLF            0

/**
 * @brief
 *
 * @attention
 */
#define     SHELL_EXEC_UNDEF_FUNC       0

/**
 * @brief
 *
 */
#define     SHELL_PARAMETER_MAX_NUMBER  8

/**
 * @brief
 */
#define     SHELL_HISTORY_MAX_NUMBER    5

/**
 * @brief
 *
 */
#define     SHELL_DOUBLE_CLICK_TIME     200

/**
 * @brief
 *
 */
#define     SHELL_QUICK_HELP            1

/**
 * @brief
 *
 *
 */
#define     SHELL_KEEP_RETURN_VALUE     0

/**
 * @brief
 */
#define     SHELL_MAX_NUMBER            5

/**
 * @brief
 *
 */
#define     SHELL_PRINT_BUFFER          256

/**
 * @brief
 *
 * @note
 */
#define     SHELL_SCAN_BUFFER          0

/**
 * @brief (ms)
 *
 * @note
 */
#define     SHELL_GET_TICK()            os_sys_time_get()

/**
 * @brief
 * @note
 */
#define     SHELL_USING_LOCK            0

/**
 * @brief
 *
 */
#define     SHELL_MALLOC(size)          0

/**
 * @brief
 *
 */
#define     SHELL_FREE(obj)             0

/**
 * @brief
 */
#define     SHELL_SHOW_INFO             1

/**
 * @brief
 */
#define     SHELL_CLS_WHEN_LOGIN        1

/**
 * @brief
 */
#define     SHELL_DEFAULT_USER          "letter"

/**
 * @brief
 *
 */
#define     SHELL_DEFAULT_USER_PASSWORD ""

/**
 * @brief
 *
 *
 * @note
 */
#define     SHELL_LOCK_TIMEOUT          0 * 60 * 1000

#endif
