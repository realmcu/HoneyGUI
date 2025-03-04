/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_gallery.c
  * @brief gallery widget
  * @details create gallery widget
  * @author roy@realsil.com.cn
  * @date 2024/05/08
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
#include "gui_img.h"
// #include "acc_init.h"
#include "gui_matrix.h"
#include "gui_gallery.h"
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

static int16_t gui_gallery_get_position(gui_img_t *img)
{
    int16_t pos = 0;
    int16_t img_x = img->base.x;
    int16_t img_w = 0;

    img_w = img->base.w * gui_img_get_transform_scale_x(img);
    pos = img_x + (img_w >> 1);
    return pos;
}

static void gui_gallery_reset_position(gui_gallery_t *this, gui_img_t *img, int16_t pos)
{
    // int16_t w = this->base.w;
    int16_t h = this->base.h;
    // int16_t img_x = img->base.x;
    int16_t img_w = 0, img_h = 0;
    float scale = (pos == this->pos_c) ? 1.f : this->side_scale;

    gui_img_scale(img, scale, scale);
    img_w = img->base.w * scale;
    img_h = img->base.h * scale;
    gui_img_set_location(img, pos - (img_w >> 1), (h - img_h) >> 1);
}

static void gui_gallery_img_copy(gui_img_t *img_des, gui_img_t *img_src)
{
    gui_list_t  child_list_tmp;
    gui_list_t  brother_list_tmp;

    child_list_tmp = img_des->base.child_list;
    brother_list_tmp = img_des->base.brother_list;

    memcpy(img_des, img_src, sizeof(gui_img_t));
    img_des->base.child_list = child_list_tmp;
    img_des->base.brother_list = brother_list_tmp;
}

static void *gui_gallery_get_img_data(gui_gallery_t *this, int16_t idx)
{
    void *data = this->img_array[0];

    if ((idx >= 0) && (idx < this->num_pic))
    {
        data = this->img_array[idx];
    }
    else if (this->loop_en)
    {
        idx = (idx + 3 * this->num_pic) % this->num_pic;
        data = this->img_array[idx];
    }

    return data;
}

static bool gui_gallery_img_exist(gui_gallery_t *this, int32_t idx)
{
    if (this->loop_en || ((idx >= 0) && (idx < this->num_pic)))
    {
        return true;
    }
    return false;
}
static void gui_gallery_img_roll(gui_gallery_t *this, uint8_t dir)
{
    int16_t idx_center = this->idx_center;
    gui_img_t *img_btm_l = this->img_bottom[GALLERY_IMG_IDX_BTM_LEFT];
    gui_img_t *img_btm_r = this->img_bottom[GALLERY_IMG_IDX_BTM_RIGHT];

    if (dir == GALLERY_LEFT)
    {
        // cover left
        if (!(this->loop_en) && (idx_center == this->num_pic - 1))
        {
            gui_log("right edge\n");
            return;
        }
        idx_center++;
        gui_gallery_img_copy(this->img_bottom[GALLERY_IMG_IDX_BTM_LEFT], this->img[GALLERY_IMG_IDX_LEFT]);
        gui_gallery_img_copy(this->img[GALLERY_IMG_IDX_LEFT], this->img[GALLERY_IMG_IDX_CENTER]);
        gui_gallery_img_copy(this->img[GALLERY_IMG_IDX_CENTER], this->img[GALLERY_IMG_IDX_RIGHT]);
        gui_gallery_img_copy(this->img[GALLERY_IMG_IDX_RIGHT], this->img_bottom[GALLERY_IMG_IDX_BTM_RIGHT]);

        img_btm_r->data = gui_gallery_get_img_data(this, idx_center + 2);
        gui_img_refresh_size(img_btm_r);
        gui_gallery_reset_position(this, this->img_bottom[GALLERY_IMG_IDX_BTM_RIGHT], this->pos_r);

        gui_obj_show(img_btm_l, false);
        gui_obj_show(img_btm_r, false);

    }
    else //if (dir == GALLERY_RIGHT)
    {
        if (!(this->loop_en) && (idx_center == 0))
        {
            gui_log("left edge\n");
            return;
        }
        idx_center--;
        gui_gallery_img_copy(this->img_bottom[GALLERY_IMG_IDX_BTM_RIGHT], this->img[GALLERY_IMG_IDX_RIGHT]);
        gui_gallery_img_copy(this->img[GALLERY_IMG_IDX_RIGHT], this->img[GALLERY_IMG_IDX_CENTER]);
        gui_gallery_img_copy(this->img[GALLERY_IMG_IDX_CENTER], this->img[GALLERY_IMG_IDX_LEFT]);
        gui_gallery_img_copy(this->img[GALLERY_IMG_IDX_LEFT], this->img_bottom[GALLERY_IMG_IDX_BTM_LEFT]);

        img_btm_l->data = gui_gallery_get_img_data(this, idx_center - 2);
        gui_img_refresh_size(img_btm_l);
        gui_gallery_reset_position(this, this->img_bottom[GALLERY_IMG_IDX_BTM_LEFT], this->pos_l);

        gui_obj_show(img_btm_r, false);
        gui_obj_show(img_btm_l, false);

    }
    this->idx_center = (this->loop_en) ? ((idx_center + this->num_pic) % this->num_pic) : idx_center;
    gui_log("this->idx_center %d\n", this->idx_center);
}


