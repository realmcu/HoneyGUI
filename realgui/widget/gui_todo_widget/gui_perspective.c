/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_perspective.c
  * @brief perspective widget
  * @details create 3D widget
  * @author howie_wang@realsil.com.cn
  * @date 2023/10/20
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <math.h>
#include <string.h>
#include "gui_obj.h"
#include "tp_algo.h"
#include "wheel_algo.h"
#include "draw_img.h"
#include "acc_api.h"
#include "gui_matrix.h"
#include "gui_perspective.h"
#include "gui_fb.h"


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

/**
  * @brief  ...
  * @note   ...
  * @param[IN]  param1 ...
  * @param[IN]  param2 ...
  * @return ...
  * @retval ...
  */

static void gui_perspective_scale_3d(gui_vertex_t *vertex, float scale)
{
    vertex->x *= scale;
    vertex->y *= scale;
    vertex->z *= scale;
}

static bool gui_perspective_point_in_rect(draw_img_t *img, int16_t x, int16_t y)
{
    if (x < img->img_target_x)
    {
        return false;
    }
    if (x > img->img_target_x + img->img_target_x)
    {
        return false;
    }
    if (y < img->img_target_y)
    {
        return false;
    }
    if (y > img->img_target_y + img->img_target_h)
    {
        return false;
    }
    return true;
}

static void gui_perspective_prepare(gui_obj_t *obj)
{
    struct gui_matrix rotate_3D;
    gui_vertex_t rv0, rv1, rv2, rv3;
    gui_vertex_t tv0, tv1, tv2, tv3;
    gui_dispdev_t *dc = gui_get_dc();
    touch_info_t *tp = tp_get_info();
    touch_info_t *wheel = wheel_get_info();

    gui_perspective_t *this = (gui_perspective_t *)obj;
    float w = 300;
    float h = 300;
    float d = w * fix_sin(60);

    gui_vertex_t v0 = {-w / 2 + 10, -h / 2, d};
    gui_vertex_t v1 = {w  / 2 - 10, -h / 2, d};
    gui_vertex_t v2 = {w  / 2 - 10, h  / 2, d};
    gui_vertex_t v3 = {-w / 2 + 10, h  / 2, d};

    float xoff, yoff, zoff, t;
    uint8_t last;

    for (uint8_t i = 0; i < 6; i++)
    {
        matrix_identity(&this->img[i].matrix);
    }

    for (int i = 0; i < 6; i++)
    {
        if (this->src_mode[i] == IMG_SRC_FILESYS)
        {
            this->data[i] = this->file_path[i];
            this->img[i].data = this->data[i];
        }
        else if (this->src_mode[i] == IMG_SRC_MEMADDR)
        {
            this->img[i].data = this->data[i];
        }
        else if (this->src_mode[i] == IMG_SRC_FTL)
        {
            this->img[i].data = this->ftl[i];
        }
    }

    gui_perspective_scale_3d(&v0, 1.0f);
    gui_perspective_scale_3d(&v1, 1.0f);
    gui_perspective_scale_3d(&v2, 1.0f);
    gui_perspective_scale_3d(&v3, 1.0f);

    for (uint8_t i = 0; i < 6; i++)
    {
        this->ry[i] = i * 60;
    }

    switch (tp->type)
    {
    case TOUCH_HOLD_X:
        {
            this->release_x = tp->deltaX;
        }
        break;
    default:
        break;
    }

    switch (wheel->type)
    {
    case TOUCH_HOLD_Y:
        {
            this->release_x = wheel->history_y;
        }
        break;
    case WHEEL_FINISHED:
        break;
    default:
        break;
    }

    for (uint8_t i = 0; i < 6; i++)
    {
        this->ry[i] += this->release_x;
        this->temp[i] = this->ry[i];
    }

    for (uint8_t i = 0; i < 6 - 1; i++)
    {
        for (uint8_t j = 0; j < 6 - i - 1; j++)
        {
            if (fix_cos(this->temp[j]) > fix_cos(this->temp[j + 1]))
            {
                t = this->temp[j];
                this->temp[j] = this->temp[j + 1];
                this->temp[j + 1] = t;
            }
        }
    }

    for (uint8_t i = 0; i < 6; i++)
    {
        matrix_compute_rotate(0, this->ry[i], 0, &rotate_3D);

        matrix_transfrom_rotate(&rotate_3D, &v0, &tv0, 0, 0, 0);
        matrix_transfrom_rotate(&rotate_3D, &v1, &tv1, 0, 0, 0);
        matrix_transfrom_rotate(&rotate_3D, &v2, &tv2, 0, 0, 0);
        matrix_transfrom_rotate(&rotate_3D, &v3, &tv3, 0, 0, 0);

        matrix_compute_rotate(0, 0, 0, &rotate_3D);//set here for x-axis

        xoff = (float)dc->screen_width / 2;
        yoff = (float)dc->screen_height / 2 + 100;
        zoff = -2 * d;

        matrix_transfrom_rotate(&rotate_3D, &tv0, &rv0, xoff, yoff, zoff);
        matrix_transfrom_rotate(&rotate_3D, &tv1, &rv1, xoff, yoff, zoff);
        matrix_transfrom_rotate(&rotate_3D, &tv2, &rv2, xoff, yoff, zoff);
        matrix_transfrom_rotate(&rotate_3D, &tv3, &rv3, xoff, yoff, zoff);

        draw_img_load_scale(&this->img[i], (IMG_SOURCE_MODE_TYPE)this->src_mode[i]);

        gui_vertex_t p = {(float)(dc->screen_width) / 2, 0, 2 * d};
        matrix_transfrom_blit(this->img[i].img_w, this->img[i].img_h, &p, &rv0, &rv1, &rv2, &rv3,
                              &this->img[i].matrix);
        memcpy(&this->img[i].inverse, &this->img[i].matrix, sizeof(struct gui_matrix));
        matrix_inverse(&this->img[i].inverse);

        draw_img_new_area(&this->img[i], NULL);
        if (gui_perspective_point_in_rect(&this->img[i], tp->x, tp->y) == true)
        {
            switch (tp->type)
            {
            case TOUCH_SHORT:
                {
                    //gui_obj_enable_event(obj, (gui_event_t)(GUI_EVENT_1 + i));
                    GUI_ASSERT(0);
                }
                break;
            default:
                break;
            }
        }
    }

    last = this->checksum;
    this->checksum = this->release_x;

    if (last != this->checksum)
    {
        gui_fb_change();
    }
}

