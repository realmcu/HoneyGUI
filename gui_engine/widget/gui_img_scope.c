/*
 * File      : gui_img_scope.c
 * This file is part of GUI Engine
 */
#include <guidef.h>
#include <gui_img_scope.h>
#include <string.h>
//#include <gui_matrix.h>
#include <gui_obj.h>
#include <draw_img.h>
#include <tp_algo.h>
#include <gui_kb.h>
#include "acc_engine.h"

static void img_prepare(gui_obj_t *obj)
{
    GUI_ASSERT(obj != NULL);
    gui_img_t *img = (gui_img_t *)obj;
    gui_dispdev_t *dc = gui_get_dc();
    touch_info_t *tp = tp_get_info();
    kb_info_t *kb = kb_get_info();
    draw_img_t *draw_img = &img->draw_img;

    struct gui_rgb_data_head head;
    memcpy(&head, draw_img->data, sizeof(head));

    /* set image information */
    draw_img->img_w = head.w;
    draw_img->img_h = head.h;
    obj->w = draw_img->img_w;
    obj->h = draw_img->img_h;

    if (tp->type == TOUCH_SHORT)
    {
        if (tp->x > obj->x && tp->x < obj->x + obj->w && tp->y > obj->y && tp->y < obj->y + obj->h)
        {
            gui_obj_event_set(obj, GUI_EVENT_TOUCH_CLICKED);
        }
    }
}

static void img_scope_draw_cb(gui_obj_t *obj)
{
    GUI_ASSERT(obj != NULL);
    gui_img_t *img = (gui_img_t *)obj;
    struct gui_dispdev *dc = gui_get_dc();
    draw_img_t *draw_img = &img->draw_img;

    rtgui_rect_t draw_rect = {0};
    draw_rect.x1 = obj->dx;
    draw_rect.y1 = obj->dy;
    draw_rect.x2 = draw_rect.x1 + obj->w;
    draw_rect.y2 = draw_rect.y1 + obj->h;
    draw_rect.xboundleft = GUI_TYPE(gui_img_scope_t, obj)->scope_x1;
    draw_rect.xboundright = GUI_TYPE(gui_img_scope_t, obj)->scope_x2;
    draw_rect.yboundbottom = GUI_TYPE(gui_img_scope_t, obj)->scope_y2;
    draw_rect.yboundtop = GUI_TYPE(gui_img_scope_t, obj)->scope_y1;
    if (gui_get_acc() != NULL)
    {
        gui_get_acc()->blit(draw_img, dc, &draw_rect);
    }
    else
    {
        GUI_ASSERT(NULL != NULL);
    }
}
static void img_end(gui_obj_t *obj)
{
    GUI_ASSERT(obj != NULL);
}
static void img_destory(gui_obj_t *obj)
{
    GUI_ASSERT(obj != NULL);
    gui_log("do obj %s free\n", obj->name);
    //gui_free(obj);
}


void gui_img_scope_ctor(gui_img_t *this, gui_obj_t *parent, const char *name, void *addr,
                        int16_t x,
                        int16_t y, int16_t w, int16_t h)
{
    draw_img_t *draw_img = &this->draw_img;
    //for base class
    gui_obj_t *base = (gui_obj_t *)this;
    gui_obj_ctor(base, parent, name, x, y, w, h);

    //for root class
    gui_obj_t *root = (gui_obj_t *)this;
    root->type = IMAGE_SCOPE;
    root->obj_prepare = img_prepare;
    root->obj_draw = img_scope_draw_cb;
    root->obj_end = img_end;
    root->obj_destory = img_destory;
    draw_img->blend_mode = IMG_FILTER_BLACK;
    //for self

    draw_img->data = addr;
    draw_img->opacity_value = 255;

}

gui_img_scope_t *gui_img_scope_create(void *parent, void *addr, int16_t x, int16_t y)
{
    GUI_ASSERT(parent != NULL);
    char     *name = "DEFAULT_IMG";

    gui_img_scope_t *img = gui_malloc(sizeof(gui_img_scope_t));
    GUI_ASSERT(img != NULL);
    memset(img, 0x00, sizeof(gui_img_scope_t));

    gui_img_scope_ctor((void *)img, (gui_obj_t *)parent, name, addr, x, y, 0, 0);
    gui_list_init(&(GET_BASE(img)->child_list));
    if ((GET_BASE(img)->parent) != NULL)
    {
        gui_list_insert_before(&((GET_BASE(img)->parent)->child_list), &(GET_BASE(img)->brother_list));
    }
    GET_BASE(img)->create_done = true;
    return img;
}

