/**
 * @file shell_port.c
 * @author Letter (NevermindZZT@gmail.com)
 * @brief
 * @version 0.1
 * @date 2019-02-22
 *
 * @copyright (c) 2019 Letter
 *
 */

#include "shell.h"
#include "shell_port.h"
#include <sys/time.h>
#include <stdio.h>


Shell shell;
char shellBuffer[512];

/**
 * @brief 获取系统tick
 *
 * @return unsigned int 系统tick
 */
unsigned int userGetTick()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000 + tv.tv_usec / 1000) & 0x7FFFFFFF;
}

/**
 * @brief 用户shell写
 *
 * @param data 数据
 * @param len 数据长度
 *
 * @return short 实际写入的数据长度
 */
signed short userShellWrite(char *data, unsigned short len)
{
    unsigned short length = len;
    while (length--)
    {
        putchar(*data++);
    }
    return len;
}


/**
 * @brief 用户shell读
 *
 * @param data 数据
 * @param len 数据长度
 *
 * @return short 实际读取到
 */
signed short userShellRead(char *data, unsigned short len)
{
    unsigned short length = len;
    while (length--)
    {
        *data++ = getchar();
    }
    return len;
}
/**
 * @brief 用户shell初始化
 *
 */
void userShellInit(void)
{
    shell.write = userShellWrite;
    shell.read = userShellRead;

    shellInit(&shell, shellBuffer, 512);
}
//CEVENT_EXPORT(EVENT_INIT_STAGE2, userShellInit);

