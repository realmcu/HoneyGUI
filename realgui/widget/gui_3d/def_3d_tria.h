#ifndef DEF_3D_TRIA_H
#define DEF_3D_TRIA_H


#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include "def_3d_common.h"


typedef struct
{
    uint32_t            state;
    gui_3d_vertex_t     transform_vertex[3];
} gui_3d_tria_face_t;



void gui_3d_tria_scene(gui_3d_tria_face_t *face, size_t face_index, gui_obj_attrib_t *attrib,
                       gui_3d_world_t *world, gui_3d_camera_t *camera);



#ifdef __cplusplus
}
#endif

#endif
