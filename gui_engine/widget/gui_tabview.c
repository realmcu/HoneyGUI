/*
 * File      : gui_tabview.c
 * This file is part of GUI Engine
 */

#include <guidef.h>
#include <gui_tabview.h>
#include <string.h>
#include <gui_server.h>
#include <gui_obj.h>
#include <gui_magic_img.h>
#include <tp_algo.h>
#ifdef RTK_MODULE_VG_LITE
#include "gui_vg_lite_clock.h"
#endif

void tabview_prepare(gui_obj_t *obj)
{
    gui_dispdev_t *dc = gui_get_dc();
    touch_info_t *tp = tp_get_info();
    //GUI_ASSERT(tp != 0);
    gui_tabview_t *tabview = (gui_tabview_t *)obj;
    if (tabview->jump.jump_flag)
    {
        tabview->cur_id.y = tabview->jump.jump_id.y;
        tabview->cur_id.x = tabview->jump.jump_id.x;
        tabview->jump.jump_flag = false;
        return;
    }
    bool cover;
    cover = false;
    bool cover1 = false;
    gui_tree_get_cover(obj, CURTAINVIEW, &cover1);
    //gui_log("cover1 = %d\n", cover1);
    bool cover2 = false;
    gui_tree_get_cover(obj, ICONLIST, &cover2);
    //gui_log("cover2 = %d\n", cover2);
    cover = cover1 || cover2;
    gui_tree_get_cover(obj, SEEKBAR, &cover1);
    cover = cover1 || cover;
    //gui_log("cover = %d\n", cover);
    //gui_log("outer cover=%d,cur_id.x=%d,cur_id.y=%d\n", cover, ext->cur_id.x, ext->cur_id.y);
    if (cover && tabview->cur_id.x == 0)
    {
        return;
    }
    if (tabview->cur_id.x != 0)
    {
        if (tp->type == TOUCH_HOLD_Y || tp->type == TOUCH_DOWN_SLIDE || tp->type == TOUCH_UP_SLIDE)
        {
            return;
        }
    }
    if (tabview->cur_id.y != 0)
    {
        if (tp->type == TOUCH_HOLD_X || tp->type == TOUCH_LEFT_SLIDE || tp->type == TOUCH_RIGHT_SLIDE)
        {
            return;
        }
    }
    //gui_log("tp->type = %d\n", tp->type);
    switch (tp->type)
    {
    case TOUCH_HOLD_X:
        if (tabview->tab_cnt_left == 0 && tabview->tab_cnt_right == 0)
        {
            break;
        }
        obj->dx += tp->deltaX;
        if (tabview->cur_id.x == 0 && tabview->tab_cnt_right == 0)
        {
            if (obj->dx < 0)
            {
                obj->dx = 0;
                break;
            }
        }
        if (tabview->cur_id.x == 0 && tabview->tab_cnt_left == 0)
        {
            if (obj->dx > 0)
            {
                obj->dx = 0;
                break;
            }
        }
        break;
    case TOUCH_HOLD_Y:
        if (tabview->tab_cnt_down == 0 && tabview->tab_cnt_up == 0)
        {
            break;
        }
        obj->dy += tp->deltaY;
        if (tabview->cur_id.y == 0 && tabview->tab_cnt_down == 0)
        {
            if (obj->dy < 0)
            {
                obj->dy = 0;
                break;
            }
        }
        if (tabview->cur_id.y == 0 && tabview->tab_cnt_up == 0)
        {
            if (obj->dy > 0)
            {
                obj->dy = 0;
                break;
            }
        }
        break;
    case TOUCH_LEFT_SLIDE:
        gui_log("TOUCH_LEFT_SLIDE\n");
        if (tabview->tab_cnt_right == 0 && tabview->cur_id.x == 0)
        {
            break;
        }
        //when current tab is the end,come back to current tab if sliding.
        if (tabview->cur_id.x == tabview->tab_cnt_right)
        {
            return;
        }
        tabview->cur_id.x = tabview->cur_id.x + 1;
        break;
    case TOUCH_RIGHT_SLIDE:
        gui_log("TOUCH_RIGHT_SLIDE0\n");
        if (tabview->tab_cnt_left == 0 && tabview->cur_id.x == 0)
        {
            break;
        }
        //when current tab is the end,come back to current tab if sliding.
        if (tabview->cur_id.x == tabview->tab_cnt_left)
        {
            return;
        }
        tabview->cur_id.x = tabview->cur_id.x - 1;
        break;
    case TOUCH_DOWN_SLIDE:
        gui_log("TOUCH_DOWN_SLIDE\n");
        if (tabview->tab_cnt_down == 0 && tabview->cur_id.y == 0)
        {
            break;
        }
        //when current tab is the end,come back to current tab if sliding.
        if (tabview->cur_id.y == tabview->tab_cnt_down)
        {
            return;
        }
        tabview->cur_id.y = tabview->cur_id.y + 1;
        break;
    case TOUCH_UP_SLIDE:
        gui_log("TOUCH_UP_SLIDE\n");
        if (tabview->tab_cnt_up == 0 && tabview->cur_id.y == 0)
        {
            break;
        }
        //when current tab is the end,come back to current tab if sliding.
        if (tabview->cur_id.y == tabview->tab_cnt_up)
        {
            return;
        }
        tabview->cur_id.y = tabview->cur_id.y - 1;
        break;
    default:
        break;
    }
}