static void gui_perspective_draw_cb(gui_obj_t *obj)
{
    gui_dispdev_t *dc = gui_get_dc();
    touch_info_t *tp = tp_get_info();
    gui_perspective_t *this = (gui_perspective_t *)obj;
    draw_img_t *draw_img;

    for (uint8_t j = 0; j < 6; j++)
    {
        for (uint8_t i = 0; i < 6; i++)
        {
            if (this->temp[j] == this->ry[i])
            {
                draw_img = &this->img[i];
                // cache img to buffer
                draw_img_cache(draw_img, (IMG_SOURCE_MODE_TYPE)this->src_mode[i]);

                // blit
                gui_acc_blit_to_dc(draw_img, dc, NULL);

                // release img if cached
                draw_img_free(draw_img, (IMG_SOURCE_MODE_TYPE)this->src_mode[i]);
            }
        }
    }
}

static void gui_perspective_end(gui_obj_t *obj)
{
    GUI_ASSERT(obj != NULL);
    gui_perspective_t *perspective = (gui_perspective_t *)obj;

    for (uint8_t i = 0; i < 6; i++)
    {
        if (draw_img_acc_end_cb != NULL)
        {
            draw_img_acc_end_cb(&perspective->img[i]);
        }
    }
}

static void gui_perspective_destroy(gui_obj_t *obj)
{
    gui_perspective_t *this = (gui_perspective_t *)obj;

    for (uint8_t i = 0; i < 6; i++)
    {
        if (this->src_mode[i] == IMG_SRC_FILESYS)
        {
#ifdef _WIN32
            // free path transforming memory on win
            gui_free(this->file_path[i]);
#endif
            this->file_path[i] = NULL;
        }
    }
}

static void gui_perspective_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_PREPARE:
            gui_perspective_prepare(obj);
            break;

        case OBJ_DRAW:
            gui_perspective_draw_cb(obj);
            break;

        case OBJ_END:
            gui_perspective_end(obj);
            break;

        case OBJ_DESTROY:
            gui_perspective_destroy(obj);
            break;

        default:
            break;
        }
    }
}

