/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_api.h
  * @brief Application Programming Interface for UI
  * @details file system; operation system; hardware;
  * @author howie_wang@realsil.com.cn
  * @date 2023/10/17
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef __GUI_API_H__
#define __GUI_API_H__
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

/** @defgroup SERVER SERVER
  * @brief
  * @{
  */

/*============================================================================*
 *                         Types
 *============================================================================*/
/** @defgroup SERVER_Exported_Types SERVER Exported Types
  * @brief
  * @{
  */

/** @brief  ... */



/** End of SERVER_Exported_Types
  * @}
  */

/*============================================================================*
 *                         Constants
 *============================================================================*/
/** @defgroup SERVER_Exported_Constants SERVER Exported Constants
  * @brief
  * @{
  */


/** End of SERVER_Exported_Constants
  * @}
  */

/*============================================================================*
 *                         Macros
 *============================================================================*/
/** @defgroup SERVER_Exported_Macros SERVER Exported Macros
  * @brief
  * @{
  */


#define GUI_ASSERT(EX)                                                         \
    if (!(EX))                                                                    \
    {                                                                             \
        gui_assert_handler(#EX, __FUNCTION__, __LINE__);                           \
    } //!< assert here
#define GUI_LINE(EX)                                                         \
    if ((EX))                                                                    \
    {                                                                             \
        gui_log("[GUI] debug at function:%s, line number:%d \n", __FUNCTION__, __LINE__); \
    } //!< print line number
#define __param_name(line) param_##line
#define _param_name(line) __param_name(line)
#define param_name() _param_name(__LINE__)
/*
#define GUI_TIME_TICK()  uint32_t param_name() = rt_tick_get();
#define GUI_TIME_END(start)  uint32_t param_name() = rt_tick_get(); \
    gui_log("[%s: %d] total time is %dms\n", __func__, __LINE__,param_name() - param_##start);



#define PF_TIME_TICK()  uint32_t param_name() = ((*(uint32_t *)(0x4005817C)) & 0x3FFFFFF);
#define PF_TIME_END(start)  extern uint32_t SystemCpuClock; \
    uint32_t param_name() = ((*(uint32_t *)(0x4005817C)) & 0x3FFFFFF); \
    gui_log("line = %d, s = 0x%x, e = 0x%x; t=%ld us\n", \
               __LINE__,param_name(), param_##start, ((param_name() - param_##start) ) / 90);
*/
#define PRINT_MATRIX(matrix) gui_log("-----------------MATRIX------------------------\n"); \
    gui_log("%f    ",matrix->m[0][0]); \
    gui_log("%f    ",matrix->m[0][1]); \
    gui_log("%f  \n",matrix->m[0][2]); \
    gui_log("%f    ",matrix->m[1][0]); \
    gui_log("%f    ",matrix->m[1][1]); \
    gui_log("%f  \n",matrix->m[1][2]); \
    gui_log("%f    ",matrix->m[2][0]); \
    gui_log("%f    ",matrix->m[2][1]); \
    gui_log("%f  \n",matrix->m[2][2]); \
    gui_log("-----------------------------------------------\n");  //!< print matrix(3*3)

#define PRINT_POX(pox)       gui_log("-----------------POX---------------------------\n"); \
    gui_log("%f  \n",(pox)->p[0]); \
    gui_log("%f  \n",(pox)->p[1]); \
    gui_log("%f  \n",(pox)->p[2]); \
    gui_log("-----------------------------------------------\n"); //!< print position

/** End of SERVER_Exported_Macros
  * @}
  */



/*============================================================================*
 *                         Functions
 *============================================================================*/
/** @defgroup SERVER_Exported_Functions SERVER Exported Functions
  * @brief
  * @{
  */

extern float gui_get_scale_x(void);
extern float gui_get_scale_y(void);
void gui_dc_info_register(struct gui_dispdev *info);
char gui_dc_get_gpu_type(void);
unsigned char *gui_dc_get_fb(void);
void gui_dc_set_gpu_type(char type);
void gui_os_api_register(struct gui_os_api *info);

void gui_indev_info_register(struct gui_indev *info);

void ext_button_set_indicate(void (*callback)(void));

gui_touch_port_data_t *touchpad_get_data(void);

gui_kb_port_data_t *kb_get_data(void);

gui_wheel_port_data_t *wheel_get_data(void);

struct gui_indev *gui_get_indev(void);
struct gui_dispdev *gui_get_dc(void);

float gui_get_scale_x(void);

float gui_get_scale_y(void);

uint32_t gui_get_screen_width(void);

uint32_t gui_get_screen_height(void);
int gui_flash_boost(void);
int gui_flash_boost_disable(void);
void *gui_thread_create(const char *name, void (*entry)(void *param), void *param,
                        uint32_t stack_size, uint8_t priority);

bool gui_thread_delete(void *handle);

bool gui_thread_mdelay(uint32_t ms);

void gui_set_tick_hook(void (*hook)(void));

bool gui_thread_suspend(void *handle);

bool gui_thread_resume(void *handle);

bool gui_mq_create(void *handle, const char *name, uint32_t msg_size, uint32_t max_msgs);

bool gui_mq_send(void *handle, void *buffer, uint32_t size, uint32_t timeout);

bool gui_mq_send_urgent(void *handle, void *buffer, uint32_t size, uint32_t timeout);

bool gui_mq_recv(void *handle, void *buffer, uint32_t size, uint32_t timeout);

void *gui_malloc(uint32_t n);
void *gui_realloc(void *ptr_old, uint32_t n);
void gui_mem_debug(void);


void gui_free(void *rmem);

void gui_assert_handler(const char *ex_string, const char *func, uint32_t line);

int gui_fs_open(const char *file, int flags);
int gui_fs_close(int d);
int gui_fs_read(int fd, void *buf, size_t len);
int gui_fs_write(int fd, const void *buf, size_t len);
int gui_fs_lseek(int fd, int offset, int whence);
int gui_fs_closedir(gui_fs_DIR *d);
int gui_fs_ioctl(int fd, int cmd);
gui_fs_DIR *gui_fs_opendir(const char *name);
struct gui_fs_dirent *gui_fs_readdir(gui_fs_DIR *d);
void *gui_get_file_address(const char *file);

char *gui_strdup(const char *s);
void gui_log(const char *format, ...);
void gui_log_hexdump(const char *name, uint8_t *buf, uint16_t size);
void gui_display_on(void);
void gui_display_off(void);
uint32_t gui_ms_get(void);
uint32_t gui_us_get(void);
void gui_task_ext_execution_sethook(void (*hook)(void));

/** End of SERVER_Exported_Functions
  * @}
  */

/** End of SERVER
  * @}
  */
#ifdef __cplusplus
}
#endif
#endif
