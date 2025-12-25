/**
 * @file acc_sw_jpeg.h
 * @author wenjing_jiang@realsil.com.cn
 * @brief JPEG decoder interface
 * @version 0.1
 * @date 2024-06-21
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef __ACC_SW_JPEG_H__
#define __ACC_SW_JPEG_H__

#include "guidef.h"

#ifdef __cplusplus
extern "C" {
#endif

void *gui_acc_jpeg_load(void *input, int len, int *w, int *h, int *channel);
void gui_acc_jpeg_free(void *decode_image);

#ifdef __cplusplus
}
#endif

#endif