static void gui_gallery_prepare(gui_obj_t *obj)
{
    gui_dispdev_t *dc = gui_get_dc();
    touch_info_t *tp = tp_get_info();

    gui_gallery_t *this = (gui_gallery_t *)obj;
    gui_img_t *img_btm_l = this->img_bottom[GALLERY_IMG_IDX_BTM_LEFT];
    gui_img_t *img_btm_r = this->img_bottom[GALLERY_IMG_IDX_BTM_RIGHT];
//    int16_t x = this->base.x;
//    int16_t y = this->base.y;
//    int16_t w = this->base.w;
    int16_t h = this->base.h;

    uint8_t last;

// calculate
//    int16_t center_line_pos_l = 0, center_line_pos_r = 0;

//    center_line_pos_l = x + w * (1 - this->center_percent) / 2;
//    center_line_pos_r = x + w * (1 + this->center_percent) / 2;


// touch
    // gui_log("tp->type %d \n", tp->type);
    switch (tp->type)
    {
    case TOUCH_HOLD_X:
        {
            if (gui_obj_point_in_obj_rect(obj, tp->x, tp->y) == false)
            {
                return;
            }

            if (this->release_x != tp->deltaX)
            {
                int16_t delta_deltaX = tp->deltaX - this->release_x;
                int16_t delta_pos = 2 * delta_deltaX;
                float scale_resolution = (1 - this->side_scale) / (this->pos_c - this->pos_l);

                this->release_x = tp->deltaX;

                // gui_log("\ndeltax %d  delta_deltaX %d \n", tp->deltaX, delta_deltaX);
                for (uint8_t i = 0; i < 3; i++)
                {
                    gui_img_t *img = this->img[i];
                    int16_t img_x = img->base.x;
                    // int16_t img_y = img->base.y;
                    int16_t img_w = 0, img_h = 0;
//                    float scale_cur = img->scale_x;
                    float scale_nxt = 0;
                    int16_t img_pos_cur = 0, img_pos_nxt = 0;

                    // gui_log("x %d, scale_cur %f\n", img_x, scale_cur);

                    // scale
                    img_w = img->base.w * gui_img_get_transform_scale_x(img);
                    img_pos_cur = img_x + (img_w >> 1);
                    img_pos_nxt = img_pos_cur + delta_pos;
                    if (img_pos_nxt <= this->pos_l) { img_pos_nxt = this->pos_l; }
                    if (img_pos_nxt >= this->pos_r) { img_pos_nxt = this->pos_r; }
                    scale_nxt = 1 - abs(img_pos_nxt - this->pos_c) * scale_resolution;
                    gui_img_scale(img, scale_nxt,  scale_nxt);

                    // location
                    img_w = img->base.w * gui_img_get_transform_scale_x(img);
                    img_h = img->base.h * gui_img_get_transform_scale_y(img);
                    gui_img_set_location(img, img_pos_nxt - (img_w >> 1), (h - img_h) >> 1);
                }

                // reset left/right
                {
                    int16_t pos = 0;
                    pos = gui_gallery_get_position(this->img[GALLERY_IMG_IDX_CENTER]);

                    if (pos < this->pos_c)
                    {
                        gui_gallery_reset_position(this, this->img[GALLERY_IMG_IDX_LEFT], this->pos_l);
                        gui_obj_show(img_btm_l, false);
                    }
                    else
                    {
                        gui_gallery_reset_position(this, this->img[GALLERY_IMG_IDX_RIGHT], this->pos_r);
                        gui_obj_show(img_btm_r, false);
                    }
                }

                // update bottom img
                {
                    int16_t pos = 0;
//                    int16_t img_w = 0, img_h = 0;

                    // bottom pop up
                    pos = gui_gallery_get_position(this->img[GALLERY_IMG_IDX_LEFT]);
                    if (pos > this->pos_l)
                    {
                        // left
                        int32_t idx = this->idx_center - 2;
                        gui_gallery_reset_position(this, img_btm_l, this->pos_l);
                        if (gui_gallery_img_exist(this, idx))
                        {
                            gui_obj_show(img_btm_l, true);
                        }
                    }
                    pos = gui_gallery_get_position(this->img[GALLERY_IMG_IDX_RIGHT]);
                    if (pos < this->pos_r)
                    {
                        int32_t idx = this->idx_center + 2;

                        gui_gallery_reset_position(this, img_btm_r, this->pos_r);
                        if (gui_gallery_img_exist(this, idx))
                        {
                            gui_obj_show(img_btm_r, true);
                        }
                    }

                    // left/right covered by center
                    pos = gui_gallery_get_position(this->img[GALLERY_IMG_IDX_CENTER]);
                    if (pos <= this->pos_l + 5)
                    {
                        // cover left
                        gui_gallery_img_roll(this, GALLERY_LEFT);
                    }
                    else if (pos >= this->pos_r - 5)
                    {
                        gui_gallery_img_roll(this, GALLERY_RIGHT);
                    }
                }
            }
        }
        break;
    case TOUCH_LEFT_SLIDE:
        {
            if (gui_obj_point_in_obj_rect(obj, tp->x, tp->y) == false)
            {
                return;
            }

            gui_log("TOUCH_LEFT_SLIDE......%d\n", this->idx_center);
            if (this->idx_hold == this->idx_center)
            {
                gui_gallery_img_roll(this, GALLERY_LEFT);
            }
            gui_gallery_reset_position(this, this->img[GALLERY_IMG_IDX_LEFT], this->pos_l);
            gui_gallery_reset_position(this, this->img[GALLERY_IMG_IDX_CENTER], this->pos_c);
            gui_gallery_reset_position(this, this->img[GALLERY_IMG_IDX_RIGHT], this->pos_r);
            gui_obj_show(img_btm_l, false);
            gui_obj_show(img_btm_r, false);
        }
        break;
    case TOUCH_RIGHT_SLIDE:
        {
            if (gui_obj_point_in_obj_rect(obj, tp->x, tp->y) == false)
            {
                return;
            }

            gui_log("TOUCH_RIGHT_SLIDE......%d\n", this->idx_center);
            if (this->idx_hold == this->idx_center)
            {
                gui_gallery_img_roll(this, GALLERY_RIGHT);
            }
            gui_gallery_reset_position(this, this->img[GALLERY_IMG_IDX_LEFT], this->pos_l);
            gui_gallery_reset_position(this, this->img[GALLERY_IMG_IDX_CENTER], this->pos_c);
            gui_gallery_reset_position(this, this->img[GALLERY_IMG_IDX_RIGHT], this->pos_r);
            gui_obj_show(img_btm_l, false);
            gui_obj_show(img_btm_r, false);
        }
        break;
    case TOUCH_INVALIDE:
        {
//            int16_t pos = 0;
            this->release_x = 0;

            gui_gallery_reset_position(this, this->img[GALLERY_IMG_IDX_LEFT], this->pos_l);
            gui_gallery_reset_position(this, this->img[GALLERY_IMG_IDX_CENTER], this->pos_c);
            gui_gallery_reset_position(this, this->img[GALLERY_IMG_IDX_RIGHT], this->pos_r);
            gui_obj_show(img_btm_l, false);
            gui_obj_show(img_btm_r, false);
            // gui_log("TOUCH_INVALIDE......%d\n", this->idx_center);
            this->idx_hold = this->idx_center;
        }
        break;
    default:
        break;
    }

    last = this->checksum;
    this->checksum = this->release_x;

    if (last != this->checksum)
    {
        gui_fb_change();
    }
}

