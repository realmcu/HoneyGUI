/**
 * @file shell_ext.h
 * @author Letter (NevermindZZT@gmail.com)
 * @brief shell extensions
 * @version 3.0.0
 * @date 2019-12-31
 *
 * @copyright (c) 2019 Letter
 *
 */

#ifndef __SHELL_EXT_H__
#define __SHELL_EXT_H__

#include "shell.h"

/**
 * @brief
 *
 */
typedef enum
{
    NUM_TYPE_DEC,                                           /**<  */
    NUM_TYPE_BIN,                                           /**<  */
    NUM_TYPE_OCT,                                           /**<  */
    NUM_TYPE_HEX,                                           /**<  */
    NUM_TYPE_FLOAT                                          /**<  */
} ShellNumType;

unsigned int shellExtParsePara(Shell *shell, char *string);
int shellExtRun(Shell *shell, ShellCommand *command, int argc, char *argv[]);

#endif
