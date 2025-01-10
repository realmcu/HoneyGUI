/**
 \internal
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_prism_mirror3d.h
  * @brief Prism mirror 3D widget
  * @details This file contains the interface for creating and managing 3D prism mirror widgets,which can transform and animate within a 3D world.
  * @author wenjing_jiang@realsil.com.cn
  * @date 2025/1/8
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
#ifndef __GUI_PRISM_MIRROR_H__
#define __GUI_PRISM_MIRROR_H__
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
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

/*============================================================================*
 *                         Types
 *============================================================================*/
typedef struct
{
    uint8_t face_nums;          /**< Number of faces on the prism.
                                     This specifies how many faces the prism should have.
                                     A common 3D prism might have 6 faces including front, back, left, right, top, and bottom. */

    bool auto_rotation;         /**< Flag indicating whether automatic rotation is enabled.
                                     If set to true, the prism will continuously rotate without any user input,
                                     allowing for a constant animation effect. */

    float sensitivity;          /**< Sensitivity coefficient for user input.
                                     This value determines how responsive the prism's rotation is to user interactions.
                                     Values less than 1 reduce the sensitivity, making the prism rotate slower in response to input.
                                     Higher values increase sensitivity, resulting in faster rotation. */
} prism_mirror3d_config_t;

/**
 * @brief Structure to represent the transform state for a prism.
 */
typedef struct
{
    gui_point_4d_t worldPosition;   /**< The position of the prism in the world coordinate system */
    gui_point_4d_t cameraPosition;  /**< The position of the camera observing the prism */
    float rot_x;                    /**< Rotation around the X axis */
    float rot_y;                    /**< Rotation around the Y axis */
    float rot_z;                    /**< Rotation around the Z axis */
    float angle;                    /**< Angle of rotation */
    float scale;                    /**< Scale factor */
} gui_prism_transform_state_t;

/**
 * @brief Structure to represent a 3D prism mirror widget.
 */
typedef struct
{
    gui_3d_t render_object;               /**< The 3D render object */
    int16_t face_flags_rotation;                   /**< Flags defining the visibility of faces */
    prism_mirror3d_config_t conf;
    gui_prism_transform_state_t raw_state; /**< The raw transform state */
    gui_prism_transform_state_t target_state; /**< The target transform state for animations */
    gui_prism_transform_state_t interpolated_state; /**< The interpolated state during animations */
} gui_prism_mirror3d_t;



/*============================================================================*
 *                         Constants
 *============================================================================*/

/*============================================================================*
 *                         Macros
 *============================================================================*/

/*============================================================================*
 *                         Functions
 *============================================================================*/

/**
 * @brief Callback for rendering a 3D prism mirror.
 *
 * @param parent The parent 3D object.
 * @param face The face index to render.
 * @param world The world coordinate system.
 * @param camera The camera observing the object.
 * @param light The light affecting the object.
 */
static void prism_mirror3d_render_callback(gui_3d_t *parent, size_t face, gui_3d_world_t *world,
                                           gui_3d_camera_t *camera, gui_3d_light_t *light);

/**
 * @brief Updates the angle of the prism mirror. Called on a timer or event.
 *
 * @param param The user-provided parameter.
 */
static void prism_mirror3d_update_angle_cb(void *param);

/**
 * @brief Animation callback for rendering.
 *
 * @param param The user-provided parameter.
 */
static void prism_mirror3d_render_animate_cb(void *param);

/**
 * @brief Callback for when a prism face is clicked.
 *
 * @param obj The object being clicked.
 * @param e The event type.
 * @param param The user-provided parameter.
 */
static void prism_mirror3d_on_face_click_cb(void *obj, gui_event_t e, void *param);

/**
 * @brief Swaps the current and target states of the prism.
 *
 * @param prism The prism mirror object.
 */
static void gui_prism_mirror3d_swap_states(gui_prism_mirror3d_t *prism);

/**
 * @brief Interpolates and animates the rendering of a prism.
 *
 * @param render_object The object being rendered.
 */
static void gui_prism_render_interpolated_animate(gui_prism_mirror3d_t *render_object);

/**
 * @brief Updates the face flags for a prism mirror based on its current state.
 *
 * @param prism_mirror3d The prism mirror object.
 */
static void gui_prism_mirror3d_update_face_flags(gui_prism_mirror3d_t *prism_mirror3d);

/**
 * @brief Enters an animation for the prism mirror.
 *
 * @param parent The parent prism mirror object.
 */
void gui_prism_mirror3d_enter_animate(gui_prism_mirror3d_t *parent);

/**
 * @brief Adds a click switch event to the prism mirror.
 *
 * @param prism_mirror3d The prism mirror object.
 * @param callback The callback for the event.
 */
void gui_prism_mirror3d_click_switch_app_add_event(gui_prism_mirror3d_t *prism_mirror3d,
                                                   gui_event_cb_t callback);

/**
 * @brief Gets the currently active face the prism enters.
 *
 * @return The active face index.
 */
int16_t *gui_prism_mirror3d_get_enter_face(void);

