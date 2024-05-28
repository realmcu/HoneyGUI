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

#include "stdlib.h"
unsigned int userGetTick();

#ifndef SHELL_TASK_WHILE
/**
 * @brief Whether to use the default shell task while loop
 */
#define     SHELL_TASK_WHILE            1
#endif /** SHELL_TASK_WHILE */

#ifndef SHELL_USING_CMD_EXPORT
/**
 * @brief Whether to use command export method
 */
#define     SHELL_USING_CMD_EXPORT      1
#endif /** SHELL_USING_CMD_EXPORT */

#ifndef SHELL_USING_COMPANION
/**
 * @brief Whether to use shell companion objects
 */
#define     SHELL_USING_COMPANION       1
#endif /** SHELL_USING_COMPANION */

#ifndef SHELL_SUPPORT_END_LINE
/**
 * @brief Support shell end line mode
 */
#define     SHELL_SUPPORT_END_LINE      0
#endif /** SHELL_SUPPORT_END_LINE */

#ifndef SHELL_HELP_LIST_USER
/**
 * @brief Whether to list user in the output command list
 */
#define     SHELL_HELP_LIST_USER        0
#endif /** SHELL_HELP_LIST_USER */

#ifndef SHELL_HELP_LIST_VAR
/**
 * @brief Whether to list variables in the output command list
 */
#define     SHELL_HELP_LIST_VAR         0
#endif /** SHELL_HELP_LIST_VAR */

#ifndef SHELL_HELP_LIST_KEY
/**
 * @brief Whether to list keys in the output command list
 */
#define     SHELL_HELP_LIST_KEY         0
#endif /** SHELL_HELP_LIST_KEY */

#ifndef SHELL_HELP_SHOW_PERMISSION
/**
 * @brief Whether to display command permissions in the output command list
 */
#define     SHELL_HELP_SHOW_PERMISSION  1
#endif /** SHELL_HELP_SHOW_PERMISSION */

#ifndef SHELL_ENTER_LF
/**
 * @brief Using LF as a command line carriage return trigger
 */
#define     SHELL_ENTER_LF              1
#endif /** SHELL_ENTER_LF */

#ifndef SHELL_ENTER_CR
/**
 * @brief Using CR as a command line carriage return trigger
 */
#define     SHELL_ENTER_CR              1
#endif /** SHELL_ENTER_CR */

#ifndef SHELL_ENTER_CRLF
/**
 * @brief Using CRLF as a command line carriage return trigger
 */
#define     SHELL_ENTER_CRLF            0
#endif /** SHELL_ENTER_CRLF */

#ifndef SHELL_EXEC_UNDEF_FUNC
/**
 * @brief Using the function of executing unexported functions
 */
#define     SHELL_EXEC_UNDEF_FUNC       0
#endif /** SHELL_EXEC_UNDEF_FUNC */

#ifndef SHELL_PARAMETER_MAX_NUMBER
/**
 * @brief Maximum number of shell command parameters
 */
#define     SHELL_PARAMETER_MAX_NUMBER  8
#endif /** SHELL_PARAMETER_MAX_NUMBER */

#ifndef SHELL_HISTORY_MAX_NUMBER
/**
 * @brief Number of historical command records
 */
#define     SHELL_HISTORY_MAX_NUMBER    5
#endif /** SHELL_HISTORY_MAX_NUMBER */

#ifndef SHELL_DOUBLE_CLICK_TIME
/**
 * @brief Double click interval (ms)
 */
#define     SHELL_DOUBLE_CLICK_TIME     200
#endif /** SHELL_DOUBLE_CLICK_TIME */

#ifndef SHELL_QUICK_HELP
/**
 * @brief Quick help
 */
#define     SHELL_QUICK_HELP            1
#endif /** SHELL_QUICK_HELP */

#ifndef SHELL_KEEP_RETURN_VALUE
/**
 * @brief Save command return value
 */
#define     SHELL_KEEP_RETURN_VALUE     0
#endif /** SHELL_KEEP_RETURN_VALUE */

#ifndef SHELL_MAX_NUMBER
/**
 * @brief Maximum number of shells managed
 */
#define     SHELL_MAX_NUMBER            5
#endif /** SHELL_MAX_NUMBER */

#ifndef SHELL_PRINT_BUFFER
/**
 * @brief Shell formatted output buffer size
 */
#define     SHELL_PRINT_BUFFER          128
#endif /** SHELL_PRINT_BUFFER */

#ifndef SHELL_SCAN_BUFFER
/**
 * @brief Shell formatted input buffer size
 */
#define     SHELL_SCAN_BUFFER          0
#endif /** SHELL_SCAN_BUFFER */

#ifndef SHELL_GET_TICK
/**
 * @brief Obtain system time (ms)
 */
#define     SHELL_GET_TICK()           userGetTick()
#endif /** SHELL_GET_TICK */

#ifndef SHELL_USING_LOCK
/**
 * @brief Using lock
 */
#define     SHELL_USING_LOCK            0
#endif /** SHELL_USING_LOCK */

#ifndef SHELL_MALLOC
/**
 * @brief Shell memory allocation
 */
#define     SHELL_MALLOC(size)          malloc(size)
#endif /** SHELL_MALLOC */

#ifndef SHELL_FREE
/**
 * @brief Shell memory free
 */
#define     SHELL_FREE(obj)             free(obj)
#endif /** SHELL_FREE */

#ifndef SHELL_SHOW_INFO
/**
 * @brief Whether to show shell information
 */
#define     SHELL_SHOW_INFO             1
#endif /** SHELL_SHOW_INFO */

#ifndef SHELL_CLS_WHEN_LOGIN
/**
 * @brief Whether to clear the command line after logging in
 */
#define     SHELL_CLS_WHEN_LOGIN        1
#endif /** SHELL_CLS_WHEN_LOGIN */

#ifndef SHELL_DEFAULT_USER
/**
 * @brief Shell default user
 */
#define     SHELL_DEFAULT_USER          "GUI"
#endif /** SHELL_DEFAULT_USER */

#ifndef SHELL_DEFAULT_USER_PASSWORD
/**
 * @brief Shell default user password
 */
#define     SHELL_DEFAULT_USER_PASSWORD ""
#endif /** SHELL_DEFAULT_USER_PASSWORD */

#ifndef SHELL_LOCK_TIMEOUT
/**
 * @brief Shell auto lock timeout
 */
#define     SHELL_LOCK_TIMEOUT          0 * 60 * 1000
#endif /** SHELL_LOCK_TIMEOUT */

#ifndef SHELL_USING_FUNC_SIGNATURE
/**
 * @brief Use function signature
 */
#define     SHELL_USING_FUNC_SIGNATURE  0
#endif /** SHELL_USING_FUNC_SIGNATURE */

#ifndef SHELL_SUPPORT_ARRAY_PARAM
/**
 * @brief Support array parameters
 */
#define     SHELL_SUPPORT_ARRAY_PARAM   0
#endif /** SHELL_SUPPORT_ARRAY_PARAM */

#endif
