#ifndef DEF_3D_H
#define DEF_3D_H


#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#define GUI_3D_FACESTATE_ACTIVE 1
#define GUI_3D_FACESTATE_CLIPPED 2
#define GUI_3D_FACESTATE_BACKFACE 4
typedef enum
{
    GUI_3D_CULLMODE_NONE,
    GUI_3D_CULLMODE_CW,
    GUI_3D_CULLMODE_CCW,
} GUI_3D_CULLMODE;

typedef enum
{
    GUI_3D_FACE_TRANSFORM_LOCAL_TO_LOCAL,
    GUI_3D_FACE_TRANSFORM_LOACL_TO_GLOBAL,
    GUI_3D_FACE_TRANSFORM_GLOBAL_TO_GLOBAL,
} GUI_3D_FACE_TRANSFORM;

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
} gui_point_4d_t;

typedef gui_point_4d_t gui_vector4D_t;

typedef struct
{
    gui_point_4d_t  position;
    gui_point_4d_t normal;
    //px_color    clr;
    float    u, v;
} gui_3d_vertex_t;

typedef struct
{
    uint32_t            state;
    gui_3d_vertex_t     vertex[3];
    gui_3d_vertex_t     transform_vertex[3];
} gui_3d_face_t;

typedef gui_3d_matrix_t gui_3d_world_t;


typedef struct
{
    float near_z;
    float far_z;
    float fov;
    float d;

    gui_point_4d_t position;
    gui_point_4d_t targetDirection;

    float viewplane_width, viewplane_height;
    float viewport_width, viewport_height, viewport_center_x, viewport_center_y;
    float aspect_ratio;

    gui_3d_matrix_t mat_cam, mat_per, mat_scr;

    float *zbuffer;
} gui_3d_camera_t;

gui_point_4d_t gui_point_4d(float x, float y, float z);

void gui_3d_world_inititalize(gui_3d_matrix_t *world, float x, float y, float z, float rotX,
                              float rotY, float rotZ, float scale);

bool gui_3d_camera_UVN_initialize(gui_3d_camera_t *camera, gui_point_4d_t cameraPosition,
                                  gui_point_4d_t cameraTarget, float near, float far, float fov, float viewPortWidth,
                                  float viewPortHeight);


void gui_3d_scene(gui_3d_face_t *face, gui_3d_world_t *world, gui_3d_camera_t *camera);


#ifdef __cplusplus
}
#endif

#endif
