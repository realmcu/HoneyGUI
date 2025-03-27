/**
 * @file lv_custom_tile_snapshot.h
 *
 */

#ifndef LV_CUSTOM_TILE_SNAPSHOT_H
#define LV_CUSTOM_TILE_SNAPSHOT_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "string.h"
#include "stdio.h"
#include "stdlib.h"

#include "lvgl.h"
#include "lv_draw_buf.h"
#include "lv_snapshot.h"
#include "lv_image.h"
/*********************
 *      DEFINES
 *********************/


/**********************
 *      TYPEDEFS
 **********************/

/**
 * @brief Delete snapshot resources
 * @param widget Associated widget object
 * @param img_snapshot Target snapshot image object to operate
 */
void delete_snapshot(lv_obj_t *widget, lv_obj_t *img_snapshot);

/**
 * @brief Update widget snapshot cache
 * @param widget Source widget to capture
 * @param img_snapshot Image object for displaying snapshot
 * @note Automatically destroys old snapshot and creates new ARGB8888 format
 */
void update_snapshot(lv_obj_t *widget, lv_obj_t *img_snapshot);

/**
 * @brief Create new widget snapshot
 * @param widget Source widget to capture
 * @param img_snapshot Image object for displaying snapshot
 * @note Initializes black background and updates snapshot immediately
 */
void create_snapshot(lv_obj_t *widget, lv_obj_t *img_snapshot);

/**
 * @brief Delete snapshot event callback
 * @param e LVGL event object
 * @note Gets widget reference from event user data
 */
void snapshot_custom_cb_delete(lv_event_t *e);

/**
 * @brief Create snapshot event callback
 * @param e LVGL event object
 * @note Gets widget reference from event user data
 */
void snapshot_custom_cb_create(lv_event_t *e);

lv_obj_t *create_snapshot_obj_directly(lv_obj_t *parent, lv_obj_t *target);

/**
 * @brief Create a snapshot object with event bindings
 *
 * @param parent Parent container for the snapshot image
 * @param target Target widget to capture snapshot from
 * @param create_enent_id Event ID for snapshot creation trigger
 * @param delete_enent_id Event ID for snapshot deletion trigger
 * @return lv_obj_t* Pointer to the created snapshot image object
 */
lv_obj_t *create_snapshot_obj_with_enent(lv_obj_t *parent, lv_obj_t *target,
                                         uint32_t create_enent_id, uint32_t delete_enent_id);
/**********************
 * GLOBAL PROTOTYPES
 **********************/


/**********************
 *      MACROS
 **********************/



#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_CUSTOM_TILE_SNAPSHOT_H*/
