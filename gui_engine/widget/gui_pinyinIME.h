/**
\internal
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_pinyin_IME.h
  * @brief
  * @details
  * @author roy@realsil.com.cn
  * @date 2024/5/20
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
\endinternal
  */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef __GUI_PINYIN_IME_H__
#define __GUI_PINYIN_IME_H__
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "guidef.h"

/** @defgroup WIDGET WIDGET
  * @brief
  * @{
  */
/*============================================================================*
 *                         Types
 *============================================================================*/
/** @defgroup WIDGET_Exported_Types WIDGET Exported Types
  * @brief
  * @{
  */

/** @brief  image widget structure */

/** End of WIDGET_Exported_Types
  * @}
  */

/*============================================================================*
 *                         Constants
 *============================================================================*/
/** @defgroup WIDGET_Exported_Constants WIDGET Exported Constants
  * @brief
  * @{
  */
typedef enum
{
    IME_PAGE_FORWARD    = 0,
    IME_PAGE_BACKWARD   = 1,

} GUI_PINYIN_IME_t;



/** End of WIDGET_Exported_Constants
  * @}
  */

/*============================================================================*
 *                         Macros
 *============================================================================*/
/** @defgroup WIDGET_Exported_Macros WIDGET Exported Macros
  * @brief
  * @{
  */

#define CHAR_LEN_BYTE      (3) // depend on dictinary: UTF-8

/** End of WIDGET_Exported_Macros
  * @}
  */

/*============================================================================*
 *                         Variables
 *============================================================================*/
/** @defgroup WIDGET_Exported_Variables WIDGET Exported Variables
  * @brief
  * @{
  */

typedef struct
{
    uint16_t len;
    char *key;
    uint32_t offset;
    uint32_t num_wd;    // number of words
} Dict_info_t;

typedef struct
{
    uint32_t last_key_len: 8;           // last key len input
    uint16_t char_len_byte;          // how many bytes needed for one candidate word
    uint16_t cand_num;              // total number of candidate words from dictionary
    uint16_t cand_page_size;        // number of candidate words for each page
    uint16_t cand_page;             // candidate words page, count from 0
    uint8_t *buff_cand;             // [cand_page_size * char_len_byte], candidate words buff for keyboard
    uint32_t dict_len;              // dictionary length (key number)
    Dict_info_t *dict_info;         // dict info full list
    Dict_info_t *cand_dict_info;    // matched single dict info
    uint8_t *dict_addr;             // dictionary address in memory
    void *api;                      // ime api: gui_ime_api_t
} gui_ime_t;

typedef struct
{
    void (*reset_page)(gui_ime_t *this);
    void (*page_flip)(gui_ime_t *this, uint8_t dir);
    uint16_t (*get_page_size)(gui_ime_t *this);
    uint8_t *(*get_cand_buff)(gui_ime_t *this);
    uint16_t (*get_cand_num)(gui_ime_t *this);
    uint8_t (*get_char_len_byte)(gui_ime_t *this);
    void (*clear_cand)(gui_ime_t *this);
    uint16_t (*update_cand)(gui_ime_t *this, void *key, uint8_t len);

} gui_ime_api_t;

/** End of WIDGET_Exported_Variables
  * @}
  */

/*============================================================================*
 *                         Functions
 *============================================================================*/
/** @defgroup WIDGET_Exported_GUI_Functions WIDGET Exported Functions
  * @brief
  * @{
  */
/**
* @brief set dictonary and info file for ime
* @note
* @param this the ime object pointer
* @param file_dict_info   the dictionary info file
* @param file_dict the dictionary file
*/
void gui_IME_set_dict(gui_ime_t *this, void *file_dict_info, void *file_dict);

/**
 * @brief create a pinyin ime object
 * @note
 * @param kb_cand_page_size number of candidate words for one page, depended on keyboard ui layout
 */
gui_ime_t *gui_pinyin_IME_create(uint16_t kb_cand_page_size);

/** End of WIDGET_Exported_GUI_Functions
  * @}
  */

/** End of WIDGET
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif
