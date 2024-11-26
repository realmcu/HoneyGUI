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
#include "gui_fb.h"
#include <guidef.h>
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

static void face_transfrom(gui_3d_t *this, size_t s/*shape_offset*/, size_t i /*face_offset*/,
                           gui_3d_world_t *world, gui_3d_camera_t *camera)
{
    gui_dispdev_t *dc = gui_get_dc();

    gui_3d_scene(this->face + i, world, camera);

    int material_id = this->desc->attrib.material_ids[i];
    this->img[i].data = (void *)this->desc->textures[material_id];

    gui_rgb_data_head_t *head = (gui_rgb_data_head_t *)this->img[i].data;
    float width = head->w;
    float height = head->h;

    gui_3d_point_2d_t src[4];
    gui_3d_point_2d_t dst[4];
    for (size_t j = 0; j < 4; ++j)
    {
        src[j].x = this->face[i].vertex[j].u * width;
        src[j].y = (1.0f - this->face[i].vertex[j].v) * height;

        dst[j].x = this->face[i].transform_vertex[j].position.x;
        dst[j].y = this->face[i].transform_vertex[j].position.y;
    }

    gui_3d_generate_2d_matrix(src, dst, (float *)&this->img[i].matrix);
    memcpy(&this->img[i].inverse, &this->img[i].matrix, sizeof(gui_matrix_t));
    matrix_inverse(&this->img[i].inverse);

    this->img[i].img_w = width;
    this->img[i].img_h = height;
    this->img[i].blend_mode = IMG_SRC_OVER_MODE;
    this->img[i].high_quality = true;
    this->img[i].opacity_value = UINT8_MAX;
    draw_img_new_area(this->img + i, NULL);
}

static void convert_to_face(gui_3d_t *this, size_t i /*face_offset*/)
{
    size_t index_offset = 0;
    for (size_t s = 0; s < i; s++)
    {
        index_offset += this->desc->attrib.face_num_verts[i];
    }

    for (size_t j = 0; j < this->desc->attrib.face_num_verts[i]; ++j)
    {
        tinyobj_vertex_index_t idx = this->desc->attrib.faces[index_offset + j];

        float *v = &this->desc->attrib.vertices[3 * idx.v_idx];
        float *vt = &this->desc->attrib.texcoords[2 * idx.vt_idx];
        float *vn = &this->desc->attrib.normals[3 * idx.vn_idx];

        this->face[i].vertex[j].position.x = v[0];
        this->face[i].vertex[j].position.y = v[1];
        this->face[i].vertex[j].position.z = v[2];
        this->face[i].vertex[j].position.w = 1;

        this->face[i].vertex[j].u = vt[0];
        this->face[i].vertex[j].v = vt[1];

        this->face[i].vertex[j].normal.x = vn[0];
        this->face[i].vertex[j].normal.y = vn[1];
        this->face[i].vertex[j].normal.z = vn[2];
        this->face[i].vertex[j].normal.w = 1;
    }
}

static void gui_3d_update_att(gui_obj_t *obj)
{
    gui_3d_t *this = (void *)obj;
    if (this->animate)
    {
        animate_frame_update(this->animate, obj);
    }
}

