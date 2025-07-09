/**
 * @file gui_fb.c
 * @author howie_wang (howie_wang@realtek.com.cn)
 * @brief
 * @version 0.1
 * @date 2023-10-16
 *
 * @copyright Copyright (c) 2023
 *
 */
#include <string.h>
#include "guidef.h"
#include "gui_matrix.h"
#include "gui_fb.h"
#include "gui_obj.h"
#include "gui_server.h"
#include "gui_post_process.h"


static bool fb_change = false;
static uint32_t gui_obj_count;
static uint32_t obj_count;
static int frame_count_per_second;
static uint32_t spf = 5;


uint32_t gui_get_obj_count(void)
{
    return gui_obj_count;
}
static void obj_reset_active(gui_obj_t *obj)
{
    gui_node_list_t *node = NULL;
    gui_list_for_each(node, &obj->child_list)
    {
        gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);
        obj->active = false;
        obj_reset_active(obj);
    }
}

static bool obj_is_active(gui_obj_t *obj)
{
    GUI_ASSERT(obj != NULL);

    if (gui_obj_out_screen(obj))
    {
        obj->active = false;
        return false;
    }

    gui_point3f_t p[4] =
    {
        {0, 0, 1},
        {obj->w, 0, 1},
        {0, obj->h, 1},
        {obj->w, obj->h, 1}
    };

    float x_min = 0.0f;
    float x_max = 0.0f;
    float y_min = 0.0f;
    float y_max = 0.0f;


    matrix_multiply_point(obj->matrix, p);
    x_min = p->p[0];
    x_max = p->p[0];
    y_min = p->p[1];
    y_max = p->p[1];
    matrix_multiply_point(obj->matrix, p + 1);
    matrix_multiply_point(obj->matrix, p + 2);
    matrix_multiply_point(obj->matrix, p + 3);

    for (uint32_t i = 1; i < 3; i++)
    {
        gui_point3f_t *point = p + i;
        if (point->p[0] < x_min)
        {
            x_min = point->p[0];
        }
    }

    for (uint32_t i = 1; i < 3; i++)
    {
        gui_point3f_t *point = p + i;
        if (point->p[0] > x_max)
        {
            x_max = point->p[0];
        }
    }

    for (uint32_t i = 1; i < 3; i++)
    {
        gui_point3f_t *point = p + i;
        if (point->p[1] < y_min)
        {
            y_min = point->p[1];
        }
    }

    for (uint32_t i = 1; i < 3; i++)
    {
        gui_point3f_t *point = p + i;
        if (point->p[1] > y_max)
        {
            y_max = point->p[1];
        }
    }

    if ((x_min < (int)gui_get_screen_width()) && (x_max > 0) && \
        (y_min < (int)gui_get_screen_height()) && (y_max > 0))
    {
        obj->active = true;
        return true;
    }
    obj->active = false;

    return false;
}

static void obj_input_prepare(gui_obj_t *object)
{
    GUI_ASSERT(object->name != NULL);
    gui_node_list_t *node = NULL;

    gui_list_for_each(node, &object->child_list)
    {
        GUI_ASSERT(node != NULL);
        gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);
        if (obj->not_show)
        {
            continue;
        }
        if (obj->parent->matrix)
        {
            memcpy(obj->matrix, obj->parent->matrix, sizeof(gui_matrix_t));
        }
        matrix_translate(obj->x, obj->y, obj->matrix);
        if (obj->has_input_prepare_cb)
        {
            obj->obj_cb(obj, OBJ_INPUT_PREPARE);
        }
        if (obj_is_active(obj) == false)
        {
            continue;
        }
        obj_input_prepare(obj);
    }
}

static void obj_draw_prepare(gui_obj_t *object)
{
    GUI_ASSERT(object->name != NULL);
    gui_node_list_t *node = NULL;

    gui_list_for_each(node, &object->child_list)
    {
        GUI_ASSERT(node != NULL);
        gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);
        if (!(obj->create_done))
        {
            continue;
        }

        obj->opacity_value = obj->parent->opacity_value;

        if (obj->parent->matrix)
        {
            memcpy(obj->matrix, obj->parent->matrix, sizeof(gui_matrix_t));
        }
        matrix_translate(obj->x, obj->y, obj->matrix);

        if (obj->has_prepare_cb)
        {
            obj->obj_cb(obj, OBJ_PREPARE);
        }

        extern void gui_obj_timer_handler(gui_obj_t *obj); //not called for appliacation
        gui_obj_timer_handler(obj);

        if (obj->not_show)
        {
            continue;
        }
        if (obj_is_active(obj) == false)
        {
            continue;
        }



        obj_draw_prepare(obj);
    }
}

static void obj_draw_scan(gui_obj_t *obj)
{

    gui_node_list_t *node = NULL;
    struct gui_dispdev *dc = gui_get_dc();
    gui_list_for_each(node, &obj->child_list)
    {
        gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);
        if (obj->active)
        {
            if (obj->has_draw_cb)
            {
                obj->obj_cb(obj, OBJ_DRAW);
            }
        }
        else
        {
            continue;
        }
        obj_draw_scan(obj);
        if (obj->need_preprocess)
        {
            obj->obj_cb(obj, OBJ_PREPROCESS);
        }
    }
}


