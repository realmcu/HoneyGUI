/**
 \internal
*****************************************************************************************
*     Copyright(c) 2023, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_obj.h
  * @brief  The object widget.
  * @details Create a object.
  * @author luke_sun@realsil.com.cn
  * @date 2023/10/25
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2023 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
 \endinternal
  */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef __GUI_OBJ_H__
#define __GUI_OBJ_H__



#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "guidef.h"
#include "gui_obj_type.h"
#include "gui_obj_event.h"
#include "gui_obj_tree.h"
#include "gui_obj_suppress.h"


/*============================================================================*
 *                         Types
 *============================================================================*/


/*============================================================================*
 *                         Constants
 *============================================================================*/


/*============================================================================*
 *                         Macros
 *============================================================================*/


/*============================================================================*
 *                         Variables
 *============================================================================*/


/*============================================================================*
 *                         Functions
 *============================================================================*/

/**
 * @brief Get the root GUI object.
 *
 * This function returns a pointer to the root GUI object in the widget tree.
 *
 * @return Pointer to the root GUI object.
 */
gui_obj_t *gui_obj_get_root(void);

/**
 * @brief Get the fake_root GUI object, which would not be drawn.
 *
 * This function returns a pointer to the fake_root GUI object in the widget tree.
 *
 * @return Pointer to the fake_root GUI object.
 */
gui_obj_t *gui_obj_get_fake_root(void);

#ifndef DOXYGEN
/**
  * @param _this pointer to the GUI image object.
  * @param parent the father widget it nested in.
  * @param filename the obj widget name.
  * @param x the X-axis coordinate of the widget.
  * @param y the Y-axis coordinate of the widget.
  * @param w the width of the widget.
  * @param h the height of the widget.
  * <b>Example usage</b>
  * \code{.c}
  * static void app_main_task(void *parent, const char *name, int16_t x,
                               int16_t y, int16_t w, int16_t h)
  * {
  *    gui_obj_t *base = (gui_obj_t *)this_widget;
  *    gui_obj_ctor(base, parent, name, x, y, w, h);
  *
  * }
  * \endcode
  */
void gui_obj_ctor(gui_obj_t  *_this,
                  gui_obj_t  *parent,
                  const char *name,
                  int16_t     x,
                  int16_t     y,
                  int16_t     w,
                  int16_t     h);
#endif

/**
 * @brief Create an obj widget.
 *
 * @param parent Parent widget it nested in.
 * @param name Obj widget name.
 * @param x X-axis coordinate of the widget.
 * @param y Y-axis coordinate of the widget.
 * @param w Width of the widget.
 * @param h Height of the widget.
 * @return Pointer to the created obj widget.
 */
gui_obj_t *gui_obj_create(void       *parent,
                          const char *name,
                          int16_t     x,
                          int16_t     y,
                          int16_t     w,
                          int16_t     h);

/**
  * @brief Set object show or not.
  * @param obj Root of the widget tree.
  * @param enable True for show, false for hide.
  * * <b>Example usage</b>
  * \code{.c}
  * static void app_main_task(void)
  * {
  *     gui_img_t *hour;
  *     gui_obj_show(hour,false);
  *     gui_obj_show(hour,true);
  *
  * }
  *
  * \endcode
  */
void gui_obj_show(void *obj, bool enable);


/**
 * @brief Judge the obj if out of screen.
 */
bool gui_obj_out_screen(gui_obj_t *obj);

/**
 * @brief Calculate the clipping rectangle of a GUI object relative to its top-level ancestor.
 *
 * @param obj GUI object for which the clipping rectangle is calculated.
 * @param rect Output rectangle that will contain the calculated clipping area.
 */
void gui_obj_get_clip_rect(gui_obj_t *obj, gui_rect_t *rect);

/**
 * @brief Judge the obj if in range of this_widget rect.
 *
 * @param obj Pointer to the GUI object.
 * @param x X-axis coordinate of the widget.
 * @param y Y-axis coordinate of the widget.
 * @param w Width of the widget.
 * @param h Height of the widget.
 * @return True if in range, false otherwise.
*/
bool gui_obj_in_rect(gui_obj_t *obj,
                     int16_t    x,
                     int16_t    y,
                     int16_t    w,
                     int16_t    h);

/**
 * @brief Enable all short click actions from parent object to the root object.
 *
 * @param obj Root of the widget tree.
 */
void gui_obj_enable_this_parent_short(gui_obj_t *obj);


/**
 * @brief Get the area of this_widget obj.
 *
 * @param obj Pointer to the GUI object.
 * @param x X-axis coordinate of the widget.
 * @param y Y-axis coordinate of the widget.
 * @param w Width of the widget.
 * @param h Height of the widget.
 */
void gui_obj_get_area(gui_obj_t *obj,
                      int16_t   *x,
                      int16_t   *y,
                      int16_t   *w,
                      int16_t   *h);

/**
 * @brief Judge the point if in range of this_widget obj rect.
 *
 * @param obj Widget object pointer.
 * @param x X-axis coordinate.
 * @param y Y-axis coordinate.
 * @return True if in range, false otherwise.
 */
bool gui_obj_point_in_obj_rect(gui_obj_t *obj, int16_t x, int16_t y);

/**
 * @brief Judge the point if in range of this_widget obj circle.
 *
 * @param obj Widget object pointer.
 * @param x X-axis coordinate.
 * @param y Y-axis coordinate.
 * @return True if in range, false otherwise.
 */
bool gui_obj_point_in_obj_circle(gui_obj_t *obj, int16_t x, int16_t y);

