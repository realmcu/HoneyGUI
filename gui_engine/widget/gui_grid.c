
#include <gui_grid.h>
#include "gui_list.h"
#include "tp_algo.h"
#include "gui_img.h"
static void (obj_update_att)(struct _gui_obj_t *obj)
{
    GUI_RENDER_DATA
    uint32_t member_count = 0;
    struct gui_grid *this = (void *)obj;

    gui_list_t *node = NULL;
    gui_list_t *tmp = NULL;
    gui_list_for_each_safe(node, tmp, &obj->child_list)
    {
        gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);
        obj->x = member_count / this->col * this->gap_col_scale;
        obj->y = member_count % this->col * this->gap_row_scale;
        member_count++;
    }
    if (!this->row_count_flag)
    {
        this->row_count = member_count;
        this->row_count_flag = true;
    }
    if (!this->col_count_flag)
    {
        this->col_count = member_count;
        this->col_count_flag = true;
    }

    if (this->style == GRID_3D)
    {
        static int touch_y  = 0;
        {
            {

                {

                    if ((obj->dx < (int)gui_get_screen_width()) && ((obj->dx + obj->w) >= 0) && \
                        (obj->dy < (int)gui_get_screen_height()) && ((obj->dy + obj->h) >= 0))
                    {
                        if ((tp->x >= ((gui_grid_t *)obj)->start_x) && (tp->x < ((gui_grid_t *)obj)->start_x + obj->w))
                        {
                            //gui_log("tp->type:%d,%d,%d,%d\n",tp->type, tp->y,((gui_grid_t *)obj)->start_y,obj->h);
                            if ((tp->y >= ((gui_grid_t *)obj)->start_y) && (tp->y < ((gui_grid_t *)obj)->start_y + obj->h))
                            {
                                if ((tp->type == TOUCH_HOLD_Y))
                                {
                                    ////gui_log("obj->y:%d,%d",obj->y, ((gui_grid_t *)obj)->start_y-(obj->h-gui_get_screen_height()));
                                    touch_y = tp->deltaY + ((gui_grid_t *)obj)->yold;
                                    if (touch_y > ((gui_grid_t *)obj)->start_y)
                                    {
                                        //touch_y = ((gui_grid_t *)obj)->start_y;

                                    }
                                    else if (touch_y < (((gui_grid_t *)obj)->start_y - (obj->h - (int)gui_get_screen_height())) &&
                                             touch_y != 0)
                                    {
                                        ////gui_log("obj->yyyy:%d,%d",obj->y, ((gui_grid_t *)obj)->start_y-(obj->h-(int)gui_get_screen_height()));
                                        touch_y = ((gui_grid_t *)obj)->start_y - (obj->h - (int)gui_get_screen_height());
                                    }
                                    touch_y -= ((gui_grid_t *)obj)->start_y;
                                }
                                else
                                {
                                    if (touch_y > ((gui_grid_t *)obj)->start_y)
                                    {
                                        //touch_y= ((gui_grid_t *)obj)->start_y;

                                    }
                                    else if (touch_y < (((gui_grid_t *)obj)->start_y - (obj->h - (int)gui_get_screen_height())) &&
                                             touch_y != 0)
                                    {
                                        ////gui_log("obj->yyyy:%d,%d",obj->y, ((gui_grid_t *)obj)->start_y-(obj->h-(int)gui_get_screen_height()));
                                        touch_y = ((gui_grid_t *)obj)->start_y - (obj->h - (int)gui_get_screen_height());
                                    }

                                    ((gui_grid_t *)obj)->yold = obj->y;
                                    //touch_y -=((gui_grid_t *)obj)->start_y;
                                }
                            }
                        }
                    }
                }
            }



            static bool press_flag = 0;
            uint32_t member_count = 0;
            if (tp->pressed)
            {
                press_flag = true;
            }



            if (touch_y < -this->change_threshold && press_flag && this->row_count > 1)
            {
                touch_y = 0;
                this->row_count -= 1;
                press_flag = false;
            }
            if (touch_y > this->change_threshold && press_flag && this->row_count < this->col_count)
            {
                touch_y = 0;
                this->row_count += 1;
                press_flag = false;
            }
            if (touch_y < -this->change_threshold || touch_y > this->change_threshold)
            {
                touch_y = 0;
            }
            if (touch_y > 0 && this->row_count == this->col_count)
            {
                touch_y = 0;
            }



            member_count = this->row_count;
            //gui_log("member_count:%d,%d,%d\n",member_count,this->row_count,tp->pressed);
            gui_list_for_each_safe(node, tmp, &obj->child_list)
            {
                member_count--;
                gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);
                obj->x = member_count / this->col * this->gap_col_scale;


                obj->y = member_count % this->col * this->gap_row_scale + touch_y;
                if ((this->row_count - member_count == (this->row_count + 1)) && touch_y > 0)
                {
                    obj->x = 0;
                    obj->y = (member_count) % this->col * this->gap_row_scale + touch_y - gui_img_get_height(
                                 (void *)obj) * 1.5f;
                }
                //gui_log("xy:%d,%d,%d\n",obj->x, obj->y,this->row_count-member_count);
            }
        }

    }
}
//static float get_scale_offset_x(gui_obj_t *img, float scale_x)
//{
//    /**
//     * @note (1-scale)(center-x)
//     *
//     */
//    return (1.0f - scale_x) * (((float)(gui_get_screen_width() / 2)) - (float)(img->x));
//}

