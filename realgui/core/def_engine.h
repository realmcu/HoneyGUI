/**
 * @file def_vg.h
 * @author wenjing_jiang(wenjing_jiang@realsil.com.cn)
 * @brief
 * @version 0.1
 * @date 2024-06-14
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef __DEF_ENGINE_H__
#define __DEF_ENGINE_H__



#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "def_type.h"
#include "def_color.h"
#include "def_fb.h"



typedef struct draw_img
{
    uint16_t img_w;
    uint16_t img_h;
    int16_t img_target_x; //display start X
    int16_t img_target_y;  //display start Y
    uint16_t img_target_w;
    uint16_t img_target_h;
    void *data;
    gui_matrix_t matrix; //seems can remve by howie
    gui_matrix_t inverse;
    uint8_t opacity_value;
    uint32_t blend_mode : 3;
    uint32_t checksum : 8;
    uint32_t high_quality : 1;
    uint32_t color_mix;
    void *acc_user;
} draw_img_t;

typedef struct draw_circle
{
    int16_t x;
    int16_t y;
    int16_t Radius;
    int16_t lineWidth;
    gui_color_t color;
} draw_circle_t;


/* acc_engine_t struct define start */
typedef struct acc_engine
{
    void (*blit)(draw_img_t *image, gui_dispdev_t *dc, gui_rect_t *rect);
    //todo
    void (*draw_circle)(draw_circle_t *circle, gui_dispdev_t *dc, gui_rect_t *rect);
    void (*blur)(gui_dispdev_t *dc, gui_rect_t *rect, uint8_t blur_degree, void *cache_mem);

    void *(* jpeg_load)(void *input, int len, int *w, int *h, int *channel);
    void (* jpeg_free)(void *);
    void *(* idu_load)(void *input);
    void (* idu_free)(void *);
    bool enable_async;
    void *hw_acc_cache_mem; //for hardware acceleration cache memory
    uint32_t hw_acc_cache_size; //for hardware acceleration cache memory size

    // void (*draw_rectangle)(canvas_rectangle_t *r, struct gui_dispdev *dc);
    // void (*draw_arc)(canvas_arc_t *a, struct gui_dispdev *dc);
    // void (*draw_line)(canvas_line_t *l, struct gui_dispdev *dc);
    // void (*draw_polyline)(canvas_polyline_t *p, struct gui_dispdev *dc);
    // void (*draw_path)(draw_path_t *path, struct gui_dispdev *dc);
    // void (*draw_wave)(canvas_wave_t *w, struct gui_dispdev *dc);
    // void (*draw_palette_wheel)(canvas_palette_wheel_t *pw, struct gui_dispdev *dc);

    // void (*draw_svg)(void *svg, uint32_t data_length, struct gui_dispdev *dc, int x, int y,
    //                  float scale, float rotate_degree, float rotate_center_x, float rotate_center_y);
    // void (*begin_path)(canvas_path_t *data);
    // void (*move_to)(canvas_path_t *data, float x, float y);
    // void (*bezier_to)(canvas_path_t *data, float c1x, float c1y, float c2x, float c2y, float x,
    //                   float y);
    // void (*LineTo)(canvas_path_t *data, float x, float y);

} acc_engine_t;
/* acc_engine_t struct define end */



#ifdef __cplusplus
}
#endif

#endif

