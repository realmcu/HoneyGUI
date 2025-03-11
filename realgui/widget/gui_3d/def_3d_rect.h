#ifndef DEF_3D_RECT_H
#define DEF_3D_RECT_H


#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include "def_3d_common.h"


typedef struct
{
    uint32_t            state;
    gui_3d_vertex_t     vertex[4];
    gui_3d_vertex_t     transform_vertex[4];
    gui_3d_vertex_t     transform_world_vertex[4];
} gui_3d_rect_face_t;

typedef struct
{
    bool initialized;
    float included_angle;
    float blend_ratio;
    gui_point_4d_t position;
    gui_point_4d_t targetDirection;
    gui_3d_RGBAcolor_t color;
} gui_3d_light_t;

void gui_3d_light_inititalize(gui_3d_light_t *light, gui_point_4d_t lightPosition,
                              gui_point_4d_t lightTarget, float included_angle, float blend_ratio, gui_3d_RGBAcolor_t color);

void gui_3d_rect_scene(gui_3d_rect_face_t *face, gui_3d_world_t *world, gui_3d_camera_t *camera);

void gui_3d_rect_face_transform_local_to_local(gui_3d_rect_face_t *face, gui_3d_matrix_t *m);


#ifdef __cplusplus
}
#endif

#endif
