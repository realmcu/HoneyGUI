/**
 \internal
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_list.h
  * @brief The list widget is a container for any number of list note, and each note can have other widgets added to it.
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
#include "gui_img.h"

/*============================================================================*
 *                         Types
 *============================================================================*/
/* LIST_STYLE enum start*/
typedef enum
{
    LIST_CLASSIC = 0, ///< Classic list.
    LIST_CIRCLE,           ///< Circle list.
    LIST_ZOOM,             ///< Zoom center list.
    LIST_CARD,             ///< Stack like card.
    LIST_FAN,              ///< Rotate like fan.
    LIST_HELIX,            ///< Rotate like helix.
    LIST_CURL,             ///< Rotate curly.
    LIST_FADE,             ///< Fade in.
} LIST_STYLE;

/* LIST_STYLE enum end*/

typedef enum
{
    VERTICAL    = 0,
    HORIZONTAL  = 1,
} LIST_DIR;

/** @brief List Structure. */
typedef struct gui_list
{
    gui_obj_t base;
    uint32_t dir             : 1; // 0:vertical, 1:horizontal.
    uint32_t style           : 4; // List style.
    uint32_t auto_align      : 1; // Automatic alignment of notes.
    uint32_t inertia         : 1; // Enable inertia effect while tp released.
    uint32_t need_update_bar : 1;
    uint32_t note_num        : 8; // number of whole notes.
    uint32_t space           : 8;

    uint16_t note_length;        // List note length.
    int16_t speed;
    int16_t record[5];
    float factor;                // (0~1.0] Deceleration factor, defaults to 0.05.

    int offset;                  // offset = hold + tp_delta, when sliding.
    int hold;
    int total_length;
    int16_t out_scope;           // Out scope of list. Don't support CARD style.
    int16_t card_stack_location; // The distance from stack location to the screen bottom. Only support CARD style.

    uint16_t max_created_note_index; // Max index of the created notes.
    uint16_t last_created_note_index; // Index of the last created note.
    void (* note_design)(gui_obj_t *obj, void *param);
    void *design_param;

    gui_img_t *bar;
    void *bar_data;
    gui_color_t bar_color;

    uint8_t checksum;
} gui_list_t;

/** @brief List Note Structure. */
typedef struct gui_list_note
{
    gui_obj_t base;
    int start_x;
    int start_y;
    int t_x;
    int t_y;
    uint16_t animate_cnt;
    bool is_speed_positive;   // To judge move direction.

    int16_t index;            // Index of the note in the list.

    uint8_t checksum;
} gui_list_note_t;

/*============================================================================*
 *                         Constants
 *============================================================================*/


/*============================================================================*
 *                         Macros
 *============================================================================*/
#define GUI_MAX_SPEED 50
#define GUI_MIN_SPEED 10
#define LIST_TAB_ANIMATE_MAX 15
#define LIST_BAR_WIDTH 5
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
 * @param x The X-axis coordinate relative to parent widget.
 * @param y The Y-axis coordinate relative to parent widget.
 * @param w Width.
 * @param h Height.
 * @param note_length The length of each note.
 * @param space The space of each note.
 * @param dir The direction of the list.
 * @return Return the widget object pointer.
 */
gui_list_t *gui_list_create(void       *parent,
                            const char *name,
                            int16_t     x,
                            int16_t     y,
                            int16_t     w,
                            int16_t     h,
                            uint16_t    note_length,
                            uint8_t     space,
                            LIST_DIR    dir,
                            void (* note_design)(gui_obj_t *obj, void *param),
                            void        *design_param,
                            bool        create_bar);

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

/**
 * @brief Set list bar color.
 * @param list Pointer to the list widget.
 * @param color The list bar color.
 */
void gui_list_set_bar_color(gui_list_t *list, gui_color_t color);

/**
 * @brief Set note number of list.
 * @param list Pointer to the list widget.
 * @param num The spacific number, must be a nonnegative number.
 */
void gui_list_set_note_num(gui_list_t *list, uint16_t num);

/**
 * @brief Set card stack location, only valid when list style is LIST_CARD.
 * @param location The distance from stack location to the screen bottom of right.
 */
void gui_list_set_card_stack_location(gui_list_t *list, int16_t location);

/**
 * @brief Set out scope of list, which is the distance that can be slightly exceeded when scrolling.
 * @param out_scope The out scope of list.
 */
void gui_list_set_out_scope(gui_list_t *list, int16_t out_scope);

/**
 * @brief Set auto align of list, which is used to automatically align notes when the list stops moving.
 * @param auto_align true: enable auto align, false: disable auto align.
 */
void gui_list_set_auto_align(gui_list_t *list, bool auto_align);

/**
 * @brief Set inertia of list, which is used to enable inertia effect while tp released.
 * @param inertia Default is true. true: enable inertia, false: disable inertia.
 */
void gui_list_set_inertia(gui_list_t *list, bool inertia);

#ifdef __cplusplus
}
#endif
#endif
