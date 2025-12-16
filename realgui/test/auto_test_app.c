/**
 * @file auto_test_app.c
 * @author howie_wang (howie_wang@realtek.com.cn)
 * @brief
 * @version 0.1
 * @date 2024-06-26
 *
 * @copyright Copyright (c) 2024
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "gui_server.h"
#include "gui_components_init.h"
#include "gui_img.h"
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>

#define COMPILE_DATE __DATE__
#define COMPILE_TIME __TIME__

static void auto_test_app_entry(void)
{
    gui_log("auto_test_app start \n");
}

#if defined _HONEYGUI_SIMULATOR_
uint8_t resource_root[1024 * 1024 * 20];
#endif
static int fd;
static int auto_test_write_to_file(const void *buf, size_t len)
{
    write(fd, buf, strlen(buf));
}


static void sanitize_filename(char *filename)
{
    for (int i = 0; filename[i]; i++)
    {
        if (filename[i] == ' ' || filename[i] == ':')
        {
            filename[i] = '_';
        }
    }
}
static int auto_test_app_init(void)
{
    char filename[] = COMPILE_DATE "_" COMPILE_TIME;
    sanitize_filename(filename);
    char full_filename[256];

    snprintf(full_filename, sizeof(full_filename), "auto_test_%s.txt", filename);


    fd = open(full_filename, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (fd == -1)
    {
        fprintf(stderr, "Error opening file: %s\n", strerror(errno));
        GUI_ASSERT(fd != -1);
    }
    extern int (*write_to_file)(const void *buf, size_t len);
    write_to_file = auto_test_write_to_file;

    // if (close(fd) == -1) {
    //     fprintf(stderr, "Error closing file: %s\n", strerror(errno));
    //     return;
    // }

    extern int gui_server_init(void);
    gui_server_init();
    auto_test_app_entry();
    return 0;
}
GUI_INIT_APP_EXPORT(auto_test_app_init);




