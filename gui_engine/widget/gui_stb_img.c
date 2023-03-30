/*
 * File      : gui_stb_img.c
 * This file is part of GUI Engine
 */
#include <string.h>
#include <gui_obj.h>
#include <draw_img.h>
#include <gui_stb_img.h>

static void stb_image_prepare(gui_obj_t *obj)
{
    GUI_ASSERT(obj != NULL);
    gui_img_t *img = (gui_img_t *)obj;
    draw_img_t *draw_img = &img->draw_img;
    rtgui_image_create(draw_img, true);
}
static void stb_image_draw_cb(gui_obj_t *obj)
{
    GUI_ASSERT(obj != NULL);
    gui_stb_img_t *img = (gui_stb_img_t *)obj;
    draw_img_t *draw_img = &img->base.draw_img;
    struct gui_dispdev *dc = gui_get_dc();
    rtgui_rect_t draw_rect = {0};
    draw_rect.x1 = obj->dx;
    draw_rect.y1 = obj->dy;
    draw_rect.x2 = draw_rect.x1 + obj->w;
    draw_rect.y2 = draw_rect.y1 + obj->h;

    if (((gui_img_file_head_t *)(img->base.draw_img.data))->type == 11)
    {
        if (img->gif_info->delay_ms_sum)
        {
            uint32_t cur_time_ms = gui_ms_get();
            uint8_t index = (cur_time_ms - img->gif_info->init_time_ms) / img->gif_info->delay_ms_sum;
            int32_t delay_cur = (cur_time_ms - img->gif_info->init_time_ms) % img->gif_info->delay_ms_sum;
            if (index >= img->gif_info->cycle_index)
            {
                return;
            }
            uint8_t i = 0;
            while (delay_cur >= 0)
            {
                delay_cur -= img->gif_info->delay_ms[i++];
            }
            img->gif_info->current_frame = --i;
        }
    }
    rtgui_image_blit(draw_img, dc, &draw_rect);
}

static void stb_image_end(gui_obj_t *obj)
{
    GUI_ASSERT(obj != NULL);
    gui_stb_img_t *img = (gui_stb_img_t *)obj;
    draw_img_t *draw_img = &img->base.draw_img;
    rtgui_image_destroy(draw_img);
}

void gui_stbimg_set_attribute(gui_stb_img_t *this, void *addr, uint32_t size,
                              uint8_t type, int16_t x, int16_t y)
{
    GUI_ASSERT(this != NULL);
    this->src_changed = true;

    if (((gui_img_file_head_t *)(this->base.draw_img.data))->type == 11)
    {
        if (type == 11)
        {
            this->gif_info->init_time_ms =  gui_ms_get();
        }
        else
        {
            gui_free(this->gif_info);
        }
    }
    else
    {
        if (type == 11)
        {
            gui_gif_info_t *gif = gui_malloc(sizeof(gui_gif_info_t));
            gif->init_time_ms =  gui_ms_get();
            this->gif_info = gif;
            this->gif_info->total_frame = 1;
            this->gif_info->cycle_index = 0xFFFFFFFF;
        }
    }

    gui_free(this->base.draw_img.data);

    void *img_buf = gui_malloc(size + sizeof(gui_img_file_head_t));
    memcpy(((char *)img_buf + sizeof(gui_img_file_head_t)), addr, size);

    gui_img_file_head_t head;
    memset(&head, 0x0, sizeof(head));
    head.size.vector_size = size;
    head.type = type ;
    GUI_ASSERT(addr != NULL);
    memcpy(img_buf, &head, sizeof(head));

    this->base.draw_img.data = img_buf;

}

void gui_stbimg_from_mem_ctor(gui_stb_img_t *this, gui_obj_t *parent, const char *name,
                              void *addr,
                              uint32_t size, uint8_t type, int16_t x, int16_t y)
{
    //for base class
    gui_img_t *img = (gui_img_t *)this;
    draw_img_t *draw_img = &img->draw_img;

    gui_img_from_mem_ctor(img, parent, name, addr, x, y, 0, 0);

    //for root class
    gui_obj_t *root = (gui_obj_t *)this;
    root->type = IMAGE_FROM_MEM;
    root->obj_prepare = stb_image_prepare;
    root->obj_draw = stb_image_draw_cb;
    root->obj_end = stb_image_end;

    //for self
    void *img_buf = gui_malloc(size + sizeof(gui_img_file_head_t));
    memcpy(((char *)img_buf + sizeof(gui_img_file_head_t)), addr, size);
    gui_img_file_head_t head;
    memset(&head, 0x0, sizeof(head));
    head.size.vector_size = size;
    head.type = type ;
    memcpy(img_buf, &head, sizeof(head));
    draw_img->data = img_buf;
    this->src_changed = true;
    if (type == 11)
    {
        gui_gif_info_t *gif = gui_malloc(sizeof(gui_gif_info_t));
        gif->init_time_ms =  gui_ms_get();
        this->gif_info = gif;
        this->gif_info->total_frame = 1;
        this->gif_info->cycle_index = 0xFFFFFFFF;
    }
}

gui_stb_img_t *gui_stbimg_create_from_mem(void *parent,  const char *name,
                                          void *addr, uint32_t size, uint8_t type, int16_t x,
                                          int16_t y)
{
    GUI_ASSERT(parent != NULL);
    if (name == NULL)
    {
        name = "DEFAULT_STB_IMAGE";
    }
    gui_stb_img_t *img = gui_malloc(sizeof(gui_stb_img_t));
    GUI_ASSERT(img != NULL);
    memset(img, 0x00, sizeof(gui_stb_img_t));

    gui_stbimg_from_mem_ctor(img, (gui_obj_t *)parent, name, addr, size, type, x, y);
    gui_list_init(&(GET_BASE(img)->child_list));
    if ((GET_BASE(img)->parent) != NULL)
    {
        gui_list_insert_before(&((GET_BASE(img)->parent)->child_list), &(GET_BASE(img)->brother_list));
    }
    GET_BASE(img)->create_done = true;
    return img;
}


