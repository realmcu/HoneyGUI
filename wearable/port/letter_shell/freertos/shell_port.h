/*
 * @Author: your name
 * @Date: 2022-04-06 15:40:33
 * @LastEditTime: 2022-04-08 11:13:13
 * @LastEditors: Please set LastEditors
 * @Description: koroFileHeader https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 * @FilePath: \wristbande:\project\honeycomb_bee3pro1\HoneyComb\sdk\src\app\wristband\ports\letter_shell\letter_shell_user.h
 */

/*
 * Copyright (c) 2022
 *
 *
 *
 *
 *
 *
 */

#ifndef __SHELL_PORT_H__
#define __SHELL_PORT_H__


#ifdef __cplusplus
extern "C" {
#endif

#include "shell.h"

extern Shell shell_user;
#define WS_PRINTF  shellPrint

#define LOG_E(...)                     WS_PRINTF("\033[31;22m[E/FAL] (%s:%d) ", __FUNCTION__, __LINE__);WS_PRINTF(__VA_ARGS__);WS_PRINTF("\033[0m\n")
#define LOG_I(...)                     WS_PRINTF("\033[32;22m[I/FAL] ");                                WS_PRINTF(__VA_ARGS__);WS_PRINTF("\033[0m\n")

#ifdef __cplusplus
}
#endif

#endif
