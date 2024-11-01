#ifndef DEF_3D_H
#define DEF_3D_H


#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

typedef enum
{
    GUI_3D_CULLMODE_NONE,
    GUI_3D_CULLMODE_CW,
    GUI_3D_CULLMODE_CCW,
} GUI_3D_CULLMODE;

typedef struct _gui_3d_matrix
{
    union
    {
        struct
        {
            float        _11, _12, _13, _14;
            float        _21, _22, _23, _24;
            float        _31, _32, _33, _34;
            float        _41, _42, _43, _44;
        };
        float m[4][4];
    };
} gui_3d_matrix_t;

typedef struct _gui_point4D
{
    float x;
    float y;
    float z;
    float w;
} gui_point4D_t;

typedef struct
{
    gui_point4D_t  position;
    gui_point4D_t normal;
    //px_color    clr;
    float    u, v;
} gui_3D_vertex;

typedef struct
{
    uint32_t        state;
    gui_3D_vertex   vertex[3];
    gui_3D_vertex   transform_vertex[3];
} gui_3d_face_t;

typedef gui_3d_matrix_t gui_3d_world_t;


typedef struct
{
    float near_z;
    float far_z;
    float fov;
    float d;

    gui_point4D_t position;
    gui_point4D_t targetDirection;

    float viewplane_width, viewplane_height;
    float viewport_width, viewport_height, viewport_center_x, viewport_center_y;
    float aspect_ratio;

    gui_3d_matrix_t mat_cam, mat_per, mat_scr;

    float *zbuffer;
} gui_3d_camera_t;

void gui_3d_world_inititalize(gui_3d_matrix_t *world, float x, float y, float z, float rotX,
                              float rotY, float rotZ, float scale);

bool gui_3d_camera_UVN_initialize(gui_3d_camera_t *camera, gui_point4D_t cameraPosition,
                                  gui_point4D_t cameraTarget, float near, float far, float fov, float viewPortWidth,
                                  float viewPortHeight);




#ifdef __cplusplus
}
#endif

#endif
