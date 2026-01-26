/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef __GUI_LOTTIE_H__
#define __GUI_LOTTIE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "guidef.h"
#include "gui_api.h"
#include "gui_obj.h"

typedef struct gui_lottie
{
    gui_obj_t base;
    void *lottie_data;
    void *image_buf;
    uint16_t img_w;
    uint16_t img_h;
    float frame_rate;
    float total_frames;
    float current_frame;
    uint32_t last_time;
    uint8_t opacity_value;
    uint8_t playing : 1;
    uint8_t loop : 1;
    uint8_t from_file : 1;
} gui_lottie_t;

/**
 * @brief Create a Lottie widget from memory.
 * @param parent Parent widget.
 * @param name Widget name.
 * @param addr Lottie JSON data address.
 * @param size Lottie JSON data size.
 * @param x X coordinate.
 * @param y Y coordinate.
 * @param w Width.
 * @param h Height.
 * @return Pointer to the created Lottie widget.
 */
gui_lottie_t *gui_lottie_create_from_mem(void       *parent,
                                         const char *name,
                                         void       *addr,
                                         uint32_t    size,
                                         int16_t     x,
                                         int16_t     y,
                                         int16_t     w,
                                         int16_t     h);

/**
 * @brief Create a Lottie widget from file.
 * @param parent Parent widget.
 * @param name Widget name.
 * @param filename Lottie JSON file path.
 * @param x X coordinate.
 * @param y Y coordinate.
 * @param w Width.
 * @param h Height.
 * @return Pointer to the created Lottie widget.
 */
gui_lottie_t *gui_lottie_create_from_file(void       *parent,
                                          const char *name,
                                          const char *filename,
                                          int16_t     x,
                                          int16_t     y,
                                          int16_t     w,
                                          int16_t     h);

/**
 * @brief Start playing the Lottie animation.
 * @param _this Lottie widget pointer.
 */
void gui_lottie_play(gui_lottie_t *_this);

/**
 * @brief Pause the Lottie animation.
 * @param _this Lottie widget pointer.
 */
void gui_lottie_pause(gui_lottie_t *_this);

/**
 * @brief Stop the Lottie animation.
 * @param _this Lottie widget pointer.
 */
void gui_lottie_stop(gui_lottie_t *_this);

/**
 * @brief Set the loop mode of the Lottie animation.
 * @param _this Lottie widget pointer.
 * @param loop True for loop, false for single play.
 */
void gui_lottie_set_loop(gui_lottie_t *_this, bool loop);

/**
 * @brief Set the current frame of the Lottie animation.
 * @param _this Lottie widget pointer.
 * @param frame Frame number.
 */
void gui_lottie_set_frame(gui_lottie_t *_this, float frame);

/**
 * @brief Get the current frame of the Lottie animation.
 * @param _this Lottie widget pointer.
 * @return Current frame number.
 */
float gui_lottie_get_frame(gui_lottie_t *_this);

/**
 * @brief Get the total frames of the Lottie animation.
 * @param _this Lottie widget pointer.
 * @return Total frame count.
 */
float gui_lottie_get_total_frames(gui_lottie_t *_this);

/**
 * @brief Set the opacity of the Lottie widget.
 * @param _this Lottie widget pointer.
 * @param opacity Opacity value (0-255).
 */
void gui_lottie_set_opacity(gui_lottie_t *_this, uint8_t opacity);

#ifdef __cplusplus
}
#endif

#endif