static void gui_gallery_draw_cb(gui_obj_t *obj)
{
//    gui_dispdev_t *dc = gui_get_dc();
//    gui_gallery_t *this = (gui_gallery_t *)obj;
//    draw_img_t *draw_img;

}

static void gui_gallery_end(gui_obj_t *obj)
{
    GUI_ASSERT(obj != NULL);
//    gui_gallery_t *gallery = (gui_gallery_t *)obj;

}

static void gui_gallery_destroy(gui_obj_t *obj)
{

}

static void gui_gallery_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_PREPARE:
            gui_gallery_prepare(obj);
            break;

        case OBJ_DRAW:
            gui_gallery_draw_cb(obj);
            break;

        case OBJ_END:
            gui_gallery_end(obj);
            break;

        case OBJ_DESTROY:
            gui_gallery_destroy(obj);
            break;

        default:
            break;
        }
    }
}

static gui_img_t *gui_gallery_create_img(gui_gallery_t  *this, char *name, int16_t idx, int16_t pos)
{
    void *img_data = NULL;
    gui_img_t *img = NULL;
    int16_t img_w = 0, img_h = 0;
    float scale = (pos == this->pos_c) ? 1.f : this->side_scale;

    img_data = gui_gallery_get_img_data(this, idx);
    img_data = (!img_data) ? this->img_array[0] : img_data;  // default idx 0 img
    img = gui_img_create_from_mem(this, name,
                                  img_data,
                                  this->base.x, this->base.y, this->base.w, this->base.h);
    gui_img_set_mode(img, IMG_BYPASS_MODE);
    gui_img_scale(img, scale, scale);
    img_w = img->base.w * gui_img_get_transform_scale_x(img);
    img_h = img->base.h * gui_img_get_transform_scale_y(img);
    gui_img_set_location(img, pos - (img_w >> 1), (this->base.h - img_h) >> 1);
    return img;
}

