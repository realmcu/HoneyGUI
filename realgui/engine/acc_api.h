/**
 * @file acc_api.h
 * @author wenjing_jiang@realsil.com.cn
 * @brief
 * @version 0.1
 * @date 2023-12-06
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef __ACC_API_H__
#define __ACC_API_H__

#include <guidef.h>
#include <draw_img.h>
#include <draw_path.h>



#ifdef __cplusplus
extern "C" {
#endif

void gui_acc_blit_to_dc(draw_img_t *image, struct gui_dispdev *dc, gui_rect_t *rect);

void *gui_acc_jpeg_load(void *input, int len, int *w, int *h, int *channel);

void gui_acc_jpeg_free(void *decode_image);


#ifdef __cplusplus
}
#endif

#endif
