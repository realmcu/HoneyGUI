/**
 * @file gui_prism_mirror3d.h
 * @author wenjing_jiang@realsil.com.cn
 * @brief
 * @version 1.0
 * @date 2024-12-25
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef GUI_PRISM_3D_H
#define GUI_PRISM_3D_H

#include "guidef.h"
#include "gui_img.h"
#include "gui_tabview.h"
#include "gui_tab.h"
#include "gui_img.h"
#include "gui_obj.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include <gui_app.h>
#include "gui_server.h"
#include "gui_components_init.h"
#include "gui_canvas.h"
#include "def_3d.h"
#include "gui_3d.h"

/**
 * The number of faces on the prism. Typically 6 for a standard 3D prism,
 * representing front, back, left, right, top, and bottom.
 */
#define PRISM_FACE_NUMS 6

/**
 * Enables or disables automatic rotation of the prism.
 * Set to 1 to enable automatic rotation, allowing the prism to
 * continuously rotate without user input. Set to 0 to disable.
 */
#define AUTO_ROTATION 1

typedef struct
{
    gui_point_4d_t worldPosition;
    gui_point_4d_t cameraPosition;
    float rot_x;
    float rot_y;
    float rot_z;
    float angle;
    float scale;
} gui_prism_transform_state_t;

typedef struct
{
    gui_3d_t render_object;
    int32_t face_flags;
    gui_prism_transform_state_t raw_state;
    gui_prism_transform_state_t target_state;
    gui_prism_transform_state_t interpolated_state;
} gui_prism_mirror3d_t;

void prism_mirror3d_render_callback(gui_3d_t *parent, size_t face, gui_3d_world_t *world,
                                    gui_3d_camera_t *camera, gui_3d_light_t *light);
void prism_mirror3d_update_angle_cb(void *param);
void prism_mirror3d_render_animate_cb(void *param);
void prism_mirror3d_on_face_click_cb(void *obj, gui_event_t e, void *param);
void gui_prism_mirror3d_enter_animate(gui_prism_mirror3d_t *parent);
void gui_prism_mirror3d_swap_states(gui_prism_mirror3d_t *prism);
void gui_prism_mirror3d_click_switch_app_add_event(gui_prism_mirror3d_t *prism_mirror3d,
                                                   gui_event_cb_t callback);
uint8_t gui_prism_mirror3d_get_enter_face(void);

void gui_prism_mirror3d_set_animate(gui_prism_mirror3d_t *prism_mirror3d);
void gui_prism_mirror3d_set_scale(gui_prism_mirror3d_t *prism_mirror3d, float scale);
void gui_prism_mirror3d_set_position(gui_prism_mirror3d_t *prism_mirror3d, int16_t x, int16_t y,
                                     int16_t z);
void gui_prism_mirror3d_set_rotation_angles(gui_prism_mirror3d_t *prism_mirror3d, int16_t x,
                                            int16_t y, int16_t z);
void gui_prism_mirror3d_update_face_flags(gui_prism_mirror3d_t *prism_mirror3d);
void gui_prism_render_interpolated_animate(gui_prism_mirror3d_t *render_object);
gui_prism_mirror3d_t *gui_prism_mirror3d_create(gui_obj_t *parent, const char *name,
                                                void *desc_addr,
                                                uint16_t x, uint16_t y, uint16_t w, uint16_t h);
#endif // GUI_PRISM_3D_H