static void gui_3d_prepare(gui_3d_t *this)
{
    touch_info_t *tp = tp_get_info();
    gui_obj_t *obj = (gui_obj_t *)this;
    gui_3d_update_att(obj);
    gui_dispdev_t *dc = gui_get_dc();

    // this->flags = TINYOBJ_FLAG_TRIANGULATE;
    this->flags = 0;

    this->face = gui_malloc(sizeof(gui_3d_face_t) * this->desc->attrib.num_face_num_verts);

    this->img = gui_malloc(sizeof(draw_img_t) * this->desc->attrib.num_face_num_verts);
    memset(this->img, 0x00, sizeof(draw_img_t) * this->desc->attrib.num_face_num_verts);


    for (size_t i = 0; i < this->desc->num_shapes; i++)
    {
        gui_3d_world_t world;
        gui_3d_camera_t camera;
        GUI_ASSERT(this->shape_transform_cb != NULL);

        for (size_t j = 0; j < this->desc->shapes[i].length /*number of face*/; j++)
        {
            this->shape_transform_cb(this, this->desc->shapes[i].face_offset + j, &world, &camera);
            // gui_log("Shape[%d][%s] has [%d] faces, and offset is %d\n", i, this->desc->shapes[i].name, this->desc->shapes[i].length, this->desc->shapes[i].face_offset + j);
            convert_to_face(this, this->desc->shapes[i].face_offset + j);

            face_transfrom(this, i, this->desc->shapes[i].face_offset + j, &world, &camera);

        }

    }
    gui_fb_change();

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

    for (uint32_t i = 0; i < this->desc->attrib.num_face_num_verts; i++)
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

    if (this->img != NULL)
    {
        gui_free(this->img);
        this->img = NULL;
    }

    gui_free(this->face);
    this->face = NULL;
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

    tinyobj_attrib_free(&this->desc->attrib);
    tinyobj_shapes_free(this->desc->shapes, this->desc->num_shapes);
    tinyobj_materials_free(this->desc->materials, this->desc->num_materials);

    gui_free(this->desc->texture_sizes);
    gui_free(this->desc->textures);
    gui_free(this->desc);
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

static char *read_string_from_ptr(unsigned char **ptr)
{
    if (*ptr == NULL)
    {
        return NULL;
    }
    unsigned int length;
    memcpy(&length, *ptr, sizeof(unsigned int));
    *ptr += sizeof(unsigned int);
    if (length == 0)
    {
        return NULL;
    }
    char *string = (char *)TINYOBJ_MALLOC(length * sizeof(char));
    memcpy(string, *ptr, length);
    *ptr += length;

    return string;
}

static gui_3d_description_t *gui_get_3d_desc(void *desc_addr)
{
    gui_3d_description_t *desc = (gui_3d_description_t *)gui_malloc(sizeof(gui_3d_description_t));
    unsigned char *ptr = (unsigned char *)desc_addr;

    //attrib
    memcpy(&desc->attrib.num_vertices, ptr, sizeof(unsigned int));
    ptr += sizeof(unsigned int);
    memcpy(&desc->attrib.num_normals, ptr, sizeof(unsigned int));
    ptr += sizeof(unsigned int);
    memcpy(&desc->attrib.num_texcoords, ptr, sizeof(unsigned int));
    ptr += sizeof(unsigned int);
    memcpy(&desc->attrib.num_faces, ptr, sizeof(unsigned int));
    ptr += sizeof(unsigned int);
    memcpy(&desc->attrib.num_face_num_verts, ptr, sizeof(unsigned int));
    ptr += sizeof(unsigned int);
    memcpy(&desc->attrib.pad0, ptr, sizeof(int));
    ptr += sizeof(int);

    desc->attrib.vertices = (float *)TINYOBJ_MALLOC(desc->attrib.num_vertices * 3 * sizeof(float));
    memcpy(desc->attrib.vertices, ptr, desc->attrib.num_vertices * 3 * sizeof(float));
    ptr += desc->attrib.num_vertices * 3 * sizeof(float);
    desc->attrib.normals = (float *)TINYOBJ_MALLOC(desc->attrib.num_normals * 3 * sizeof(float));
    memcpy(desc->attrib.normals, ptr, desc->attrib.num_normals * 3 * sizeof(float));
    ptr += desc->attrib.num_normals * 3 * sizeof(float);
    desc->attrib.texcoords = (float *)TINYOBJ_MALLOC(desc->attrib.num_texcoords * 2 * sizeof(float));
    memcpy(desc->attrib.texcoords, ptr, desc->attrib.num_texcoords * 2 * sizeof(float));
    ptr += desc->attrib.num_texcoords * 2 * sizeof(float);
    desc->attrib.faces = (tinyobj_vertex_index_t *)TINYOBJ_MALLOC(desc->attrib.num_faces * sizeof(
                                                                      tinyobj_vertex_index_t));
    memcpy(desc->attrib.faces, ptr, desc->attrib.num_faces * sizeof(tinyobj_vertex_index_t));
    ptr += desc->attrib.num_faces * sizeof(tinyobj_vertex_index_t);
    desc->attrib.face_num_verts = (int *)TINYOBJ_MALLOC(desc->attrib.num_face_num_verts * sizeof(int));
    memcpy(desc->attrib.face_num_verts, ptr, desc->attrib.num_face_num_verts * sizeof(int));
    ptr += desc->attrib.num_face_num_verts * sizeof(int);
    desc->attrib.material_ids = (int *)TINYOBJ_MALLOC(desc->attrib.num_faces * sizeof(int));
    memcpy(desc->attrib.material_ids, ptr, desc->attrib.num_faces * sizeof(int));
    ptr += desc->attrib.num_faces * sizeof(int);

    //shape
    memcpy(&desc->num_shapes, ptr, sizeof(unsigned int));
    ptr += sizeof(unsigned int);
    desc->shapes = (tinyobj_shape_t *)TINYOBJ_MALLOC(desc->num_shapes * sizeof(tinyobj_shape_t));
    for (size_t i = 0; i < desc->num_shapes; i++)
    {
        desc->shapes[i].name = read_string_from_ptr(&ptr);

        memcpy(&desc->shapes[i].face_offset, ptr, sizeof(unsigned int));
        ptr += sizeof(unsigned int);
        memcpy(&desc->shapes[i].length, ptr, sizeof(unsigned int));
        ptr += sizeof(unsigned int);
    }

    //material
    memcpy(&desc->num_materials, ptr, sizeof(unsigned int));
    ptr += sizeof(unsigned int);
    desc->materials = (tinyobj_material_t *)TINYOBJ_MALLOC(desc->num_materials * sizeof(
                                                               tinyobj_material_t));
    for (size_t i = 0; i < desc->num_materials; i++)
    {
        tinyobj_material_t *mat = &desc->materials[i];
        mat->name = read_string_from_ptr(&ptr);
        memcpy(mat->ambient, ptr, 3 * sizeof(float));
        ptr += 3 * sizeof(float);
        memcpy(mat->diffuse, ptr, 3 * sizeof(float));
        ptr += 3 * sizeof(float);
        memcpy(mat->specular, ptr, 3 * sizeof(float));
        ptr += 3 * sizeof(float);
        memcpy(mat->transmittance, ptr, 3 * sizeof(float));
        ptr += 3 * sizeof(float);
        memcpy(mat->emission, ptr, 3 * sizeof(float));
        ptr += 3 * sizeof(float);
        memcpy(&mat->shininess, ptr, sizeof(float));
        ptr += sizeof(float);
        memcpy(&mat->ior, ptr, sizeof(float));
        ptr += sizeof(float);
        memcpy(&mat->dissolve, ptr, sizeof(float));
        ptr += sizeof(float);
        memcpy(&mat->illum, ptr, sizeof(int));
        ptr += sizeof(int);

        mat->ambient_texname = read_string_from_ptr(&ptr);
        mat->diffuse_texname = read_string_from_ptr(&ptr);
        mat->specular_texname = read_string_from_ptr(&ptr);
        mat->specular_highlight_texname = read_string_from_ptr(&ptr);
        mat->bump_texname = read_string_from_ptr(&ptr);
        mat->displacement_texname = read_string_from_ptr(&ptr);
        mat->alpha_texname = read_string_from_ptr(&ptr);
    }

    //textures
    desc->textures = (unsigned char **)gui_malloc(desc->num_materials * sizeof(unsigned char *));
    desc->texture_sizes = (unsigned int *)gui_malloc(desc->num_materials * sizeof(unsigned int));
    for (size_t i = 0; i < desc->num_materials; i++)
    {
        memcpy(&desc->texture_sizes[i], ptr, sizeof(unsigned int));
        ptr += sizeof(unsigned int);
        desc->textures[i] = (unsigned char *)gui_malloc(desc->texture_sizes[i] * sizeof(unsigned char));
        memcpy(desc->textures[i], ptr, desc->texture_sizes[i] * sizeof(unsigned char));
        ptr += desc->texture_sizes[i] * sizeof(unsigned char);
    }

    return desc;

}



static void gui_3d_ctor(gui_3d_t               *this,
                        gui_obj_t              *parent,
                        const char             *name,
                        void                   *desc_addr,
                        int16_t                 x,
                        int16_t                 y,
                        int16_t                 w,
                        int16_t                 h)
{
    //for obj class
    gui_obj_t *obj = (gui_obj_t *)this;
    gui_obj_ctor(obj, parent, name, x, y, w, h);

    obj->obj_cb = gui_3d_cb;
    obj->has_prepare_cb = true;
    obj->has_draw_cb = true;
    obj->has_end_cb = true;
    obj->has_destroy_cb = true;

    // for self
    this->desc = gui_get_3d_desc(desc_addr);
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/

void gui_3d_set_shape_transform_cb(gui_3d_t *this, size_t s/*shape_offset*/,
                                   void (*cb)(gui_3d_t *this, size_t s, gui_3d_world_t *world, gui_3d_camera_t *camera))
{
    this->shape_transform_cb = cb;
}

void gui_3d_set_animate(gui_3d_t     *this,
                        uint32_t      dur,
                        int           repeat_count,
                        void         *callback,
                        void         *p)
{
    GUI_SET_ANIMATE_HELPER
}

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
                        void       *desc_addr,
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

    gui_3d_ctor(this, (gui_obj_t *)parent, name, desc_addr, x, y, w, h);

    gui_list_init(&(GET_BASE(this)->child_list));
    if ((GET_BASE(this)->parent) != NULL)
    {
        gui_list_insert_before(&((GET_BASE(this)->parent)->child_list),
                               &(GET_BASE(this)->brother_list));
    }

    GET_BASE(this)->create_done = true;
    return this;
}