static void gui_gallery_ctor(gui_gallery_t         *this,
                             gui_obj_t                 *parent,
                             const char                *name,
                             gui_gallery_config_t *config,
                             int16_t                    x,
                             int16_t                    y,
                             int16_t                    w,
                             int16_t                    h)
{
    gui_obj_t *root = (gui_obj_t *)this;
//    void **array = config->img_array;
//    void *img_data = NULL;
//    char *path;
    gui_img_t *img = NULL;
    int16_t indx = 0;

//    int16_t x_c_bg, y_c_bg, w_c_bg, h_c_bg;
    int16_t x_l;//, y_l, w_l, h_l;
    int16_t x_c;//, y_c, w_c, h_c;
    int16_t x_r;//, y_r, w_r, h_r;

    int16_t img_w = 0, img_h = 0;

    gui_obj_ctor(root, parent, name, x, y, w, h);

    root->type = GALLERY;
    root->obj_cb = gui_gallery_cb;
    root->has_prepare_cb = true;
    root->has_draw_cb = true;
    root->has_end_cb = true;
    root->has_destroy_cb = true;

    this->img_array = config->img_array;
    this->num_pic = config->num_pic;
    this->loop_en = true;
    this->idx_center = 0;
    this->center_percent = config->center_percent;
    this->side_scale = config->side_scale;
    this->side_pos_percent = config->side_pos_percent;
    this->pos_l = x + w * this->side_pos_percent;
    this->pos_r =  x + w * (1 - this->side_pos_percent);
    this->pos_c = w >> 1;

    x_l = this->pos_l;
    x_r = this->pos_r;
    x_c = this->pos_c;



    gui_list_init(&(GET_BASE(this)->child_list));
    if ((GET_BASE(this)->parent) != NULL)
    {
        gui_list_insert_before(&((GET_BASE(this)->parent)->child_list),
                               &(GET_BASE(this)->brother_list));
    }

    // bg main
    this->img_main_bg = gui_img_create_from_mem(this, "GLRY_M_BG",
                                                config->data_main_bg,
                                                x, y, w, h);
    gui_img_set_mode(this->img_main_bg, IMG_BYPASS_MODE);


    // bg center
    this->img_center_bg = gui_img_create_from_mem(this, "GLRY_C_BG",
                                                  config->data_center_bg,
                                                  x, y, w, h);
    gui_img_set_mode(this->img_center_bg, IMG_SRC_OVER_MODE);
    gui_img_set_opacity(this->img_center_bg, 150);
    img_w = this->img_center_bg->base.w;
    img_h = this->img_center_bg->base.h;
    // gui_img_set_location(this->img_center_bg, (w - img_w) >> 1, y_c_bg);
    gui_img_set_location(this->img_center_bg, (w - img_w) >> 1, (h - img_h) >> 1);


    // img bottom left
    indx = 0 - 2;
    img = gui_gallery_create_img(this, "GLRY_PIC_BTML", indx, x_l);
    gui_obj_show(img, false);
    this->img_bottom[GALLERY_IMG_IDX_BTM_LEFT] = img;


    // img bottom right
    indx = 0 + 2;
    img = gui_gallery_create_img(this, "GLRY_PIC_BTMR", indx, x_r);
    gui_obj_show(img, false);
    this->img_bottom[GALLERY_IMG_IDX_BTM_RIGHT] = img;


    // img left
    indx = 0 - 1;
    img = gui_gallery_create_img(this, "GLRY_PIC_L", indx, x_l);
    gui_obj_show(img, gui_gallery_img_exist(this, indx));
    this->img[GALLERY_IMG_IDX_LEFT] = img;


    // img right
    indx = 0 + 1;
    img = gui_gallery_create_img(this, "GLRY_PIC_R", indx, x_r);
    gui_obj_show(img, gui_gallery_img_exist(this, indx));
    this->img[GALLERY_IMG_IDX_RIGHT] = img;

    // img center
    indx = 0;
    img = gui_gallery_create_img(this, "GLRY_PIC_C", indx, x_c);
    this->img[GALLERY_IMG_IDX_CENTER] = img;
}
/*============================================================================*
 *                           Public Functions
 *============================================================================*/

gui_gallery_t *gui_gallery_create(void                      *parent,
                                  const char                *name,
                                  gui_gallery_config_t *cofig,
                                  int16_t                    x,
                                  int16_t                    y,
                                  int16_t                    w,
                                  int16_t                    h)
{
    gui_gallery_t *this;

    GUI_ASSERT(parent != NULL);
    if (name == NULL)
    {
        name = "DEFAULT_GLRY";
    }

    this = gui_malloc(sizeof(gui_gallery_t));
    GUI_ASSERT(this != NULL);
    memset(this, 0x00, sizeof(gui_gallery_t));

    gui_gallery_ctor(this, (gui_obj_t *)parent, name, cofig, x, y, w, h);

    GET_BASE(this)->create_done = true;
    return this;
}