static void obj_draw_end(gui_obj_t *obj)
{
    gui_node_list_t *node = NULL;
    gui_list_for_each(node, &obj->child_list)
    {
        gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);
        obj_count++;
        if (obj->has_end_cb)
        {
            obj->obj_cb(obj, OBJ_END);
        }

        matrix_identity(obj->matrix);
        obj->active = false;

        obj_draw_end(obj);
    }
}

static void gui_fb_draw(gui_obj_t *root)
{
    gui_dispdev_t *dc = gui_get_dc();
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
        gui_flash_boost();
        for (uint32_t i = 0; i < dc->section_total; i++)
        {
            if (i % 2 == 0)
            {
                dc->frame_buf = dc->disp_buf_1;
            }
            else
            {
                dc->frame_buf = dc->disp_buf_2;
            }
            if (dc->frame_buf)
            {
                if (dc->lcd_section_hook)
                {
                    dc->lcd_section_hook(dc);
                }
                else
                {
                    memset(dc->frame_buf, 0x0, (dc->fb_height * dc->fb_width * dc->bit_depth) >> 3);
                }
            }
            dc->section_count = i;

            if (dc->pfb_type == PFB_Y_DIRECTION)
            {
                dc->section.x1 = 0;
                dc->section.y1 = dc->section_count * dc->fb_height;
            }
            else
            {
                dc->section.x1 = dc->section_count * dc->fb_width;
                dc->section.y1 = 0;
            }

            dc->section.x2 = _UI_MIN(dc->section.x1 + dc->fb_width - 1, dc->screen_width - 1);
            dc->section.y2 = _UI_MIN(dc->section.y1 + dc->fb_height - 1, dc->screen_height - 1);
            obj_draw_scan(root);
            post_process_handle();
            if (dc->get_lcd_us != NULL)
            {
                read_time = line_time * (i * dc->fb_height + hfp_line);
                write_time = dc->get_lcd_us();
                while (write_time < read_time)
                {
                    write_time = dc->get_lcd_us();
                }
            }
            if (dc->lcd_draw_sync != NULL)
            {
                dc->lcd_draw_sync();
            }
            dc->lcd_update(dc);
        }

        gui_flash_boost_disable();
    }
    else if (dc->type == DC_SINGLE)
    {
        memset(dc->frame_buf, 0x00, (dc->fb_height * dc->fb_width * dc->bit_depth) >> 3);
        dc->section = (gui_rect_t) {0, 0, dc->fb_width - 1, dc->fb_height - 1};
        obj_draw_scan(root);
        post_process_handle();
        if (dc->lcd_draw_sync != NULL)
        {
            dc->lcd_draw_sync();
        }
        dc->lcd_update(dc);
    }
    else if (dc->type == DC_DOUBLE)
    {
        if (dc->frame_buf == NULL)
        {
            dc->frame_buf = dc->disp_buf_1;
        }
        else if (dc->frame_buf == dc->disp_buf_2)
        {
            dc->frame_buf = dc->disp_buf_1;
        }
        else
        {
            dc->frame_buf = dc->disp_buf_2;
        }
        memset(dc->frame_buf, 0x00, (dc->fb_height * dc->fb_width * dc->bit_depth) >> 3);
        dc->section = (gui_rect_t) {0, 0, dc->fb_width - 1, dc->fb_height - 1};
        obj_draw_scan(root);
        post_process_handle();
        if (dc->lcd_draw_sync != NULL)
        {
            dc->lcd_draw_sync();
        }
        dc->lcd_update(dc);
    }
    post_process_end();
}

uint32_t gui_fps()
{
    return frame_count_per_second;
}
void gui_fb_change(void)
{
    fb_change = true;
}

uint32_t gui_spf()
{
    return spf;
}
void gui_fb_disp(gui_obj_t *root, bool enable_event)
{
    if (root == NULL)
    {
        GUI_ASSERT(NULL != NULL);
        return;
    }

    gui_dispdev_t *dc = gui_get_dc();
    if (dc->lcd_frame_monitor)
    {
        dc->lcd_frame_monitor->start_cb();
    }

    if ((dc->frame_buf == NULL) && (dc->disp_buf_1 == NULL) && (dc->disp_buf_2 == NULL))
    {
        GUI_ASSERT(NULL != NULL);
        return;
    }

    obj_input_prepare(root);
    obj_reset_active(root);
    if (dc->lcd_frame_monitor)
    {
        dc->lcd_frame_monitor->input_prepare_cb();
    }
    obj_draw_prepare(root);

    if (dc->lcd_frame_monitor)
    {
        dc->lcd_frame_monitor->draw_prepare_cb();
    }

    static int one_second;
    int tick = gui_ms_get();
    static int frame_count;
    if (tick - one_second >= 1000)
    {
        one_second = tick;
        frame_count_per_second = frame_count;
        frame_count = 0;
        // gui_log("fps:%d\r", frame_count_per_second);
    }
    if (fb_change == true)
    {
        frame_count++;
        gui_fb_draw(root);
        fb_change = false;
    }
    else
    {
        gui_thread_mdelay(17);
    }
    if (dc->lcd_frame_monitor)
    {
        dc->lcd_frame_monitor->draw_cb();
    }

    obj_draw_end(root);

    gui_obj_event_dispatch(enable_event);

    gui_obj_count = obj_count;
    obj_count = 0;

    dc->frame_count++;

    if (dc->lcd_frame_monitor)
    {
        dc->lcd_frame_monitor->end_cb();
    }

}


