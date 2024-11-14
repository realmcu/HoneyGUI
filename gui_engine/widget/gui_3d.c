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

#include "stb_image.h"



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
    if (is_mtl == false)
    {
        gui_3d_t *this = (gui_3d_t *)ctx;

        *data = (char *)this->obj_data;
        (*len) = this->obj_len;
    }
    else
    {
        gui_3d_t *this = (gui_3d_t *)ctx;

        *data = (char *)this->mtl_data;
        (*len) = this->mtl_len;

    }

}

static void set_arc_w_and_h(gui_rgb_data_head_t *head, uint16_t w, uint16_t h)
{
    head->scan = 0;
    head->align = 0;
    head->resize = 0;//0-no resize;1-50%(x&y);2-70%;3-80%
    head->compress = 0;
    head->rsvd = 0;
    head->type = ARGB8888;
    head->version = 0;
    head->rsvd2 = 0;
    head->w = w;
    head->h = h;
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
        gui_3d_world_inititalize(&world, 0, 0, 3.0f, 50, 50, 0, 1);
        gui_3d_matrix_t result;
        GUI_UNUSED(result);
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


        int material_id = this->attrib.material_ids[i];

        int width, height, channels;

        char *path = gui_malloc(200);
        sprintf(path, "%s%s", "gui_engine\\example\\demo\\3d\\",
                this->materials[material_id].diffuse_texname);

        unsigned char *img = stbi_load(path, &width, &height, &channels, 0);
        gui_free(path);

        if (img == NULL)
        {
            gui_log("Error loading image: %s\n", stbi_failure_reason());
            GUI_ASSERT(img != NULL);
        }


        this->img[i].data = gui_malloc(width * height * channels + 8);
        set_arc_w_and_h(this->img[i].data, width, height);
        memcpy((unsigned char *)this->img[i].data + 8, img, width * height * channels);
        stbi_image_free(img);

        gui_3d_point_2d_t src[4] = {{0, 0}, {width, 0}, {width, height}, {0, height}};
        gui_3d_point_2d_t dst[4] = {{x0, y0}, {x1, y1}, {x2, y2}, {x3, y3}};

        gui_3d_generate_2d_matrix(src, dst, (float *)&this->img[i].matrix);
        memcpy(&this->img[i].inverse, &this->img[i].matrix, sizeof(gui_matrix_t));
        matrix_inverse(&this->img[i].inverse);

        this->img[i].img_w = width;
        this->img[i].img_h = height;
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

    for (uint32_t i = 0; i < this->attrib.num_face_num_verts; i++)
    {
        gui_free(this->img[i].data);
    }
    gui_free(this->img);

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
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/

void gui_3d_set_obj(gui_3d_t *this, void *data, uint32_t len)
{
    this->obj_data = data;
    this->obj_len = len;
}

void gui_3d_set_mtl(gui_3d_t *this, void *data, uint32_t len)
{
    this->mtl_data = data;
    this->mtl_len = len;
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

    gui_3d_ctor(this, (gui_obj_t *)parent, name, x, y, w, h);

    gui_list_init(&(GET_BASE(this)->child_list));
    if ((GET_BASE(this)->parent) != NULL)
    {
        gui_list_insert_before(&((GET_BASE(this)->parent)->child_list),
                               &(GET_BASE(this)->brother_list));
    }

    GET_BASE(this)->create_done = true;
    return this;
}