//static float get_scale_offset_y(gui_obj_t *img, float scale_y)
//{
//    return (1.0f - scale_y) * (((float)(gui_get_screen_width() / 2)) - (float)(img->y));
//}

#include "gui_canvas.h"
#include "gui_magic_img.h"
static void deal_img_in_root(gui_obj_t *object, float x, float y)
{
    gui_list_t *node = NULL;
    gui_list_t *tmp = NULL;
    gui_list_for_each_safe(node, tmp, &object->child_list)
    {
        gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);
        switch (obj->type)
        {
        case IMAGE_FROM_MEM:
            {
                //gui_log("scale:%f\n", x);
                gui_magic_img_t *img = (void *)obj;
                gui_img_scale(img, x, y);

            }
            break;
        case CANVAS:
            {
                gui_canvas_t *img = (void *)obj;
                GUI_UNUSED(img);
            }
            break;
        default:
            break;
        }
        deal_img_in_root(obj, x, y);
    }


}
#include "math.h"
static void deal_img_in_root_3d(gui_obj_t *obj, float x, float y)
{
    struct gui_grid *this = (void *)obj;
    GUI_RENDER_DATA
    static int touch_y  = 0;
    {

        {
            //gui_log("tp->type:%d,%d,%d,%d\n",tp->type, tp->y,((gui_grid_t *)obj)->start_y,obj->h);
            if ((obj->dx < (int)gui_get_screen_width()) && ((obj->dx + obj->w) >= 0) && \
                (obj->dy < (int)gui_get_screen_height()) && ((obj->dy + obj->h) >= 0))
            {
                if ((tp->x >= ((gui_grid_t *)obj)->start_x) && (tp->x < ((gui_grid_t *)obj)->start_x + obj->w))
                {
                    //gui_log("tp->type:%d,%d,%d,%d\n",tp->type, tp->y,((gui_grid_t *)obj)->start_y,obj->h);
                    if ((tp->y >= ((gui_grid_t *)obj)->start_y) && (tp->y < ((gui_grid_t *)obj)->start_y + obj->h))
                    {
                        if ((tp->type == TOUCH_HOLD_Y))
                        {
                            //gui_log("obj->y:%d,%d",obj->y, ((gui_grid_t *)obj)->start_y-(obj->h-gui_get_screen_height()));
                            touch_y = tp->deltaY + ((gui_grid_t *)obj)->yold;
                            if (touch_y > ((gui_grid_t *)obj)->start_y)
                            {
                                //touch_y = ((gui_grid_t *)obj)->start_y;

                            }
                            else if (touch_y < (((gui_grid_t *)obj)->start_y - (obj->h - (int)gui_get_screen_height())) &&
                                     touch_y != 0)
                            {
                                ////gui_log("obj->yyyy:%d,%d",obj->y, ((gui_grid_t *)obj)->start_y-(obj->h-(int)gui_get_screen_height()));
                                touch_y = ((gui_grid_t *)obj)->start_y - (obj->h - (int)gui_get_screen_height());
                            }
                            touch_y -= ((gui_grid_t *)obj)->start_y;

                        }
                        else
                        {
                            if (touch_y > ((gui_grid_t *)obj)->start_y)
                            {
                                //touch_y= ((gui_grid_t *)obj)->start_y;

                            }
                            else if (touch_y < (((gui_grid_t *)obj)->start_y - (obj->h - (int)gui_get_screen_height())) &&
                                     touch_y != 0)
                            {
                                ////gui_log("obj->yyyy:%d,%d",obj->y, ((gui_grid_t *)obj)->start_y-(obj->h-(int)gui_get_screen_height()));
                                touch_y = ((gui_grid_t *)obj)->start_y - (obj->h - (int)gui_get_screen_height());
                            }

                            ((gui_grid_t *)obj)->yold = obj->y;
                            //touch_y -=((gui_grid_t *)obj)->start_y;
                        }
                    }
                }
            }
        }

    }
    gui_list_t *node = NULL;
    gui_list_t *tmp = NULL;
    static float id2 = 0;

    float id = 0;
    static bool press_flag = 0;
    uint32_t member_count = 0;
    (void)member_count;
    if (tp->pressed)
    {
        press_flag = true;
    }
    gui_log("touch_y:%d\n", this->col_count);

    //gui_log("touch_y:%d\n", touch_y);
    if (touch_y < -this->change_threshold && press_flag && id2 < this->col_count - 1)
    {
        //gui_log("press_flag:%d\n", press_flag);
        touch_y = 0;
        id2++;
        press_flag = false;
    }
    if (touch_y > this->change_threshold && press_flag && id2 > 0)
    {
        //gui_log("press_flag:%d\n", press_flag);
        touch_y = 0;
        id2--;
        press_flag = false;
    }
    if (touch_y < -this->change_threshold)
    {
        touch_y = 0;
    }
    if (touch_y > this->change_threshold)
    {
        touch_y = 0;
    }
    id = id2;

    if (id2 < 0 || id2 > 5)
    {
        return;
    }
    if (id2 == 0 && touch_y > 0)
    {
        return;
    }

    gui_log("idid:%f,%f\n", id, id2);
    gui_list_for_each_safe(node, tmp, &obj->child_list)
    {
        gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);
        switch (obj->type)
        {
        case IMAGE_FROM_MEM:
            {


                gui_magic_img_t *img = (void *)obj;
                float scale = sinf(id * M_PI / 10.0f - (float)touch_y /
                                   100.0f); //gui_log("scale:%f \n", id*M_PI/10.0f-touch_y/100);
                gui_img_scale(img, scale, scale);
                gui_img_translate((void *)img,
                                  (float)(gui_img_get_width((void *)img)) * (1.0f - scale) * 0.5f / scale, 0);
                id++;

            }
            break;
        case CANVAS:
            {
                gui_canvas_t *img = (void *)obj;
                GUI_UNUSED(img);
            }
            break;
        default:
            break;
        }
    }


}
// static int to1(int input)
// {
//     if (input < 0)
//     {
//         return -1;
//     }
//     else if (input > 0)
//     {
//         return 1;
//     }
//     else if (input == 0)
//     {
//         return 0;
//     }
//     return 0;
// }
//#include "gui_tabview.h"
static void tab_prepare_scale(gui_obj_t *obj)
{
    gui_dispdev_t *dc = gui_get_dc();
    touch_info_t *tp = (touch_info_t *)tp_get_info();
    struct gui_grid *this = (void *)obj;
    switch (this->style)
    {
    case GRID_CLASSIC:
        break;
    case GRID_3D:
        {
            static int touch_y  = 0;
            {

                {

                    if ((obj->dx < (int)gui_get_screen_width()) && ((obj->dx + obj->w) >= 0) && \
                        (obj->dy < (int)gui_get_screen_height()) && ((obj->dy + obj->h) >= 0))
                    {
                        if ((tp->x >= ((gui_grid_t *)obj)->start_x) && (tp->x < ((gui_grid_t *)obj)->start_x + obj->w))
                        {
                            //gui_log("tp->type:%d,%d,%d,%d\n",tp->type, tp->y,((gui_grid_t *)obj)->start_y,obj->h);
                            if ((tp->y >= ((gui_grid_t *)obj)->start_y) && (tp->y < ((gui_grid_t *)obj)->start_y + obj->h))
                            {
                                if ((tp->type == TOUCH_HOLD_Y))
                                {
                                    ////gui_log("obj->y:%d,%d",obj->y, ((gui_grid_t *)obj)->start_y-(obj->h-gui_get_screen_height()));
                                    touch_y = tp->deltaY + ((gui_grid_t *)obj)->yold;
                                    if (touch_y > ((gui_grid_t *)obj)->start_y)
                                    {
                                        touch_y = ((gui_grid_t *)obj)->start_y;

                                    }
                                    else if (touch_y < (((gui_grid_t *)obj)->start_y - (obj->h - (int)gui_get_screen_height())) &&
                                             touch_y != 0)
                                    {
                                        ////gui_log("obj->yyyy:%d,%d",obj->y, ((gui_grid_t *)obj)->start_y-(obj->h-(int)gui_get_screen_height()));
                                        touch_y = ((gui_grid_t *)obj)->start_y - (obj->h - (int)gui_get_screen_height());
                                    }
                                }
                                else
                                {
                                    if (touch_y > ((gui_grid_t *)obj)->start_y)
                                    {
                                        touch_y = ((gui_grid_t *)obj)->start_y;

                                    }
                                    else if (touch_y < (((gui_grid_t *)obj)->start_y - (obj->h - (int)gui_get_screen_height())) &&
                                             touch_y != 0)
                                    {
                                        ////gui_log("obj->yyyy:%d,%d",obj->y, ((gui_grid_t *)obj)->start_y-(obj->h-(int)gui_get_screen_height()));
                                        touch_y = ((gui_grid_t *)obj)->start_y - (obj->h - (int)gui_get_screen_height());
                                    }

                                    ((gui_grid_t *)obj)->yold = obj->y;
                                }
                            }
                        }
                    }
                }
                deal_img_in_root_3d(obj, touch_y, 0);
            }
            break;
        case GRID_SCALE:
            {
                /**
                     * @note tp->deltaX >0 means slide right, the left tab shows
                     *       tp->deltaX <0 means slide left, the right tab shows
                     *       the left tab's dx -320~0
                     *       the right tab's dx 320~0
                     */


                {
//gui_log("%d\n", tp->type);
                    switch (tp->type)
                    {
                    case TOUCH_ORIGIN_FROM_Y:
                    case TOUCH_ORIGIN_FROM_X:
                    case TOUCH_LEFT_SLIDE:
                    case TOUCH_RIGHT_SLIDE:
                        //this->gap_col = this->gap_col_scale;
                        //this->gap_row = this->gap_row_scale;
                        break;
                    case TOUCH_HOLD_X:
                    /*{gui_log("%d\n", tp->type);
                        if ( obj->parent->type == TAB && obj->parent->parent->type == TABVIEW)
                                                {
                                                    if(((gui_tab_t *)(obj->parent))->id.x == ((gui_tabview_t *)(obj->parent->parent))->cur_id.x && ((gui_tab_t *)(obj->parent))->id.y == ((gui_tabview_t *)(obj->parent->parent))->cur_id.y)
                                                    {
                                                        {
                                                            struct gui_grid *this = (void *)obj;
                            float scale = ((float)(gui_get_screen_width() + ((float)((tp->deltaX)))
                                                  )) / ((float)gui_get_screen_width());
                                                        obj->x = (1-scale)*0.5f*gui_get_screen_width();
                            obj->y = (1-scale)*0.5f*gui_get_screen_height();

                            deal_img_in_root(obj, scale, scale);

                            this->gap_col_scale = (uint32_t)(scale * (float)(this->gap_col));
                            this->gap_row_scale = (uint32_t)(scale * (float)(this->gap_row));
                                                        }
                                                    }
                                                }
                                                else
                                                {
                                                        struct gui_grid *this = (void *)obj;
                                                        float scale = 1.0f;
                                                        obj->x = (1-scale)*0.5f*gui_get_screen_width();
                            obj->y = (1-scale)*0.5f*gui_get_screen_height();

                            deal_img_in_root(obj, scale, scale);

                            this->gap_col_scale = (uint32_t)(scale * (float)(this->gap_col));
                            this->gap_row_scale = (uint32_t)(scale * (float)(this->gap_row));
                                                }

                    }

                    break;
                                case TOUCH_HOLD_Y:
                    if ( obj->parent->type == TAB && obj->parent->parent->type == TABVIEW)
                                                {
                                                    if(((gui_tab_t *)(obj->parent))->id.x == ((gui_tabview_t *)(obj->parent->parent))->cur_id.x && ((gui_tab_t *)(obj->parent))->id.y == ((gui_tabview_t *)(obj->parent->parent))->cur_id.y)
                                                    {
                                                        struct gui_grid *this = (void *)obj;
                                                        float scale = 1.0f;
                                                        obj->x = (1-scale)*0.5f*gui_get_screen_width();
                            obj->y = (1-scale)*0.5f*gui_get_screen_height();

                            deal_img_in_root(obj, scale, scale);

                            this->gap_col_scale = (uint32_t)(scale * (float)(this->gap_col));
                            this->gap_row_scale = (uint32_t)(scale * (float)(this->gap_row));
                                                        }
                                                    }

                    break;*/
                    case TOUCH_HOLD_Y:
                        {
                            if (obj->dx == 0 && obj->dy == 0)
                            {
                                float scale = ((float)(gui_get_screen_width() + ((float)((tp->deltaX)))
                                                      )) / ((float)gui_get_screen_width());
                                if (scale < 0.3f) { scale = 0.3f; }
                                if (scale > 3.0f) { scale = 3.0f; }
                                deal_img_in_root(obj, scale, scale);
                                struct gui_grid *this = (void *)obj;
                                this->gap_col_scale = scale * this->gap_col;
                                this->gap_row_scale = scale * this->gap_row;
                            }
                        }


                    default:
                        break;
                    }
                }
            }
            break;
        default:
            break;
        }


    }
}
void gui_grid_ctor(struct gui_grid *this,
                   gui_obj_t *parent, int16_t x,
                   int16_t y,
                   int16_t row,
                   int16_t col,
                   uint16_t gap_col,
                   uint16_t gap_row)
{
    gui_obj_ctor(&this->base, parent, "grid", x, y, gap_col * col, 700);
    GET_BASE(this)->obj_update_att = obj_update_att;
    GET_BASE(this)->obj_prepare = tab_prepare_scale;
    this->row = row;
    this->col = col;
    this->gap_col = gap_col;
    this->gap_row = gap_row;
    this->gap_col_scale = gap_col;
    this->gap_row_scale = gap_row;
    this->change_threshold = 50;
    this->start_x = x;
    this->start_y = y;
}
gui_grid_t *gui_grid_create(void *parent,
                            int16_t x,
                            int16_t y,
                            int16_t row,
                            int16_t col,
                            uint16_t gap_col,
                            uint16_t gap_row)
{
#define _GUI_NEW_gui_grid_createparam this, parent, x, y, row, col, gap_col, gap_row
    GUI_NEW(gui_grid_t, gui_grid_ctor, _GUI_NEW_gui_grid_createparam)
}
void gui_grid_style(gui_grid_t *grid, enum gui_grid_style style)
{
    grid->style = style;
    if (style == GRID_3D)
    {
        grid->col += 2;
    }

}








