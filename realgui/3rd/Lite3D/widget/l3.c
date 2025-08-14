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
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "l3.h"
#include "l3_common.h"
#include "l3_tria.h"
#include "l3_rect.h"
#include "gui_version.h"
#include "l3_port.h"


/*============================================================================*
 *                           Public Functions
 *============================================================================*/


// Function to return a detailed version string
char *l3_version(void)
{
    // Static string to hold the version details
    // Use snprintf for safe string formatting (change buffer size if needed)
    static char version_info[256];
    snprintf(version_info, sizeof(version_info),
             "Version: %s\nBranch: %s\nCommit: %s\nBuild Date: %s\nRepository Status: %s",
             VERSION_TAG, VERSION_BRANCH, VERSION_COMMIT, VERSION_BUILD_DATE, VERSION_REPO_STATUS);

    return version_info;
}

l3_model_t *l3_create_model(void                 *desc_addr,
                            L3_DRAW_TYPE          draw_type,
                            int16_t               x,
                            int16_t               y,
                            int16_t               w,
                            int16_t               h)
{
    l3_description_t *desc = l3_load_description((void *)desc_addr);

    l3_model_t *this = l3_malloc(sizeof(l3_model_t));
    assert(this != NULL);
    memset(this, 0x00, sizeof(l3_model_t));

    this->x = x;
    this->y = y;
    this->viewPortWidth = w;
    this->viewPortHeight = h;

    this->desc = desc;
    this->draw_type = draw_type;

    switch (this->desc->face_type)
    {
    case LITE_3D_FACE_RECTANGLE:
        {
            this->face.rect_face = l3_malloc(sizeof(l3_rect_face_t) *
                                             this->desc->attrib.num_face_num_verts);
            memset(this->face.rect_face, 0x1,
                   sizeof(l3_rect_face_t) * this->desc->attrib.num_face_num_verts);

            if (this->draw_type != L3_DRAW_FRONT_AND_SORT)
            {
                this->img = l3_malloc(sizeof(l3_draw_rect_img_t) * this->desc->attrib.num_face_num_verts);
                memset(this->img, 0x00, sizeof(l3_draw_rect_img_t) * this->desc->attrib.num_face_num_verts);
            }
        }
        break;

    case LITE_3D_FACE_TRIANGLE:
        {
            this->face.tria_face = l3_malloc(sizeof(l3_tria_face_t) *
                                             this->desc->attrib.num_face_num_verts);
            memset(this->face.tria_face, 0x1,
                   sizeof(l3_tria_face_t) * this->desc->attrib.num_face_num_verts);
        }
        break;

    default:
        break;
    }

    if (this->draw_type == L3_DRAW_FRONT_AND_SORT)
    {
        this->combined_img = l3_malloc(sizeof(l3_draw_rect_img_t));
        memset(this->combined_img, 0x00, sizeof(l3_draw_rect_img_t));

        this->combined_img->data = l3_malloc(w * h * 2 + sizeof(l3_img_head_t));
        memset(this->combined_img->data, 0x00, w * h * 2 + sizeof(l3_img_head_t));

        l3_img_head_t *head = (l3_img_head_t *)this->combined_img->data;
        head->w = w;
        head->h = h;
        head->type = LITE_RGB565;
    }


    return this;
}

void l3_set_target_canvas(l3_model_t *_this, \
                          uint16_t x, uint16_t y, \
                          uint16_t w, uint16_t h, \
                          uint16_t bit_depth, \
                          uint8_t *canvas)
{
    _this->canvas.frame_buf = canvas;
    _this->canvas.bit_depth = bit_depth;
    _this->canvas.section.x1 = x;
    _this->canvas.section.y1 = y;
    _this->canvas.section.x2 = x + w - 1;
    _this->canvas.section.y2 = y + h - 1;
}

void l3_set_face_image(l3_model_t *_this, uint8_t face_index, void *image_addr)
{
    if (_this->desc->face_type == LITE_3D_FACE_RECTANGLE)
    {
        l3_set_rect_face_image(_this, face_index, image_addr);
    }
    else
    {
        //todo
    }
}

void l3_set_global_transform(l3_model_t *_this, l3_global_transform_cb cb)
{
    _this->global_transform_cb = (void (*)(l3_model_t *))cb;
}

void l3_set_face_transform(l3_model_t *_this, l3_face_transform_cb cb)
{
    _this->face_transform_cb = (l3_4x4_matrix_t(*)(l3_model_t *, size_t))cb;
}

void l3_push(l3_model_t *_this)
{
    if (_this == NULL)
    {
        return;
    }

    switch (_this->desc->face_type)
    {
    case LITE_3D_FACE_RECTANGLE:
        l3_rect_push(_this);
        break;

    case LITE_3D_FACE_TRIANGLE:
        l3_tria_push(_this);
        break;

    default:
        break;
    }
}

void l3_draw(l3_model_t *_this)
{
    if (_this == NULL)
    {
        return;
    }

    switch (_this->desc->face_type)
    {
    case LITE_3D_FACE_RECTANGLE:
        l3_rect_draw(_this);
        break;

    case LITE_3D_FACE_TRIANGLE:
        l3_tria_draw(_this);
        break;

    default:
        break;
    }
}


void l3_free_model(l3_model_t *_this)
{
    switch (_this->desc->face_type)
    {
    case LITE_3D_FACE_RECTANGLE:
        l3_rect_free_model(_this);
        break;

    case LITE_3D_FACE_TRIANGLE:
        l3_tria_free_model(_this);
        break;

    default:
        break;
    }

    if (_this->combined_img != NULL)
    {
        l3_free(_this->combined_img->data);
        _this->combined_img->data = NULL;

        l3_free(_this->combined_img);
        _this->combined_img = NULL;
    }

    if (_this->desc != NULL)
    {
        if (_this->desc->textures != NULL)
        {
            l3_free(_this->desc->textures);
            _this->desc->textures = NULL;
        }

        l3_free(_this->desc);
        _this->desc = NULL;
    }
    l3_free(_this);

}
