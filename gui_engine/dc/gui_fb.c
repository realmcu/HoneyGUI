
//#include <math.h>
#include <string.h>
//#include <stdlib.h>
#include <guidef.h>
#include <gui_fb.h>
#include <gui_obj.h>
#include "gui_server.h"
#include "tp_algo.h"
#include "gui_kb.h"



static void obj_is_active(gui_obj_t *obj)
{

    int sx = obj->dx + obj->ax + obj->tx;
    int sy = obj->dy + obj->ay + obj->ty;
    int ex = sx + obj->w;
    int ey = sy + obj->h;


    if ((sx < (int)gui_get_screen_width()) && ((ex > 0) || sx == 0) && \
        (sy < (int)gui_get_screen_height()) && ((ey > 0) || sy == 0))
    {
        obj->active = true;
    }
}


static void obj_draw_prepare(gui_obj_t *object)
{
    GUI_ASSERT(object->name != NULL);
    gui_list_t *node = NULL;

    gui_list_for_each(node, &object->child_list)
    {
        GUI_ASSERT(node != NULL);
        gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);
        if (!(obj->create_done))
        {
            continue;
        }
        if (obj->not_show)
        {
            continue;
        }
        if (obj->obj_update_att != NULL)
        {
            obj->obj_update_att(obj);//update change x, not change dx
        }
        obj->dx += obj->parent->dx;
        obj->dy += obj->parent->dy;
        obj->ax = obj->x + obj->parent->ax;
        obj->ay = obj->y + obj->parent->ay;
        obj->sx *= obj->parent->sx;
        obj->sy *= obj->parent->sy;
        obj->tx += obj->parent->tx;
        obj->ty += obj->parent->ty;
        if (obj->obj_prepare != NULL)
        {
            obj->obj_prepare(obj);
        }
        if (obj->not_show)
        {
            continue;
        }
        obj_is_active(obj);
        obj_draw_prepare(obj);
    }
}

static void obj_draw_scan(gui_obj_t *obj)
{
    gui_list_t *node = NULL;
    struct gui_dispdev *dc = gui_get_dc();
    gui_list_for_each(node, &obj->child_list)
    {
        gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);
        if (obj->active)
        {
            dc->section.y1 = dc->section_count * dc->fb_height;
            dc->section.y2 = _UI_MIN(dc->section.y1 + dc->fb_height, dc->screen_height);
            if (obj->obj_draw != NULL)
            {
                obj->obj_draw(obj);
            }

        }
        obj_draw_scan(obj);
    }
}


#define MAX_EVENT_CNT   10
static gui_event_cb_t event_cb[MAX_EVENT_CNT];
static gui_event_t event_code[MAX_EVENT_CNT];
static void *event_obj[MAX_EVENT_CNT];
static uint8_t event_cnt = 0;

static void obj_draw_end(gui_obj_t *obj)
{
    gui_list_t *node = NULL;
    gui_list_for_each(node, &obj->child_list)
    {
        gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);
        if (obj->active)
        {
            if (obj->obj_end != NULL)
            {
                obj->obj_end(obj);
            }

            for (uint32_t i = 0; i < obj->event_dsc_cnt; i++)
            {
                gui_event_dsc_t *event_dsc = obj->event_dsc + i;
                if (event_dsc->filter == event_dsc->event_code)
                {
                    event_cb[event_cnt] = event_dsc->event_cb;
                    event_code[event_cnt] = event_dsc->event_code;
                    event_obj[event_cnt] = obj;
                    event_dsc->event_code = GUI_EVENT_INVALIDE;
                    event_cnt++;
                }
            }
        }
        obj->dx = 0;
        obj->dy = 0;
        obj->ax = 0;
        obj->ay = 0;
        obj->tx = 0;
        obj->ty = 0;
        obj->sx = 1.0f;
        obj->sy = 1.0f;
        obj->active = false;
        obj_draw_end(obj);
    }
}





void gui_fb_disp(gui_obj_t *root)
{
    if (root == NULL)
    {
        GUI_ASSERT(NULL != NULL);
        return;
    }

    gui_dispdev_t *dc = gui_get_dc();
    if ((dc->frame_buf == NULL) && (dc->disp_buf_1 == NULL) && (dc->disp_buf_2 == NULL))
    {
        GUI_ASSERT(NULL != NULL);
        return;
    }

    struct gui_touch_data *raw = touchpad_get_data();
    tp_algo_process(raw);

    gui_kb_port_data_t *kb_raw = kb_get_data();
    kb_algo_process(kb_raw);

    obj_draw_prepare(root);
    uint32_t active_line = dc->driver_ic_active_width;
    uint32_t hfp_line = dc->driver_ic_hfp;
    uint32_t hbp_line = dc->driver_ic_hbp;
    uint32_t fps = dc->driver_ic_fps;
    uint32_t line_time = 1000000 / (fps * (active_line + hfp_line + hbp_line));

    uint32_t read_time = 0;
    uint32_t write_time = 0;

    if (dc->reset_lcd_timer != NULL)
    {
        dc->reset_lcd_timer();
    }

    if (dc->lcd_te_wait != NULL)
    {
        dc->lcd_te_wait();
    }


    if (dc->type == DC_RAMLESS)
    {
        uint32_t section_count = dc->screen_height / dc->fb_height + ((dc->screen_height % dc->fb_height) ?
                                                                      1 : 0);
        gui_flash_boost();
        for (uint32_t i = 0; i < section_count; i++)
        {
            if (i % 2 == 0)
            {
                dc->frame_buf = dc->disp_buf_1;
            }
            else
            {
                dc->frame_buf = dc->disp_buf_2;
            }
            memset(dc->frame_buf, 0x0, dc->fb_height * dc->fb_width * dc->bit_depth >> 3);
            dc->section_count = i;

            obj_draw_scan(root);
            if (dc->get_lcd_us != NULL)
            {
                read_time = line_time * (i * dc->fb_height + hfp_line);
                write_time = dc->get_lcd_us();
                while (write_time < read_time)
                {
                    write_time = dc->get_lcd_us();
                }
                //gui_log("read = %dus, write = %d us\n", read_time, write_time);
            }

            dc->lcd_update(dc);
        }

        gui_flash_boost_disable();
    }
    else if (dc->type == DC_SINGLE)
    {
        memset(dc->frame_buf, 0x00, dc->fb_height * dc->fb_width * dc->bit_depth >> 3);

#if 0
        uint32_t start_tick = rt_tick_get_millisecond();
        obj_draw_scan(root);
        uint32_t end_tick = rt_tick_get_millisecond();
        gui_log("draw time :%dms \n", end_tick - start_tick);
        uint32_t lcd_start_tick = rt_tick_get_millisecond();
        dc->lcd_update(dc);
        uint32_t lcd_end_tick = rt_tick_get_millisecond();
        gui_log("lcd time :%dms \n", lcd_end_tick - lcd_start_tick);
#else
        obj_draw_scan(root);
        dc->lcd_update(dc);
#endif
    }
    obj_draw_end(root);
    for (uint8_t i = 0; i < event_cnt; i++)
    {
        event_cb[i](event_obj[i], event_code[i]);
    }
    event_cnt = 0;

}


