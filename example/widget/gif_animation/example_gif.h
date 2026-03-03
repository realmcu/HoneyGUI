/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef __EXAMPLE_GIF_H__
#define __EXAMPLE_GIF_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief GIF event callback
 */
void gif_event_cb(void *obj, gui_event_t *e);

/**
 * @brief GIF single click event callback
 */
void gif_event_clicked_cb(void *obj, gui_event_t *e);

/**
 * @brief GIF double click event callback
 */
void gif_event_double_clicked_cb(void *obj, gui_event_t *e);

/**
 * @brief GIF triple click event callback
 */
void gif_event_triple_clicked_cb(void *obj, gui_event_t *e);

/**
 * @brief GIF long touch event callback
 */
void gif_event_long_cb(void *obj, gui_event_t *e);

#ifdef __cplusplus
}
#endif

#endif /* __EXAMPLE_GIF_H__ */
