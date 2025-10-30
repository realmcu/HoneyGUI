/**
\internal
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_turn_table.h
  * @brief turn_table design
  * @details Evenly layout vertically and horizontally
  * @author luke_sun@realsil.com.cn
  * @date 2024/5/10
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
#ifndef __GUI_turn_table_H__
#define __GUI_turn_table_H__



#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <guidef.h>
#include <gui_fb.h>
#include "gui_obj.h"
#include "gui_img.h"
#include "math.h"


/*============================================================================*
 *                         Types
 *============================================================================*/

typedef enum turn_table_state
{
    TT_RESTING,
    TT_ANTICLOCKWISE,
    TT_CLOCKWISE,
} TURN_TABLE_STATE_T;

typedef struct gui_turn_table_coordinate
{
    uint16_t x;
    uint16_t y;
} gui_turn_table_coordinate_t;

typedef struct gui_turn_table
{
    gui_obj_t base;
    uint8_t checksum;
    bool skip_up_tp;
    bool skip_down_tp;
    bool skip_left_tp;
    bool skip_right_tp;
//icon
    uint8_t current_icon_index;
    uint8_t icon_quantity;
    uint8_t icon_radius;
    uint8_t icon_margin;
    uint8_t icon_distence;
    uint8_t screen_fillet_radius;
    uint8_t top_bottom_icon_quantity;
    uint8_t left_right_icon_quantity;
    uint8_t active_icon_quantity;
    gui_turn_table_coordinate_t *static_list;
    gui_img_t * *icon_list;
    float icon_scale;
    uint8_t *name_list;
//static
    gui_img_t *static_icon;
//highlight
    float icon_highlight_scale;
    gui_img_t *highlight_icon;
    uint8_t highlight_index;
//rotate
    float resetting_angle;
    float last_angle[3]; //last_angle[0] is newest, last_angle[2] is oldest
    int sector_index;
    float sector_angle;
    TURN_TABLE_STATE_T state;
    int turn_count;
    bool need_update;
} gui_turn_table_t;

/*============================================================================*
 *                         Constants
 *============================================================================*/


/*============================================================================*
 *                         Macros
 *============================================================================*/

#define TT_IMG_NAME_LENGTH  10 //strlen(tt_img_xxx)

/*============================================================================*
 *                         Variables
 *============================================================================*/


/*============================================================================*
 *                         Functions
 *============================================================================*/

/**
 * @brief Create turn table widget.
 *
 * @param parent Pointer to parent app or container on which the turn table will be created
 * @param icon_quantity The number of icons to be displayed on the turn table
 * @param icon_radius The radius of the icons, usually half of the icon width, used to control the size of the icons
 * @param top_bottom_icon_quantity The number of icons at the top and bottom
 * @param left_right_icon_quantity The number of icons on the left and right sides
 * @return gui_turn_table_t* Returns a pointer to the newly created turn table
 */
gui_turn_table_t *gui_turn_table_create(void *parent,
                                        uint8_t icon_quantity,
                                        uint8_t icon_radius,
                                        uint8_t top_bottom_icon_quantity,
                                        uint8_t left_right_icon_quantity);

/**
 * @brief Sets the edge icon quantities for the turn table GUI.
 *
 * This function sets the quantities of different icons appearing on the top/bottom and left/right edges of the GUI turn table.
 *
 * @param this Pointer to the GUI turn table object.
 * @param top_bottom_quantity The desired quantity of icons to be displayed on the top and bottom edges of the turn table.
 * @param left_right_quantity The desired quantity of icons to be displayed on the left and right edges of the turn table.
 *
 */
void gui_turn_table_set_edge_icon_quantity(gui_turn_table_t *this,
                                           uint8_t top_bottom_quantity,
                                           uint8_t left_right_quantity);

/**
 * @brief
 *
 * @param this
 * @param icon_margin
 * @param icon_distence
 * @param screen_fillet_radius
 */
