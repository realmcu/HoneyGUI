/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file
  * @brief
  * @details
  * @author
  * @date
  * @version
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <string.h>
#include "guidef.h"
#include "gui_obj.h"
#include "gui_3d.h"
#include "tp_algo.h"
#include "def_3d.h"
#include "draw_img.h"
#include "acc_init.h"
#define TINYOBJ_LOADER_C_IMPLEMENTATION
#include "tinyobj_loader_c.h"



/*============================================================================*
 *                           Types
 *============================================================================*/

/*============================================================================*
 *                           Constants
 *============================================================================*/

/*============================================================================*
 *                            Macros
 *============================================================================*/

/*============================================================================*
 *                            Variables
 *============================================================================*/

/*============================================================================*
 *                           Private Functions
 *============================================================================*/

static void get_file_data(void *ctx, const char *filename, const int is_mtl,
                          const char *obj_filename, char **data, size_t *len)
{
    gui_3d_t *this = (gui_3d_t *)ctx;

    *data = (char *)this->data;
    (*len) = this->len;
}




static void gui_3d_prepare(gui_3d_t *this)
{
    touch_info_t *tp = tp_get_info();
    gui_obj_t *obj = (gui_obj_t *)this;
    gui_dispdev_t *dc = gui_get_dc();

    // this->flags = TINYOBJ_FLAG_TRIANGULATE;
    this->flags = 0;

    tinyobj_parse_obj(&this->attrib, &this->shapes, &this->num_shapes, &this->materials,
                      &this->num_materials, "todo-filename-by-howie",
                      get_file_data, this, this->flags);

    // unsigned int num_faces = this->attrib.num_faces;

    this->face = gui_malloc(sizeof(gui_3d_face_t) * this->attrib.num_face_num_verts);

    this->img = gui_malloc(sizeof(draw_img_t) * this->attrib.num_face_num_verts);
    memset(this->img, 0x00, sizeof(draw_img_t) * this->attrib.num_face_num_verts);


    for (uint32_t i = 0; i < this->attrib.num_face_num_verts; i++)
    {
        size_t index_offset = 0;
        for (size_t s = 0; s < i; s++)
        {
            index_offset += this->attrib.face_num_verts[i];
        }
        for (size_t j = 0; j < this->attrib.face_num_verts[i]; ++j)
        {
            tinyobj_vertex_index_t idx = this->attrib.faces[index_offset + j];

            float *v = &this->attrib.vertices[3 * idx.v_idx];
            float *vt = &this->attrib.texcoords[2 * idx.vt_idx];
            float *vn = &this->attrib.normals[3 * idx.vn_idx];

            gui_3d_face_t  *face = this->face + i;

            face->vertex[j].position.x = v[0];
            face->vertex[j].position.y = v[1];
            face->vertex[j].position.z = v[2];
            face->vertex[j].position.w = 1;

            face->vertex[j].u = vt[0];
            face->vertex[j].v = vt[1];

            face->vertex[j].normal.x = vn[0];
            face->vertex[j].normal.y = vn[1];
            face->vertex[j].normal.z = vn[2];
            face->vertex[j].normal.w = 1;
        }

    }

#if 0
    for (size_t i = 0; i < this->num_materials; i++)
    {
        gui_log("material %s\n", this->materials[i].name);
        gui_log("  Ka %f %f %f\n", this->materials[i].ambient[0], this->materials[i].ambient[1],
                this->materials[i].ambient[2]);
        gui_log("  Kd %f %f %f\n", this->materials[i].diffuse[0], this->materials[i].diffuse[1],
                this->materials[i].diffuse[2]);
        gui_log("  Ks %f %f %f\n", this->materials[i].specular[0], this->materials[i].specular[1],
                this->materials[i].specular[2]);
        gui_log("  Ke %f %f %f\n", this->materials[i].emission[0], this->materials[i].emission[1],
                this->materials[i].emission[2]);
        gui_log("  Ns %f\n", this->materials[i].shininess);
        gui_log("  Ni %f\n", this->materials[i].ior);
        gui_log("  d %f\n", this->materials[i].dissolve);
        gui_log("  illum %d\n", this->materials[i].illum);

        if (this->materials[i].ambient_texname) { gui_log("  map_Ka %s\n",  this->materials[i].ambient_texname); }
        if (this->materials[i].diffuse_texname) { gui_log("  map_Kd %s\n",  this->materials[i].diffuse_texname); }
        if (this->materials[i].specular_texname) { gui_log("  map_Ks %s\n",  this->materials[i].specular_texname); }
        if (this->materials[i].alpha_texname) { gui_log("  map_d %s\n",   this->materials[i].alpha_texname); }
        if (this->materials[i].bump_texname) { gui_log("  map_bump %s\n", this->materials[i].bump_texname); }
        if (this->materials[i].displacement_texname) { gui_log("  disp %s\n",    this->materials[i].displacement_texname); }
    }
#endif


    for (uint32_t i = 0; i < this->attrib.num_face_num_verts; i++)
    {
        gui_3d_world_t world;
        gui_3d_camera_t camera;

        gui_3d_camera_UVN_initialize(&camera, gui_point_4d(0, 0, 0), gui_point_4d(0, 0, 1), 1, 32767, 90,
                                     dc->screen_width, dc->screen_height);
        gui_3d_world_inititalize(&world, 0, 0, 5.0f, 50, 50, 0, 1);
        gui_3d_matrix_t result;
        // gui_3d_generate_rotate_around_line(&result, 1, 1, 0, 0, 1, 0, -20);
        // gui_3d_face_transform_local_to_local(this->face + i, &result);
        gui_3d_scene(this->face + i, &world, &camera);

        float x0 = this->face[i].transform_vertex[0].position.x;
        float y0 = this->face[i].transform_vertex[0].position.y;

        float x1 = this->face[i].transform_vertex[1].position.x;
        float y1 = this->face[i].transform_vertex[1].position.y;

        float x2 = this->face[i].transform_vertex[2].position.x;
        float y2 = this->face[i].transform_vertex[2].position.y;

        float x3 = this->face[i].transform_vertex[3].position.x;
        float y3 = this->face[i].transform_vertex[3].position.y;

        gui_log("(%f, %f), (%f, %f), (%f, %f), (%f, %f) \n", x0, y0, x1, y1, x2, y2, x3, y3);

        float w = 256.0f;
        float h = 236.0f;
        gui_3d_point_2d_t src[4] = {{0, 0}, {w, 0}, {w, h}, {0, h}};
        gui_3d_point_2d_t dst[4] = {{x0, y0}, {x1, y1}, {x2, y2}, {x3, y3}};


        gui_3d_generate_2d_matrix(src, dst, (float *)&this->img[i].matrix);
        memcpy(&this->img[i].inverse, &this->img[i].matrix, sizeof(gui_matrix_t));
        matrix_inverse(&this->img[i].inverse);

        this->img[i].data = this->test_img_data;
        this->img[i].img_w = w;
        this->img[i].img_h = h;
        this->img[i].blend_mode = IMG_BYPASS_MODE;
        this->img[i].high_quality = true;
        this->img[i].opacity_value = UINT8_MAX;
        draw_img_new_area(this->img + i, NULL);

    }

    GUI_UNUSED(this);
    GUI_UNUSED(obj);
    GUI_UNUSED(tp);
    GUI_UNUSED(dc);
}