// void tabview_update(gui_obj_t *obj)
// {

// }
void gui_tabview_ctor(gui_tabview_t *this, gui_obj_t *parent, const char *filename, int16_t x,
                      int16_t y,
                      int16_t w, int16_t h)
{
    gui_obj_ctor(&this->base, parent, filename, x, y, w, h);
    ((gui_obj_t *)this)->obj_prepare = tabview_prepare;
    // ((gui_obj_t *)this)->obj_update_att = tabview_update;
    GET_BASE(this)->type = TABVIEW;
}

gui_tabview_t *gui_tabview_create(void *parent, const char *filename, int16_t x, int16_t y,
                                  int16_t w, int16_t h)
{
//#define _GUI_NEW_gui_tabview_create_param this, parent, filename, x, y, w, h
    //GUI_NEW(gui_tabview_t, gui_tabview_ctor, _GUI_NEW_gui_tabview_create_param)
    gui_tabview_t *this = gui_malloc(sizeof(gui_tabview_t));
    memset(this, 0, sizeof(gui_tabview_t));
    gui_tabview_ctor(this, parent, filename, x, y, w, h);
    gui_list_init(&(GET_BASE(this)->child_list));
    if ((GET_BASE(this)->parent) != NULL)
    {
        gui_list_insert_before(&((GET_BASE(this)->parent)->child_list), &(GET_BASE(this)->brother_list));
    }
    GET_BASE(this)->create_done = true;
    return this;
}

void gui_tabview_set_style(gui_tabview_t *this, enum gui_tabview_style style)
{
    this->style = style;
}

void gui_tab_jump(gui_tabview_t *parent_tabview, int8_t idx, int8_t idy)
{
    parent_tabview->jump.jump_flag = true;
    parent_tabview->jump.jump_id.x = idx;
    parent_tabview->jump.jump_id.y = idy;
}

