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
        gui_log("\033[1;31m");   \
        gui_log("[GUI] debug at function:%s, line number:%d \n", __FUNCTION__, __LINE__); \
        gui_log("\033[0m");   \
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

/** @brief white color in gui_color_t structure */
#define APP_COLOR_WHITE                     gui_rgba(UINT8_MAX, UINT8_MAX, UINT8_MAX, UINT8_MAX)
/** @brief red color in gui_color_t structure */
#define APP_COLOR_RED                       gui_rgba(UINT8_MAX, 0, 0, UINT8_MAX)
/** @brief green color in gui_color_t structure */
#define APP_COLOR_GREEN                     gui_rgba(0, UINT8_MAX, 0, UINT8_MAX)
/** @brief blue color in gui_color_t structure */
#define APP_COLOR_BLUE                      gui_rgba(0, 0, UINT8_MAX, UINT8_MAX)

/**
 * @link https://www.rapidtables.com/web/color/RGB_Color.html
 * @brief APP_COLOR_##HTML / CSS Name       Decimal Code(R,G,B)
*/
#define APP_COLOR_CRIMSON                   gui_rgb(220,20,60)
#define APP_COLOR_FIREBRICK                 gui_rgb(178,34,34)
#define APP_COLOR_WHITE_OPACITY             gui_rgba(255,255,255,150)
#define APP_COLOR_SILVER                    gui_rgb(192,192,192)
#define APP_COLOR_SILVER_OPACITY(opacity)   gui_rgba(192,192,192, opacity)
#define APP_COLOR_BLACK                     gui_rgb(0,0,0)
#define APP_COLOR_GRAY                      gui_rgb(128,128,128)
#define APP_COLOR_BLACK_OPACITY(opacity)    gui_rgba(0,0,0, opacity)
#define APP_COLOR_GRAY_OPACITY(opacity)     gui_rgba(128,128,128, opacity)
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

void gui_dc_info_register(struct gui_dispdev *info);
void gui_acc_info_register(struct acc_engine *info);
void gui_os_api_register(struct gui_os_api *info);
void gui_indev_info_register(struct gui_indev *info);




void ext_button_set_indicate(void (*callback)(void));

gui_touch_port_data_t *touchpad_get_data(void);

gui_kb_port_data_t *kb_get_data(void);

gui_wheel_port_data_t *wheel_get_data(void);

struct gui_indev *gui_get_indev(void);
struct gui_dispdev *gui_get_dc(void);
struct acc_engine *gui_get_acc(void);


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

void *gui_malloc(size_t n);
void *gui_realloc(void *ptr_old, size_t n);
void gui_free(void *rmem);
void gui_mem_debug(void);

void *gui_lower_malloc(size_t n);
void *gui_lower_realloc(void *ptr_old, size_t n);
void gui_lower_free(void *rmem);
void gui_lowr_mem_debug(void);
void gui_sleep_cb(void);

void gui_assert_handler(const char *ex_string, const char *func, uint32_t line);

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
char *gui_strdup(const char *s);
char *gui_filepath_transforming(void *addr);

/**
 * @brief Read from flash translation layer
 *
 * @param addr The starting address to read from.
 * @param buf Buffer to store the read data.
 * @param len Number of bytes to read.
 * @return int
 */
int gui_ftl_read(uint32_t addr, uint8_t *buf, uint32_t len);

/**
 * @brief Write to the flash translation layer
 *
 * @param addr The starting address to write to.
 * @param buf The buffer containing the data to write.
 * @param len The number of bytes to write.
 * @return int
 */
int gui_ftl_write(uint32_t addr, const uint8_t *buf, uint32_t len);

/**
 * @brief Erase a section of the flash translation layer
 *
 * @param addr The starting address of the section to erase.
 * @param len The number of bytes to erase.
 * @return int
 */
int gui_ftl_erase(uint32_t addr, uint32_t len);

void gui_log(const char *format, ...);
void gui_log_hexdump(const char *name, uint8_t *buf, uint16_t size);
void gui_display_on(void);
void gui_display_off(void);
uint32_t gui_ms_get(void);
uint32_t gui_us_get(void);
void gui_task_ext_execution_sethook(void (*hook)(void));

/**
 * @brief Get a color value from red, green, blue and opacity values.
 *
 * @param red  Range of unsigned char
 * @param green Range of unsigned char
 * @param blue Range of unsigned char
 * @param opacity Range of unsigned char
 * @return gui_color_t
 */
gui_color_t gui_rgba(unsigned char red,
                     unsigned char green,
                     unsigned char blue,
                     unsigned char opacity);

/**
 * @brief Get a color value from red, green and blue.
 *
 * @param red  Range of unsigned char
 * @param green Range of unsigned char
 * @param blue Range of unsigned char
 * @return gui_color_t
 */
gui_color_t gui_rgb(unsigned char red, unsigned char green, unsigned char blue);
/**
 * @brief Get fps.
 *
 * @return fps
 */
uint32_t gui_fps(void);
/**
 * @brief Get ram used in byte.
 *
 * @return uint32_t
 */
uint32_t gui_mem_used(void);
/**
 * @brief Get low ram (another ram) used in byte.
 *
 * @return uint32_t
 */
uint32_t gui_low_mem_used(void);

/**
 * @brief Play the specified music file.
 *
 * This function starts playing the music file specified by the path `music_file`.
 *
 * @param[in] music_file The path to the music file to play.
 *
 * @return #GUI_SUCCESS if the operation was successful.
 */
gui_error_t gui_music_play(const char *music_file);

/**
 * @brief Stop the currently playing music.
 *
 * This function stops playback of any music currently being played.
 *
 * @return #GUI_SUCCESS if the music was successfully stopped.
 */
gui_error_t gui_music_stop(void);

/**
 * @brief Check if the music playback has completed.
 *
 * This function checks if the currently playing music has finished playing.
 *
 * @return true if the music playback has completed.
 * @return false if the music is still playing.
 */
bool gui_music_completion_status(void);
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
