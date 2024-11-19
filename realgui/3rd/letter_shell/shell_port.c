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
    shellFs.getcwd = (size_t (*)(char *, size_t))getcwd;
    shellFs.chdir = (size_t (*)(char *))chdir;
    shellFs.listdir = userShellListDir;
    shellFsInit(&shellFs, shellPathBuffer, 512);

    shell.write = userShellWrite;
    shell.read = userShellRead;
    shellSetPath(&shell, shellPathBuffer);
    shellInit(&shell, shellBuffer, 512);
    shellCompanionAdd(&shell, SHELL_COMPANION_ID_FS, &shellFs);
}
//CEVENT_EXPORT(EVENT_INIT_STAGE2, userShellInit);



#include <pthread.h>
#include <stdbool.h>
static void *shell_task(void *arg)
{
    char data;
    while (true)
    {
        if (shell.read && shell.read(&data, 1) == 1)
        {
            shellHandler(&shell, data);
        }
    }
}
static int gui_port_console_init(void)
{
    pthread_t thread;
    userShellInit();
    pthread_create(&thread, NULL, shell_task, NULL);
    return 0;
}


#ifdef ENABLE_RTK_GUI_CONSOLE
#include "gui_components_init.h"
GUI_INIT_COMPONENT_EXPORT(gui_port_console_init);

void func(int i, char ch, char *str)
{
    printf("\033[1;31m");
    printf("Hello, World!\n");

    printf("\033[0m");
    printf("Hello, World again!\n");
}
SHELL_EXPORT_CMD(
    SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC) | SHELL_CMD_DISABLE_RETURN,
    io, func, test);

#endif