/**
 * @brief Do crc check.
 *
 * @param seed Initial value to start the checksum calculation.
 * @param data Pointer to the array of bytes for which the checksum is to be calculated.
 * @param len Number of bytes in the array.
 * @return Calculated checksum value.
 */
uint8_t gui_obj_checksum(uint8_t seed, uint8_t *data, uint8_t len);


/**
 * @brief Print name by bfs order.
 *
 * @param object Widget pointer.
 * @return Pointer to the root object.
 */
gui_obj_t *gui_get_root(gui_obj_t *object);

/**
 * @brief Calculate the absolute coordinates of a GUI object.
 *
 * This function calculates the absolute (global) X and Y coordinates
 * of a given GUI object based on its local position within the parent hierarchy.
 *
 * @param obj Pointer to the GUI object for which to calculate absolute coordinates.
 * @param absolute_x Pointer to an integer where the absolute X coordinate will be stored.
 * @param absolute_y Pointer to an integer where the absolute Y coordinate will be stored.
 *
 * @note This function assumes that @p obj is a valid pointer and
 *       that @p absolute_x and @p absolute_y are valid pointers to integers.
 */
void gui_obj_absolute_xy(gui_obj_t *obj, int *absolute_x, int *absolute_y);

/**
 * @brief Set the visibility of a GUI object.
 *
 * This function sets the visibility of a given GUI object by adjusting its hidden state.
 *
 * @param obj Pointer to the GUI object that will be updated.
 * @param hidden Boolean flag indicating whether the object should be hidden (true) or shown (false).
 */
void gui_obj_hidden(gui_obj_t *obj, bool hidden);

/**
 * @brief Set or retrieve the name of a GUI widget.
 *
 * This function sets the name of a given GUI widget if the provided name is valid.
 * It returns the current name of the widget.
 *
 * @param widget Pointer to the GUI widget whose name will be set or retrieved.
 * @param name Pointer to a string containing the new name for the widget. If the name is valid, it will be set as the widget's name.
 *
 * @return Current name of the widget.
 */
const char *gui_widget_name(gui_obj_t *widget, const char *name);

/**
 * @brief Update touch pad speed vertical.
 *
 * This function updates the current speed and records the speed change history.
 *
 * @param speed Pointer to the current speed, which will be updated by the function.
 * @param speed_recode Array to record speed changes, which will be updated by the function.
 */
void gui_update_speed(int *speed, int speed_recode[]);

/**
 * @brief Inertial calculation.
 *
 * This function performs inertial calculations based on the current speed, end speed, and offset.
 *
 * @param speed Pointer to the current speed, which will be updated by the function.
 * @param end_speed Target end speed.
 * @param offset Pointer to the offset, which will be updated by the function.
 */
void gui_inertial(int *speed, int end_speed, int *offset);

/**
 * @brief Get widget count.
 */
uint32_t gui_get_obj_count(void);
/**
 * @brief Set the location of a GUI object.
 *
 * This function sets the X and Y coordinates of the specified GUI object.
 *
 * @param obj Pointer to the GUI object to set location for.
 * @param x X coordinate to set.
 * @param y Y coordinate to set.
 */
void gui_set_location(gui_obj_t *obj, uint16_t x, uint16_t y);

/**
 * @brief API to create a widget tree structure from an XML file and associate it with a parent widget.
 *
 * @param xml Path to the XML file to be parsed.
 * @param parent_widget Parent widget to which the tree structure is to be associated.
 */
void gui_dom_create_tree_nest(const char *xml, gui_obj_t *parent_widget);

/**
 * @brief Extracts the preview image file path from an XML file.
 *
 * This function parses the given XML file and attempts to find the preview
 * image file path by looking for specific tags within the XML.
 *
 * @param xml_file Path to the XML file to be parsed.
 * @return String containing the path to the preview image file. If the XML
 *         file cannot be loaded or the preview image file path cannot be found,
 *         returns NULL.
 */
char *gui_dom_get_preview_image_file(const char *xml);

/**
 * @brief Update the speed based on displacement.
 *
 * This function updates the speed value based on the given displacement.
 * It also uses a speed record array to achieve this.
 *
 * @param speed Pointer to the speed variable to update.
 * @param speed_recode Array holding the speed records.
 * @param displacement Displacement value to consider for speed update.
 */
void gui_update_speed_by_displacement(int *speed, int speed_recode[], int displacement);

/**
 * @brief Move a widget object to specified coordinates.
 *
 * This function moves the specified widget object to a new (x, y) coordinate
 * position.
 *
 * @param obj Pointer to the widget object to be moved.
 * @param x New x-coordinate for the widget object.
 * @param y New y-coordinate for the widget object.
 */
void gui_obj_move(gui_obj_t *obj, int x, int y);

/**
 * @brief Set a timer for a GUI object.
 *
 * This function sets a timer for the specified GUI object with a given interval.
 * The timer can be configured to reload automatically or run only once.
 * When the timer expires, the provided callback function is called.
 *
 * @param obj Pointer to the GUI object to set the timer for.
 * @param interval Interval in milliseconds for the timer.
 * @param reload Boolean flag indicating whether the timer should reload automatically (true) or run only once (false).
 * @param callback Pointer to the callback function to be called when the timer expires.
 */
void gui_obj_create_timer(gui_obj_t *obj, uint32_t interval, bool reload, void (*callback)(void *));

void gui_obj_delete_timer(gui_obj_t *obj);

void gui_obj_start_timer(gui_obj_t *obj);

void gui_obj_stop_timer(gui_obj_t *obj);


#ifdef __cplusplus
}
#endif

#endif
