/**
*****************************************************************************************
*     Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_api_fs.h
  * @brief Application Programming Interface for UI
  * @details File System
  * @author sienna_shen@realsil.com.cn
  * @date 2025/3/5
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2025 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef __GUI_API_FS_H__
#define __GUI_API_FS_H__
#ifdef __cplusplus
extern "C" {
#endif


/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <guidef.h>

/*============================================================================*
 *                         Macros
 *============================================================================*/

#define GUI_FS_O_BINARY 0100000

/*============================================================================*
 *                         Functions
 *============================================================================*/

int gui_open(const char *file, int flags);
int gui_close(int d);
int gui_read(int fd, void *buf, size_t len);
int gui_write(int fd, const void *buf, size_t len);
int gui_lseek(int fd, int offset, int whence);
int gui_closedir(gui_dir_t *d);
int gui_ioctl(int fd, int cmd);
gui_dir_t *gui_opendir(const char *name);
struct gui_fs_dirent *gui_readdir(gui_dir_t *d);
void *gui_get_file_address(const char *file);

#ifdef __cplusplus
}
#endif
#endif
