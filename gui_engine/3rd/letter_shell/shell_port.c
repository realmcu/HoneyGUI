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

#include <sys/time.h>
#include <stdio.h>
#include <dirent.h>
#include "shell.h"
#include "shell_port.h"
#include "shell_fs.h"

Shell shell;
char shellBuffer[512];
ShellFs shellFs;
char shellPathBuffer[512] = "/";

/**
 * @brief Get system tick
 */
unsigned int userGetTick()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000 + tv.tv_usec / 1000) & 0x7FFFFFFF;
}

/**
 * @brief Shell write for user
 *
 * @param data
 * @param len
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
 * @brief Shell read for user
 *
 * @param data
 * @param len
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
 * @brief List directory
 *
 * @param path
 * @param buffer
 * @param maxLen
 * @return size_t
 */
size_t userShellListDir(char *path, char *buffer, size_t maxLen)
{
    DIR *dir;
    struct dirent *ptr;
    int i;
    dir = opendir(path);
    memset(buffer, 0, maxLen);
    while ((ptr = readdir(dir)) != NULL)
    {
        strcat(buffer, ptr->d_name);
        strcat(buffer, "\t");
    }
    closedir(dir);
    return 0;
}

/**
 * @brief User shell initialization
 */
void userShellInit(void)
{
    shellFs.getcwd = getcwd;
    shellFs.chdir = chdir;
    shellFs.listdir = userShellListDir;
    shellFsInit(&shellFs, shellPathBuffer, 512);

    shell.write = userShellWrite;
    shell.read = userShellRead;
    shellSetPath(&shell, shellPathBuffer);
    shellInit(&shell, shellBuffer, 512);
    shellCompanionAdd(&shell, SHELL_COMPANION_ID_FS, &shellFs);
}
//CEVENT_EXPORT(EVENT_INIT_STAGE2, userShellInit);

void gui_port_console_init(void)
{
    userShellInit();
    shellTask(&shell);
}