static void gui_perspective_ctor(gui_perspective_t         *this,
                                 gui_obj_t                 *parent,
                                 const char                *name,
                                 gui_perspective_imgfile_t *img_file,
                                 int16_t                    x,
                                 int16_t                    y,
                                 int16_t                    w,
                                 int16_t                    h)
{
    gui_obj_t *root = (gui_obj_t *)this;
    void **array;
    draw_img_t *draw_img;
    char *path;

    gui_obj_ctor(root, parent, name, x, y, w, h);

    root->type = CUBE;
    root->obj_cb = gui_perspective_cb;
    root->has_prepare_cb = true;
    root->has_draw_cb = true;
    root->has_end_cb = true;
    root->has_destroy_cb = true;

    array = (void **)img_file->img_path;

    for (uint8_t i = 0; i < 6; i++)
    {
        draw_img = &this->img[i];
        this->src_mode[i] = img_file->src_mode[i];

        if (img_file->src_mode[i] == IMG_SRC_FILESYS)
        {
            path = array[i];
#ifdef _WIN32
            path = gui_filepath_transforming(array[i]);
#endif
            this->file_path[i] = path;
        }
        else if (img_file->src_mode[i] == IMG_SRC_MEMADDR)
        {
            this->data[i] = array[i];
        }
        else if (img_file->src_mode[i] == IMG_SRC_FTL)
        {
            this->data[i] = array[i];
            this->ftl[i] =  array[i];
        }

        draw_img->opacity_value = UINT8_MAX;
        draw_img->blend_mode = IMG_SRC_OVER_MODE;
        this->temp[i] = i * 30;
        this->ry[i] = i * 30;
    }
}
/*============================================================================*
 *                           Public Functions
 *============================================================================*/
void gui_perspective_set_mode(gui_perspective_t *perspective,
                              uint8_t            img_index,
                              BLEND_MODE_TYPE    mode)
{
    GUI_ASSERT(perspective != NULL);
    draw_img_t *draw_img = &perspective->img[img_index];
    draw_img->blend_mode = mode;
}

void gui_perspective_set_img(gui_perspective_t *perspective, gui_perspective_imgfile_t *img_file)
{
    gui_perspective_t *this = perspective;
    void *path;

    GUI_ASSERT(this != NULL);

    for (uint8_t i = 0; i < 6; i++)
    {
        // reset file data
        if (this->src_mode[i] == IMG_SRC_FILESYS)
        {
#ifdef _WIN32
            gui_free(this->file_path[i]);
#endif
            this->file_path[i] = NULL;
            this->data[i] = NULL;
        }

        // set new images
        this->src_mode[i] = img_file->src_mode[i];
        if (img_file->src_mode[i] == IMG_SRC_FILESYS)
        {
            path = (void *)img_file->img_path[i];
#ifdef _WIN32
            path = gui_filepath_transforming(path);
#endif
            this->file_path[i] = path;
        }
        else if (img_file->src_mode[i] == IMG_SRC_MEMADDR)
        {
            this->data[i] = img_file->data_addr[i];
        }
        else if (img_file->src_mode[i] == IMG_SRC_FTL)
        {
            this->data[i] = img_file->data_addr[i];
            this->ftl[i] =  img_file->data_addr[i];
        }
    }
}

gui_perspective_t *gui_perspective_create(void                      *parent,
                                          const char                *name,
                                          gui_perspective_imgfile_t *img_file,
                                          int16_t                    x,
                                          int16_t                    y)
{
    gui_perspective_t *this;

    GUI_ASSERT(parent != NULL);
    if (name == NULL)
    {
        name = "DEFAULT_PERS";
    }

    this = gui_malloc(sizeof(gui_perspective_t));
    GUI_ASSERT(this != NULL);
    memset(this, 0x00, sizeof(gui_perspective_t));

    gui_perspective_ctor(this, (gui_obj_t *)parent, name, img_file, x, y, 0, 0);

    gui_list_init(&(GET_BASE(this)->child_list));
    if ((GET_BASE(this)->parent) != NULL)
    {
        gui_list_insert_before(&((GET_BASE(this)->parent)->child_list),
                               &(GET_BASE(this)->brother_list));
    }

    GET_BASE(this)->create_done = true;
    return this;
}
