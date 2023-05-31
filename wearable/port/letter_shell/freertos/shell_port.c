/**
 * @file shell_companion.c
 * @author Letter (nevermindzzt@gmail.com)
 * @brief shell companion object support
 * @version 3.0.3
 * @date 2020-07-22
 *
 * @copyright (c) 2020 Letter
 *
 */
#include "shell.h"
#include "string.h"
#include "stdio.h"
#include "stdarg.h"
#include "shell_ext.h"
#include "rtl_uart.h"
#include "rtl_pinmux.h"
#include "rtl_rcc.h"
#include "os_task.h"
#include "os_sync.h"
#include "trace.h"
#include "shell_port.h"
#include "drv_uart.h"

Shell shell_user;
char shellBuffer[512];
//static SemaphoreHandle_t shellMutex;
void *shell_task_handle;
/**
 * @brief
 *
 * @param data
 * @param len
 *
 * @return short
 */
static short userShellWrite(char *data, unsigned short len)
{
    //return mcu_uart_write(data, len);
    return drv_uart4_write(data, len);
}

static void user_func(uint8_t data)
{
    shellHandler(&shell_user, data);
}

#if 0
/**
 * @brief
 *
 * @param data
 * @param len
 *
 * @return short
 */
short userShellRead(char *data, unsigned short len)
{

}

/**
 * @brief
 *
 * @param shell shell
 *
 * @return int 0
 */
int userShellLock(Shell *shell)
{
    xSemaphoreTakeRecursive(shellMutex, portMAX_DELAY);
    return 0;
}

/**
 * @brief
 *
 * @param shell shell
 *
 * @return int 0
 */
int userShellUnlock(Shell *shell)
{
    xSemaphoreGiveRecursive(shellMutex);
    return 0;
}
#endif
/**
 * @brief
 *
 */

void userShellInit(void)
{
    //uart_config[LETTER_SHELL_UART_INDEX].uart_user_def = user_func;

    shell_user.write = userShellWrite;
    //shell_user.read = userShellRead;
    //shell.lock = userShellLock;
    //shell.unlock = userShellUnlock;
    shellInit(&shell_user, shellBuffer, 512);
}