#include "gui_tab.h"
static void tab_update_att(gui_obj_t *obj)
{
    gui_dispdev_t *dc = gui_get_dc();
    touch_info_t *tp = tp_get_info();
    gui_tab_t *tab = (gui_tab_t *)obj;
    gui_tabview_t *parent = (gui_tabview_t *)(obj->parent);
    obj->x = (tab->id.x - parent->cur_id.x) * (int)gui_get_screen_width();
    obj->y = (tab->id.y - parent->cur_id.y) * (int)gui_get_screen_height();
    // gui_log("tab_update_att x : %d , y : %d\n",obj->x,obj->y);
}
static float get_scale_offset_x(gui_obj_t *img, float scale_x)
{
    /**
     * @note (1-scale)(center-x)
     *
     */
    return (1.0f - scale_x) * (((float)(gui_get_screen_width() / 2)) - (float)(img->x));
}
static float get_scale_offset_y(gui_obj_t *img, float scale_y)
{
    return (1.0f - scale_y) * (((float)(gui_get_screen_height() / 2)) - (float)(img->y));
}
#include "gui_canvas.h"
#include "gui_curtain.h"
static void deal_img_in_root(gui_obj_t *object, float xx, float yy)
{
    gui_list_t *node = NULL;
    gui_list_t *tmp = NULL;
    float x = xx * 0.6f + 0.4f;
    float y = yy * 0.6f + 0.4f;
    gui_list_for_each_safe(node, tmp, &object->child_list)
    {
        gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);
        if ((!(obj->parent->type == CURTAIN &&
               ((gui_curtain_t *)obj->parent)->orientation != CURTAIN_MIDDLE)) &&
            (!(obj->parent->parent->type == CURTAIN &&
               ((gui_curtain_t *)obj->parent->parent)->orientation != CURTAIN_MIDDLE)))
        {
            switch (obj->type)
            {
            case IMAGE_FROM_MEM:
                {
                    gui_magic_img_t *img = (void *)obj;
                    if (((gui_tab_t *)object)->style == REDUCTION_FADE ||
                        ((gui_tab_t *)object)->style == REDUCTION)
                    {
                        gui_img_scale_add(img, x, y);
                        gui_img_translate(img, get_scale_offset_x((void *)img, x) / x, get_scale_offset_y((void *)img,
                                          x) / y);
                    }
                    if (((gui_tab_t *)object)->style == REDUCTION_FADE ||
                        ((gui_tab_t *)object)->style == FADE)
                    {
                        img->base.draw_img.opacity_value = x > 0.4f ? (x - 0.4f) / 0.6f * UINT8_MAX : 0;
                    }
                }
                break;
            case CANVAS:
                {
                    gui_canvas_t *img = (void *)obj;
                    if (((gui_tab_t *)object)->style == REDUCTION_FADE ||
                        ((gui_tab_t *)object)->style == REDUCTION)
                    {
                        gui_canvas_api.scale(img, x, y);
                        gui_canvas_api.translate(img, get_scale_offset_x((void *)img, x),
                                                 get_scale_offset_y((void *)img, x));
                    }
                    if (((gui_tab_t *)object)->style == REDUCTION_FADE ||
                        ((gui_tab_t *)object)->style == FADE)
                    {
                        img->opacity_value = x > 0.4f ? (x - 0.4f) / 0.6f * UINT8_MAX : 0;
                    }
                }
                break;
#ifdef RTK_MODULE_VG_LITE
            case VG_LITE_CLOCK:
                {
                    gui_vg_lite_clock_t *vg_lite_clock = (gui_vg_lite_clock_t *)obj;
                    gui_vg_lite_scale(vg_lite_clock, x, y);
                    //gui_vg_lite_translate(vg_lite_clock, get_scale_offset_x((void *)vg_lite_clock, x) / x, get_scale_offset_y((void *)vg_lite_clock, x) / y);
                }
                break;
#endif

            default:
                break;
            }
        }
        deal_img_in_root(obj, x, y);
    }
}
static void deal_img_in_root_scale(gui_obj_t *object, float xx, float yy)
{
    gui_list_t *node = NULL;
    gui_list_t *tmp = NULL;
    float x = xx * 0.6f + 0.4f;
    float y = yy * 0.6f + 0.4f;
    gui_list_for_each_safe(node, tmp, &object->child_list)
    {
        gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);
        if ((!(obj->parent->type == CURTAIN &&
               ((gui_curtain_t *)obj->parent)->orientation != CURTAIN_MIDDLE)) &&
            (!(obj->parent->parent->type == CURTAIN &&
               ((gui_curtain_t *)obj->parent->parent)->orientation != CURTAIN_MIDDLE)))
        {
            switch (obj->type)
            {
            case IMAGE_FROM_MEM:
                {
                    gui_magic_img_t *img = (void *)obj;
                    gui_img_scale(img, x, y);
                    gui_img_translate(img, get_scale_offset_x((void *)img, x) / x, get_scale_offset_y((void *)img,
                                      x) / y);
                    //img->base.draw_img.opacity_value = x * UINT8_MAX;
                }
                break;
            case CANVAS:
                {
                    gui_canvas_t *img = (void *)obj;
                    gui_canvas_api.scale(img, x, y);
                    gui_canvas_api.translate(img, get_scale_offset_x((void *)img, x),
                                             get_scale_offset_y((void *)img,
                                                                x));
                    //img->opacity_value = x * UINT8_MAX;
                }
                break;
#ifdef MODULE_VG_LITE
            case VG_LITE_CLOCK:
                {
                    gui_vg_lite_clock_t *vg_lite_clock = (gui_vg_lite_clock_t *)obj;
                    gui_vg_lite_scale(vg_lite_clock, x, y);
                    //gui_vg_lite_translate(vg_lite_clock, get_scale_offset_x((void *)vg_lite_clock, x) / x, get_scale_offset_y((void *)vg_lite_clock, x) / y);
                }
                break;
#endif

            default:
                break;
            }
        }
        deal_img_in_root(obj, x, y);
    }
}
static void deal_img_in_root_fade(gui_obj_t *object, float xx, float yy)
{
    gui_list_t *node = NULL;
    gui_list_t *tmp = NULL;
    float x = xx * 0.6f + 0.4f;
    float y = yy * 0.6f + 0.4f;
    gui_list_for_each_safe(node, tmp, &object->child_list)
    {
        gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);
        if ((!(obj->parent->type == CURTAIN &&
               ((gui_curtain_t *)obj->parent)->orientation != CURTAIN_MIDDLE)) &&
            (!(obj->parent->parent->type == CURTAIN &&
               ((gui_curtain_t *)obj->parent->parent)->orientation != CURTAIN_MIDDLE)))
        {
            switch (obj->type)
            {
            case IMAGE_FROM_MEM:
                {
                    gui_magic_img_t *img = (void *)obj;

                    img->base.draw_img.opacity_value = x * UINT8_MAX;
                }
                break;
            case CANVAS:
                {
                    gui_canvas_t *img = (void *)obj;

                    img->opacity_value = x * UINT8_MAX;
                }
                break;
#ifdef MODULE_VG_LITE
            case VG_LITE_CLOCK:
                {
                    gui_vg_lite_clock_t *vg_lite_clock = (gui_vg_lite_clock_t *)obj;
                    //gui_vg_lite_scale(vg_lite_clock, x, y);
                    //gui_vg_lite_translate(vg_lite_clock, get_scale_offset_x((void *)vg_lite_clock, x) / x, get_scale_offset_y((void *)vg_lite_clock, x) / y);
                }
                break;
#endif

            default:
                break;
            }
        }
        deal_img_in_root(obj, x, y);
    }
}
static int to1(int input)
{
    if (input < 0)
    {
        return -1;
    }
    else if (input > 0)
    {
        return 1;
    }
    else if (input == 0)
    {
        return 0;
    }
    return 0;
}
static void tab_prepare_classic(gui_obj_t *obj)
{

}
static void tab_prepare_scale_fade(gui_obj_t *obj)
{
    gui_dispdev_t *dc = gui_get_dc();
    touch_info_t *tp = tp_get_info();
    gui_tab_t *tab = (gui_tab_t *)obj;
    gui_tabview_t *parent = (gui_tabview_t *)(obj->parent);
    /**
     * @note tp->deltaX >0 means slide right, the left tab shows
     *       tp->deltaX <0 means slide left, the right tab shows
     *       the left tab's dx -320~0
     *       the right tab's dx 320~0
     */
    if (((tp->type == TOUCH_HOLD_X) || (tp->type == TOUCH_LEFT_SLIDE) ||
         (tp->type == TOUCH_RIGHT_SLIDE) || (tp->type == TOUCH_INVALIDE))  &&
        (tab->id.x - parent->cur_id.x == 0) && tab->id.y == 0)
    {
        //gui_log("tp->type:%d,%f,%d\n",tp->type,((float)(gui_get_screen_width()+(float)obj->dx))/((float)gui_get_screen_width()),tab->id.x - parent->cur_id.x);

        //gui_log("%f,%f\n",((float)(gui_get_screen_width()+tp->deltaX))/((float)gui_get_screen_width()), (gui_get_screen_width()+tp->deltaX)/gui_get_screen_width());

        deal_img_in_root(obj, ((float)(gui_get_screen_width() + ((float)(-to1(tp->deltaX))) *
                                       (float)obj->dx)) / ((float)gui_get_screen_width()),
                         ((float)(gui_get_screen_width() + ((float)(-to1(tp->deltaX))) * (float)obj->dx)) / ((
                                     float)gui_get_screen_width()));
        // deal_img_in_root(obj, 0.5, 0.5);
    }
    else if ((tp->type == TOUCH_HOLD_X) && (tp->deltaX < 0) && (tab->id.x - parent->cur_id.x == 1) &&
             tab->id.y == 0)
    {
        deal_img_in_root(obj, ((float)(gui_get_screen_width() - (float)obj->dx)) / ((
                                                                                        float)gui_get_screen_width()),
                         ((float)(gui_get_screen_width() - (float)obj->dx)) / ((float)gui_get_screen_width()));
    }
    else if ((tp->type == TOUCH_HOLD_X) && (tp->deltaX > 0) && (tab->id.x - parent->cur_id.x == -1) &&
             tab->id.y == 0)
    {
        // gui_log("(float)obj->dx:%f\n",((float)obj->dx)/((float)gui_get_screen_width())+1.0f);
        deal_img_in_root(obj, ((float)obj->dx) / ((float)gui_get_screen_width()) + 1.0f,
                         ((float)obj->dx) / ((float)gui_get_screen_width()) + 1.0f);
    }
}

