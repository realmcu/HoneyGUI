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
#include <guidef.h>
#include <gui_matrix.h>
#include <gui_fb.h>
#include <gui_obj.h>
#include "gui_server.h"

#define MAX_EVENT_CNT   10
static gui_event_cb_t event_cb[MAX_EVENT_CNT];
static gui_event_t event_code[MAX_EVENT_CNT];
static void *event_cb_param[MAX_EVENT_CNT];
static void *event_obj[MAX_EVENT_CNT];
static uint8_t event_cnt = 0;
static bool fb_change = false;


static bool obj_is_active(gui_obj_t *obj)
{
    GUI_ASSERT(obj != NULL);
    float m00 = obj->matrix->m[0][0];
    float m01 = obj->matrix->m[0][1];
    float m02 = obj->matrix->m[0][2];
    float m10 = obj->matrix->m[1][0];
    float m11 = obj->matrix->m[1][1];
    float m12 = obj->matrix->m[1][2];
    float m20 = obj->matrix->m[2][0];
    float m21 = obj->matrix->m[2][1];
    float m22 = obj->matrix->m[2][2];

    if ((m01 == 0) && \
        (m10 == 0) && \
        (m20 == 0) && \
        (m21 == 0) && \
        (m22 == 1)) //scale and translate, no rotate
    {
        float x_min = m02;
        float x_max = m02 + m00 * obj->w;
        float y_min = m12;
        float y_max = m12 + m11 * obj->h;
        if ((x_min > (int)gui_get_screen_width()) || \
            (x_max < 0) || \
            (y_min > (int)gui_get_screen_height()) || \
            (y_max < 0))
        {
            obj->active = false;
            return false;
        }
    }

    gui_point_t p[4] =
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
        gui_point_t *point = p + i;
        if (point->p[0] < x_min)
        {
            x_min = point->p[0];
        }
    }

    for (uint32_t i = 1; i < 3; i++)
    {
        gui_point_t *point = p + i;
        if (point->p[0] > x_max)
        {
            x_max = point->p[0];
        }
    }

    for (uint32_t i = 1; i < 3; i++)
    {
        gui_point_t *point = p + i;
        if (point->p[1] < y_min)
        {
            y_min = point->p[1];
        }
    }

    for (uint32_t i = 1; i < 3; i++)
    {
        gui_point_t *point = p + i;
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

static void obj_input_prepare(gui_obj_t *obj)
{
    gui_list_t *node = NULL;
    gui_list_for_each(node, &obj->child_list)
    {
        gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);
        if (obj->has_input_prepare_cb)
        {
            if ((obj->matrix != NULL) && (obj->parent->matrix))
            {
                memcpy(obj->matrix, obj->parent->matrix, sizeof(gui_matrix_t));
                matrix_translate(obj->x, obj->y, obj->matrix);
            }

            obj->obj_cb(obj, OBJ_INPUT_PREPARE);
            if (obj_is_active(obj) == false)
            {
                continue;
            }
        }
        obj_input_prepare(obj);
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

        obj->opacity_value = obj->parent->opacity_value;

        if ((obj->matrix != NULL) && (obj->parent->matrix))
        {
            memcpy(obj->matrix, obj->parent->matrix, sizeof(gui_matrix_t));
            matrix_translate(obj->x, obj->y, obj->matrix);
        }

        if (obj->has_prepare_cb)
        {
            if (!obj->gesture)
            {
                obj->obj_cb(obj, OBJ_PREPARE);
            }
        }
        if (obj->not_show)
        {
            continue;
        }
        if (obj_is_active(obj) == false)
        {
            continue;
        }
#ifdef ENABLE_GUI_CONTAIN_FUNCTION
        int16_t x1 = _UI_MAX(obj->parent->range_x, obj->x);
        int16_t y1 = _UI_MAX(obj->parent->range_y, obj->y);
        int16_t x2 = _UI_MIN(obj->parent->range_x + obj->parent->range_w - 1, obj->x + obj->w - 1);
        int16_t y2 = _UI_MIN(obj->parent->range_y + obj->parent->range_h - 1, obj->y + obj->h - 1);

        if ((x2 > x1) && (y2 > y1))
        {
            obj->range_x = x1 - obj->x;
            obj->range_y = y1 - obj->y;
            obj->range_w = x2 - x1 + 1;
            obj->range_h = y2 - y1 + 1;
        }
        else
        {
            GUI_ASSERT(NULL != NULL);
        }

#endif
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
            if (obj->has_draw_cb)
            {

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

                obj->obj_cb(obj, OBJ_DRAW);
            }
        }
        else
        {
            continue;
        }
        obj_draw_scan(obj);
    }
}


static void obj_draw_end(gui_obj_t *obj)
{
    gui_list_t *node = NULL;
    gui_list_for_each(node, &obj->child_list)
    {
        gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);
        if (obj->has_end_cb)
        {
            obj->obj_cb(obj, OBJ_END);
        }
        if (obj->active)
        {
            for (uint32_t i = 0; i < obj->event_dsc_cnt; i++)
            {
                gui_event_dsc_t *event_dsc = obj->event_dsc + i;
                if (event_dsc->filter == event_dsc->event_code)
                {
                    event_cb[event_cnt] = event_dsc->event_cb;
                    event_code[event_cnt] = event_dsc->event_code;
                    event_cb_param[event_cnt] = event_dsc->user_data;
                    event_obj[event_cnt] = obj;
                    event_dsc->event_code = GUI_EVENT_INVALIDE;
                    event_cnt++;
                }
            }
        }

        matrix_identity(obj->matrix);
        obj->active = false;

        obj->skip_tp_left_hold = true;
        obj->skip_tp_right_hold = true;
        obj->skip_tp_up_hold = true;
        obj->skip_tp_down_hold = true;
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
                memset(dc->frame_buf, 0x0, dc->fb_height * dc->fb_width * (dc->bit_depth >> 3));
            }
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
        memset(dc->frame_buf, 0x00, dc->fb_height * dc->fb_width * (dc->bit_depth >> 3));

        obj_draw_scan(root);
        if (dc->lcd_draw_sync != NULL)
        {
            dc->lcd_draw_sync();
        }
        dc->lcd_update(dc);

    }
}


void gui_fb_change(void)
{
    fb_change = true;
}
static uint32_t spf = 5;
uint32_t gui_spf()
{
    return spf;
}
void gui_fb_disp(gui_obj_t *root)
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
    if (dc->lcd_frame_monitor)
    {
        dc->lcd_frame_monitor->input_prepare_cb();
    }
    obj_draw_prepare(root);
    if (dc->lcd_frame_monitor)
    {
        dc->lcd_frame_monitor->draw_prepare_cb();
    }

    static int last_ms;
    if (fb_change == true)
    {
        gui_fb_draw(root);
        fb_change = false;
        {
            int ms = gui_ms_get();
            if (last_ms)
            {
                spf = ms - last_ms;
                //gui_log("spf:%d ms", spf);
            }
            else { spf = 0; }
            last_ms = ms;
        }
    }
    else
    {
        gui_thread_mdelay(17);
        last_ms = gui_ms_get();
    }
    if (dc->lcd_frame_monitor)
    {
        dc->lcd_frame_monitor->draw_cb();
    }

    obj_draw_end(root);
    for (uint8_t i = 0; i < event_cnt; i++)
    {
        event_cb[i](event_obj[i], event_code[i], event_cb_param[i]);
    }
    event_cnt = 0;
    dc->frame_count++;

    if (dc->lcd_frame_monitor)
    {
        dc->lcd_frame_monitor->end_cb();
    }

}