static void gui_3d_draw(gui_3d_t *this)
{
    touch_info_t *tp = tp_get_info();
    gui_obj_t *obj = (gui_obj_t *)this;
    gui_dispdev_t *dc = gui_get_dc();

    GUI_UNUSED(this);
    GUI_UNUSED(obj);
    GUI_UNUSED(tp);
    GUI_UNUSED(dc);

    for (uint32_t i = 0; i < this->attrib.num_face_num_verts; i++)
    {
        draw_img_cache(this->img + i, IMG_SRC_MEMADDR);
        gui_acc_blit_to_dc(this->img + i, dc, NULL);
        draw_img_free(this->img + i, IMG_SRC_MEMADDR);
    }



}

static void gui_3d_end(gui_3d_t *this)
{
    touch_info_t *tp = tp_get_info();
    gui_obj_t *obj = (gui_obj_t *)this;
    gui_dispdev_t *dc = gui_get_dc();

    GUI_UNUSED(this);
    GUI_UNUSED(obj);
    GUI_UNUSED(tp);
    GUI_UNUSED(dc);

    gui_free(this->face);
    this->face = NULL;

    tinyobj_attrib_free(&this->attrib);
    tinyobj_shapes_free(this->shapes, this->num_shapes);
    tinyobj_materials_free(this->materials, this->num_materials);
}

static void gui_3d_destory(gui_3d_t *this)
{
    touch_info_t *tp = tp_get_info();
    gui_obj_t *obj = (gui_obj_t *)this;
    gui_dispdev_t *dc = gui_get_dc();

    GUI_UNUSED(this);
    GUI_UNUSED(obj);
    GUI_UNUSED(tp);
    GUI_UNUSED(dc);
}

static void gui_3d_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_PREPARE:
            gui_3d_prepare((gui_3d_t *)obj);
            break;

        case OBJ_DRAW:
            gui_3d_draw((gui_3d_t *)obj);
            break;

        case OBJ_END:
            gui_3d_end((gui_3d_t *)obj);
            break;

        case OBJ_DESTORY:
            gui_3d_destory((gui_3d_t *)obj);
            break;

        default:
            break;
        }
    }
}





static void gui_3d_ctor(gui_3d_t *this,
                        gui_obj_t              *parent,
                        const char             *name,
                        void                   *data,
                        uint32_t               len,
                        int16_t                 x,
                        int16_t                 y,
                        int16_t                 w,
                        int16_t                 h)
{
    //for obj class
    gui_obj_t *obj = (gui_obj_t *)this;
    gui_obj_ctor(obj, parent, name, x, y, w, h);

    this->data = data;
    this->len = len;

    obj->obj_cb = gui_3d_cb;
    obj->has_prepare_cb = true;
    obj->has_draw_cb = true;
    obj->has_end_cb = true;
    obj->has_destroy_cb = true;
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/

/**
 * @brief
 *
 * @param parent
 * @param name
 * @param data
 * @param x
 * @param y
 * @param w
 * @param h
 * @return gui_3d_t*
 */
gui_3d_t *gui_3d_create(void       *parent,
                        const char *name,
                        void       *data,
                        uint32_t    len,
                        int16_t     x,
                        int16_t     y,
                        int16_t     w,
                        int16_t     h)
{
    GUI_ASSERT(parent != NULL);

    if (name == NULL)
    {
        name = "WIDGET_3D";
    }

    gui_3d_t *this = gui_malloc(sizeof(gui_3d_t));
    GUI_ASSERT(this != NULL);
    memset(this, 0x00, sizeof(gui_3d_t));

    gui_3d_ctor(this, (gui_obj_t *)parent, name, data, len, x, y, w, h);

    gui_list_init(&(GET_BASE(this)->child_list));
    if ((GET_BASE(this)->parent) != NULL)
    {
        gui_list_insert_before(&((GET_BASE(this)->parent)->child_list),
                               &(GET_BASE(this)->brother_list));
    }

    GET_BASE(this)->create_done = true;
    return this;
}