//static void tab_prepare_reduction(gui_obj_t *obj)
//{

//}
//static void tab_prepare_fade(gui_obj_t *obj)
//{

//}
static void tab_prepare_reduction_fade(gui_obj_t *obj)
{
    tab_prepare_scale_fade(obj);
}
static void tab_prepare_scale(gui_obj_t *obj)
{
    gui_dispdev_t *dc = gui_get_dc();
    touch_info_t *tp = tp_get_info();
    gui_tab_t *tab = (gui_tab_t *)obj;
    gui_tabview_t *parent = (gui_tabview_t *)(obj->parent);
    /**
     * @note tp->deltaX >0 means slide right, the left tab shows
     *       tp->deltaX <0 means slide left, the right tab shows
     *       the left tab's dx -320~0
     *       the right tab's dx 320~0
     */
    if (((tp->type == TOUCH_HOLD_X) || (tp->type == TOUCH_LEFT_SLIDE) ||
         (tp->type == TOUCH_RIGHT_SLIDE) || (tp->type == TOUCH_INVALIDE))  &&
        (tab->id.x - parent->cur_id.x == 0) && tab->id.y == 0)
    {
        ////gui_log("tp->type:%d,%f,%d\n",tp->type,((float)(gui_get_screen_width()+(float)obj->dx))/((float)gui_get_screen_width()),tab->id.x - parent->cur_id.x);

        ////gui_log("%f,%f\n",((float)(gui_get_screen_width()+tp->deltaX))/((float)gui_get_screen_width()), (gui_get_screen_width()+tp->deltaX)/gui_get_screen_width());

        deal_img_in_root_scale(obj, ((float)(gui_get_screen_width() + ((float)(-to1(tp->deltaX))) *
                                             (float)obj->dx)) / ((float)gui_get_screen_width()),
                               ((float)(gui_get_screen_width() + ((float)(-to1(tp->deltaX))) * (float)obj->dx)) / ((
                                           float)gui_get_screen_width()));
        //deal_img_in_root(obj, 0.5, 0.5);
    }
    else if ((tp->type == TOUCH_HOLD_X) && (tp->deltaX < 0) && (tab->id.x - parent->cur_id.x == 1) &&
             tab->id.y == 0)
    {
        deal_img_in_root_scale(obj, ((float)(gui_get_screen_width() - (float)obj->dx)) / ((
                                   float)gui_get_screen_width()),
                               ((float)(gui_get_screen_width() - (float)obj->dx)) / ((float)gui_get_screen_width()));
    }
    else if ((tp->type == TOUCH_HOLD_X) && (tp->deltaX > 0) && (tab->id.x - parent->cur_id.x == -1) &&
             tab->id.y == 0)
    {
        ////gui_log("(float)obj->dx:%f\n",((float)obj->dx)/((float)gui_get_screen_width())+1.0f);
        deal_img_in_root_scale(obj, ((float)obj->dx) / ((float)gui_get_screen_width()) + 1.0f,
                               ((float)obj->dx) / ((float)gui_get_screen_width()) + 1.0f);
    }
}
static void tab_prepare_fade(gui_obj_t *obj)
{
    gui_dispdev_t *dc = gui_get_dc();
    touch_info_t *tp = tp_get_info();
    gui_tab_t *tab = (gui_tab_t *)obj;
    gui_tabview_t *parent = (gui_tabview_t *)(obj->parent);
    /**
     * @note tp->deltaX >0 means slide right, the left tab shows
     *       tp->deltaX <0 means slide left, the right tab shows
     *       the left tab's dx -320~0
     *       the right tab's dx 320~0
     */
    if (((tp->type == TOUCH_HOLD_X) || (tp->type == TOUCH_LEFT_SLIDE) ||
         (tp->type == TOUCH_RIGHT_SLIDE) || (tp->type == TOUCH_INVALIDE))  &&
        (tab->id.x - parent->cur_id.x == 0) && tab->id.y == 0)
    {
        ////gui_log("tp->type:%d,%f,%d\n",tp->type,((float)(gui_get_screen_width()+(float)obj->dx))/((float)gui_get_screen_width()),tab->id.x - parent->cur_id.x);

        ////gui_log("%f,%f\n",((float)(gui_get_screen_width()+tp->deltaX))/((float)gui_get_screen_width()), (gui_get_screen_width()+tp->deltaX)/gui_get_screen_width());

        deal_img_in_root_fade(obj, ((float)(gui_get_screen_width() + ((float)(-to1(tp->deltaX))) *
                                            (float)obj->dx)) / ((float)gui_get_screen_width()),
                              ((float)(gui_get_screen_width() + ((float)(-to1(tp->deltaX))) * (float)obj->dx)) / ((
                                          float)gui_get_screen_width()));
        //deal_img_in_root(obj, 0.5, 0.5);
    }
    else if ((tp->type == TOUCH_HOLD_X) && (tp->deltaX < 0) && (tab->id.x - parent->cur_id.x == 1) &&
             tab->id.y == 0)
    {
        deal_img_in_root_fade(obj, ((float)(gui_get_screen_width() - (float)obj->dx)) / ((
                                  float)gui_get_screen_width()),
                              ((float)(gui_get_screen_width() - (float)obj->dx)) / ((float)gui_get_screen_width()));
    }
    else if ((tp->type == TOUCH_HOLD_X) && (tp->deltaX > 0) && (tab->id.x - parent->cur_id.x == -1) &&
             tab->id.y == 0)
    {
        ////gui_log("(float)obj->dx:%f\n",((float)obj->dx)/((float)gui_get_screen_width())+1.0f);
        deal_img_in_root_fade(obj, ((float)obj->dx) / ((float)gui_get_screen_width()) + 1.0f,
                              ((float)obj->dx) / ((float)gui_get_screen_width()) + 1.0f);
    }
}
static void tab_prepare(gui_obj_t *this)
{
    if (this->parent->type == TABVIEW && ((gui_tab_t *)this)->id.x != 4)
    {
        switch (((gui_tabview_t *)(this->parent))->style)
        {

        case SLIDE_FADE:
            tab_prepare_fade(this);;
            break;
        case SLIDE_SCALE:
            tab_prepare_scale(this);
            break;
        case SLIDE_SCALE_FADE:
            tab_prepare_scale_fade(this);;
            break;
        case SLIDE_WHEEL:
        case SLIDE_CLASSIC:
        default:
            tab_prepare_classic(this);
            break;
        }
    }
    switch (((gui_tab_t *)this)->style)
    {
    case CLASSIC:
        break;
    case REDUCTION:
        // tab_prepare_reduction(this);
        tab_prepare_scale(this);
        break;
    case FADE:
        // tab_prepare_fade(this);
        tab_prepare_fade(this);;
        break;
    case REDUCTION_FADE:
        tab_prepare_reduction_fade(this);
        break;
    default:
        break;
    }
}
void gui_tab_ctor(gui_tab_t *this, gui_obj_t *parent, const char *filename, int16_t x, int16_t y,
                  int16_t w, int16_t h, int16_t idx, int16_t idy)
{
    gui_obj_ctor(&this->base, parent, filename, x, y, w, h);
    ((gui_obj_t *)this)->obj_update_att = tab_update_att;
    GET_BASE(this)->obj_prepare = tab_prepare;
    GET_BASE(this)->type = TAB;
    if (parent != NULL)
    {
        gui_tabview_t *parent_ext = (gui_tabview_t *)parent;
        this->id.x = idx;
        this->id.y = idy;
        parent_ext->tab_cnt++;
        if (idx > 0)
        {
            parent_ext->tab_cnt_right++;
        }
        else if (idx < 0)
        {
            parent_ext->tab_cnt_left--;
        }
        if (idy > 0)
        {
            parent_ext->tab_cnt_down++;
        }
        else if (idy < 0)
        {
            parent_ext->tab_cnt_up--;
        }
        switch (parent_ext->style)
        {

        case SLIDE_FADE:
            this->style = FADE;
            break;
        case SLIDE_SCALE:
            this->style = REDUCTION;
            break;
        case SLIDE_SCALE_FADE:
            this->style = REDUCTION_FADE;
            break;
        case SLIDE_WHEEL:
        case SLIDE_CLASSIC:
        default:
            break;
        }

    }

}

