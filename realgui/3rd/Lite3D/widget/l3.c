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
#include "l3_obj.h"
#include "l3_gltf.h"
#include "l3_common.h"
#include "gui_version.h"


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

l3_model_base_t *l3_create_model(void            *desc_addr,
                                 L3_DRAW_TYPE     draw_type,
                                 int16_t          x,
                                 int16_t          y,
                                 int16_t          w,
                                 int16_t          h)
{
    unsigned char *ptr = (unsigned char *)desc_addr;
    l3_desc_file_head_t *file_head = (l3_desc_file_head_t *)ptr;
    if (file_head->magic != 0x3344)
    {
        return NULL;
    }

    switch (file_head->model_type)
    {
    case L3_MODEL_TYPE_OBJ:
        {
            l3_obj_model_t *obj_model = l3_create_obj_model((void *)desc_addr, draw_type, x, y, w, h);
            return &obj_model->base;
        }

    case L3_MODEL_TYPE_GLTF:
        {
            l3_gltf_model_t *gltf_model = l3_create_gltf_model((void *)desc_addr, draw_type, x, y, w, h);
            return &gltf_model->base;
        }

    default:
        return NULL;

    }
}

l3_model_base_t *l3_create_model_ftl(void            *desc_addr,
                                     L3_DRAW_TYPE     draw_type,
                                     int16_t          x,
                                     int16_t          y,
                                     int16_t          w,
                                     int16_t          h)
{
    l3_desc_file_head_t file_head;
    memcpy(&file_head, desc_addr, sizeof(l3_desc_file_head_t));
    uint8_t *data = (uint8_t *)l3_malloc(file_head.file_size);
    memcpy(data, desc_addr, file_head.file_size);

    switch (file_head.model_type)
    {
    case L3_MODEL_TYPE_OBJ:
        {
            l3_obj_model_t *obj_model = l3_create_obj_model((void *)data, draw_type, x, y, w, h);
            obj_model->base.raw_data_from_ftl = data;
            return &obj_model->base;
        }

    case L3_MODEL_TYPE_GLTF:
        {
            l3_gltf_model_t *gltf_model = l3_create_gltf_model((void *)data, draw_type, x, y, w, h);
            gltf_model->base.raw_data_from_ftl = data;
            return &gltf_model->base;
        }

    default:
        return NULL;

    }
}

void l3_push(l3_model_base_t *_this)
{
    _this->ops->push(_this);
}

void l3_draw(l3_model_base_t *_this)
{
    _this->ops->draw(_this);
}

void l3_free_model(l3_model_base_t *_this)
{
    _this->ops->model_free(_this);
}

void l3_set_global_transform(l3_model_base_t *_this, l3_global_transform_cb cb)
{
    if (_this->ops->set_global_transform)
    {
        _this->ops->set_global_transform(_this, cb);
    }
}

void l3_set_face_transform(l3_model_base_t *_this, l3_face_transform_cb cb)
{
    if (_this->ops->set_face_transform)
    {
        _this->ops->set_face_transform(_this, cb);
    }
}

void l3_set_target_canvas(l3_model_base_t *_this, \
                          uint16_t x, uint16_t y, \
                          uint16_t w, uint16_t h, \
                          uint16_t bit_depth, \
                          uint8_t *canvas)
{
    _this->ops->set_target_canvas(_this, x, y, w, h, bit_depth, canvas);
}

void l3_set_face_image(l3_model_base_t *_this, uint8_t face_index, void *image_addr)
{
    if (_this->ops->set_face_image)
    {
        _this->ops->set_face_image(_this, face_index, image_addr);
    }
}

bool l3_model_on_click(l3_model_base_t *_this, int x, int y)
{
    return _this->ops->on_click(_this, x, y);
}