void gui_turn_table_generate_layout_by_calculate(gui_turn_table_t *this,
                                                 uint8_t icon_margin,
                                                 uint8_t icon_distence,
                                                 uint8_t screen_fillet_radius);

/**
 * @brief Set turn table tp.
 *
 * @param this Pointer to the GUI turn table object
 * @param up If up is true, touchpad 'up' sustain is allowed.
 * @param down If down is true, touchpad 'down' sustain is allowed.
 * @param left If left is true, touchpad 'left' sustain is allowed.
 * @param right If right is true, touchpad 'right' sustain is allowed.
 */
void gui_turn_table_set_tp(gui_turn_table_t *this,
                           bool up,
                           bool down,
                           bool left,
                           bool right);

/**
 * @brief Generates a layout for the turn table GUI using a specified array.
 *
 * This function sets up the layout configuration of the GUI turn table using a given
 * fixed point array and the active icon quantity.
 * If the supplied active icon quantity does not match the one set on the turn table,
 * it logs an error message and returns without creating the layout.
 *
 * @param this Pointer to the GUI turn table object.
 * @param fixed_point The array of coordinates used to generate the layout of the turn table.
 * @param active_icon_quantity The quantity of active icons that will be displayed on the turn table.
 *
 * @note Assumes centerX and centerY to be at the center of the icon after scale.
 *       The offset from the fixed point to the center of the scaled icon is the icon's radius times its scale.
 *
 * @return Void
 */
void gui_turn_table_generate_layout_by_array(gui_turn_table_t *this,
                                             gui_turn_table_coordinate_t *fixed_point,
                                             uint8_t active_icon_quantity);

/**
 * @brief Adds a default icon to the turn table GUI.
 *
 * This function adds an icon to the GUI turn table at a specific location described in the `static_list`.
 * If the `icon_list` at the index is not NULL, it will continue to the next index.
 * The function also scales the added icon according to the `icon_scale` of the table.
 * If the highlight index matches the current index, it will generate a highlighted icon at screen center.
 *
 * @param this Pointer to the GUI turn table object.
 * @param icon_addr The address of the icon to be added.
 * @param event_cb This is the callback function that will be called when the icon is clicked.
 *
 * @note The x & y for the non-active icons are the same as the first active icon.
 * @note The function will return immediately after an icon is added.
 *
 * @return Void
 */
void gui_turn_table_add_icon_default(gui_turn_table_t *this,
                                     void *icon_addr,
                                     gui_event_cb_t event_cb);

/**
 * @brief Sets the highlight index for the turn table GUI.
 *
 * This function sets the index at which the icon will be highlighted in the GUI turn table.
 *
 * @param this Pointer to the GUI turn table object.
 * @param highlight_index Index of the icon to be highlighted.
 *
 * @return Void
 */
void gui_turn_table_set_highlight_index(gui_turn_table_t *this,
                                        uint8_t highlight_index);

/**
 * @brief Sets the scale of icons for the turn table GUI.
 *
 * This function sets the scale for all icons and the highlighted icon in the GUI turn table.
 *
 * @param this Pointer to the GUI turn table object on which the operation is performed.
 * @param icon_scale The scale for all icons.
 * @param icon_highlight_scale The scale for highlighted icon.
 *
 * @return Void
 */
void gui_turn_table_set_icon_scale(gui_turn_table_t *this,
                                   float icon_scale,
                                   float icon_highlight_scale);

/**
 * @brief Adds a static icon to the turn table GUI.
 *
 * This function creates an image from the provided memory address and adds it to the GUI turn table.
 * If a static icon already exists, it is freed before the new icon is added.
 * The new icon is scaled according to the turn table's icon scale.
 *
 * @param this Pointer to the GUI turn table object on which the operation is performed.
 * @param icon_addr The memory address of the icon to be added.
 *
 * @return Void
 */
void gui_turn_table_add_icon_static(gui_turn_table_t *this,
                                    void *icon_addr);


#ifdef __cplusplus
}
#endif



#endif



