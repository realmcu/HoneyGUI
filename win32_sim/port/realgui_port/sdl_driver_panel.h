/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SDL_DRIVER_PANEL_H
#define SDL_DRIVER_PANEL_H

#include <SDL.h>
#include <stdbool.h>
#include <stdint.h>

void sdl_panel_set_control_panel(int x, int y, int w, int h);
/**
 * @brief Add a button to the control panel
 *
 * @param x X coordinate relative to control panel (left-top corner of button)
 * @param y Y coordinate relative to control panel (left-top corner of button)
 * @param w Button width in pixels
 * @param h Button height in pixels
 * @param radius Button shape: -1=circle, 0=rectangle, >0=rounded corner radius
 * @param on_press Callback function called when button is pressed (can be NULL)
 * @param on_release Callback function called when button is released (can be NULL)
 */
void sdl_panel_add_button(int x, int y, int w, int h, int radius,
                          void (*on_press)(void), void (*on_release)(void));
void sdl_panel_draw(SDL_Surface *surface);
bool sdl_panel_check_click(int x, int y, bool is_down);

#endif /* SDL_DRIVER_PANEL_H */