/**
 * @brief Sets the animation parameters for a prism mirror.
 *
 * @param prism_mirror3d The prism mirror object.
 */
void gui_prism_mirror3d_set_animate(gui_prism_mirror3d_t *prism_mirror3d);

/**
 * @brief Sets the scale for a prism mirror.
 *
 * @param prism_mirror3d The prism mirror object.
 * @param scale The scale factor.
 */
void gui_prism_mirror3d_set_scale(gui_prism_mirror3d_t *prism_mirror3d, float scale);

/**
 * @brief Sets the position for a prism mirror in 3D space.
 *
 * @param prism_mirror3d The prism mirror object.
 * @param x The X coordinate.
 * @param y The Y coordinate.
 * @param z The Z coordinate.
 */
void gui_prism_mirror3d_set_position(gui_prism_mirror3d_t *prism_mirror3d, int16_t x, int16_t y,
                                     int16_t z);

/**
 * @brief Sets the rotation angles for a prism mirror.
 *
 * @param prism_mirror3d The prism mirror object.
 * @param x The rotation angle around X axis.
 * @param y The rotation angle around Y axis.
 * @param z The rotation angle around Z axis.
 */
void gui_prism_mirror3d_set_rotation_angles(gui_prism_mirror3d_t *prism_mirror3d, int16_t x,
                                            int16_t y, int16_t z);

/**
 * @brief Sets the raw state of a 3D prism mirror object.
 *
 * This function initializes or updates the raw state of the given
 * 3D prism mirror object with specified world position, camera position,
 * rotation angles, and scale factor. The raw state typically represents
 * the current scene's configuration.
 *
 * @param prism_mirror3d Pointer to the 3D prism mirror object to be modified.
 * @param world_position Array of 3 floats specifying the x, y, z world coordinates of the prism's position.
 * @param camera_position Array of 3 floats specifying the x, y, z coordinates for the camera's position relative to the prism.
 * @param rot_x Rotation angle around the x-axis, in degrees.
 * @param rot_y Rotation angle around the y-axis, in degrees.
 * @param rot_z Rotation angle around the z-axis, in degrees.
 * @param scale Scaling factor applied to the prism.
 */
void gui_prism_mirror3d_set_raw_state(
    gui_prism_mirror3d_t *prism_mirror3d,
    float world_position[3],
    float camera_position[3],
    float rot_x,
    float rot_y,
    float rot_z,
    float scale
);

/**
 * @brief Sets the target state of a 3D prism mirror object.
 *
 * This function sets the target configuration for the 3D prism mirror object,
 * allowing it to transition to a desired state defined by the given parameters.
 * The target state typically represents the future or goal configuration for the scene.
 *
 * @param prism_mirror3d Pointer to the 3D prism mirror object to be adjusted.
 * @param world_position Array of 3 floats specifying the x, y, z world coordinates of the target position for the prism.
 * @param camera_position Array of 3 floats specifying the x, y, z coordinates for the camera's target position relative to the prism.
 * @param rot_x Rotation angle around the x-axis, in degrees for target orientation.
 * @param rot_y Rotation angle around the y-axis, in degrees for target orientation.
 * @param rot_z Rotation angle around the z-axis, in degrees for target orientation.
 * @param scale Target scaling factor for the prism adjustment.
 */
void gui_prism_mirror3d_set_target_state(
    gui_prism_mirror3d_t *prism_mirror3d,
    float world_position[3],
    float camera_position[3],
    float rot_x,
    float rot_y,
    float rot_z,
    float scale
);
/**
 * @brief Creates a new prism mirror 3D object.
 *
 * This function initializes and creates a 3D prism object within the graphical user interface.
 * It sets up the object's basic properties such as its position, size, and initial transformation states.
 *
 * @param parent Pointer to the parent GUI object.
 *               The created prism object will be a child of this parent, inheriting context and layout.
 * @param name The name identifier for the prism object.
 *             This string is used to uniquely identify the object within the GUI environment.
 * @param desc_addr Pointer to the description data for the prism.
 *                  This can include detailed geometric or material properties for rendering.
 * @param x The X coordinate for the position of the prism within the parent object.
 * @param y The Y coordinate for the position of the prism within the parent object.
 * @param w The width of the prism object.
 * @param h The height of the prism object.
 * @param config Pointer to a configuration structure (prism_mirror3d_config_t).
 *               This structure allows for customizing properties such as the number of faces,
 *               automatic rotation behavior, and input sensitivity.
 *               If NULL, default settings will be applied.
 * @return A pointer to the newly created prism mirror 3D object.
 *         Returns NULL if the creation fails due to memory allocation issues or invalid parameters.
 */
gui_prism_mirror3d_t *gui_prism_mirror3d_create(gui_obj_t *parent, const char *name,
                                                void *desc_addr,
                                                uint16_t x, uint16_t y, uint16_t w, uint16_t h,
                                                prism_mirror3d_config_t *config);


#endif // __GUI_PRISM_MIRROR_H__
#ifdef __cplusplus
}
#endif
