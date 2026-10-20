/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/**
 * @file web_sim.h
 * @brief Browser (WebAssembly) simulator port
 */
#ifndef WEB_SIM_H
#define WEB_SIM_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Run the GUI server loop recorded by gui_thread_create(). Never returns
 * while the simulator is running; yields to the browser every frame. */
void web_sim_run_server(void);

/* Select the screen before main() runs. The DRV_LCD_* / DRV_PIXEL_BITS
 * compile-time macros are only the defaults, so one prebuilt module can run
 * projects with different screens. pixel_bits is 8, 16, 24 or 32. */
void web_sim_configure(int32_t width, int32_t height, int32_t pixel_bits, int32_t corner_radius);

/* The configured screen; valid before the engine starts. */
int32_t web_sim_screen_width(void);
int32_t web_sim_screen_height(void);

/* Frames the GUI server has run so far (it advances while the simulator is alive). */
uint32_t web_sim_frame_count(void);

/* Called from JavaScript (see web_sim.js) to deliver pointer input in
 * screen coordinates. */
void web_sim_touch(int32_t event, int32_t x, int32_t y);
void web_sim_wheel(int32_t event, int32_t delta);
void web_sim_key(int32_t key, int32_t pressed);

#ifdef __cplusplus
}
#endif

#endif /* WEB_SIM_H */
