/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_api_fs.h
  * @brief Application Programming Interface for UI
  * @details File System
  * @author sienna_shen@realsil.com.cn
  * @date 2025/3/5
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
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

void gui_change_default_path(int argc, char **argv);
int gui_fs_open(const char *file, int flags);
int gui_fs_close(int d);
int gui_fs_read(int fd, void *buf, size_t len);
int gui_fs_write(int fd, const void *buf, size_t len);
int gui_fs_lseek(int fd, int offset, int whence);
int gui_fs_closedir(gui_fs_dir *d);
int gui_fs_ioctl(int fd, int cmd);
gui_fs_dir *gui_fs_opendir(const char *name);
struct gui_fs_dirent *gui_fs_readdir(gui_fs_dir *d);
void *gui_get_file_address(const char *file);


/**
 * @brief Resolves a relative file path to an absolute file path.
 *
 * This function takes a relative file path and returns the corresponding absolute
 * file path.
 *
 * @param relative_path The relative path to be resolved. It should be a valid
 *                      relative path string.
 *
 * @return A pointer to a string containing the absolute file path. The caller
 *         is responsible for managing the memory of the returned path string.
 *         If the relative path cannot be resolved, NULL may be returned.
 */
const char *gui_get_path_by_relative(const char *relative_path);

char *gui_strdup(const char *s);
char *gui_filepath_transforming(void *addr);

#ifdef __cplusplus
}
#endif
#endif
