/**
 \internal
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_list.c
  * @brief the list widget is a container for any number of list notele, and each notele can have other widgets added to it.
  * @author shel_deng@realsil.com.cn
  * @date 2025/04/23
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
#ifndef __GUI_LIST_H__
#define __GUI_LIST_H__
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "guidef.h"
#include "gui_fb.h"
#include "gui_obj.h"
#include "gui_matrix.h"

/*============================================================================*
 *                         Types
 *============================================================================*/
/* LIST_STYLE enum start*/
typedef enum
{
    LIST_CLASSIC = 0x0000, ///< Classic list
    LIST_CIRCLE,           ///< Circle list
    LIST_ZOOM,             ///< Zoom center list
    LIST_CARD,             ///< Stack like card
    LIST_FAN,              ///< Rotate like fan
    LIST_HELIX,            ///< Rotate like helix
    LIST_CURL,             ///< Rotate curly
} LIST_STYLE;

/* LIST_STYLE enum end*/

typedef enum
{
    VERTICAL    = 0x0000,
    HORIZONTAL  = 0x0001,
} LIST_DIR;

/** @brief  List Structure */

typedef struct gui_list
{
    gui_obj_t base;
    LIST_STYLE style;
    LIST_DIR dir;        // 0:vertical, 1:horizontal
    uint8_t widget_num;
    uint8_t space;
    uint16_t note_length; // list note length
    int16_t speed;
    int16_t record[5];
    float factor;        // (0~1.0] deceleration factor, defaults to 0.05

    int offset;          // offset = hold + tp_delta, when sliding
    int hold;
    int total_length;

    uint8_t checksum;
} gui_list_t;

/** @brief  List_note Structure */
typedef struct gui_list_note
{
    gui_obj_t base;
    int start_x;
    int start_y;
    int t_x;
    int t_y;
    uint16_t animate_cnt;
    bool is_speed_positive;    // to judge move direction

    uint8_t checksum;
} gui_list_note_t;

/*============================================================================*
 *                         Constants
 *============================================================================*/


/*============================================================================*
 *                         Macros
 *============================================================================*/
#define OUT_SCOPE 0 // out scope of list
#define GUI_MAX_SPEED 50
#define GUI_MIN_SPEED 7
#define LIST_TAB_ANIMATE_MAX 15
/*============================================================================*
 *                         Variables
 *============================================================================*/


/*============================================================================*
 *                         Functions
 *============================================================================*/

/**
 * @brief Create a list widget.
 * @param parent The father widget it nested in.
 * @param name The name of the widget.
 * @param x The X-axis coordinate relative to parent widget
 * @param y The Y-axis coordinate relative to parent widget
 * @param w Width
 * @param h Height
 * @param note_length The length of each note.
 * @param space The space of each note.
 * @param dir The direction of the list.
 * @return return the widget object pointer.
 */
gui_list_t *gui_list_create(void       *parent,
                            const char *name,
                            int16_t     x,
                            int16_t     y,
                            int16_t     w,
                            int16_t     h,
                            uint16_t    note_length,
                            uint8_t     space,
                            LIST_DIR    dir);

/**
 * @brief Add list note.
 * @param list The list widget it nested in.
 * @param add_at_top True: Add note at top of the list.
 * @return return the note object pointer.
 */
gui_list_note_t *gui_list_add_note(gui_list_t *list, bool add_at_top);

/**
 * @brief Set list moving style.
 * @param list Pointer to the list widget.
 * @param style The moving style of the list.
 */
void gui_list_set_style(gui_list_t *list, LIST_STYLE style);

/**
 * @brief Set list deceleration factor, which defaults to 0.05.
 * @param list Pointer to the list widget.
 * @param factor The deceleration factor.
 */
void gui_list_set_factor(gui_list_t *list, float factor);

/**
 * @brief Set list offset, can be used to change list initial position.
 * @param list Pointer to the list widget.
 * @param offset The list offset.
 */
void gui_list_set_offset(gui_list_t *list, int16_t offset);


#ifdef __cplusplus
}
#endif
#endif