gui_tab_t *gui_tab_create(void *parent, const char *filename, int16_t x, int16_t y,
                          int16_t w, int16_t h, int16_t idx, int16_t idy)
{
    if (w == 0)
    {
        w = (int)gui_get_screen_width();
    }
    if (h == 0)
    {
        h = (int)gui_get_screen_height();
    }
#define _GUI_NEW_gui_tab_create_param this, parent, filename, x, y, w, h, idx, idy
    GUI_NEW(gui_tab_t, gui_tab_ctor, _GUI_NEW_gui_tab_create_param)
}
#include <math.h>
#include "gui_img.h"
static void tabview_theme1_update(gui_obj_t *obj)
{
    gui_dispdev_t *dc = gui_get_dc();
    touch_info_t *tp = tp_get_info();

    size_t count = 0;
    int degree = tp->deltaX + tp->deltaY;
    float rad = (degree) * M_PI / 180.0f;
    gui_list_t *node = NULL;
    gui_tabview_theme1_t *tb = (void *)obj;
    for (node = (&(obj->child_list))->next; node != (&(obj->child_list)); node = node->next)
    {
        gui_obj_t *child_obj = gui_list_entry(node, gui_obj_t, brother_list);
        gui_magic_img_t *img = (void *)child_obj;
        switch (count)
        {
        case 0:
            child_obj->x = tb->ccw - tb->ccw * cosf(rad) - 128 / 2;
            child_obj->y = 0;
            gui_img_scale_add(img, sinf(rad) * 0.5f + 0.5f, sinf(rad) * 0.5f + 0.5f);
            //img->set_alpha(img, (sinf(rad)*0.5f+0.5f)*0xff);
            child_obj->x -= (128 / 2) * (sinf(rad) * 0.5f + 0.5f - 1.0f);
            child_obj->y -= (128 / 2) * (sinf(rad) * 0.5f + 0.5f - 1.0f);
            break;
        case 2:
            child_obj->x = tb->ccw + tb->ccw * sinf(rad) - 128 / 2;
            child_obj->y = 0;
            gui_img_scale_add(img, sinf(rad + M_PI / 2.0f) * 0.5f + 0.5f,
                              sinf(rad + M_PI / 2.0f) * 0.5f + 0.5f);
            //img->set_alpha(img, (sinf(rad+M_PI/2.0f)*0.5f+0.5f)*0xff);
            child_obj->x -= (128 / 2) * (sinf(rad + M_PI / 2.0f) * 0.5f + 0.5f - 1.0f);
            child_obj->y -= (128 / 2) * (sinf(rad + M_PI / 2.0f) * 0.5f + 0.5f - 1.0f);
            break;
        case 1:
            child_obj->x = tb->ccw + tb->ccw * cosf(rad) - 128 / 2;
            child_obj->y = 0;
            gui_img_scale_add(img, sinf(rad + M_PI) * 0.5f + 0.5f, sinf(rad + M_PI) * 0.5f + 0.5f);
            //img->set_alpha(img, (sinf(rad+M_PI)*0.5f+0.5f)*0xff);
            child_obj->x -= (128 / 2) * (sinf(rad + M_PI) * 0.5f + 0.5f - 1.0f);
            child_obj->y -= (128 / 2) * (sinf(rad + M_PI) * 0.5f + 0.5f - 1.0f);
            break;

        default:
            break;
        }

        count++;
    }
}
#include "gui_tabview_theme1.h"
void (tabview_theme1_ctor)(struct gui_tabview_theme1 *this, gui_obj_t *parent, int16_t x,
                           int16_t y,
                           int16_t r)
{
    gui_obj_ctor(&this->base, parent, "_tabview_theme1", x, y, 0, 0);
    this->ccw = r;
    GET_BASE(this)->obj_update_att = tabview_theme1_update;
}
gui_tabview_theme1_t *gui_tabview_theme1_create(void *parent, int16_t x, int16_t y, int16_t r)
{
#define _param_tabview_theme1_ctor this, parent, x,y,r
    GUI_NEW(gui_tabview_theme1_t, tabview_theme1_ctor, _param_tabview_theme1_ctor)
}


