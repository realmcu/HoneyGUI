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
#define TINYOBJ_LOADER_C_IMPLEMENTATION
#include "tinyobj_loader_c.h"


#include "cube_obj.txt"
#include "def_3d.h"



static void get_file_data(void *ctx, const char *filename, const int is_mtl,
                          const char *obj_filename, char **data, size_t *len)
{
    // NOTE: If you allocate the buffer with malloc(),
    // You can define your own memory management struct and pass it through `ctx`
    // to store the pointer and free memories at clean up stage(when you quit an
    // app)
    // This example uses mmap(), so no free() required.
    (void)ctx;

    *data = (char *)_accube;
    (*len) = sizeof(_accube);
}



static void get_face_by_obj(gui_3d_face_t  *face, size_t i)
{
    char *filename = "win32_sim\\assets\\cube.obj";
    size_t index_offset = 0;
    tinyobj_attrib_t attrib;
    tinyobj_shape_t *shapes = NULL;
    size_t num_shapes;
    tinyobj_material_t *materials = NULL;
    size_t num_materials;
    unsigned int flags = TINYOBJ_FLAG_TRIANGULATE;
    // unsigned int flags = 0;

    tinyobj_parse_obj(&attrib, &shapes, &num_shapes, &materials, &num_materials, filename,
                      get_file_data, NULL, flags);


    for (size_t s = 0; s < i; s++)
    {
        index_offset += attrib.face_num_verts[i];
    }


    for (size_t j = 0; j < attrib.face_num_verts[i]; ++j)
    {
        tinyobj_vertex_index_t idx = attrib.faces[index_offset + j];
        // printf("%d/%d/%d ", idx.v_idx, idx.vt_idx, idx.vn_idx);

        // Print the corresponding vertex coordinates
        float *v = &attrib.vertices[3 * idx.v_idx];
        float *vt = &attrib.vertices[2 * idx.vt_idx];
        float *vn = &attrib.vertices[3 * idx.vn_idx];

        face->vertex[j].position.x = v[0];
        face->vertex[j].position.y = v[1];
        face->vertex[j].position.z = v[2];
        face->vertex[j].position.w = 1;
    }


}

static void canvas_cb_black(gui_canvas_t *canvas)
{

    gui_3d_world_t world;
    gui_3d_camera_t camera;
    gui_3d_face_t face;

    for (uint32_t i = 0; i < 12; i++)
    {

        get_face_by_obj(&face, i);

        gui_3d_camera_UVN_initialize(&camera, gui_point_4d(0, 0, 0), gui_point_4d(0, 0, 1), 1, 32767, 90,
                                     480, 480);
        gui_3d_world_inititalize(&world, 0, 0, 5.0f, 0, 0, 0, 1);
        gui_3d_scene(&face, &world, &camera);
        NVGcontext *vg = canvas->vg;

        nvgBeginPath(vg);
        nvgMoveTo(vg, face.transform_vertex[0].position.x, face.transform_vertex[0].position.y);
        nvgLineTo(vg, face.transform_vertex[1].position.x, face.transform_vertex[1].position.y);
        nvgLineTo(vg, face.transform_vertex[2].position.x, face.transform_vertex[2].position.y);

        nvgClosePath(vg);

        nvgStrokeWidth(vg, 1.0f);
        nvgStrokeColor(vg, nvgRGB(255, 0, 0));
        nvgStroke(vg);


        // nvgFillColor(vg, nvgRGBA(0, 255, 0, 200));
        // nvgFill(vg);

    }

}



static void app_ui_design(gui_app_t *app)
{

    gui_canvas_t *canvas = gui_canvas_create(&(app->screen), "canvas", 0, 0, 0, 454, 454);
    gui_canvas_set_canvas_cb(canvas, canvas_cb_black);
    return;

}


static gui_app_t rtk_gui_demo =
{
    .screen =
    {
        .name = "rtk_gui_demo",
        .x    = 0,
        .y    = 0,
    },
    .ui_design = app_ui_design,
    .active_ms = 1000 * 60 * 60,
};


static int app_init(void)
{
    gui_server_init();
    gui_app_startup(&rtk_gui_demo);
    return 0;
}

GUI_INIT_APP_EXPORT(app_init);


