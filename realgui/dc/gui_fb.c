/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#include <string.h>
#include "guidef.h"
#include "gui_matrix.h"
#include "gui_fb.h"
#include "gui_obj.h"
#include "gui_post_process.h"


static bool fb_change = false;
static uint32_t gui_obj_count;
static uint32_t obj_count;
static int frame_count_per_second;

static gui_color_t fb_bg_color =
{
    .color.rgba.r = 0,
    .color.rgba.g = 0,
    .color.rgba.b = 0,
    .color.rgba.a = 255,
};

void gui_set_bg_color(gui_color_t color)
{
    fb_bg_color = color;
}

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

static bool rect_intersection(const gui_rect_t *a, const gui_rect_t *b, gui_rect_t *result)
{
    result->x1 = _UI_MAX(a->x1, b->x1);
    result->y1 = _UI_MAX(a->y1, b->y1);
    result->x2 = _UI_MIN(a->x2, b->x2);
    result->y2 = _UI_MIN(a->y2, b->y2);

    // Check if intersection is valid
    return (result->x1 <= result->x2) && (result->y1 <= result->y2);
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
    x_min = p->x;
    x_max = p->x;
    y_min = p->y;
    y_max = p->y;
    matrix_multiply_point(obj->matrix, p + 1);
    matrix_multiply_point(obj->matrix, p + 2);
    matrix_multiply_point(obj->matrix, p + 3);

    for (uint32_t i = 1; i < 3; i++)
    {
        gui_point3f_t *point = p + i;
        if (point->x < x_min)
        {
            x_min = point->x;
        }
    }

    for (uint32_t i = 1; i < 3; i++)
    {
        gui_point3f_t *point = p + i;
        if (point->x > x_max)
        {
            x_max = point->x;
        }
    }

    for (uint32_t i = 1; i < 3; i++)
    {
        gui_point3f_t *point = p + i;
        if (point->y < y_min)
        {
            y_min = point->y;
        }
    }

    for (uint32_t i = 1; i < 3; i++)
    {
        gui_point3f_t *point = p + i;
        if (point->y > y_max)
        {
            y_max = point->y;
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

        extern void gui_obj_timer_handler(gui_obj_t *obj); // not called for application
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
    // struct gui_dispdev *dc = gui_get_dc();
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
        if (obj->need_preprocess)
        {
            obj->obj_cb(obj, OBJ_PREPROCESS);
        }
        obj_draw_scan(obj);
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

/**
 * @brief Calculate the section rectangle for a given section index.
 *
 * PFB (Partial Frame Buffer) divides the screen into horizontal or vertical
 * strips called "sections". Each section is rendered independently.
 * For Y-direction: sections are horizontal strips (full width, fb_height tall).
 * For X-direction: sections are vertical strips (fb_width wide, full height).
 */
static gui_rect_t calc_section_rect(gui_dispdev_t *dc, uint32_t section_idx)
{
    gui_rect_t rect;
    if (dc->pfb_type == PFB_Y_DIRECTION)
    {
        rect.x1 = 0;
        rect.y1 = section_idx * dc->fb_height;
    }
    else
    {
        rect.x1 = section_idx * dc->fb_width;
        rect.y1 = 0;
    }
    rect.x2 = _UI_MIN(rect.x1 + dc->fb_width - 1, dc->screen_width - 1);
    rect.y2 = _UI_MIN(rect.y1 + dc->fb_height - 1, dc->screen_height - 1);
    return rect;
}

/**
 * @brief Find intersections between a section and all dirty regions.
 *
 * Tests each dirty region (including cleanup rects) against the section.
 * Returns the intersecting sub-rectangles. Sections with no intersections
 * can be skipped entirely (no rendering needed).
 *
 * @param section   Current section rectangle
 * @param mgr       Dirty region manager (includes real + cleanup regions)
 * @param out       Output array for intersection results
 * @param max_count Maximum intersections to store
 * @return Number of intersections found
 */
static uint16_t find_dirty_intersections(const gui_rect_t *section,
                                         const gui_dirty_region_manager_t *mgr,
                                         gui_rect_t *out, uint16_t max_count)
{
    uint16_t count = 0;
    for (uint16_t j = 0; j < mgr->count && count < max_count; j++)
    {
        gui_rect_t intersection;
        if (rect_intersection(section, &mgr->regions[j], &intersection))
        {
            out[count++] = intersection;
        }
    }
    return count;
}

/**
 * @brief Merge multiple intersection rects into a single bounding box.
 *
 * When a section intersects with multiple dirty regions (including cleanup
 * rects from the previous frame), we cannot render them separately because
 * each obj_draw_scan() call writes to fb[0] using dc->section width as stride.
 * Multiple calls with different strides would overwrite each other, leaving
 * only the last result. Merging into one bounding box ensures a single
 * obj_draw_scan() call with consistent stride.
 *
 * Example:
 *   intersections[0] = [50, 100, 150, 119]   (dirty region A)
 *   intersections[1] = [300, 100, 400, 119]   (cleanup rect)
 *   result bbox      = [50, 100, 400, 119]    (single render area)
 */
static gui_rect_t merge_to_bbox(const gui_rect_t *rects, uint16_t count)
{
    gui_rect_t bbox = rects[0];
    for (uint16_t k = 1; k < count; k++)
    {
        bbox.x1 = _UI_MIN(bbox.x1, rects[k].x1);
        bbox.y1 = _UI_MIN(bbox.y1, rects[k].y1);
        bbox.x2 = _UI_MAX(bbox.x2, rects[k].x2);
        bbox.y2 = _UI_MAX(bbox.y2, rects[k].y2);
    }
    return bbox;
}

/**
 * @brief Draw debug borders for dirty regions in the current section.
 *
 * Only draws borders for real dirty regions (not cleanup rects).
 * In full_refresh mode, draws a screen-sized border.
 * Controlled by gui_dirty_border_enable() API.
 */
static void draw_section_borders(gui_dispdev_t *dc, gui_dirty_region_manager_t *mgr,
                                 uint16_t real_dirty_count)
{
    if (!gui_dirty_border_is_enabled())
    {
        return;
    }

    if (mgr->full_refresh || mgr->count == 0)
    {
        gui_rect_t screen_rect = {0, 0, dc->screen_width - 1, dc->screen_height - 1};
        draw_dirty_border(dc, &screen_rect);
    }
    else
    {
        for (uint16_t j = 0; j < real_dirty_count; j++)
        {
            draw_dirty_border(dc, &mgr->regions[j]);
        }
    }
}

/**
 * @brief Wait for LCD scan line to avoid tear effect.
 *
 * Compares the LCD's current scan position with the host's write position.
 * If the host is about to overtake the scan line (which causes visible
 * tearing), busy-waits until safe to proceed.
 */
static void tear_effect_wait(gui_dispdev_t *dc, uint32_t time_base,
                             uint32_t section_idx, uint32_t line_section_cnt)
{
    if (dc->get_lcd_us == NULL)
    {
        return;
    }

    uint32_t scan_line_time_us = dc->driver_ic_scan_line_time_us;
    uint32_t write_line_time_us = dc->host_write_line_time_us;

    uint32_t scanned_line_cnt = (dc->get_lcd_us() - time_base) / scan_line_time_us;
    uint32_t writed_line_cnt = section_idx * line_section_cnt;

    uint32_t scanned_line_cnt_next = (dc->get_lcd_us() + write_line_time_us * line_section_cnt -
                                      time_base) / scan_line_time_us;
    uint32_t writed_line_cnt_next = (section_idx + 1) * line_section_cnt;

    while ((writed_line_cnt > scanned_line_cnt) && (writed_line_cnt_next > scanned_line_cnt_next))
    {
        scanned_line_cnt = (dc->get_lcd_us() - time_base) / scan_line_time_us;
        scanned_line_cnt_next = (dc->get_lcd_us() + write_line_time_us * line_section_cnt -
                                 time_base) / scan_line_time_us;
    }
}

static void gui_pfb_draw(gui_obj_t *root)
{
    gui_dispdev_t *dc = gui_get_dc();
    gui_dirty_region_manager_t *mgr = gui_dirty_region_get_manager();

    gui_flash_boost();

    uint32_t time_base = 0;
    if (dc->get_lcd_us != NULL)
    {
        time_base = dc->get_lcd_us();
    }

    /* Prepare dirty border cleanup: inject previous frame's border edges
     * as cleanup regions, return count of real (current frame) dirty regions */
    uint16_t real_dirty_count = dirty_border_prepare(mgr);

    for (uint32_t i = 0; i < dc->section_total; i++)
    {
        /* --- 1. Calculate section bounds --- */
        gui_rect_t section_rect = calc_section_rect(dc, i);
        uint32_t line_section_cnt = (dc->pfb_type == PFB_Y_DIRECTION)
                                    ? dc->fb_height : dc->fb_width;

        /* --- 2. Dirty region intersection check --- */
        gui_rect_t intersections[GUI_MAX_DIRTY_REGIONS];
        uint16_t intersection_count = 0;

        if (!mgr->full_refresh && mgr->count > 0)
        {
            intersection_count = find_dirty_intersections(&section_rect, mgr,
                                                          intersections, GUI_MAX_DIRTY_REGIONS);
            if (intersection_count == 0)
            {
                continue;
            }
        }

        /* --- 3. Select framebuffer (double buffering) --- */
        dc->frame_buf = (i % 2 == 0) ? dc->disp_buf_1 : dc->disp_buf_2;
        dc->section_count = i;

        /* --- 4. Clear framebuffer and render --- */
        gui_fb_clear(dc->frame_buf, fb_bg_color, ((size_t)dc->fb_height * dc->fb_width));

        if (mgr->full_refresh || mgr->count == 0)
        {
            dc->section = section_rect;
        }
        else
        {
            dc->section = merge_to_bbox(intersections, intersection_count);
        }
        obj_draw_scan(root);

        /* --- 5. Post-processing and debug borders --- */
        post_process_handle();
        draw_section_borders(dc, mgr, real_dirty_count);

        /* --- 6. Tear effect sync and LCD update --- */
        tear_effect_wait(dc, time_base, i, line_section_cnt);

        if (dc->lcd_draw_sync != NULL)
        {
            dc->lcd_draw_sync();
        }
        dc->lcd_update(dc);
    }

    gui_flash_boost_disable();
    gui_dirty_region_clear();
}

static void gui_fb_draw(gui_obj_t *root)
{
    uint32_t measure_start_time = gui_ms_get();
    gui_dispdev_t *dc = gui_get_dc();

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
        gui_pfb_draw(root);
    }
    else if (dc->type == DC_SINGLE)
    {
        gui_dirty_region_manager_t *mgr = gui_dirty_region_get_manager();

        if (mgr->full_refresh || mgr->count == 0)
        {
            // Full screen refresh (fallback mode or no dirty regions)
            // TODO: Performance optimization - clear only dirty regions instead of full framebuffer
            gui_fb_clear(dc->frame_buf, fb_bg_color, ((size_t)dc->fb_height * dc->fb_width));
            dc->section = (gui_rect_t) {0, 0, dc->fb_width - 1, dc->fb_height - 1};
            obj_draw_scan(root);
        }
        else
        {
            // Dirty region optimized rendering
            // TODO: Performance optimization - clear only dirty regions instead of full framebuffer
            gui_fb_clear(dc->frame_buf, fb_bg_color, ((size_t)dc->fb_height * dc->fb_width));

            for (uint16_t i = 0; i < mgr->count; i++)
            {
                dc->section = mgr->regions[i];
                obj_draw_scan(root);
            }
        }

        post_process_handle();

        if (gui_dirty_border_is_enabled())
        {
            if (mgr->full_refresh || mgr->count == 0)
            {
                gui_rect_t full_screen = {0, 0, dc->fb_width - 1, dc->fb_height - 1};
                draw_dirty_border(dc, &full_screen);
            }
            else
            {
                for (uint16_t i = 0; i < mgr->count; i++)
                {
                    draw_dirty_border(dc, &mgr->regions[i]);
                }
            }
        }

        if (dc->lcd_draw_sync != NULL)
        {
            dc->lcd_draw_sync();
        }
        dc->lcd_update(dc);

        // Clear dirty regions at frame end
        gui_dirty_region_clear();
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
        memset(dc->frame_buf, 0x00, ((size_t)dc->fb_height * dc->fb_width * dc->bit_depth) >> 3);
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
    uint32_t measure_end_time = gui_ms_get();
    if (dc->fb_measure_enable == true)
    {
        gui_log("fb draw time %d ms", measure_end_time - measure_start_time);
    }
}

uint32_t gui_fps()
{
    return frame_count_per_second;
}
void gui_fb_change(void)
{
    fb_change = true;
}


void gui_fb_disp(gui_obj_t *root, bool enable_event)
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

    obj_input_prepare(root);
    obj_reset_active(root);

    obj_draw_prepare(root);


    static uint32_t one_second = 0;
    uint32_t tick = gui_ms_get();
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


    obj_draw_end(root);

    gui_obj_event_dispatch(enable_event);

    gui_obj_count = obj_count;
    obj_count = 0;

    dc->frame_count++;


}


